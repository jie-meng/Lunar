#include "FindFiledialog.h"
#include <list>
#include <QVBoxLayout>
#include <QLineEdit>
#include "util/file.hpp"
#include "util/thread.hpp"
#include "treeview.h"
#include "extension.h"
#include "lunarcommon.h"
#include <QString>

using namespace std;
using namespace util;

namespace gui {

FindFileThread::FindFileThread(QObject *parent) : 
    QThread(parent),
    lua_state_ok_(false)
{
}

void FindFileThread::start(const QString& find_with_text)
{
    find_with_text_ = find_with_text;
    QThread::start();
}

FindFileThread::~FindFileThread()
{
}

bool FindFileThread::initLuaState()
{
    if (lua_state_ok_)
        return true;

    if (!isPathFile(LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionFile()))
    {
        error_information_ =
            strFormat("Extension: extension file %s not exist", (LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionFile()).c_str());
        lua_state_ok_ = false;
        return false;
    }

    openUtilExtendLibs(lua_state_.getState());

    int err = lua_state_.parseFile(LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionFile());
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        lua_state_ok_ = false;
    }
    else
    {
        lua_state_.registerFunction("sendLog", sendLog);
        error_information_ = "";
        lua_state_ok_ = true;
    }

    return lua_state_ok_;
}

void FindFileThread::run()
{
    if (!initLuaState())
        return;
    
    findFiles(QStringToStdString(find_with_text_));
    if (!files_found_.empty())
    {
        for (auto file : files_found_)
        {
            auto path_name = splitPathname(file);
            QStringList qls;
            qls.append(StdStringToQString(path_name.second));
            qls.append(StdStringToQString(file));
            
            Q_EMIT found(qls);
        }
    }
}

void FindFileThread::findFiles(const string& find_with_text)
{
    files_found_.clear();
    
    if (!lua_state_ok_)
        return;
    
    error_information_ = "";
    
    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncFindFiles());
    luaPushString(lua_state_.getState(), find_with_text);
    
    int err = luaCallFunc(lua_state_.getState(), 1, 1);
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        LogSocket::getInstance().sendLog(error_information_, "127.0.0.1", LunarGlobal::getInstance().getLogSockPort());
    }
    else
    {
        int ret_cnt = luaGetTop(lua_state_.getState());
        if (ret_cnt > 0)
        {
            if (luaGetType(lua_state_.getState(), 1) == LuaTable)
            {   
                vector< pair<any, any> > vec = luaToArray(lua_state_.getState(), 1);
                vector< pair<any, any> >::iterator it;
                for (it=vec.begin(); it != vec.end(); ++it)
                    files_found_.push_back(it->second.toString());
            }   
        }
    }
    
    luaPop(lua_state_.getState(), -1);
}

//FindFileDialog
FindFileDialog::FindFileDialog(QWidget* parent) :
    QDialog(parent),
    pfile_name_(NULL),
    ptree_view_(NULL)
{
    init();
}

FindFileDialog::~FindFileDialog()
{
}

void FindFileDialog::init()
{
    initGui();
    initConnections();
    
    if (!pfile_name_->text().isEmpty())
        startFinding(pfile_name_->text());
}

void FindFileDialog::initGui()
{
    pfile_name_ = new QLineEdit(StdStringToQString(LunarGlobal::getInstance().getLastFindFileText()));
    pfile_name_->setSelection(0, LunarGlobal::getInstance().getLastFindFileText().length());
    
    QStringList header;
    header.append(tr("Name"));
    header.append(tr("Path"));
    ptree_view_ = new TreeView(header);

    QVBoxLayout* pcenter_layout = new QVBoxLayout;
    pcenter_layout->addWidget(pfile_name_);
    pcenter_layout->addWidget(ptree_view_);
    setLayout(pcenter_layout);

    setWindowTitle(tr("Find files"));

    ptree_view_->resizeColumnToContents(0);
    ptree_view_->resizeColumnToContents(1);
    setFixedWidth(ptree_view_->columnWidth(0) + ptree_view_->columnWidth(1));
    setFixedHeight(sizeHint().height());
}

void FindFileDialog::initConnections()
{   
    connect(pfile_name_, &QLineEdit::textChanged, [this] (const QString& text) { startFinding(text); });
    
    connect(pfile_name_, &QLineEdit::returnPressed, [this] () { ptree_view_->setFocus(); });
    
    connect(&find_file_thread_, SIGNAL(found(const QStringList&)), ptree_view_, SLOT(addItem(const QStringList&)));
    
    connect(&find_file_thread_, SIGNAL(finished()), this, SLOT(findFinish()));

    connect(ptree_view_, &TreeView::itemSelected, [this](const QStringList& item, int number)
    {
        Q_EMIT selectDoc(item.at(1));
        close();
    });
}

void FindFileDialog::findFinish()
{
    if (checkPending())
        return;
    
    ptree_view_->resizeColumnToContents(0);
    ptree_view_->resizeColumnToContents(1);
    setFixedWidth(ptree_view_->columnWidth(0) + ptree_view_->columnWidth(1));
    setFixedHeight(sizeHint().height());
}

void FindFileDialog::startFinding(const QString& findWithText)
{
    if (find_file_thread_.isRunning())
    {
        pending_find_text_ = findWithText;
        return;
    }

    ptree_view_->clear();
    if (!findWithText.isEmpty())
        find_file_thread_.start(findWithText);
    
    LunarGlobal::getInstance().setLastFindFileText(QStringToStdString(findWithText));    
}

bool FindFileDialog::checkPending()
{
    if (pending_find_text_.isEmpty())
        return false;
    
    startFinding(pending_find_text_);
    pending_find_text_ = "";
    return true;
}
    
} //namespace gui
