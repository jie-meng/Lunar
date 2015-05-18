#include "mainwindow.h"
#include <map>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QKeySequence>
#include <QToolBar>
#include <QStatusBar>
#include <QIcon>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDockWidget>
#include <QtCore/QUrl>
#include <QtCore/QWaitCondition>
#include "util/file.hpp"
#include "lunarcommon.h"
#include "maintabwidget.h"
#include "finddialog.h"
#include "aboutdialog.h"
#include "fileexplorerwidget.h"
#include "outputwidget.h"
#include "luaexecutor.h"
#include "searchresultswidget.h"
#include "extension.h"
#include "searchinputwidget.h"

namespace gui
{

using namespace std;
using namespace util;

//std::string MainWindow::s_file_filter_= "Lua Files(*.lua);;All Files(*.*)";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    pfile_new_action_(NULL),
    pfile_open_action_(NULL),
    pfile_save_action_(NULL),
    pfile_save_as_action_(NULL),
    pfile_save_all_action_(NULL),
    pfile_close_action_(NULL),
    pfile_close_all_action_(NULL),
    pfile_dump_action_(NULL),
    pfile_goto_next_action_(NULL),
    pfile_goto_prev_action_(NULL),
    pedit_find_action_(NULL),
    pedit_search_action_(NULL),
    pedit_font_action_(NULL),
    pedit_comment_action_(NULL),
    pview_file_explorer_action_(NULL),
    pview_search_results_action_(NULL),
    pview_documents_action_(NULL),
    pview_close_docks_action_(NULL),
    prun_run_action_(NULL),
    prun_stop_action_(NULL),
    phelp_about_action_(NULL),
    pstatus_text_(NULL),
    pmain_tabwidget_(NULL),
    pfind_dlg_(NULL),
    pfile_explorer_widget_(NULL),
    poutput_widget_(NULL),
    plua_executor_(NULL),
    pleft_widget_(NULL),
    pbottom_widget_(NULL),
    pbottom_tab_widget_(NULL),
    psearch_results_widget_(NULL)
{
    //ctor
    //user call Init after ctor"Lua Files(*.lua);;All Files(*.*)"
}

MainWindow::~MainWindow()
{
    //dtor
}

void MainWindow::processCmdParam()
{
    if (LunarGlobal::getInstance().getArgCnt()>1)
    {
        for (int i=1; i<LunarGlobal::getInstance().getArgCnt(); i++)
        {
            std::string filepath = util::relativePathToAbsolutePath(LunarGlobal::getInstance().getArg(i));
            if(util::isPathExists(filepath))
                if(util::isPathFile(filepath))
                    pmain_tabwidget_->addDocViewTab(StdStringToQString(filepath));
        }
    }
}

void MainWindow::initLeftDockWidget()
{
    pleft_widget_ = new QDockWidget(tr("Management"), this);
    pfile_explorer_widget_ = new FileExplorerWidget();
    pleft_widget_->setWidget(pfile_explorer_widget_);
    pleft_widget_->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, pleft_widget_);

    //hide at first
    pleft_widget_->close();
}

void MainWindow::initBottomDockWidget()
{
    pbottom_widget_ = new QDockWidget(tr("Logs and others"), this);
    pbottom_tab_widget_ = new QTabWidget;
    //pbottom_tab_widget_->setTabPosition(QTabWidget::South);
    pbottom_widget_->setWidget(pbottom_tab_widget_);
    poutput_widget_ = new OutputWidget();
    pbottom_tab_widget_->addTab(poutput_widget_, tr("Output"));
    psearch_results_widget_ = new SearchResultsWidget;
    pbottom_tab_widget_->addTab(psearch_results_widget_, tr("Search Resuts"));
    pbottom_widget_->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, pbottom_widget_);

    //hide at first
    pbottom_widget_->close();
}

void MainWindow::initLuaExecutor()
{
    plua_executor_ = new LuaExecutor(this);
}

