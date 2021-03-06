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
#include "recentprojectpathdialog.h"
#include "recentdocdialog.h"
#include "findfiledialog.h"
#include "recentrundocdialog.h"
#include "aboutdialog.h"
#include "fileexplorerwidget.h"
#include "outputwidget.h"
#include "luaexecutor.h"
#include "searchresultswidget.h"
#include "extension.h"
#include "searchinputwidget.h"
#include "docview.h"
#include "jump_manager.h"

namespace gui
{

using namespace std;
using namespace util;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    pfile_new_action_(NULL),
    pfile_open_action_(NULL),
    pfile_save_action_(NULL),
    pfile_save_as_action_(NULL),
    pfile_save_all_action_(NULL),
    pfile_find_action_(NULL),
    pfile_close_action_(NULL),
    pfile_close_all_action_(NULL),
    pfile_goto_next_action_(NULL),
    pfile_goto_prev_action_(NULL),
    pfile_recent_docs_action_(NULL),
    pfile_recent_project_path_action_(NULL),
    pfile_settings_action_(NULL),
    pedit_intelligent_selection_action_(NULL),
    pedit_find_action_(NULL),
    pedit_replace_action_(NULL),
    pedit_search_action_(NULL),
    pedit_font_action_(NULL),
    pedit_comment_action_(NULL),
    pedit_comment_block_action_(NULL),
    pedit_goto_definition_action_(NULL),
    pedit_template_(NULL),
    pedit_jump_back_action_(NULL),
    pedit_jump_forward_action_(NULL),
    pedit_file_explorer_context_menu_action_(NULL),
    pedit_goto_line_begin_end_(NULL),
    pview_file_explorer_action_(NULL),
    pview_search_results_action_(NULL),
    pview_output_action_(NULL),
    pview_locate_current_file_(NULL),
    pview_documents_action_(NULL),
    pview_close_docks_action_(NULL),
    pview_zoom_in_(NULL),
    pview_zoom_out_(NULL),
    pview_zoom_to_origin_(NULL),
    pview_goto_tabs_(9),
    prun_run_action_(NULL),
    prun_run_recent_action_(NULL),
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
    psearch_results_widget_(NULL),
    is_closing_(false)
{
    //JumpManager init here to make JumpManager's position init at start,
    //or it'll not get correct value when first time use.
    JumpManager::getInstance();
}

MainWindow::~MainWindow()
{
}

void MainWindow::processCmdParam()
{
    if (LunarGlobal::getInstance().getArgCnt() > 1)
    {
        for (int i=1; i<LunarGlobal::getInstance().getArgCnt(); i++)
        {
            std::string filepath = util::relativePathToAbsolutePath(LunarGlobal::getInstance().getArg(i));
            if (util::isPathFile(filepath)) {
                pmain_tabwidget_->addDocViewTab(StdStringToQString(filepath));
            } else if (util::isPathDir(filepath)) {
                setCurrentPath(filepath);
                addNewRecentProjectPath(filepath);
            }
        }
    }
    else
    {
        if (currentPath() != "/")
        {
            addNewRecentProjectPath(currentPath());
        }
    }
}

void MainWindow::initLeftDockWidget()
{
    pleft_widget_ = new QDockWidget(tr(""), this);
    pfile_explorer_widget_ = new FileExplorerWidget();
    pleft_widget_->setWidget(pfile_explorer_widget_);
    pleft_widget_->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, pleft_widget_);

    //hide at first
    pleft_widget_->close();
}

