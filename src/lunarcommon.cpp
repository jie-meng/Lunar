#include "lunarcommon.h"
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <algorithm>
#include "util/file.hpp"
#include "util/cfg.hpp"
#include "util/process.hpp"
#include "extension.h"

using namespace std;
using namespace util;

QString StdStringToQString(const std::string &s)
{
    return QString::fromStdString(s);
}

std::string QStringToStdString(const QString &s)
{
    return s.toStdString();
}

QString qtReadFile(const QString& filename, const char* codec)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&f);
    in.setCodec(codec);
    QString str;
    while(!in.atEnd())
        str.append(in.readLine() + "\n");

    return str;
}

bool qtWriteFile(const QString& filename, const QString& content, bool append, const char* codec)
{
    QFile::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (append)
        mode |= QIODevice::Append;

    QFile f(filename);
    if (!f.open(mode))
        return false;

    QTextStream out(&f);
    out.setCodec(codec);
    out<<content;
    out.flush();
    f.close();

    return true;
}

void initLunarCommon(int argc, char* argv[])
{
    LunarGlobal::getInstance().init(argc, argv);
}

void LunarMsgBox(const std::string& str)
{
    QMessageBox::information(NULL, QObject::tr("Message"), StdStringToQString(str));
}

void LunarMsgBoxQ(const QString& str)
{
    QMessageBox::information(NULL, QObject::tr("Message"), str);
}

int scriptMessage(lua_State* plua_state)
{
    string message = luaToString(plua_state, 1, "");
    LunarMsgBox(message);
    return 0;
}

int sendLog(lua_State* plua_state)
{
    string log = luaToString(plua_state, 1, "");
    string ip = luaToString(plua_state, 2, "127.0.0.1");
    unsigned short port = (unsigned short)luaToInteger(plua_state, 3, LunarGlobal::getInstance().getLogSockPort());
    LogSocket::getInstance().sendLog(log, ip, port);
    return 0;
}

QString qstrReplaceOnce(const QString& str_src, const QString& str_find, const QString& str_replace)
{
    QString ret_str = str_src;
    int pos = 0;

    if ((pos = ret_str.indexOf(str_find, 0)) >= 0)
    {
        ret_str.replace(pos, str_find.length(), str_replace);
        return ret_str;
    }
    return ret_str;
}

void trimInvalidPath(const list<string>& input, list<string>& output)
{
    for (list<string>::const_iterator it = input.begin(); it != input.end(); ++it)
    {
        if (isPathDir(*it))
            output.push_back(*it);
    }
}

void getRecentProjectPath(std::list<std::string>& out)
{
    string paths = LunarGlobal::getInstance().getRecentProjectPath();
    list<string> ls;
    strSplit(paths, ",", ls);
    for (list<string>::iterator it = ls.begin(); it != ls.end(); ++it)
    {
        if (isPathDir(*it))
            out.push_back(strTrim(strReplaceAll(*it, "\\", "/")));
    }
}

void addNewRecentProjectPath(const std::string& path)
{
    string s = strReplaceAll(path, "\\", "/");
    list<string> input;
    getRecentProjectPath(input);
    list<string> ls;
    trimInvalidPath(input, ls);

    list<string>::iterator it = find(ls.begin(), ls.end(), s);
    if (it != ls.end())
        ls.erase(it);

    ls.push_front(s);

    while (ls.size() > LunarGlobal::getInstance().getRecentProjectPathCnt())
        ls.pop_back();

    LunarGlobal::getInstance().setRecentProjectPath(strJoin(ls, ","));
}

void saveRecentProjectPath(const std::list<std::string>& paths)
{
    list<string> ls;
    trimInvalidPath(paths, ls);
    LunarGlobal::getInstance().setRecentProjectPath(strJoin(ls, ","));
}

LogSocket::LogSocket() :
    log_sock_(util::Family_IPv4)
{
}

void LogSocket::sendLog(const std::string& log, const std::string& ip, unsigned short port)
{
    if (LunarGlobal::getInstance().isLogEnable())
        log_sock_.sendTo(log.c_str(), log.length(), ip, port);
}

const string kCfg = "lunar.cfg";

LunarGlobal::LunarGlobal() :
    file_filter_("Lua Files(*.lua);;")
{
}

LunarGlobal::~LunarGlobal()
{
}

void LunarGlobal::init(int argc, char* argv[])
{
    //input args process
    argc_ = argc;
    for (int i=0; i<argc; i++)
        argvec_.push_back(string(argv[i]));

    string path_name = appPath();
    app_path_ = splitPathname(path_name).first;
    app_name_ = splitPathname(path_name).second;

    readCfg();
    writeCfg();
}

