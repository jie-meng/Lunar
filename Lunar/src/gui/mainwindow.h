#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
//#include "processmsgthread.h"
#include "util/base.hpp"

class QAction;
class QLabel;
class QGraphicsView;
class MsgRecvThread;
class LuaExecutor;

namespace gui
{

class MainTabWidget;
class FindDialog;
class OutputWidget;
class DockWidgetEx;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();
    bool Init();
    static std::string get_file_filter() { return s_file_filter_; }
public Q_SLOTS:
    void OpenDoc(const QString& filepath);
    //output
    void AddOutput(const QString& output);
    void SendInput(const QString& input);
    void DumpOutput();
    void ClearOutput();
private Q_SLOTS:
    void FileNew();
    void FileOpen();
    void FileSave();
    void FileSaveAs();
    void FileSaveAll();
    void FileClose();
    void FileDump();
    void FileGotoNext();
    void FileGotoPrev();
    void EditFind();
    void EditSetFont();
    void HelpAbout();
    bool Find(const QString& str, bool first_find, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool wrap, bool find_in_output);
    void Replace(const QString&, bool find_in_output);
    void ReplaceAll(const QString& str, const QString& replace_with_text, Qt::CaseSensitivity cs, bool find_previous, bool whole_word, bool find_in_output);
    void Run();
    void RunInSysCmd();
    void Stop();
    void SetStatusText(const QString& text);
    void OnBottomDockClose();
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void closeEvent(QCloseEvent* e);
private:
    void InitActions();
    void InitMenubar();
    void InitToolbar();
    void InitStatusBar();
    void InitMainWidget();
    void InitConnections();
    void InitFindDialog();
    void InitBottomDockWidget();
    void InitLuaExecutor();
    void InitRunner();
    void ProcessCmdParam();
    void RunEx(bool run_in_syscmd);
    std::string FormatFileFilter(const std::string& file_filter);
    QAction* pfile_new_action_;
    QAction* pfile_open_action_;
    QAction* pfile_save_action_;
    QAction* pfile_save_as_action_;
    QAction* pfile_save_all_action_;
    QAction* pfile_close_action_;
    QAction* pfile_dump_action_;
    QAction* pfile_goto_next_action_;
    QAction* pfile_goto_prev_action_;
    QAction* pedit_find_action_;
    QAction* pedit_font_action_;
    QAction* prun_run_action_;
    QAction* prun_run_syscmd_action_;
    QAction* prun_stop_action_;
    QAction* phelp_about_action_;
    QLabel* pstatus_text_;
    MainTabWidget* pmain_tabwidget_;
    FindDialog* pfind_dlg_;
    OutputWidget* poutput_widget_;
    LuaExecutor* plua_executor_;
    static std::string s_file_filter_;
//    MsgRecvThread msg_recv_thread_;
    DockWidgetEx* pbottom_widget_;
    bool output_widget_on_;
private:
    DISALLOW_COPY_AND_ASSIGN(MainWindow)
};

} // namespace gui

#endif // MAINWINDOW_H
