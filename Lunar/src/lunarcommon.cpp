#include "lunarcommon.h"
#include <QMessageBox>
#include <vector>
#include "util/file.hpp"
#include "util/cfg.hpp"
#include "util/process.hpp"
#include "extension.h"

using namespace std;
using namespace util;

//static LogSocket s_logger;

void InitLunarCommon(int argc, char* argv[])
{
    //setPrintFunc(UtilBind(&LogSocket::sendLog, &s_logger, _1));
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
    string message = luaGetString(plua_state, 1, "");
    LunarMsgBox(message);
    return 0;
}

int sendLog(lua_State* plua_state)
{
    string log = luaGetString(plua_state, 1, "");
    string ip = luaGetString(plua_state, 2, "127.0.0.1");
    unsigned short port = (unsigned short)luaGetInteger(plua_state, 3, 9966);
    LogSocket::getInstance().sendLog(log, ip, port);
    return 0;
}

////////////////////////////////////////////////////
// class name : LogSocket
// description :
// author :
// time : 2012-01-17-10.47
////////////////////////////////////////////////////
LogSocket::LogSocket() :
    log_sock_(util::Family_IPv4)
{
}

void LogSocket::sendLog(const std::string& log, const std::string& ip, unsigned short port)
{
    if (LunarGlobal::getInstance().isLogEnable())
        log_sock_.sendTo(log.c_str(), log.length(), ip, port);
}

////////////////////////////////////////////////////
// class name : LunarGlobal
// description :
// author :
// time : 2012-01-19-09.19
////////////////////////////////////////////////////
const string kCfg = "cfg";

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

    if (!isPathFile(getAppPath() + "/" + getExtensionFile()))
    {
        string str =
"function parseFileType(filename)\n\
\n\
\tlocal path, name = file.splitPathname(filename)\n\
\n\
\t-- Lunar extension\n\
\tif name == \"extension.lua\" then\n\
\t\tif file.isPathFile(path .. \"/Lunar\") or file.isPathFile(path .. \"/Lunar.exe\") then\n\
\t\t\treturn { type = \"lua\", auto_complete_type = 1, api = \"apis/lua\" }\n\
\t\tend\n\
\tend\n\
\n\
\t-- lua\n\
\tif string.lower(file.fileExtension(name)) == \"lua\" then\n\
\t\treturn { type = \"lua\", auto_complete_type = 1, api = \"apis/lua\", executor = \"luaexec\" }\n\
\tend\n\
end\n\
\n\
function fileFilter()\n\
\tfilter = {}\n\
\ttable.insert(filter, \"Lua Files(*.lua)\")\n\
\treturn filter\n\
end\n\
\n\
function ignoreFile(filename)\n\
\tlocal ext = file.fileExtension(filename)\n\
\tif ext == \"so\" or\n\
\t\text == \"o\" or\n\
\t\text == \"lib\" or\n\
\t\text == \"dll\" or\n\
\t\text == \"obj\" or\n\
\t\text == \"exe\" or\n\
\t\text == \"exp\" or\n\
\t\text == \"bin\" then\n\
\t\treturn true\n\
\tend\n\
\n\
\treturn false\n\
end\n";

        writeTextFile(getAppPath() + "/" + getExtensionFile(), str);
    }
}

void LunarGlobal::parseExtensionFileFilter()
{
    string str = Extension::getInstance().fileFilter();
    if ("" != str)
        file_filter_ = str;
    file_filter_ += "All Files(*.*)";
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
    string font_type = text_cfg.getValue("Font.Type", string("Consolas"));
    int font_size = text_cfg.getValue("Font.Size", 10);
    font_ = QFont(StdStringToQString(font_type), font_size);
    mainwindow_width_ = text_cfg.getValue("MainWindow.Width", 800);
    mainwindow_height_ = text_cfg.getValue("MainWindow.Height", 600);
    extension_func_parsefiletype_ = text_cfg.getValue("Extension.Func.ParseFileType", "parseFileType");
    extension_func_filefilter_ = text_cfg.getValue("Extension.Func.FileFilter", "fileFilter");
    extension_func_ignore_file_ = text_cfg.getValue("Extension.Func.IgnoreFile", "ignoreFile");
    log_sock_port_ = text_cfg.getValue("Log.SockPort", 9966);
    is_log_enable_ = text_cfg.getValue<bool>("Log.Enable", false);
    load_api_min_interval_ = text_cfg.getValue<size_t>("LoadApi.MinInterval", 5);
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
    text_cfg.setValue("Extension.Func.IgnoreFile", extension_func_ignore_file_);
    text_cfg.setValue("Log.SockPort", log_sock_port_);
    text_cfg.setValue("Log.Enable", is_log_enable_);
    text_cfg.setValue("LoadApi.MinInterval", load_api_min_interval_);

    text_cfg.save();
}