bool MainWindow::init()
{
    initActions();
    initMenubar();
    initToolbar();
    InitStatusBar();
    InitMainWidget();
    initFindDialog();
    initLeftDockWidget();
    initBottomDockWidget();
    initLuaExecutor();
    initConnections();
    initExtension();
    processCmdParam();

    pmain_tabwidget_->setAcceptDrops(false);
    setAcceptDrops(true);

    return true;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QList<QUrl>::iterator it;
    for(it=urls.begin(); it!=urls.end(); ++it)
    {
        QString fileName = it->toLocalFile();
        if (fileName.isEmpty())
            break;
        pmain_tabwidget_->addDocViewTab(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if(pmain_tabwidget_->hasUnsavedFiles())
    {
       int ret = QMessageBox::question(this, "question", "Quit without save?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
       if(ret == QMessageBox::No)
       {
           e->ignore();
       }
    }

    if (windowState() != Qt::WindowMaximized && windowState() != Qt::WindowFullScreen)
    {
        LunarGlobal::getInstance().setMainwindowWidth(this->width());
        LunarGlobal::getInstance().setMainwindowHeight(this->height());
    }
    LunarGlobal::getInstance().quit();
}

void MainWindow::initActions()
{
    pfile_new_action_ = new QAction(tr("&New"), this);
    pfile_new_action_->setShortcut(QKeySequence::New);
    pfile_new_action_->setStatusTip(tr("New a file."));
    pfile_new_action_->setIcon(QIcon(tr(":/res/new.png")));

    pfile_open_action_ = new QAction(tr("&Open"), this);
    pfile_open_action_->setShortcut(QKeySequence::Open);
    pfile_open_action_->setStatusTip(tr("Open a file."));
    pfile_open_action_->setIcon(QIcon(tr(":/res/open.png")));

    pfile_save_action_ = new QAction(tr("&Save"), this);
    pfile_save_action_->setShortcut(QKeySequence::Save);
    pfile_save_action_->setStatusTip(tr("Save a file."));
    pfile_save_action_->setIcon(QIcon(tr(":/res/save.png")));

    pfile_save_as_action_ = new QAction(tr("Save as"), this);
    pfile_save_as_action_->setStatusTip(tr("Save as ..."));

    pfile_save_all_action_ = new QAction(tr("Save all"), this);
    pfile_save_all_action_->setStatusTip(tr("Save all files."));

    pfile_close_action_ = new QAction(tr("Close"), this);
    pfile_close_action_->setStatusTip(tr("Close current file."));
    pfile_close_action_->setShortcut(Qt::CTRL + Qt::Key_W);
    pfile_close_action_->setIcon(QIcon(tr(":/res/close.png")));

    pfile_close_all_action_ = new QAction(tr("Close all"), this);
    pfile_close_all_action_->setStatusTip(tr("Close all files."));
    pfile_close_all_action_->setShortcut(Qt::CTRL + Qt::SHIFT +  Qt::Key_W);
    pfile_close_all_action_->setIcon(QIcon(tr(":/res/close_all.png")));

    pfile_dump_action_ = new QAction(tr("Du&mp output"), this);
    pfile_dump_action_->setStatusTip(tr("Dump output."));
    pfile_dump_action_->setShortcut(Qt::CTRL + Qt::Key_M);
    pfile_dump_action_->setIcon(QIcon(tr(":/res/dump.png")));

    pfile_goto_next_action_ = new QAction(tr("Goto next"), this);
    pfile_goto_next_action_->setStatusTip(tr("Goto next document."));
    pfile_goto_next_action_->setShortcut(Qt::CTRL + Qt::Key_PageDown);
    pfile_goto_next_action_->setIcon(QIcon(tr(":/res/next.png")));

    pfile_goto_prev_action_ = new QAction(tr("Goto prev"), this);
    pfile_goto_prev_action_->setStatusTip(tr("Goto prev document."));
    pfile_goto_prev_action_->setShortcut(Qt::CTRL + Qt::Key_PageUp);
    pfile_goto_prev_action_->setIcon(QIcon(tr(":/res/prev.png")));

    pedit_find_action_ = new QAction(tr("&Find"), this);
    pedit_find_action_->setStatusTip(tr("Find."));
    pedit_find_action_->setShortcut(QKeySequence::Find);
    pedit_find_action_->setIcon(QIcon(tr(":/res/find.png")));

    pedit_search_action_ = new QAction(tr("Searc&h"), this);
    pedit_search_action_->setStatusTip(tr("Search in files."));
    pedit_search_action_->setShortcut(Qt::CTRL + Qt::Key_H);
    pedit_search_action_->setIcon(QIcon(tr(":/res/search.png")));

    pedit_font_action_ = new QAction(tr("Font"), this);
    pedit_font_action_->setStatusTip(tr("Set font."));
    pedit_font_action_->setIcon(QIcon(tr(":/res/font.png")));

    pedit_comment_action_ = new QAction(tr("Comment"), this);
    pedit_comment_action_->setStatusTip(tr("Comment current selection."));
    pedit_comment_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Slash);
	pedit_comment_action_->setIcon(QIcon(tr(":/res/comment.png")));

    pview_file_explorer_action_ = new QAction(tr("File Explorer"), this);
    pview_file_explorer_action_->setStatusTip(tr("File Explorer."));
    pview_file_explorer_action_->setShortcut(Qt::CTRL + Qt::Key_Tab);
    pview_file_explorer_action_->setIcon(QIcon(tr(":/res/file_explorer.png")));

    pview_search_results_action_ = new QAction(tr("Search &results"), this);
    pview_search_results_action_->setStatusTip((tr("Go to search results.")));
    pview_search_results_action_->setShortcut(Qt::CTRL + Qt::Key_R);
    pview_search_results_action_->setIcon(QIcon(tr(":/res/search_results.png")));

    pview_documents_action_ = new QAction(tr("Doc&uments"), this);
    pview_documents_action_->setStatusTip((tr("Go to documents edit.")));
    pview_documents_action_->setShortcut(Qt::CTRL + Qt::Key_E);
    pview_documents_action_->setIcon(QIcon(tr(":/res/document_edit.png")));

    pview_close_docks_action_ = new QAction(tr("Close docks"), this);
    pview_close_docks_action_->setStatusTip(tr("Close dock views."));
    pview_close_docks_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_B);
    pview_close_docks_action_->setIcon(QIcon(tr(":/res/close_docks.png")));

    prun_run_action_ = new QAction(tr("Run"), this);
    prun_run_action_->setStatusTip(tr("Run."));
    prun_run_action_->setIcon(QIcon(tr(":/res/run.png")));
    prun_run_action_->setShortcut(Qt::Key_F5);

    prun_stop_action_ = new QAction(tr("Stop"), this);
    prun_stop_action_->setStatusTip(tr("Stop."));
    prun_stop_action_->setIcon(QIcon(tr(":/res/stop.png")));
    prun_stop_action_->setShortcut(Qt::Key_F8);

    phelp_about_action_ = new QAction(tr("&About"), this);
    phelp_about_action_->setStatusTip(tr("About."));
    phelp_about_action_->setIcon(QIcon(tr(":/res/about.png")));
}

void MainWindow::initMenubar()
{
    QMenu* pfile_menu = menuBar()->addMenu(tr("&File"));
    pfile_menu->addAction(pfile_new_action_);
    pfile_menu->addAction(pfile_open_action_);
    pfile_menu->addAction(pfile_save_action_);
    pfile_menu->addAction(pfile_save_as_action_);
    pfile_menu->addAction(pfile_save_all_action_);
    pfile_menu->addAction(pfile_close_action_);
    pfile_menu->addAction(pfile_close_all_action_);
    pfile_menu->addAction(pfile_dump_action_);
    pfile_menu->addAction(pfile_goto_next_action_);
    pfile_menu->addAction(pfile_goto_prev_action_);

    QMenu* pedit_menu = menuBar()->addMenu(tr("&Edit"));
    pedit_menu->addAction(pedit_find_action_);
    pedit_menu->addAction(pedit_search_action_);
    pedit_menu->addAction(pedit_font_action_);
    pedit_menu->addAction(pedit_comment_action_);

    QMenu* pview_menu = menuBar()->addMenu(tr("&View"));
    pview_menu->addAction(pview_file_explorer_action_);
    pview_menu->addAction(pview_search_results_action_);
    pview_menu->addAction(pview_documents_action_);
    pview_menu->addAction(pview_close_docks_action_);

    QMenu* prun_menu = menuBar()->addMenu(tr("&Run"));
    prun_menu->addAction(prun_run_action_);
    prun_menu->addAction(prun_stop_action_);

    QMenu* phelp_menu = menuBar()->addMenu(tr("&Help"));
    phelp_menu->addAction(phelp_about_action_);
}

void MainWindow::initToolbar()
{
    QToolBar* ptoolbar = addToolBar(tr("Tools"));
    ptoolbar->addAction(pfile_new_action_);
    ptoolbar->addAction(pfile_open_action_);
    ptoolbar->addAction(pfile_save_action_);
    ptoolbar->addAction(pfile_goto_prev_action_);
    ptoolbar->addAction(pfile_goto_next_action_);
    ptoolbar->addAction(prun_run_action_);
    ptoolbar->addAction(prun_stop_action_);
    ptoolbar->addAction(pedit_find_action_);
    ptoolbar->addAction(pedit_search_action_);
	ptoolbar->addAction(pedit_comment_action_);
    ptoolbar->addAction(pview_file_explorer_action_);
    ptoolbar->addAction(pview_search_results_action_);
    ptoolbar->addAction(pview_documents_action_);
    ptoolbar->addAction(pview_close_docks_action_);
    ptoolbar->addAction(pfile_close_action_);
    ptoolbar->addAction(pfile_close_all_action_);
    ptoolbar->addAction(pfile_dump_action_);
}

void MainWindow::InitStatusBar()
{
    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}"));

    pstatus_text_ = new QLabel(this);
    statusBar()->addWidget(pstatus_text_);
    //statusBar()->addPermanentWidget(new QLabel("2011-09-03"));
}