void MainWindow::initBottomDockWidget()
{
    pbottom_widget_ = new QDockWidget(tr(""), this);
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
    processCmdParam();
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
    if (is_closing_)
    {
        e->accept();
        return;
    }

    if(pmain_tabwidget_->hasUnsavedFiles())
    {
       int ret = QMessageBox::question(this, "question", "Quit without save?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
       if(ret == QMessageBox::No)
           e->ignore();
    }

    if (windowState() != Qt::WindowMaximized && windowState() != Qt::WindowFullScreen)
    {
        LunarGlobal::getInstance().setMainwindowWidth(this->width());
        LunarGlobal::getInstance().setMainwindowHeight(this->height());
    }
    LunarGlobal::getInstance().quit();

    is_closing_ = true;
    e->accept();
}

void MainWindow::initActions()
{
    pfile_new_action_ = new QAction(tr("New"), this);
    pfile_new_action_->setShortcut(QKeySequence::New);
    pfile_new_action_->setStatusTip(tr("New a file."));
    pfile_new_action_->setIcon(QIcon(tr(":/res/new.png")));

    pfile_open_action_ = new QAction(tr("Open"), this);
    pfile_open_action_->setShortcut(QKeySequence::Open);
    pfile_open_action_->setStatusTip(tr("Open a file."));
    pfile_open_action_->setIcon(QIcon(tr(":/res/open.png")));

    pfile_save_action_ = new QAction(tr("Save"), this);
    pfile_save_action_->setShortcut(QKeySequence::Save);
    pfile_save_action_->setStatusTip(tr("Save a file."));
    pfile_save_action_->setIcon(QIcon(tr(":/res/save.png")));

    pfile_save_as_action_ = new QAction(tr("Save as"), this);
    pfile_save_as_action_->setStatusTip(tr("Save as ..."));

    pfile_save_all_action_ = new QAction(tr("Save all"), this);
    pfile_save_all_action_->setStatusTip(tr("Save all files."));

    pfile_find_action_ = new QAction(tr("Find files"), this);
    pfile_find_action_->setStatusTip(tr("Find files."));
    pfile_find_action_->setShortcut(Qt::CTRL + Qt::SHIFT +  Qt::Key_O);
    pfile_find_action_->setIcon(QIcon(tr(":/res/find_files.png")));

    pfile_close_action_ = new QAction(tr("Close"), this);
    pfile_close_action_->setStatusTip(tr("Close current file."));
    pfile_close_action_->setShortcut(Qt::CTRL + Qt::Key_W);
    pfile_close_action_->setIcon(QIcon(tr(":/res/close.png")));

    pfile_close_all_action_ = new QAction(tr("Close all"), this);
    pfile_close_all_action_->setStatusTip(tr("Close all files."));
    pfile_close_all_action_->setShortcut(Qt::CTRL + Qt::SHIFT +  Qt::Key_W);
    pfile_close_all_action_->setIcon(QIcon(tr(":/res/close_all.png")));

    pfile_goto_next_action_ = new QAction(tr("Goto next"), this);
    pfile_goto_next_action_->setStatusTip(tr("Goto next document."));
#ifdef __APPLE__
    pfile_goto_next_action_->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Right);
#else
    pfile_goto_next_action_->setShortcut(Qt::CTRL + Qt::Key_PageDown);
#endif
    pfile_goto_next_action_->setIcon(QIcon(tr(":/res/next.png")));

    pfile_goto_prev_action_ = new QAction(tr("Goto prev"), this);
    pfile_goto_prev_action_->setStatusTip(tr("Goto prev document."));
#ifdef __APPLE__
    pfile_goto_prev_action_->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Left);
#else
    pfile_goto_prev_action_->setShortcut(Qt::CTRL + Qt::Key_PageUp);
