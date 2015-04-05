#include "mainwindow.h"
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
#include <map>
#include "util/file.hpp"
#include "lunarcommon.h"
#include "maintabwidget.h"
#include "finddialog.h"
#include "aboutdialog.h"
#include "outputwidget.h"
#include "luaexecutor.h"
#include "dockwidgetex.h"
#include "extension.h"

namespace gui
{

using namespace std;
using namespace util;

std::string MainWindow::s_file_filter_= "Lua Files(*.lua);;All Files(*.*)";

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
    prun_run_action_(NULL),
    prun_run_syscmd_action_(NULL),
    prun_stop_action_(NULL),
    phelp_about_action_(NULL),
    pstatus_text_(NULL),
    pmain_tabwidget_(NULL),
    pfind_dlg_(NULL),
    poutput_widget_(NULL),
    plua_executor_(NULL),
    pbottom_widget_(NULL),
    output_widget_on_(false)
{
    //ctor
    //user call Init after ctor
}

MainWindow::~MainWindow()
{
    //dtor
}

void MainWindow::ProcessCmdParam()
{
    if (LunarGlobal::get_arg_cnt()>1)
    {
        for (int i=1; i<LunarGlobal::get_arg_cnt(); i++)
        {
            std::string filepath = util::relativePathToAbsolutePath(LunarGlobal::get_arg(i));
            if(util::isPathExists(filepath))
                if(util::isPathFile(filepath))
                    pmain_tabwidget_->AddDocViewTab(StdStringToQString(filepath));
        }
    }
}

void MainWindow::InitBottomDockWidget()
{
    pbottom_widget_ = new DockWidgetEx(tr("Output"), this);

    poutput_widget_ = new OutputWidget();
    pbottom_widget_->setWidget(poutput_widget_);

    pbottom_widget_->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, pbottom_widget_);

    connect(pbottom_widget_, SIGNAL(onClose()), this, SLOT(OnBottomDockClose()));

    //hide at first
    pbottom_widget_->close();
}

void MainWindow::OnBottomDockClose()
{
    output_widget_on_ = false;
}

void MainWindow::InitLuaExecutor()
{
    plua_executor_ = new LuaExecutor(this);
}

bool MainWindow::Init()
{
    InitActions();
    InitMenubar();
    InitToolbar();
    InitStatusBar();
    InitMainWidget();
    InitFindDialog();
    InitBottomDockWidget();
    InitLuaExecutor();
    InitRunner();
    InitConnections();
    InitExtension();
    ProcessCmdParam();

    pmain_tabwidget_->setAcceptDrops(false);
    setAcceptDrops(true);

    if (LunarGlobal::get_arg_cnt()>0)
    {
        //std::string app_path = LunarGlobal::get_app_path();
        //std::string app_name = LunarGlobal::get_app_name();
        //app_name = util::strToLower(app_name);
        //QMessageBox::information(NULL, QObject::tr("path"), StdStringToQString(LunarGlobal::get_app_path()));

        map<string, string> filter_map;
        filter_map[kFileTypeLua] = std::string("Lua Files(") + FormatFileFilter(LunarGlobal::getLuaFileFilter()) + ")";
        filter_map[kFileTypeOctave] = std::string("Octave Files(") + FormatFileFilter(LunarGlobal::getOctaveFileFilter()) + ")";

        vector<string> vec;
        map<string, string>::iterator it = filter_map.find(LunarGlobal::getFileTypeDefault());
        if (it != filter_map.end())
        {
            vec.push_back(it->second);
            filter_map.erase(it);
        }

        for (map<string, string>::iterator it = filter_map.begin(); it != filter_map.end(); ++it)
            vec.push_back(it->second);
        vec.push_back("All Files(*.*)");

        s_file_filter_ = strJoin(vec, ";;");
    }

    return true;
}

