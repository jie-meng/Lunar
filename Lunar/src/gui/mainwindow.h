#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include "util/base.hpp"

class QAction;
class QLabel;
class QGraphicsView;
class LuaExecutor;
class QTabWidget;
class QDockWidget;

namespace gui
{

class MainTabWidget;
class FindDialog;
class OutputWidget;
class FileExplorerWidget;
class SearchResultsWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();
    bool init();
Q_SIGNALS:
    void fileSaved(const QString&);
    void allFilesSaved();
public Q_SLOTS:
    void openDoc(const QString& filepath);
    //output
    void addOutput(const QString& output);
    void sendInput(const QString& input);
    void dumpOutput();
    void clearOutput();
private Q_SLOTS:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSaveAll();
    void fileClose();
    void fileCloseAll();
    void fileDump();
    void fileGotoNext();
    void fileGotoPrev();
    void editFind();
    void editSearch();
    void editGotoSearchResultsWidget();
    void editGotoDocuments();
    void editSetFont();
    void editComment();
    void viewFileExplorer();
    void viewCloseDocks();
    void helpAbout();
    bool find(const QString& str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output);
    void replace(const QString&, bool find_in_output);
    void replaceAll(const QString& str, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output);
    void run();
    void stop();
    void setStatusText(const QString& text);
    void searchTextInPath(const QString& dir,
                          const QString& text,
                          const QString& file_filter,
                          bool case_sensitive,
                          bool use_regex);
    void gotoSearchResult(const QString& file, int line);
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void closeEvent(QCloseEvent* e);
private:
    void initActions();
    void initMenubar();
    void initToolbar();
    void InitStatusBar();
    void InitMainWidget();
    void initConnections();
    void initFindDialog();
    void initLeftDockWidget();
    void initBottomDockWidget();
    void initLuaExecutor();
    void initExtension();
    void processCmdParam();
    void runEx();
    QAction* pfile_new_action_;
    QAction* pfile_open_action_;
    QAction* pfile_save_action_;
    QAction* pfile_save_as_action_;
    QAction* pfile_save_all_action_;
    QAction* pfile_close_action_;
    QAction* pfile_close_all_action_;
    QAction* pfile_dump_action_;
    QAction* pfile_goto_next_action_;
    QAction* pfile_goto_prev_action_;
    QAction* pedit_find_action_;
    QAction* pedit_search_action_;
    QAction* pedit_goto_search_results_action_;
    QAction* pedit_goto_documents_action_;
    QAction* pedit_font_action_;
    QAction* pedit_comment_action_;
    QAction* pview_file_explorer_action_;
    QAction* pview_close_docks_action_;
    QAction* prun_run_action_;
    QAction* prun_stop_action_;
    QAction* phelp_about_action_;
    QLabel* pstatus_text_;
    MainTabWidget* pmain_tabwidget_;
    FindDialog* pfind_dlg_;
    FileExplorerWidget* pfile_explorer_widget_;
    OutputWidget* poutput_widget_;
    LuaExecutor* plua_executor_;
    QDockWidget* pleft_widget_;
    QDockWidget* pbottom_widget_;
    QTabWidget* pbottom_tab_widget_;
    SearchResultsWidget* psearch_results_widget_;
private:
    DISALLOW_COPY_AND_ASSIGN(MainWindow)
};

} // namespace gui

#endif // MAINWINDOW_H
