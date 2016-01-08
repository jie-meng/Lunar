#include "maintabwidget.h"
#include <QMessageBox>
#include <QTextEdit>
#include <QFontDialog>
#include "util/base.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui
{

MainTabWidget::MainTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //ctor
    init();
}

MainTabWidget::~MainTabWidget()
{
    //dtor
}

void MainTabWidget::init()
{
    setTabsClosable(true);
    setMovable(true);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClose(int)));
}

int MainTabWidget::getTabIndex(const QString& pathname)
{
    for (int i=0; i<count(); ++i)
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(i));
        if(NULL != pdocview)
        {
            if(pathname == pdocview->getPathname())
                return i;
        }
    }

    return -1;
}

int MainTabWidget::addDocViewTab(const QString& pathname)
{
    QString formatPathName = pathname;
    formatPathName.replace("\\", "/");

    int tab_index = getTabIndex(formatPathName);
    if (tab_index < 0 || "" == formatPathName)
    {
        //if does not exist, add a tab
        DocView* pdocview = new DocView(formatPathName);
        tab_index = addTab(pdocview, pdocview->getTitle());
        setTabToolTip(tab_index, formatPathName);

        connect(pdocview, SIGNAL(updateTitle(DocView*)), this, SLOT(updateTabTitleAndTip(DocView*)));
        connect(pdocview, SIGNAL(textModified(DocView*)), this, SLOT(tabTextModified(DocView*)));
    }

    setCurrentIndex(tab_index);

    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if (pdocview)
        pdocview->focusOnText();

    return tab_index;
}

std::pair<bool, QString> MainTabWidget::saveCurDocViewTab(const QString& save_dialog_init_dir)
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
    {
        pdocview->setSaveDialogInitDir(save_dialog_init_dir);
        bool ret = pdocview->saveDoc();
        return std::make_pair(ret, pdocview->getPathname());
    }
    else
    {
        return std::make_pair(false, "");
    }
}

std::pair<bool, QString> MainTabWidget::saveAsCurDocViewTab(const QString& save_dialog_init_dir)
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
    {
        pdocview->setSaveDialogInitDir(save_dialog_init_dir);
        bool ret = pdocview->saveAsDoc();
        return std::make_pair(ret, pdocview->getPathname());
    }
    else
    {
        return std::make_pair(false, "");
    }
}

void MainTabWidget::saveAllViewTabs(const QString& save_dialog_init_dir)
{
    for (int i=0; i<count(); i++)
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(i));
        if(NULL != pdocview)
        {
            pdocview->setSaveDialogInitDir(save_dialog_init_dir);
            pdocview->saveDoc();
        }
    }
}

void MainTabWidget::closeCurDocViewTab()
{
    tabClose(indexOf(currentWidget()));
}

void MainTabWidget::closeAllDocViewTabs()
{
    while (count() > 0)
    {
        tabClose(0);
    }
}

void MainTabWidget::setDocViewFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Consolas", 10), this);
    if ( ok ) {
        for (int i=0; i<count(); i++)
        {
            DocView* pdocview = dynamic_cast<DocView*>(widget(i));
            if(NULL != pdocview)
                pdocview->setEditTextFont(font);
        }
    }
}

bool MainTabWidget::findInCurTextEdit(const QString& expr,
                           bool re,
                           bool cs,
                           bool wo,
                           bool wrap,
                           bool forward,
                           bool first_find,
						   bool from_start
                           )
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        return pdocview->find(expr, re, cs, wo, wrap, forward, first_find, from_start);
    else
        return false;
}

void MainTabWidget::replaceInCurTextEdit(const QString& replace_with_text)
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        pdocview->replace(replace_with_text);
}

void MainTabWidget::updateTabTitleAndTip(DocView* pdocview)
{
    if (pdocview == currentWidget())
    {
        //change tab name
        setTabText(indexOf(pdocview), pdocview->getTitle());
        setTabToolTip(indexOf(pdocview), pdocview->getPathname());
    }
    else
    {
        for (int i=0; i<count(); i++)
        {
            if (pdocview == widget(i))
            {
                //change tab name
                setTabText(indexOf(pdocview), pdocview->getTitle());
                setTabToolTip(indexOf(pdocview), pdocview->getPathname());
                break;
            }
        }
    }
}

void MainTabWidget::tabTextModified(DocView* pdocview)
{
    if (pdocview == currentWidget())
    {
        //change tab name
        setTabText(indexOf(pdocview), "*" + pdocview->getTitle());
    }
    else
    {
        for (int i=0; i<count(); i++)
        {
            if (pdocview == widget(i))
            {
                //change tab name
                setTabText(indexOf(pdocview), "*" + pdocview->getTitle());
                break;
            }
        }
    }
}

void MainTabWidget::tabClose(int index)
{
    if (tabText(index).startsWith("*"))
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(index));
        if(NULL != pdocview)
        {
            int ret = QMessageBox::question(NULL, "question", "Save File " + pdocview->getPathname() + "?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
                pdocview->saveDoc();
            removeTab(index);
            delete pdocview;
        }
        else
        {
            QMessageBox::information(NULL, QObject::tr("Message"), tr("TabClose Failed!"));
        }
    }
    else
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(index));
        removeTab(index);
        util::safeDelete(pdocview);
    }
}

bool MainTabWidget::hasUnsavedFiles()
{
    for (int i=0; i<count(); i++)
    {
        if(tabText(i).startsWith("*"))
        {
            return true;
        }
    }
    return false;
}

void MainTabWidget::gotoNextTabIndex()
{
    if (count() > 1)
    {
        int index = currentIndex() + 1;
        if (index > count() -1)
            index = 0;

        setCurrentIndex(index);
    }
}

void MainTabWidget::gotoPrevTabIndex()
{
    if (count() > 1)
    {
        int index = currentIndex() - 1;
        if (index < 0)
            index = count() - 1;

        setCurrentIndex(index);
    }
}

void MainTabWidget::currentDocComment(bool comment_line_or_block)
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        pdocview->commentSelection(comment_line_or_block);
}

QString MainTabWidget::getCurrentDocSelectedText() const
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        return pdocview->getSelectedText();
    else
        return tr("");
}

QString MainTabWidget::getCurrentDocPathname() const
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        return pdocview->getPathname();
    else
        return tr("");
}

void MainTabWidget::currentDocGotoLine(int line)
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        pdocview->gotoLine(line);
}

void MainTabWidget::focusOnCurrentDoc()
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        pdocview->focusOnEdit();
}

} // namespace gui