std::string MainWindow::FormatFileFilter(const std::string& file_filter)
{
    std::vector<std::string> filterVec;
    util::strSplit(file_filter, ",", filterVec);
    for (std::vector<std::string>::iterator it = filterVec.begin(); it != filterVec.end(); ++it)
        *it = std::string("*.") + util::strTrim(*it);

    return util::strJoin(filterVec, ";");
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
        pmain_tabwidget_->AddDocViewTab(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if(pmain_tabwidget_->HasUnsavedFiles())
    {
       int ret = QMessageBox::question(this, "question", "Quit without save?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
       if(ret == QMessageBox::No)
       {
           e->ignore();
       }
    }

    LunarGlobal::set_mainwindow_width(this->width());
    LunarGlobal::set_mainwindow_height(this->height());
    LunarGlobal::Quit();
}

void MainWindow::InitActions()
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

    prun_run_action_ = new QAction(tr("Run"), this);
    prun_run_action_->setStatusTip(tr("Run."));
    prun_run_action_->setIcon(QIcon(tr(":/res/run.png")));
    prun_run_action_->setShortcut(Qt::Key_F5);

    prun_run_syscmd_action_ = new QAction(tr("Run in SysCmd"), this);
    prun_run_syscmd_action_->setStatusTip(tr("Run in system cmd."));
    prun_run_syscmd_action_->setIcon(QIcon(tr(":/res/run_syscmd.png")));
    prun_run_syscmd_action_->setShortcut(tr("ctrl+F5"));

    prun_stop_action_ = new QAction(tr("Stop"), this);
    prun_stop_action_->setStatusTip(tr("Stop."));
    prun_stop_action_->setIcon(QIcon(tr(":/res/stop.png")));
    prun_stop_action_->setShortcut(Qt::Key_F7);

    phelp_about_action_ = new QAction(tr("&About"), this);
    phelp_about_action_->setStatusTip(tr("About."));
}

void MainWindow::InitMenubar()
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

    QMenu* prun_menu = menuBar()->addMenu(tr("&Run"));
    prun_menu->addAction(prun_run_action_);
    prun_menu->addAction(prun_run_syscmd_action_);
    prun_menu->addAction(prun_stop_action_);

    QMenu* phelp_menu = menuBar()->addMenu(tr("&Help"));
    phelp_menu->addAction(phelp_about_action_);
}

void MainWindow::InitToolbar()
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

void MainWindow::InitFindDialog()
{
    pfind_dlg_ = new FindDialog(this);
}

void MainWindow::InitConnections()
{
    connect(pfile_new_action_, SIGNAL(triggered()), this, SLOT(FileNew()));
    connect(pfile_open_action_, SIGNAL(triggered()), this, SLOT(FileOpen()));
    connect(pfile_save_action_, SIGNAL(triggered()), this, SLOT(FileSave()));
    connect(pfile_save_as_action_, SIGNAL(triggered()), this, SLOT(FileSaveAs()));
    connect(pfile_save_all_action_, SIGNAL(triggered()), this, SLOT(FileSaveAll()));
    connect(pfile_close_action_, SIGNAL(triggered()), this, SLOT(FileClose()));
    connect(pfile_dump_action_, SIGNAL(triggered()), this, SLOT(FileDump()));
    connect(pfile_goto_next_action_, SIGNAL(triggered()), this, SLOT(FileGotoNext()));
    connect(pfile_goto_prev_action_, SIGNAL(triggered()), this, SLOT(FileGotoPrev()));
    connect(pedit_find_action_, SIGNAL(triggered()), this, SLOT(EditFind()));
    connect(pedit_font_action_, SIGNAL(triggered()), this, SLOT(EditSetFont()));
    connect(prun_run_action_, SIGNAL(triggered()), this, SLOT(Run()));
    connect(prun_run_syscmd_action_, SIGNAL(triggered()), this, SLOT(RunInSysCmd()));
    connect(prun_stop_action_, SIGNAL(triggered()), this, SLOT(Stop()));
    connect(phelp_about_action_, SIGNAL(triggered()), this, SLOT(HelpAbout()));
    //finddialog
    connect(pfind_dlg_, SIGNAL(Find(const QString&, bool, Qt::CaseSensitivity, bool, bool, bool, bool)),
            this, SLOT(Find(const QString&, bool, Qt::CaseSensitivity, bool, bool, bool, bool)));
    connect(pfind_dlg_, SIGNAL(Replace(const QString&, bool)), this, SLOT(Replace(const QString&, bool)));
    connect(pfind_dlg_, SIGNAL(ReplaceAll(const QString&, const QString&, Qt::CaseSensitivity, bool, bool, bool)),
            this, SLOT(ReplaceAll(const QString&, const QString&, Qt::CaseSensitivity, bool, bool, bool)));
    //luaexecutor
    connect(plua_executor_, SIGNAL(sendOutput(const QString&)),
            this, SLOT(AddOutput(const QString&)));
    connect(poutput_widget_, SIGNAL(sendInput(const QString&)),
            this, SLOT(SendInput(const QString&)));
}

void MainWindow::FileNew()
{
    pmain_tabwidget_->AddDocViewTab("");
}

void MainWindow::FileOpen()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", StdStringToQString(MainWindow::get_file_filter()));
    if(path.length() == 0)
    {
        //QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    else
    {
        pmain_tabwidget_->AddDocViewTab(path);
        //QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
    }
}