#endif
    pfile_goto_prev_action_->setIcon(QIcon(tr(":/res/prev.png")));

    pfile_recent_docs_action_ = new QAction(tr("Recent documents"), this);
    pfile_recent_docs_action_->setStatusTip(tr("Recent documents."));
    pfile_recent_docs_action_->setShortcut(Qt::CTRL + Qt::Key_E);
    pfile_recent_docs_action_->setIcon(QIcon(tr(":/res/documents.png")));

    pfile_recent_project_path_action_ = new QAction(tr("Reset project path"), this);
    pfile_recent_project_path_action_->setStatusTip(tr("Reset project path."));
    pfile_recent_project_path_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_P);
    pfile_recent_project_path_action_->setIcon(QIcon(tr(":/res/project_path.png")));

    pfile_settings_action_ = new QAction(tr("Lunar settings"), this);
    pfile_settings_action_->setStatusTip(tr("Go to Lunar config directory. You can edit config, make use of tools or create new plugins."));
    pfile_settings_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Y);
    pfile_settings_action_->setIcon(QIcon(tr(":/res/settings.png")));

    pedit_intelligent_selection_action_ = new QAction(tr("Intelligent selection"), this);
    pedit_intelligent_selection_action_->setStatusTip(tr("Select word or line on cursor position."));
    pedit_intelligent_selection_action_->setShortcut(Qt::ALT + Qt::Key_Up);
    pedit_intelligent_selection_action_->setIcon(QIcon(tr(":res/intelligent_selection.png")));

    pedit_find_action_ = new QAction(tr("Find"), this);
    pedit_find_action_->setStatusTip(tr("Find."));
    pedit_find_action_->setShortcut(QKeySequence::Find);
    pedit_find_action_->setIcon(QIcon(tr(":/res/find.png")));

    pedit_replace_action_ = new QAction(tr("Replace"), this);
    pedit_replace_action_->setStatusTip(tr("Replace."));
    pedit_replace_action_->setShortcut(Qt::CTRL + Qt::Key_R);

    pedit_search_action_ = new QAction(tr("Search"), this);
    pedit_search_action_->setStatusTip(tr("Search in files."));
    pedit_search_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_F);
    pedit_search_action_->setIcon(QIcon(tr(":/res/search.png")));

    pedit_font_action_ = new QAction(tr("Font"), this);
    pedit_font_action_->setStatusTip(tr("Set font."));
    pedit_font_action_->setIcon(QIcon(tr(":/res/font.png")));

    pedit_comment_action_ = new QAction(tr("Comment line"), this);
    pedit_comment_action_->setStatusTip(tr("Comment selection lines."));
    pedit_comment_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_C);
    pedit_comment_action_->setIcon(QIcon(tr(":/res/comment_line.png")));

    pedit_comment_block_action_ = new QAction(tr("Comment block"), this);
    pedit_comment_block_action_->setStatusTip(tr("Comment selection block."));
    pedit_comment_block_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_B);
    pedit_comment_block_action_->setIcon(QIcon(tr(":/res/comment_block.png")));

    pedit_goto_definition_action_ = new QAction(tr("Goto definition"), this);
    pedit_goto_definition_action_->setStatusTip(tr("Goto definition."));
    pedit_goto_definition_action_->setShortcut(Qt::CTRL + Qt::Key_G);
    pedit_goto_definition_action_->setIcon(QIcon(tr(":/res/goto_definition.png")));

    pedit_jump_back_action_ = new QAction(tr("Jump back"), this);
    pedit_jump_back_action_->setStatusTip(tr("Jump back."));
    pedit_jump_back_action_->setShortcut(Qt::CTRL + Qt::Key_Comma);
    pedit_jump_back_action_->setIcon(QIcon(tr(":/res/jump_back.png")));

    pedit_jump_forward_action_ = new QAction(tr("Jump forward"), this);
    pedit_jump_forward_action_->setStatusTip(tr("Jump forward."));
    pedit_jump_forward_action_->setShortcut(Qt::CTRL + Qt::Key_Period);
    pedit_jump_forward_action_->setIcon(QIcon(tr(":/res/jump_forward.png")));

    pedit_file_explorer_context_menu_action_ = new QAction(tr("Show File Explorer context menu"), this);
    pedit_file_explorer_context_menu_action_->setStatusTip(tr("Show File Explorer context menu."));
    pedit_file_explorer_context_menu_action_->setShortcut(Qt::CTRL + Qt::Key_J);

    pedit_goto_line_begin_end_ = new QAction(tr("Goto line begin or end"), this);
    pedit_goto_line_begin_end_->setStatusTip(tr("Goto line begin or end."));
    pedit_goto_line_begin_end_->setShortcut(Qt::ALT + Qt::Key_Down);

    pedit_template_ = new QAction(tr("Template"), this);
    pedit_template_->setStatusTip(tr("Complete code template."));
    pedit_template_->setShortcut(Qt::CTRL + Qt::Key_K);
    pedit_template_->setIcon(QIcon(tr(":/res/template.png")));

    pview_file_explorer_action_ = new QAction(tr("File Explorer"), this);
    pview_file_explorer_action_->setStatusTip(tr("File Explorer."));
    pview_file_explorer_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_E);
    pview_file_explorer_action_->setIcon(QIcon(tr(":/res/file_explorer.png")));

    pview_search_results_action_ = new QAction(tr("Search results"), this);
    pview_search_results_action_->setStatusTip((tr("Go to search results.")));
    pview_search_results_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_R);
    pview_search_results_action_->setIcon(QIcon(tr(":/res/search_results.png")));

    pview_output_action_ = new QAction(tr("Output"), this);
    pview_output_action_->setStatusTip((tr("Go to output.")));
    pview_output_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_M);
    pview_output_action_->setIcon(QIcon(tr(":/res/output.png")));

    pview_locate_current_file_ = new QAction(tr("Locate current file"), this);
    pview_locate_current_file_->setStatusTip((tr("Locate current file.")));
    pview_locate_current_file_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_K);
    pview_locate_current_file_->setIcon(QIcon(tr(":/res/locate.png")));

    pview_documents_action_ = new QAction(tr("Documents"), this);
    pview_documents_action_->setStatusTip((tr("Go to documents edit.")));
    pview_documents_action_->setShortcut(Qt::CTRL + Qt::Key_U);
    pview_documents_action_->setIcon(QIcon(tr(":/res/document_edit.png")));

    pview_close_docks_action_ = new QAction(tr("Close docks"), this);
    pview_close_docks_action_->setStatusTip(tr("Close dock views."));
    pview_close_docks_action_->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D);
    pview_close_docks_action_->setIcon(QIcon(tr(":/res/close_docks.png")));

    pview_zoom_in_ = new QAction(tr("Zoom in"), this);
    pview_zoom_in_->setStatusTip(tr("Zoom in."));
    pview_zoom_in_->setShortcut(Qt::CTRL + Qt::Key_Plus);

    pview_zoom_out_ = new QAction(tr("Zoom out"), this);
    pview_zoom_out_->setStatusTip(tr("Zoom out."));
    pview_zoom_out_->setShortcut(Qt::CTRL + Qt::Key_Minus);

    pview_zoom_to_origin_ = new QAction(tr("Zoom to origin"), this);
    pview_zoom_to_origin_->setStatusTip(tr("Zoom to origin."));
    pview_zoom_to_origin_->setShortcut(Qt::CTRL + Qt::Key_Equal);

    for (int i = 0; i < 9; ++i)
    {
        pview_goto_tabs_[i] = new QAction(QString("Go to tab %1").arg(i + 1), this);
        pview_goto_tabs_[i]->setStatusTip(QString("Go to tab %1.").arg(i + 1));
        pview_goto_tabs_[i]->setShortcut(Qt::CTRL + Qt::Key_1 + i);
    }

    prun_run_action_ = new QAction(tr("Run"), this);
    prun_run_action_->setStatusTip(tr("Run."));
    prun_run_action_->setIcon(QIcon(tr(":/res/run.png")));
    prun_run_action_->setShortcut(Qt::Key_F5);

    prun_run_recent_action_ = new QAction(tr("Run recent"), this);
    prun_run_recent_action_->setStatusTip(tr("Run recent."));
    prun_run_recent_action_->setIcon(QIcon(tr(":/res/run_recent.png")));
    prun_run_recent_action_->setShortcut(Qt::Key_F6);

    prun_stop_action_ = new QAction(tr("Stop"), this);
    prun_stop_action_->setStatusTip(tr("Stop."));
    prun_stop_action_->setIcon(QIcon(tr(":/res/stop.png")));
    prun_stop_action_->setShortcut(Qt::Key_F8);

    phelp_about_action_ = new QAction(tr("Information"), this);
    phelp_about_action_->setStatusTip(tr("Information."));
    phelp_about_action_->setIcon(QIcon(tr(":/res/about.png")));
    phelp_about_action_->setShortcut(Qt::Key_F1);
}

