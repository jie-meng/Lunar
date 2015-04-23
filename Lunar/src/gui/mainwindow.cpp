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
#include "dockwidgetex.h"
#include "extension.h"

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
    pfile_dump_action_(NULL),
    pfile_goto_next_action_(NULL),
    pfile_goto_prev_action_(NULL),
    pedit_find_action_(NULL),
    pedit_font_action_(NULL),
    pview_file_explorer_action_(NULL),
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
    file_explorer_widget_on_(false),
    output_widget_on_(false)
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
    pleft_widget_ = new DockWidgetEx(tr("File Explorer"), this);
    pfile_explorer_widget_ = new FileExplorerWidget();
    pleft_widget_->setWidget(pfile_explorer_widget_);
    pleft_widget_->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, pleft_widget_);

    connect(pleft_widget_, SIGNAL(onClose()), this, SLOT(onLeftDockClose()));

    //hide at first
    pleft_widget_->close();
}

void MainWindow::initBottomDockWidget()
{
    pbottom_widget_ = new DockWidgetEx(tr("Output"), this);
    poutput_widget_ = new OutputWidget();
    pbottom_widget_->setWidget(poutput_widget_);
    pbottom_widget_->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, pbottom_widget_);

    connect(pbottom_widget_, SIGNAL(onClose()), this, SLOT(onBottomDockClose()));

    //hide at first
    pbottom_widget_->close();
}

void MainWindow::onLeftDockClose()
{
    file_explorer_widget_on_ = false;
}

void MainWindow::onBottomDockClose()
{
    output_widget_on_ = false;
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
    pfile_close_action_->setShortcut(tr("ctrl+w"));

    pfile_dump_action_ = new QAction(tr("&Dump output"), this);
    pfile_dump_action_->setStatusTip(tr("Dump output."));
    pfile_dump_action_->setShortcut(Qt::Key_F12);

    pfile_goto_next_action_ = new QAction(tr("Goto next"), this);
    pfile_goto_next_action_->setStatusTip(tr("Goto next document"));
    pfile_goto_next_action_->setShortcut(Qt::CTRL + Qt::Key_PageDown);

    pfile_goto_prev_action_ = new QAction(tr("Goto prev"), this);
    pfile_goto_prev_action_->setStatusTip(tr("Goto prev document"));
    pfile_goto_prev_action_->setShortcut(Qt::CTRL + Qt::Key_PageUp);

    pedit_find_action_ = new QAction(tr("&Find"), this);
    pedit_find_action_->setShortcut(QKeySequence::Find);
    pedit_find_action_->setStatusTip(tr("Find."));

    pedit_font_action_ = new QAction(tr("Font"), this);
    pedit_font_action_->setStatusTip(tr("Set font."));

    pview_file_explorer_action_ = new QAction(tr("File Explorer"), this);
    pview_file_explorer_action_->setStatusTip(tr("File Explorer"));

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
    pfile_menu->addAction(pfile_dump_action_);
    pfile_menu->addAction(pfile_goto_next_action_);
    pfile_menu->addAction(pfile_goto_prev_action_);

    QMenu* pedit_menu = menuBar()->addMenu(tr("&Edit"));
    pedit_menu->addAction(pedit_find_action_);
    pedit_menu->addAction(pedit_font_action_);

    QMenu* pview_menu = menuBar()->addMenu(tr("&View"));
    pview_menu->addAction(pview_file_explorer_action_);

    QMenu* prun_menu = menuBar()->addMenu(tr("&Run"));
    prun_menu->addAction(prun_run_action_);
    prun_menu->addAction(prun_stop_action_);

    QMenu* phelp_menu = menuBar()->addMenu(tr("&Help"));
    phelp_menu->addAction(phelp_about_action_);
}

