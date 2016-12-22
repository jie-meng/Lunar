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
    bool openDoc(const QString& filepath);
    //output
    void addOutput(const QString& output);
    void sendInput(const QString& input);
    void dumpOutput();
    void clearOutput();
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void closeEvent(QCloseEvent* e);
private Q_SLOTS:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSaveAll();
    void fileClose();
    void fileCloseAll();
    void fileGotoNext();
    void fileGotoPrev();
    void resetCurrentPath();
    void resetCurrentPath(const QString& path);
    void recentProjectPath();
    void editSelectCursorWord();
    void editFind();
    void editSearch();
    void editSetFont();
    void editComment();
    void editCommentBlock();
    void editGotoDefinition();
    void editJumpBack();
    void editJumpForward();
    void viewFileExplorer();
    void viewSearchResultsWidget();
    void viewDocuments();
    void viewCloseDocks();
    void helpAbout();
    bool find(const QString& str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output);
    void replace(const QString&, bool find_in_output);
    void replaceAll(const QString& str, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output);
    void run();
    void executeScriptInPath(const QString& script, const QString& execute_path, const QString& additional_args);
    void stop();
    void searchTextInPath(const QString& dir,
                          const QString& text,
                          const QString& file_filter,
                          bool case_sensitive,
                          bool use_regex);
    void gotoSearchResult(const QString& file, int line);
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
    bool gotoPosition(const std::string& file, int line);
    std::pair<std::string, int> getCurrentPosition();
    QAction* pfile_new_action_;
    QAction* pfile_open_action_;
    QAction* pfile_save_action_;
    QAction* pfile_save_as_action_;
    QAction* pfile_save_all_action_;
    QAction* pfile_close_action_;
    QAction* pfile_close_all_action_;
    QAction* pfile_goto_next_action_;
    QAction* pfile_goto_prev_action_;
    QAction* pfile_recent_project_path_;
    QAction* pedit_select_cursor_word_action_;
    QAction* pedit_find_action_;
    QAction* pedit_search_action_;
    QAction* pedit_font_action_;
    QAction* pedit_comment_action_;
    QAction* pedit_comment_block_action_;
    QAction* pedit_goto_definition_action_;
    QAction* pedit_jump_back_action_;
    QAction* pedit_jump_forward_action_;
    QAction* pview_file_explorer_action_;
    QAction* pview_search_results_action_;
    QAction* pview_documents_action_;
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