void MainWindow::initMenubar()
{
    QMenu* pfile_menu = menuBar()->addMenu(tr("&File"));
    pfile_menu->addAction(pfile_new_action_);
    pfile_menu->addAction(pfile_open_action_);
    pfile_menu->addAction(pfile_save_action_);
    pfile_menu->addAction(pfile_save_as_action_);
    pfile_menu->addAction(pfile_save_all_action_);
    pfile_menu->addAction(pfile_find_action_);
    pfile_menu->addAction(pfile_close_action_);
    pfile_menu->addAction(pfile_close_all_action_);
    pfile_menu->addAction(pfile_goto_next_action_);
    pfile_menu->addAction(pfile_goto_prev_action_);
    pfile_menu->addAction(pfile_recent_docs_action_);
    pfile_menu->addAction(pfile_recent_project_path_action_);
    pfile_menu->addAction(pfile_settings_action_);

    QMenu* pedit_menu = menuBar()->addMenu(tr("&Edit"));
    pedit_menu->addAction(pedit_intelligent_selection_action_);
    pedit_menu->addAction(pedit_find_action_);
    pedit_menu->addAction(pedit_replace_action_);
    pedit_menu->addAction(pedit_search_action_);
    pedit_menu->addAction(pedit_font_action_);
    pedit_menu->addAction(pedit_comment_action_);
    pedit_menu->addAction(pedit_comment_block_action_);
    pedit_menu->addAction(pedit_goto_definition_action_);
    pedit_menu->addAction(pedit_template_);
    pedit_menu->addAction(pedit_jump_back_action_);
    pedit_menu->addAction(pedit_jump_forward_action_);
    pedit_menu->addAction(pedit_file_explorer_context_menu_action_);
    pedit_menu->addAction(pedit_goto_line_begin_end_);

    QMenu* pview_menu = menuBar()->addMenu(tr("&View"));
    pview_menu->addAction(pview_file_explorer_action_);
    pview_menu->addAction(pview_search_results_action_);
    pview_menu->addAction(pview_output_action_);
    pview_menu->addAction(pview_locate_current_file_);
    pview_menu->addAction(pview_documents_action_);
    pview_menu->addAction(pview_close_docks_action_);
    pview_menu->addAction(pview_zoom_in_);
    pview_menu->addAction(pview_zoom_out_);
    pview_menu->addAction(pview_zoom_to_origin_);
    for (int i = 0; i < 9; ++i)
        pview_menu->addAction(pview_goto_tabs_[i]);

    QMenu* prun_menu = menuBar()->addMenu(tr("&Run"));
    prun_menu->addAction(prun_run_action_);
    prun_menu->addAction(prun_run_recent_action_);
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
    ptoolbar->addAction(pfile_find_action_);
    ptoolbar->addAction(pfile_goto_prev_action_);
    ptoolbar->addAction(pfile_goto_next_action_);
    ptoolbar->addAction(pfile_recent_docs_action_);
    ptoolbar->addAction(pfile_recent_project_path_action_);
    ptoolbar->addAction(pfile_settings_action_);
    ptoolbar->addAction(prun_run_action_);
    ptoolbar->addAction(prun_run_recent_action_);
    ptoolbar->addAction(prun_stop_action_);
    ptoolbar->addAction(pedit_intelligent_selection_action_);
    ptoolbar->addAction(pedit_find_action_);
    ptoolbar->addAction(pedit_search_action_);
	ptoolbar->addAction(pedit_comment_action_);
    ptoolbar->addAction(pedit_comment_block_action_);
    ptoolbar->addAction(pedit_goto_definition_action_);
    ptoolbar->addAction(pedit_template_);
    ptoolbar->addAction(pedit_jump_back_action_);
    ptoolbar->addAction(pedit_jump_forward_action_);
    ptoolbar->addAction(pview_file_explorer_action_);
    ptoolbar->addAction(pview_search_results_action_);
    ptoolbar->addAction(pview_output_action_);
    ptoolbar->addAction(pview_locate_current_file_);
    ptoolbar->addAction(pview_documents_action_);
    ptoolbar->addAction(pview_close_docks_action_);
    ptoolbar->addAction(pfile_close_action_);
    ptoolbar->addAction(pfile_close_all_action_);
    ptoolbar->addAction(phelp_about_action_);
}