void LunarGlobal::parseExtensionFileFilter()
{
    string str = Extension::getInstance().fileFilter();
    if ("" != str)
        file_filter_ = str;
    file_filter_ += "All Files(*.*)";
}

void LunarGlobal::addRecentDoc(const std::string& doc)
{
    auto it = find(recent_docs_.begin(), recent_docs_.end(), doc);
    if (it != recent_docs_.end())
        recent_docs_.erase(it);

    recent_docs_.push_front(doc);

    if (recent_docs_.size() > 64)
        recent_docs_.pop_back();
}

void LunarGlobal::trimRecentDocs()
{
    recent_docs_.erase(remove_if(recent_docs_.begin(), recent_docs_.end(), [](string& file) { return !isPathFile(file); }), recent_docs_.end());
}

void LunarGlobal::removeRecentDoc(const std::string& doc)
{
    recent_docs_.remove(doc);
}

void LunarGlobal::addRecentRunDoc(const std::string& doc)
{
    auto it = find(recent_run_docs_.begin(), recent_run_docs_.end(), doc);
    if (it != recent_run_docs_.end())
        recent_run_docs_.erase(it);

    recent_run_docs_.push_front(doc);

    if (recent_run_docs_.size() > 32)
        recent_run_docs_.pop_back();
}

void LunarGlobal::trimRecentRunDoc()
{
    recent_run_docs_.erase(remove_if(recent_run_docs_.begin(), recent_run_docs_.end(), [](string& file) { return !isPathFile(file); }), recent_run_docs_.end());
}

void LunarGlobal::removeRecentRunDoc(const std::string& doc)
{
    recent_run_docs_.remove(doc);
}

void LunarGlobal::quit()
{
    writeCfg();
}

void LunarGlobal::readCfg()
{
    TextCfg text_cfg(getAppPath() + "/" + kCfg);

    autocompletion_threshold_ = text_cfg.getValue("AutoCompletion.Threshold", 2);
    autocompletion_wordtip_ = text_cfg.getValue("AutoCompletion.WordTip", 1);
    string font_type = text_cfg.getValue("Font.Type", string("Monaco"));
    int font_size = text_cfg.getValue("Font.Size", 12);
    font_ = QFont(StdStringToQString(font_type), font_size);
    mainwindow_width_ = text_cfg.getValue("MainWindow.Width", 800);
    mainwindow_height_ = text_cfg.getValue("MainWindow.Height", 600);
    extension_func_parsefiletype_ = text_cfg.getValue("Extension.Func.ParseFileType", "parseFileType");
    extension_func_filefilter_ = text_cfg.getValue("Extension.Func.FileFilter", "fileFilter");
    extension_tools_path_ = text_cfg.getValue("ExtensionToolsPath", "extension_tools");
    extension_func_is_legal_file_ = text_cfg.getValue("Extension.Func.IsLegalFile", "isLegalFile");
    extension_func_find_files_ = text_cfg.getValue("Extension.Func.FindFiles", "findFiles");
    log_sock_port_ = text_cfg.getValue("Log.SockPort", 9966);
    is_log_enable_ = text_cfg.getValue<bool>("Log.Enable", false);
    recent_project_path_ = text_cfg.getValue("Path.RecentProject", "");
    recent_project_path_cnt_ = text_cfg.getValue("Path.RecentProject.Count", 30);
}

void LunarGlobal::writeCfg()
{
    TextCfg text_cfg(getAppPath() + "/" + kCfg);

    text_cfg.setValue("Font.Type", QStringToStdString(getFont().family()));
    text_cfg.setValue("Font.Size", getFont().pointSize());
    text_cfg.setValue("AutoCompletion.Threshold", autocompletion_threshold_);
    text_cfg.setValue("AutoCompletion.WordTip", autocompletion_wordtip_);
    text_cfg.setValue("MainWindow.Width", mainwindow_width_);
    text_cfg.setValue("MainWindow.Height", mainwindow_height_);
    text_cfg.setValue("Extension.Func.ParseFileType", extension_func_parsefiletype_);
    text_cfg.setValue("Extension.Func.FileFilter", extension_func_filefilter_);
    text_cfg.setValue("Extension.Func.IsLegalFile", extension_func_is_legal_file_);
    text_cfg.setValue("Extension.Func.FindFiles", extension_func_find_files_);
    text_cfg.setValue("ExtensionToolsPath", extension_tools_path_);
    text_cfg.setValue("Log.SockPort", log_sock_port_);
    text_cfg.setValue("Log.Enable", is_log_enable_);
    text_cfg.setValue("Path.RecentProject", recent_project_path_);
    text_cfg.setValue("Path.RecentProject.Count", recent_project_path_cnt_);

    text_cfg.save();
}