void MainWindow::FileSave()
{
    pmain_tabwidget_->SaveCurDocViewTab();
}

void MainWindow::FileSaveAs()
{
    pmain_tabwidget_->SaveAsCurDocViewTab();
}

void MainWindow::FileSaveAll()
{
    pmain_tabwidget_->SaveAllViewTabs();
}

void MainWindow::FileClose()
{
    pmain_tabwidget_->CloseCurDocViewTab();
}

void MainWindow::FileDump()
{
    DumpOutput();
}

void MainWindow::FileGotoNext()
{
    pmain_tabwidget_->GotoNextTabIndex();
}

void MainWindow::FileGotoPrev()
{
    pmain_tabwidget_->GotoPrevTabIndex();
}

void MainWindow::EditFind()
{
    pfind_dlg_->show();
}

void MainWindow::EditSetFont()
{
    pmain_tabwidget_->SetDocViewFont();
}

bool MainWindow::Find(const QString &str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output)
{
    if (find_in_output)
    {
        bool found =  poutput_widget_->FindText(str, false, cs, whole_word, wrap, !find_previous, first_find);
        if (!found)
            LunarMsgBoxQ("Can not find text : \"" + str + "\" in output");

        return found;
    }
    else
    {
        bool found = pmain_tabwidget_->FindInCurTextEdit(str, false, cs, whole_word, wrap, !find_previous, first_find);
        if (!found)
            LunarMsgBoxQ("Can not find text : \"" + str + "\"");

        return found;
    }
}

void MainWindow::Replace(const QString& replace_with_text, bool find_in_output)
{
    if (find_in_output)
        return;

    pmain_tabwidget_->ReplaceInCurTextEdit(replace_with_text);
}

void MainWindow::ReplaceAll(const QString& str, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output)
{
    if (find_in_output)
        return;

    bool found = false;
    int i = 0;
    do
    {
        found = pmain_tabwidget_->FindInCurTextEdit(str, false, cs, whole_word, false, !find_previous, !found, true);
        if(found)
        {
            Replace(replace_with_text, find_in_output);
            i++;
        }

    } while(found);

    LunarMsgBox(util::strFormat("%d occurrences were replaced.", i));
}

void MainWindow::HelpAbout()
{
    AboutDialog about_dlg;
    about_dlg.exec();
}

void MainWindow::OpenDoc(const QString& filepath)
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
            pmain_tabwidget_->AddDocViewTab(filepath);
            if (this->isMinimized())
                this->showNormal();
            this->activateWindow();
        }
}

void MainWindow::AddOutput(const QString& output)
{
    poutput_widget_->append(output);
}

void MainWindow::DumpOutput()
{
    DocView* pdoc_view = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (!pdoc_view)
        return;

    string script = QStringToStdString(pdoc_view->get_pathname());
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

void MainWindow::SendInput(const QString& input)
{
    plua_executor_->input(input);
}

void MainWindow::ClearOutput()
{
    poutput_widget_->clear();
}

void MainWindow::Run()
{
    RunEx(false);
}

void MainWindow::RunInSysCmd()
{
    RunEx(true);
}

void MainWindow::Stop()
{
    if (plua_executor_->isRunning())
    {
        plua_executor_->stop();
        poutput_widget_->append(tr("Running stopped manually."));
    }
}

void MainWindow::RunEx(bool run_in_syscmd)
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

    ClearOutput();
    DocView* pdoc_view = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdoc_view)
    {
        string addtional_args = "";
        if ("" != LunarGlobal::getRunAdditionalArgs())
            addtional_args = LunarGlobal::getRunAdditionalArgs();

        string script = QStringToStdString(pdoc_view->get_pathname());
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
                                    runPath, pdoc_view->GetExecutor());
        }
        else
        {
            ret = plua_executor_->execute(script, addtional_args,
                                    runPath, pdoc_view->GetExecutor());
        }

        if (!ret)
            AddOutput("Run failed");
    }
}

void MainWindow::InitRunner()
{
    string runner = strTrim(LunarGlobal::getRunnerLua());
    if (!strContains(runner, "/") && !strContains(runner, "\\"))
    {
        //relative path
        runner = LunarGlobal::get_app_path() + "/" + runner;
    }
}

void MainWindow::InitExtension()
{
    if (!Extension::getInstance().initLuaState())
        LunarMsgBox(Extension::getInstance().errorInfo());
}

void MainWindow::SetStatusText(const QString& text)
{
    pstatus_text_->setText(text);
}

} // namespace gui