void MainWindow::InitStatusBar()
{
    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}"));

    pstatus_text_ = new QLabel(this);
    statusBar()->addWidget(pstatus_text_);
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
    connect(pfile_find_action_, SIGNAL(triggered()), this, SLOT(fileFind()));
    connect(pfile_close_action_, SIGNAL(triggered()), this, SLOT(fileClose()));
    connect(pfile_close_all_action_, SIGNAL(triggered()), this, SLOT(fileCloseAll()));
    connect(pfile_goto_next_action_, SIGNAL(triggered()), this, SLOT(fileGotoNext()));
    connect(pfile_goto_prev_action_, SIGNAL(triggered()), this, SLOT(fileGotoPrev()));
    connect(pfile_recent_project_path_action_, SIGNAL(triggered()), this, SLOT(recentProjectPath()));
    connect(pfile_recent_docs_action_, SIGNAL(triggered()), this, SLOT(recentDocs()));
    connect(pfile_settings_action_, &QAction::triggered, [this]()
    {
        resetCurrentPath(StdStringToQString(LunarGlobal::getInstance().getAppPath()));
        viewFileExplorer();
    });

    connect(pedit_intelligent_selection_action_, SIGNAL(triggered()), this, SLOT(editIntelligentSelection()));
    connect(pedit_find_action_, SIGNAL(triggered()), this, SLOT(editFind()));
    connect(pedit_replace_action_, SIGNAL(triggered()), this, SLOT(editReplace()));
    connect(pedit_search_action_, SIGNAL(triggered()), this, SLOT(editSearch()));
    connect(pedit_font_action_, SIGNAL(triggered()), this, SLOT(editSetFont()));
    connect(pedit_comment_action_, SIGNAL(triggered()), this, SLOT(editComment()));
    connect(pedit_comment_block_action_, SIGNAL(triggered()), this, SLOT(editCommentBlock()));
    connect(pedit_goto_definition_action_, SIGNAL(triggered()), this, SLOT(editGotoDefinition()));
    connect(pedit_jump_back_action_, SIGNAL(triggered()), this, SLOT(editJumpBack()));
    connect(pedit_jump_forward_action_, SIGNAL(triggered()), this, SLOT(editJumpForward()));
    connect(pedit_file_explorer_context_menu_action_, &QAction::triggered, [this]()
    {
        viewFileExplorer();
        pfile_explorer_widget_->showContextMenu();
    });
    connect(pedit_goto_line_begin_end_, &QAction::triggered, [this]()
    {
        auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
        if (pdocview)
            pdocview->gotoLineBeginOrEnd();
    });
    connect(pedit_template_, &QAction::triggered, [this]()
    {
        auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
        if (pdocview)
            pdocview->codeTemplate();
    });

    connect(pview_file_explorer_action_, SIGNAL(triggered()), this, SLOT(viewFileExplorer()));
    connect(pview_search_results_action_, SIGNAL(triggered()), this, SLOT(viewSearchResultsWidget()));
    connect(pview_output_action_, SIGNAL(triggered()), this, SLOT(viewOutput()));
    connect(pview_locate_current_file_, SIGNAL(triggered()), this, SLOT(viewLocateCurrentFile()));
    connect(pview_documents_action_, SIGNAL(triggered()), this, SLOT(viewDocuments()));
    connect(pview_close_docks_action_, SIGNAL(triggered()), this, SLOT(viewCloseDocks()));
    connect(pview_zoom_in_, &QAction::triggered, [this]()
    {
        auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
        if (pdocview)
            pdocview->zoomIn();
    });
    connect(pview_zoom_out_, &QAction::triggered, [this]()
    {
        auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
        if (pdocview)
            pdocview->zoomOut();
    });
    connect(pview_zoom_to_origin_, &QAction::triggered, [this]()
    {
        auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
        if (pdocview)
            pdocview->zoomToOrigin();
    });

    for (int i = 0; i < 9; ++i)
    {
        connect(pview_goto_tabs_[i], &QAction::triggered, [i, this]()
        {
            if (i == 8)
            {
                if (pmain_tabwidget_->count() > 0)
                    pmain_tabwidget_->setCurrentIndex(pmain_tabwidget_->count() - 1);
            }
            else
            {
                if (pmain_tabwidget_->count() > i)
                    pmain_tabwidget_->setCurrentIndex(i);
            }
        });
    }

    connect(prun_run_action_, SIGNAL(triggered()), this, SLOT(run()));
    connect(prun_run_recent_action_, SIGNAL(triggered()), this, SLOT(runRecent()));
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
    connect(pfile_explorer_widget_, SIGNAL(setProjectPath(const QString&)), this, SLOT(resetCurrentPath(const QString&)));
    connect(this, SIGNAL(fileSaved(const QString&)), pfile_explorer_widget_, SLOT(onFileSaved(const QString&)));
            connect(this, SIGNAL(allFilesSaved()), pfile_explorer_widget_, SLOT(onAllFilesSaved()));
    connect(pfile_explorer_widget_, SIGNAL(executeExtensionTool(QString,QString,QString)), this, SLOT(executeScriptInPath(QString,QString,QString)));
    connect(pfile_explorer_widget_, &FileExplorerWidget::widthChanged, [this](int width)
    {
        if (width > 0)
        {
            pleft_widget_->setFixedWidth(width);
        }
        else
        {
            pleft_widget_->setMinimumWidth(0);
            pleft_widget_->setMaximumWidth(this->width());
        }
    });
    connect(pfile_explorer_widget_, &FileExplorerWidget::renameFile, [this](const QString& from, const QString& to)
    {
        int idx = pmain_tabwidget_->findTabIndexByFile(from);
        if (idx >= 0)
            pmain_tabwidget_->renameTab(idx, to);
    });
    connect(pfile_explorer_widget_, &FileExplorerWidget::renameDir, [this](const QString& from, const QString& to)
    {
        for (int i = 0; i < pmain_tabwidget_->count(); ++i)
        {
            string cur_name = strReplaceAll(QStringToStdString(pmain_tabwidget_->getDocPathname(i)), "\\", "/");
            string from_name = strReplaceAll(QStringToStdString(from), "\\", "/");
            string to_name = strReplaceAll(QStringToStdString(to), "\\", "/");
            if (strStartWith(cur_name, from_name))
            {
                QString new_pathname = StdStringToQString(strReplace(cur_name, from_name, to_name));
                pmain_tabwidget_->setDocPathname(i, new_pathname);
            }
        }
    });
    connect(pfile_explorer_widget_, &FileExplorerWidget::removeFile, [this](const QString& file)
    {
        int idx = pmain_tabwidget_->findTabIndexByFile(file);
        if (idx >= 0)
            pmain_tabwidget_->forceCloseTab(idx);
    });
    connect(pfile_explorer_widget_, &FileExplorerWidget::removeDir, [this](const QString& dir)
    {
        vector<string> vec;
        FileFilterRecursive< vector<string> > ff(vec);
        if (listFiles(QStringToStdString(dir), vec, &ff) > 0)
        {
            for (vector<string>::iterator it = vec.begin(); it != vec.end(); ++it)
            {
                int idx = pmain_tabwidget_->findTabIndexByFile(StdStringToQString(*it));
                if (idx >= 0)
                    pmain_tabwidget_->forceCloseTab(idx);
            }
        }
    });

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
    addCurrentDocToRecent();
    pmain_tabwidget_->addDocViewTab("");
}

