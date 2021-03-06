#ifndef OPEN_FILE_DIALOG_H
#define OPEN_FILE_DIALOG_H

#include <vector>
#include <QDialog>
#include <QThread>
#include "util/luaextend.hpp"

class QLineEdit;

namespace gui
{

class FindFileThread : public QThread
{
    Q_OBJECT
public:
    FindFileThread(QObject *parent = 0);
    virtual ~FindFileThread();
    void start(const QString& find_with_text);
    inline void setStopFlag(int flag) { stop_flag_ = flag; }
signals:
    void found(const QStringList&);
protected:
    virtual void run();
private:
    bool initLuaState();
    void findFiles(const std::string& find_with_text);
private:
    QString find_with_text_;
    util::LuaState lua_state_;
    bool lua_state_ok_;
    std::string error_information_;
    std::vector<std::string> files_found_;
    int stop_flag_;
private:
    DISALLOW_COPY_AND_ASSIGN(FindFileThread)
};


class TreeView;

class FindFileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindFileDialog(QWidget *parent = 0);
    virtual ~FindFileDialog();
signals:
    void selectDoc(const QString&);
public Q_SLOTS:
    void findFinish();
    void startFinding(const QString& findWithText);
    bool checkPending();
private:
    void init();
    void initConnections();
    void initGui();
private:
    QLineEdit* pfile_name_;
    TreeView* ptree_view_;
    FindFileThread find_file_thread_;
    QString pending_find_text_;
private:
    DISALLOW_COPY_AND_ASSIGN(FindFileDialog)
};

} // namespace gui


#endif // OPEN_FILE_DIALOG_H