void MainWindow::initToolbar()
{
    QToolBar* ptoolbar = addToolBar(tr("&File"));
    ptoolbar->addAction(pfile_new_action_);
    ptoolbar->addAction(pfile_open_action_);
    ptoolbar->addAction(pfile_save_action_);
    ptoolbar->addAction(prun_run_action_);
    ptoolbar->addAction(prun_stop_action_);
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

//    if(LunarGlobal::get_arg_cnt() == 1)
//        FileNew();
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
    connect(pfile_dump_action_, SIGNAL(triggered()), this, SLOT(fileDump()));
    connect(pfile_goto_next_action_, SIGNAL(triggered()), this, SLOT(fileGotoNext()));
    connect(pfile_goto_prev_action_, SIGNAL(triggered()), this, SLOT(fileGotoPrev()));
    connect(pedit_find_action_, SIGNAL(triggered()), this, SLOT(editFind()));
    connect(pedit_font_action_, SIGNAL(triggered()), this, SLOT(editSetFont()));
    connect(pview_file_explorer_action_, SIGNAL(triggered()), this, SLOT(viewFileExplorer()));
    connect(prun_run_action_, SIGNAL(triggered()), this, SLOT(run()));
    connect(prun_stop_action_, SIGNAL(triggered()), this, SLOT(stop()));
    connect(phelp_about_action_, SIGNAL(triggered()), this, SLOT(helpAbout()));
    //finddialog
    connect(pfind_dlg_, SIGNAL(find(const QString&, bool, Qt::CaseSensitivity, bool, bool, bool, bool)),
            this, SLOT(find(const QString&, bool, Qt::CaseSensitivity, bool, bool, bool, bool)));
    connect(pfind_dlg_, SIGNAL(replace(const QString&, bool)), this, SLOT(replace(const QString&, bool)));
    connect(pfind_dlg_, SIGNAL(ReplaceAll(const QString&, const QString&, Qt::CaseSensitivity, bool, bool, bool)),
            this, SLOT(ReplaceAll(const QString&, const QString&, Qt::CaseSensitivity, bool, bool, bool)));
    //file explorer
    connect(pfile_explorer_widget_, SIGNAL(openFile(const QString&)), this, SLOT(openDoc(const QString&)));
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
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", StdStringToQString(LunarGlobal::getInstance().getFileFilter()));
    if(path.length() == 0)
    {
        //QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    else
    {
        pmain_tabwidget_->addDocViewTab(path);
        //QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
    }
}

void MainWindow::fileSave()
{
    pmain_tabwidget_->saveCurDocViewTab();
}

void MainWindow::fileSaveAs()
{
    pmain_tabwidget_->saveAsCurDocViewTab();
}

void MainWindow::fileSaveAll()
{
    pmain_tabwidget_->saveAllViewTabs();
}

void MainWindow::fileClose()
{
    pmain_tabwidget_->closeCurDocViewTab();
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

void MainWindow::editSetFont()
{
    pmain_tabwidget_->setDocViewFont();
}

void MainWindow::viewFileExplorer()
{
    if (!file_explorer_widget_on_)
    {
        pleft_widget_->show();
        file_explorer_widget_on_ = true;
    }
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
            i++;
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
        if(util::isPathFile(file_path))
        {
            pmain_tabwidget_->addDocViewTab(filepath);
            if (this->isMinimized())
                this->showNormal();
            this->activateWindow();
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
    runEx(false);
}

//void MainWindow::runInSysCmd()
//{
//    runEx(true);
//}

void MainWindow::stop()
{
    if (plua_executor_->isRunning())
    {
        plua_executor_->stop();
        poutput_widget_->append(tr("Running stopped manually."));
    }
}

void MainWindow::runEx(bool run_in_syscmd)
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

    if (!output_widget_on_)
    {
        pbottom_widget_->show();
        output_widget_on_ = true;
    }

    clearOutput();
    DocView* pdoc_view = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdoc_view)
    {
        string addtional_args = "";
//        if ("" != LunarGlobal::getInstance().getRunAdditionalArgs())
//            addtional_args = LunarGlobal::getInstance().getRunAdditionalArgs();

        string script = QStringToStdString(pdoc_view->getPathname());
        if (script == "")
            return;

        pair<string, string> path_name = util::splitPathname(script);

        std::string runPath = path_name.first;
        //for windows disk root, there gonna be an error when excute if path is "X:"
        if (strEndWith(runPath, ":"))
            runPath += "/";

        bool ret = false;
        if (run_in_syscmd)
        {
            ret = plua_executor_->executeInSysCmd(script, addtional_args,
                                    runPath, pdoc_view->getExecutor());
        }
        else
        {
            ret = plua_executor_->execute(script, addtional_args,
                                    runPath, pdoc_view->getExecutor());
        }

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