void MainWindow::fileOpen()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), pfile_explorer_widget_->getCurrentSelectedDir(), StdStringToQString(LunarGlobal::getInstance().getFileFilter()));
    if(path.length() > 0)
    {
        if (!Extension::getInstance().isLegalFile(QStringToStdString(path)))
            return;

        pmain_tabwidget_->addDocViewTab(path);
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

void MainWindow::fileFind()
{
    FindFileDialog dlg;
    connect(&dlg, SIGNAL(selectDoc(const QString&)), this, SLOT(openDoc(const QString&)));
    dlg.exec();
}

void MainWindow::fileClose()
{
    addCurrentDocToRecent();
    pmain_tabwidget_->closeCurDocViewTab();
}

void MainWindow::fileCloseAll()
{
    pmain_tabwidget_->closeAllDocViewTabs();
}

void MainWindow::fileGotoNext()
{
    pmain_tabwidget_->gotoNextTabIndex();
}

void MainWindow::fileGotoPrev()
{
    pmain_tabwidget_->gotoPrevTabIndex();
}

void MainWindow::resetCurrentPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select project path"), StdStringToQString(currentPath()));
    if (path.length() > 0)
        resetCurrentPath(path);
}

void MainWindow::resetCurrentPath(const QString& path)
{
    string stdpath = QStringToStdString(path);
    setCurrentPath(stdpath);
    pfile_explorer_widget_->loadRoot();

    addNewRecentProjectPath(stdpath);
}

void MainWindow::recentProjectPath()
{
    RecentProjectPathDialog dlg;
    connect(&dlg, SIGNAL(selectRecentProjectPath(const QString&)),
            this, SLOT(resetCurrentPath(const QString&)));
    connect(&dlg, SIGNAL(newProjectPath()), this, SLOT(resetCurrentPath()));
    dlg.exec();
}

