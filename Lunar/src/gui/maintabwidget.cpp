#include "maintabwidget.h"
#include <QMessageBox>
#include <QTextEdit>
#include <QFontDialog>
#include "util/string.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui
{

MainTabWidget::MainTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //ctor
    Init();
}

MainTabWidget::~MainTabWidget()
{
    //dtor
}

void MainTabWidget::Init()
{
    setTabsClosable(true);
    setMovable(true);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(TabClose(int)));
}

int MainTabWidget::GetTabIndex(const QString& pathname)
{
    for (int i=0; i<count(); ++i)
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(i));
        if(NULL != pdocview)
        {
            if(pathname == pdocview->get_pathname())
                return i;
        }
    }
    return -1;
}

int MainTabWidget::AddDocViewTab(const QString& pathname)
{
    QString formatPathName = pathname;
    formatPathName.replace("\\", "/");

    int tab_index = GetTabIndex(formatPathName);
    if (tab_index < 0 || "" == formatPathName)
    {
        //if does not exist, add a tab
        DocView* pdocview = new DocView(formatPathName);
        tab_index = addTab(pdocview, pdocview->get_title());
        setTabToolTip(tab_index, formatPathName);

        connect(pdocview, SIGNAL(UpdateTitle(DocView*)), this, SLOT(UpdateTabTitleAndTip(DocView*)));
        connect(pdocview, SIGNAL(TextModified(DocView*)), this, SLOT(TabTextModified(DocView*)));
    }

    setCurrentIndex(tab_index);
    return tab_index;
}

bool MainTabWidget::SaveCurDocViewTab()
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        return pdocview->SaveDoc();
    else
        return false;
}

bool MainTabWidget::SaveAsCurDocViewTab()
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        return pdocview->SaveAsDoc();
    else
        return false;
}

void MainTabWidget::SetDocViewFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Courier New", 10), this);
    if ( ok ) {
        for (int i=0; i<count(); i++)
        {
            DocView* pdocview = dynamic_cast<DocView*>(widget(i));
            if(NULL != pdocview)
                pdocview->SetEditTextFont(font);
        }
    }
}

bool MainTabWidget::FindInCurTextEdit(const QString& expr,
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
        return pdocview->Find(expr, re, cs, wo, wrap, forward, first_find, from_start);
    else
        return false;
}

void MainTabWidget::ReplaceInCurTextEdit(const QString& replace_with_text)
{
    DocView* pdocview = dynamic_cast<DocView*>(currentWidget());
    if(NULL != pdocview)
        pdocview->Replace(replace_with_text);
}

void MainTabWidget::SaveAllViewTabs()
{
    for (int i=0; i<count(); i++)
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(i));
        if(NULL != pdocview)
            pdocview->SaveDoc();
    }
}

void MainTabWidget::UpdateTabTitleAndTip(DocView* pdocview)
{
    if (pdocview == currentWidget())
    {
        //change tab name
        setTabText(indexOf(pdocview), pdocview->get_title());
        setTabToolTip(indexOf(pdocview), pdocview->get_pathname());
    }
    else
    {
        for (int i=0; i<count(); i++)
        {
            if (pdocview == widget(i))
            {
                //change tab name
                setTabText(indexOf(pdocview), pdocview->get_title());
                setTabToolTip(indexOf(pdocview), pdocview->get_pathname());
                break;
            }
        }
    }
}

void MainTabWidget::TabTextModified(DocView* pdocview)
{
    if (pdocview == currentWidget())
    {
        //change tab name
        setTabText(indexOf(pdocview), "*" + pdocview->get_title());
    }
    else
    {
        for (int i=0; i<count(); i++)
        {
            if (pdocview == widget(i))
            {
                //change tab name
                setTabText(indexOf(pdocview), "*" + pdocview->get_title());
                break;
            }
        }
    }
}

void MainTabWidget::TabClose(int index)
{
    if (tabText(index).startsWith("*"))
    {
        DocView* pdocview = dynamic_cast<DocView*>(widget(index));
        if(NULL != pdocview)
        {
            int ret = QMessageBox::question(NULL, "question", "Save File?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
                pdocview->SaveDoc();
            removeTab(index);
        }
        else
        {
            QMessageBox::information(NULL, QObject::tr("Message"), tr("TabClose Failed!"));
        }
    }
    else
    {
        removeTab(index);
    }
}

bool MainTabWidget::HasUnsavedFiles()
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

} // namespace gui

