#include "lunarcommon.h"
#include <QMessageBox>
#include <vector>
#include "util/file.hpp"
#include "util/cfg.hpp"
#include "util/process.hpp"

using namespace std;
using namespace util;

static LogSocket s_logger;

void InitLunarCommon(int argc, char* argv[])
{
    setPrintFunc(UtilBind(&LogSocket::SendLog, &s_logger, _1));
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

LogSocket::~LogSocket()
{

}

void LogSocket::SendLog(const std::string& log)
{
    log_sock_.sendTo(log.c_str(), log.length(), "127.0.0.1", 9966);
}

////////////////////////////////////////////////////
// class name : LunarGlobal
// description :
// author :
// time : 2012-01-19-09.19
////////////////////////////////////////////////////
const string ks_cfg = "cfg";

LunarGlobal::LunarGlobal()
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
        string str = "function parseFileType(filename)\n\nend";
        writeTextFile(getAppPath() + "/" + getExtensionFile(), str);
    }
}

void LunarGlobal::quit()
{
    writeCfg();
}

void LunarGlobal::readCfg()
{
    TextCfg text_cfg(getAppPath() + "/" + ks_cfg);

    autocompletion_threshold_ = text_cfg.getValue("AutoCompletion.Threshold", 2);
    autocompletion_wordtip_ = text_cfg.getValue("AutoCompletion.WordTip", 1);
    string font_type = text_cfg.getValue("Font.Type", string("Courier New"));
    int font_size = text_cfg.getValue("Font.Size", 10);
    font_ = QFont(StdStringToQString(font_type), font_size);
    mainwindow_width_ = text_cfg.getValue("MainWindow.Width", 800);
    mainwindow_height_ = text_cfg.getValue("MainWindow.Height", 600);
    runner_lua_ = text_cfg.getValue("Run.Runner.Lua", "luaexec");
    runner_octave_ = text_cfg.getValue("Run.Runner.Octave", "octave");
    run_additional_args_ = text_cfg.getValue("Run.Additional.Args", "");
    lua_file_filter_ = text_cfg.getValue("FileFilter.Lua", "lua");
    octave_file_filter_ = text_cfg.getValue("FileFilter.Octave", "m");
    file_type_default_ = text_cfg.getValue("FileFilter.DefaultType", kFileTypeLua);
    lua_api_ = text_cfg.getValue("Api.Lua", "api/lua");
    octave_api_ = text_cfg.getValue("Api.Octave", "api/octave");
    extension_func_parsefiletype_ = text_cfg.getValue("Extension.Func.ParseFileType", "parseFileType");
}

void LunarGlobal::writeCfg()
{
    TextCfg text_cfg(getAppPath() + "/" + ks_cfg);

    text_cfg.setValue("Font.Type", QStringToStdString(getFont().family()));
    text_cfg.setValue("Font.Size", getFont().pointSize());
    text_cfg.setValue("AutoCompletion.Threshold", autocompletion_threshold_);
    text_cfg.setValue("AutoCompletion.WordTip", autocompletion_wordtip_);
    text_cfg.setValue("MainWindow.Width", mainwindow_width_);
    text_cfg.setValue("MainWindow.Height", mainwindow_height_);
    text_cfg.setValue("FileFilter.Lua", lua_file_filter_);
    text_cfg.setValue("FileFilter.Octave", octave_file_filter_);
    text_cfg.setValue("FileFilter.DefaultType", file_type_default_);
    text_cfg.setValue("Run.Runner.Lua", runner_lua_);
    text_cfg.setValue("Run.Runner.Octave", runner_octave_);
    text_cfg.setValue("Run.Additional.Args", run_additional_args_);
    text_cfg.setValue("Api.Lua", lua_api_);
    text_cfg.setValue("Api.Octave", octave_api_);
    text_cfg.setValue("Extension.Func.ParseFileType", extension_func_parsefiletype_);

    text_cfg.save();
}