void MainWindow::recentDocs()
{
    RecentDocDialog dlg;
    connect(&dlg, SIGNAL(selectDoc(const QString&)), this, SLOT(openDoc(const QString&)));
    dlg.exec();
}

void MainWindow::editIntelligentSelection()
{
    auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdocview)
        pdocview->intelligentSelection();
}

void MainWindow::editFind()
{
    pfind_dlg_->show();
    pfind_dlg_->setFocusOnFindInput();
}

void MainWindow::editReplace()
{
    pfind_dlg_->show();
    pfind_dlg_->setFocusOnReplaceInput();
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
    viewSearchResultsWidget();
}

void MainWindow::gotoSearchResult(const QString& file, int line)
{
    //take a record
    auto record_pos = getCurrentPosition();

    QString doc_path = (file.startsWith("/") || (file.length()> 2 && file.at(1) == ':')) ?
            file :
            StdStringToQString(currentPath())+ "/" + file;

    if (openDoc(doc_path))
    {
        pmain_tabwidget_->currentDocGotoLine(line);
    }
}

std::pair<std::string, int> MainWindow::getCurrentPosition()
{
    string file = "";
    int line = 0;
    auto pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdocview)
    {
        auto tmp_file = QStringToStdString(pdocview->getPathname());
        if (isPathFile(tmp_file))
        {
            file = tmp_file;
            line = pdocview->getCurrentLine();
        }
    }

    return pair<string, int>(file, line);
}

void MainWindow::editSetFont()
{
    pmain_tabwidget_->setDocViewFont();
}

void MainWindow::editComment()
{
    pmain_tabwidget_->currentDocComment(true);
}

void MainWindow::editCommentBlock()
{
    pmain_tabwidget_->currentDocComment(false);
}

void MainWindow::editGotoDefinition()
{
    DocView* pdocview = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdocview)
    {
        psearch_results_widget_->clear();
        vector<string> results;
        if (pdocview->getDefinitions(results))
        {
            if (!results.empty())
            {
                int result_count = 0;
                string file = "";
                int line = 0;
                for (auto it = results.begin(); it != results.end(); ++it)
                {
                    vector<string> vec;
                    if (strSplit(*it, "\n", vec) == 3)
                    {
                        file = vec[0];
                        line = lexicalCastDefault<int>(vec[1], 0);

                        if (line > 0)
                        {
                            ++result_count;
                            psearch_results_widget_->addItem(
                                    StdStringToQString(vec[0]),
                                    StdStringToQString(vec[1]),
                                    StdStringToQString(vec[2]));
                        }
                    }
                }

                psearch_results_widget_->findFinish();

                if (result_count == 1)
                    gotoSearchResult(StdStringToQString(file), line);
                else
                    viewSearchResultsWidget();
            }
        }
    }
}

void MainWindow::editJumpBack()
{
    auto position = JumpManager::getInstance().getBackPosition();
    auto current_positon = getCurrentPosition();

    while (!position.first.empty() &&
           position.second > 0 &&
           !current_positon.first.empty() &&
           current_positon.second > 0)
    {
        if (position != current_positon)
        {
            if (gotoPosition(position.first, position.second))
                JumpManager::getInstance().moveBack();
            else
                JumpManager::getInstance().clear();
            break;
        }
        else
        {
            if (!JumpManager::getInstance().moveBack())
                break;

            position = JumpManager::getInstance().getBackPosition();
        }
    }
}

void MainWindow::editJumpForward()
{
    auto position = JumpManager::getInstance().getForwardPosition();
    auto current_positon = getCurrentPosition();

    while (
           !position.first.empty() &&
           position.second > 0 &&
           !current_positon.first.empty() &&
           current_positon.second > 0)
    {
        if (position != current_positon)
        {
            if (gotoPosition(position.first, position.second))
                JumpManager::getInstance().moveForward();
            else
                JumpManager::getInstance().clear();
            break;
        }
        else
        {
            if (!JumpManager::getInstance().moveForward())
                break;

            position = JumpManager::getInstance().getForwardPosition();
        }
    }
}

