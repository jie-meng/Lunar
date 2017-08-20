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
    init();
}

MainTabWidget::~MainTabWidget()
{
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
        if(pathname == dynamic_cast<DocView*>(widget(i))->getPathname())
            return i;
    }

    return -1;
}

bool MainTabWidget::addDocViewTab(const QString& pathname)
{
    QString formatPathName = pathname;
    formatPathName.replace("\\", "/");
    bool create_new = false;

    int tab_index = getTabIndex(formatPathName);
    if (tab_index < 0 || "" == formatPathName)
    {
        //if does not exist, add a tab
        DocView* pdocview = new DocView(formatPathName);
        tab_index = addTab(pdocview, pdocview->getTitle());
        setTabToolTip(tab_index, formatPathName);

        connect(pdocview, SIGNAL(updateTitle(DocView*)), this, SLOT(updateTabTitleAndTip(DocView*)));
        connect(pdocview, SIGNAL(textModified(DocView*)), this, SLOT(updateTabTitleAndTip(DocView*)));
        create_new = true;
    }

    setCurrentIndex(tab_index);
    if (currentWidget())
        dynamic_cast<DocView*>(currentWidget())->focusOnText();

    return create_new;
}

std::pair<bool, QString> MainTabWidget::saveCurDocViewTab(const QString& save_dialog_init_dir)
{
    if (!currentWidget())
        return std::pair<bool, QString>(false, tr(""));

    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    pdocview->setSaveDialogInitDir(save_dialog_init_dir);
    bool ret = pdocview->saveDoc();
    return std::make_pair(ret, pdocview->getPathname());
}

std::pair<bool, QString> MainTabWidget::saveAsCurDocViewTab(const QString& save_dialog_init_dir)
{
    if (!currentWidget())
        return std::pair<bool, QString>(false, tr(""));

    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    pdocview->setSaveDialogInitDir(save_dialog_init_dir);
    bool ret = pdocview->saveAsDoc();
    return std::make_pair(ret, pdocview->getPathname());
}

void MainTabWidget::saveAllViewTabs(const QString& save_dialog_init_dir)
{
    for (int i=0; i<count(); i++)
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(i));
        pdocview->setSaveDialogInitDir(save_dialog_init_dir);
        pdocview->saveDoc();
    }
}

void MainTabWidget::closeCurDocViewTab()
{
    if (!currentWidget())
        return;

    tabClose(indexOf(currentWidget()));
}

void MainTabWidget::closeAllDocViewTabs()
{
    while (count() > 0)
        tabClose(0);
}

void MainTabWidget::setDocViewFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, LunarGlobal::getInstance().getFont(), this);
    if (ok) {
        for (int i=0; i<count(); i++)    
            dynamic_cast<DocView*>(widget(i))->setEditTextFont(font);
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
    if (!currentWidget())
        return false;

    return dynamic_cast<DocView*>(currentWidget())->find(expr, re, cs, wo, wrap, forward, first_find, from_start);
}

void MainTabWidget::replaceInCurTextEdit(const QString& replace_with_text)
{
    if (!currentWidget())
        return;

    dynamic_cast<DocView*>(currentWidget())->replace(replace_with_text);
}

void MainTabWidget::updateTabTitleAndTip(DocView* pdocview)
{
    auto index = indexOf(pdocview);
    if (tabText(index) != pdocview->getTitle())
        setTabText(index, pdocview->getTitle());

    if (tabToolTip(index) != pdocview->getPathname())
        setTabToolTip(index, pdocview->getPathname());
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
            return true;
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

QString MainTabWidget::getDocPathname(int index) const
{
    return dynamic_cast<DocView*>(widget(index))->getPathname();
}

void MainTabWidget::setDocPathname(int index, const QString& pathname)
{
    DocView* pdocview = dynamic_cast<DocView*>(widget(index));
    pdocview->setPathname(pathname);
    setTabText(index, pdocview->getTitle());
    setTabToolTip(index, pdocview->getPathname());
}

void MainTabWidget::currentDocComment(bool comment_line_or_block)
{
    if (!currentWidget())
        return;

    dynamic_cast<DocView*>(currentWidget())->commentSelection(comment_line_or_block);
}

QString MainTabWidget::getCurrentDocSelectedText() const
{
    if (!currentWidget())
        return "";

    return dynamic_cast<DocView*>(currentWidget())->getSelectedText();
}

QString MainTabWidget::getCurrentDocPathname() const
{
    if (!currentWidget())
        return "";

    return dynamic_cast<DocView*>(currentWidget())->getPathname();
}

void MainTabWidget::currentDocGotoLine(int line)
{    
    if (!currentWidget())
        return;

    dynamic_cast<DocView*>(currentWidget())->gotoLine(line);
}

void MainTabWidget::focusOnCurrentDoc()
{    
    if (!currentWidget())
        return;

    dynamic_cast<DocView*>(currentWidget())->focusOnEdit();
}

int MainTabWidget::findTabIndexByFile(const QString& file)
{
    for (int i=0; i<count(); i++)
    {
        if (dynamic_cast<DocView*>(widget(i))->getPathname() == file)
            return i;
    }

    return -1;
}

void MainTabWidget::renameTab(int index, const QString& new_pathname)
{
    DocView* pdocview = dynamic_cast<DocView*>(widget(index));
    pdocview->setPathname(new_pathname);
    setTabText(index, pdocview->getTitle());
    setTabToolTip(index, pdocview->getPathname());
}

void MainTabWidget::forceCloseTab(int index)
{
    DocView* pdocview = dynamic_cast<DocView*>(widget(index));
    removeTab(index);
    util::safeDelete(pdocview);
}

} // namespace gui