void MainWindow::InitMainWidget()
{
    pmain_tabwidget_ = new gui::MainTabWidget(this);
    setCentralWidget(pmain_tabwidget_);
}

void MainWindow::initFindDialog()
{
    pfind_dlg_ = new FindDialog(this);
}

void MainWindow::initConnections()
{
    connect(pfile_new_action_, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(pfile_open_action_, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(pfile_save_action_, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(pfile_save_as_action_, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(pfile_save_all_action_, SIGNAL(triggered()), this, SLOT(fileSaveAll()));
    connect(pfile_close_action_, SIGNAL(triggered()), this, SLOT(fileClose()));
    connect(pfile_close_all_action_, SIGNAL(triggered()), this, SLOT(fileCloseAll()));
    connect(pfile_dump_action_, SIGNAL(triggered()), this, SLOT(fileDump()));
    connect(pfile_goto_next_action_, SIGNAL(triggered()), this, SLOT(fileGotoNext()));
    connect(pfile_goto_prev_action_, SIGNAL(triggered()), this, SLOT(fileGotoPrev()));
    connect(pedit_find_action_, SIGNAL(triggered()), this, SLOT(editFind()));
    connect(pedit_search_action_, SIGNAL(triggered()), this, SLOT(editSearch()));
    connect(pedit_font_action_, SIGNAL(triggered()), this, SLOT(editSetFont()));
    connect(pedit_comment_action_, SIGNAL(triggered()), this, SLOT(editComment()));
    connect(pview_file_explorer_action_, SIGNAL(triggered()), this, SLOT(viewFileExplorer()));
    connect(pview_search_results_action_, SIGNAL(triggered()), this, SLOT(viewSearchResultsWidget()));
    connect(pview_documents_action_, SIGNAL(triggered()), this, SLOT(viewDocuments()));
    connect(pview_close_docks_action_, SIGNAL(triggered()), this, SLOT(viewCloseDocks()));
    connect(prun_run_action_, SIGNAL(triggered()), this, SLOT(run()));
    connect(prun_stop_action_, SIGNAL(triggered()), this, SLOT(stop()));
    connect(phelp_about_action_, SIGNAL(triggered()), this, SLOT(helpAbout()));
    //finddialog
    connect(pfind_dlg_, SIGNAL(find(const QString&, bool, Qt::CaseSensitivity, bool, bool, bool, bool)),
            this, SLOT(find(const QString&, bool, Qt::CaseSensitivity, bool, bool, bool, bool)));
    connect(pfind_dlg_, SIGNAL(replace(const QString&, bool)), this, SLOT(replace(const QString&, bool)));
    connect(pfind_dlg_, SIGNAL(replaceAll(const QString&, const QString&, Qt::CaseSensitivity, bool, bool, bool)),
            this, SLOT(replaceAll(const QString&, const QString&, Qt::CaseSensitivity, bool, bool, bool)));
    //file explorer
    connect(pfile_explorer_widget_, SIGNAL(openFile(const QString&)), this, SLOT(openDoc(const QString&)));
    connect(this, SIGNAL(fileSaved(const QString&)), pfile_explorer_widget_, SLOT(onFileSaved(const QString&)));
            connect(this, SIGNAL(allFilesSaved()), pfile_explorer_widget_, SLOT(onAllFilesSaved()));
    //search results
    connect(psearch_results_widget_, SIGNAL(gotoSearchResult(QString,int)), this, SLOT(gotoSearchResult(QString,int)));
    //luaexecutor
    connect(plua_executor_, SIGNAL(sendOutput(const QString&)),
            this, SLOT(addOutput(const QString&)));
    connect(poutput_widget_, SIGNAL(sendInput(const QString&)),
            this, SLOT(sendInput(const QString&)));
}

void MainWindow::fileNew()
{
    pmain_tabwidget_->addDocViewTab("");
}

void MainWindow::fileOpen()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), pfile_explorer_widget_->getCurrentSelectedDir(), StdStringToQString(LunarGlobal::getInstance().getFileFilter()));
    if(path.length() == 0)
    {
        //QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    else
    {
        if (Extension::getInstance().ignoreFile(QStringToStdString(path)))
            return;

        pmain_tabwidget_->addDocViewTab(path);
        //QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
    }
}

void MainWindow::fileSave()
{
    std::pair<bool, QString> ret = pmain_tabwidget_->saveCurDocViewTab(pfile_explorer_widget_->getCurrentSelectedDir());
    if (ret.first)
        Q_EMIT fileSaved(ret.second);
}

void MainWindow::fileSaveAs()
{
    std::pair<bool, QString> ret = pmain_tabwidget_->saveAsCurDocViewTab(pfile_explorer_widget_->getCurrentSelectedDir());
    if (ret.first)
        Q_EMIT fileSaved(ret.second);
}

void MainWindow::fileSaveAll()
{
    pmain_tabwidget_->saveAllViewTabs(pfile_explorer_widget_->getCurrentSelectedDir());
    Q_EMIT allFilesSaved();
}

void MainWindow::fileClose()
{
    pmain_tabwidget_->closeCurDocViewTab();
}

void MainWindow::fileCloseAll()
{
    pmain_tabwidget_->closeAllDocViewTabs();
}

void MainWindow::fileDump()
{
    dumpOutput();
}

void MainWindow::fileGotoNext()
{
    pmain_tabwidget_->gotoNextTabIndex();
}

void MainWindow::fileGotoPrev()
{
    pmain_tabwidget_->gotoPrevTabIndex();
}

void MainWindow::editFind()
{
    pfind_dlg_->show();
    pfind_dlg_->setFocusOnFindInput();
}

void MainWindow::editSearch()
{
    QString text;
    QString file_filter;
    QString dir = StdStringToQString(currentPath());
    if (pfile_explorer_widget_)
        dir = pfile_explorer_widget_->getCurrentSelectedDir();

    QString str = pmain_tabwidget_->getCurrentDocSelectedText();
    text = StdStringToQString(strTrim(QStringToStdString(str)));
    QString pathname = pmain_tabwidget_->getCurrentDocPathname();

    file_filter = StdStringToQString(fileExtension(QStringToStdString(pathname)));

    SearchInputWidget inputwidget(QString(dir), text, file_filter);
    connect(&inputwidget, SIGNAL(inputOk(const QString&, const QString&, const QString&, bool, bool)),
            this, SLOT(searchTextInPath(const QString&, const QString&, const QString&, bool, bool)));
    inputwidget.exec();
}

void MainWindow::searchTextInPath(
                                  const QString& dir,
                                  const QString& text,
                                  const QString& file_filter,
                                  bool case_sensitive,
                                  bool use_regex)
{
    psearch_results_widget_->searchInPath(dir, text, file_filter, case_sensitive, use_regex);

    if (pbottom_widget_->isHidden())
        pbottom_widget_->show();

    pbottom_tab_widget_->setCurrentWidget(psearch_results_widget_);
    psearch_results_widget_->setFocus();
}

void MainWindow::gotoSearchResult(const QString& file, int line)
{
    openDoc(StdStringToQString(currentPath())+ "/" + file);
    pmain_tabwidget_->currentDocGotoLine(line);
}

void MainWindow::editSetFont()
{
    pmain_tabwidget_->setDocViewFont();
}

void MainWindow::editComment()
{
    pmain_tabwidget_->currentDocComment();
}

void MainWindow::viewFileExplorer()
{
    if (pleft_widget_->isHidden())
        pleft_widget_->show();

    pfile_explorer_widget_->setFocus();
}

void MainWindow::viewSearchResultsWidget()
{
    if (pbottom_widget_->isHidden())
        pbottom_widget_->show();
    pbottom_tab_widget_->setCurrentWidget(psearch_results_widget_);
    psearch_results_widget_->setFocus();
}

void MainWindow::viewDocuments()
{
    if (pmain_tabwidget_)
        pmain_tabwidget_->focusOnCurrentDoc();
}

void MainWindow::viewCloseDocks()
{
    if (pleft_widget_ && !pleft_widget_->isHidden())
        pleft_widget_->close();

    if (pbottom_widget_ && !pbottom_widget_->isHidden())
        pbottom_widget_->close();
}

bool MainWindow::find(const QString &str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output)
{
    if (find_in_output)
    {
        bool found =  poutput_widget_->findText(str, false, cs, whole_word, wrap, !find_previous, first_find);
        if (!found)
            LunarMsgBoxQ("Can not find text : \"" + str + "\" in output");

        return found;
    }
    else
    {
        bool found = pmain_tabwidget_->findInCurTextEdit(str, false, cs, whole_word, wrap, !find_previous, first_find);
        if (!found)
            LunarMsgBoxQ("Can not find text : \"" + str + "\"");

        return found;
    }
}

void MainWindow::replace(const QString& replace_with_text, bool find_in_output)
{
    if (find_in_output)
        return;

    pmain_tabwidget_->replaceInCurTextEdit(replace_with_text);
}

void MainWindow::replaceAll(const QString& str, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output)
{
    if (find_in_output)
        return;

    bool found = false;
    int i = 0;
    do
    {
        found = pmain_tabwidget_->findInCurTextEdit(str, false, cs, whole_word, false, !find_previous, !found, true);
        if(found)
        {
            replace(replace_with_text, find_in_output);
            ++i;
        }

    } while(found);

    LunarMsgBox(util::strFormat("%d occurrences were replaced.", i));
}

void MainWindow::helpAbout()
{
    AboutDialog about_dlg;
    about_dlg.exec();
}

void MainWindow::openDoc(const QString& filepath)
{
    std::string file_path = QStringToStdString(filepath);

    if("" == file_path)
    {
        if (this->isMinimized())
            this->showNormal();
        this->activateWindow();
        return;
    }

    if(util::isPathExists(file_path))
    {
        if(util::isPathFile(file_path))
        {
            if (Extension::getInstance().ignoreFile(file_path))
                return;

            pmain_tabwidget_->addDocViewTab(filepath);
            if (this->isMinimized())
                this->showNormal();
            this->activateWindow();
        }
    }
}

void MainWindow::addOutput(const QString& output)
{
    poutput_widget_->append(output);
}

void MainWindow::dumpOutput()
{
    DocView* pdoc_view = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (!pdoc_view)
        return;

    string script = QStringToStdString(pdoc_view->getPathname());
    if (script == "")
        return;

    pair<string, string> path_name = util::splitPathname(script);

    time_t time = mkCurrentTime();
    tm* ptm = localtime(&time);
    string timeFormat = strFormat("%04d-%02d-%02d_%02d-%02d-%02d",
                  ptm->tm_year + 1900,
                  ptm->tm_mon + 1,
                  ptm->tm_mday,
                  ptm->tm_hour,
                  ptm->tm_min,
                  ptm->tm_sec);

    string name = script + "_" + timeFormat + ".log";
    if (isPathFile(name))
        return;

    QString qstr = poutput_widget_->getOutput();
    if (qstr == "")
        return;

    string str = QStringToStdString(qstr);

    if (!writeTextFile(name, str))
        LunarMsgBox(strFormat("Dump output %s failed.", name.c_str()));
}

void MainWindow::sendInput(const QString& input)
{
    plua_executor_->input(input);
}

void MainWindow::clearOutput()
{
    poutput_widget_->clear();
}

void MainWindow::run()
{
    runEx();
}

void MainWindow::stop()
{
    if (plua_executor_->isRunning())
    {
        plua_executor_->stop();
        poutput_widget_->append(tr("Running stopped manually."));
    }
}

void MainWindow::runEx()
{
    if (plua_executor_->isRunning())
    {
        if (QMessageBox::No == QMessageBox::question(this, "question", "A script is running, will you still want to run current script?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        {
            return;
        }
        else
        {
            plua_executor_->stop();
        }
    }

    if (pbottom_widget_->isHidden())
        pbottom_widget_->show();
    pbottom_tab_widget_->setCurrentWidget(poutput_widget_);

    clearOutput();
    DocView* pdoc_view = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdoc_view)
    {
        string addtional_args = "";

        string script = QStringToStdString(pdoc_view->getPathname());
        if (script == "")
            return;

        pair<string, string> path_name = util::splitPathname(script);

        std::string runPath = path_name.first;
        //for windows disk root, there gonna be an error when excute if path is "X:"
        if (strEndWith(runPath, ":"))
            runPath += "/";

        bool ret = plua_executor_->execute(script, addtional_args,
                                    runPath, pdoc_view->getExecutor());

        if (!ret)
            addOutput("Run failed");
    }
}

void MainWindow::initExtension()
{
    if (!Extension::getInstance().init())
        LunarMsgBox(Extension::getInstance().errorInfo());

    LunarGlobal::getInstance().parseExtensionFileFilter();
}

void MainWindow::setStatusText(const QString& text)
{
    pstatus_text_->setText(text);
}

} // namespace gui