bool MainWindow::gotoPosition(const std::string& file, int line)
{
    if (openDoc(file, false))
    {
        pmain_tabwidget_->currentDocGotoLine(line);
        return true;
    }
    else
    {
        return false;
    }
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

void MainWindow::viewOutput()
{
    if (pbottom_widget_->isHidden())
        pbottom_widget_->show();
    pbottom_tab_widget_->setCurrentWidget(poutput_widget_);
    poutput_widget_->setFocusOnOutput();
}

void MainWindow::viewLocateCurrentFile()
{
    if (pmain_tabwidget_ && !pmain_tabwidget_->getCurrentDocPathname().isEmpty())
    {
        viewFileExplorer();
        pfile_explorer_widget_->locateFile(pmain_tabwidget_->getCurrentDocPathname());
    }
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

bool MainWindow::openDoc(const QString& file_path)
{
    return openDoc(QStringToStdString(file_path), true);
}

bool MainWindow::openDoc(const std::string& file_path, bool is_record_position)
{
    if(file_path.empty())
    {
        if (this->isMinimized())
            this->showNormal();
        this->activateWindow();
        return true;
    }

    if(!util::isPathFile(file_path))
        return false;

    if (!Extension::getInstance().isLegalFile(file_path))
    {
        return false;
    }

    addCurrentDocToRecent();
    if (is_record_position)
    {
        auto record_pos = getCurrentPosition();
        if (!record_pos.first.empty() && record_pos.second > 0)
            JumpManager::getInstance().recordPosition(record_pos.first, record_pos.second);
    }

    pmain_tabwidget_->addDocViewTab(StdStringToQString(file_path));

    if (this->isMinimized())
        this->showNormal();
    this->activateWindow();

    return true;
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
    if (script.empty())
        return;

    pair<string, string> path_name = util::splitPathname(script);

    DateTime dt = DateTime::now();
    string timeFormat = strFormat("%04d-%02d-%02d_%02d-%02d-%02d",
        dt.getYear(),
        dt.getMonth(),
        dt.getDay(),
        dt.getHour(),
        dt.getMinute(),
        dt.getSecond());

    string name = script + "_" + timeFormat + ".log";
    if (isPathFile(name))
        return;

    QString qstr = poutput_widget_->getOutput();
    if (0 == qstr.length())
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
    poutput_widget_->setFocusOnInput();

    clearOutput();
    DocView* pdoc_view = dynamic_cast<DocView*>(pmain_tabwidget_->currentWidget());
    if (pdoc_view)
    {
        string addtional_args = "";

        string script = QStringToStdString(pdoc_view->getPathname());
        if (script.empty())
            return;

        pair<string, string> path_name = util::splitPathname(script);
        std::string runPath = path_name.first;

        //for windows disk root, there gonna be an error when excute if path is "X:"
        if (strEndWith(runPath, ":"))
            runPath += "/";

        auto executor = pdoc_view->getExecutor();
        if (strStartWith(executor, "./"))
            executor = currentPath() + strRight(executor, executor.size() - 1);

        bool ret = plua_executor_->execute(script, addtional_args, runPath, executor);

        LunarGlobal::getInstance().addRecentRunDoc(script);

        if (!ret)
            addOutput("Run failed");
    }
}

void MainWindow::runDoc(const QString& doc)
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
    poutput_widget_->setFocusOnInput();

    clearOutput();

    string script = QStringToStdString(doc);
    if (script.empty())
        return;

    pair<string, string> path_name = util::splitPathname(script);
    std::string runPath = path_name.first;

    //for windows disk root, there gonna be an error when excute if path is "X:"
    if (strEndWith(runPath, ":"))
        runPath += "/";

    map<string, string> dict;
    string executor;
    if (Extension::getInstance().parseFilename(script, dict))
        executor = getValueFromMap<string>(dict, "executor", "");

    if (executor.empty())
    {
        addOutput("Cannot find support executor.");
        return;
    }

    bool ret = plua_executor_->execute(script, "", runPath, executor);

    LunarGlobal::getInstance().addRecentRunDoc(script);

    if (!ret)
        addOutput("Run failed");
}

void MainWindow::runRecent()
{
    RecentRunDocDialog dlg;
    connect(&dlg, SIGNAL(runDoc(const QString&)), this, SLOT(runDoc(const QString&)));
    dlg.exec();
}

void MainWindow::executeScriptInPath(const QString& script, const QString& execute_path, const QString& additional_args)
{
    stop();
    clearOutput();

    string ss = QStringToStdString(script);
    std::map<std::string, std::string> dict;
    if (Extension::getInstance().parseFilename(ss, dict))
    {
        std::map<std::string, std::string>::const_iterator it = dict.find("executor");
        if (it != dict.end())
        {
            string executor = strTrim(it->second);
            if (executor != "")
            {
                if (plua_executor_->execute(QStringToStdString(script), QStringToStdString(additional_args), QStringToStdString(execute_path), executor))
                {
                    if (pbottom_widget_->isHidden())
                        pbottom_widget_->show();
                    pbottom_tab_widget_->setCurrentWidget(poutput_widget_);
                    poutput_widget_->setFocusOnInput();
                }
            }
        }
    }
}

void MainWindow::stop()
{
    if (plua_executor_->isRunning())
    {
        plua_executor_->stop();
        poutput_widget_->append(tr("Running stopped manually."));
    }
}

void MainWindow::initExtension()
{
    if (!Extension::getInstance().init())
        LunarMsgBox(Extension::getInstance().errorInfo());

    LunarGlobal::getInstance().parseExtensionFileFilter();
}

void MainWindow::addCurrentDocToRecent()
{
    string cur_file = QStringToStdString(pmain_tabwidget_->getCurrentDocPathname());
    if (isPathFile(cur_file))
        LunarGlobal::getInstance().addRecentDoc(cur_file);
}

} // namespace gui
