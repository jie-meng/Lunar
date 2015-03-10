#include "lunarcommon.h"
#include <QMessageBox>
#include <vector>
#include "util/file.hpp"
#include "util/cfg.hpp"

static LogSocket s_logger;

void InitLunarCommon(int argc, char* argv[])
{
    util::setPrintFunc(UtilBind(&LogSocket::SendLog, &s_logger, _1));
    LunarGlobal::Init(argc, argv);
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
int LunarGlobal::s_argc_ = 0;
std::vector<std::string> LunarGlobal::s_argvec_;
std::string LunarGlobal::s_app_path_ = "";
std::string LunarGlobal::s_app_name_ = "";
const std::string LunarGlobal::ks_cfg = "/cfg";
int LunarGlobal::s_autocompletion_threshold_ = 2;
int LunarGlobal::s_autocompletion_wordtip_ = 1;
QFont LunarGlobal::s_font_("Courier New", 10);
unsigned short LunarGlobal::s_process_sock_port_ = 0;
size_t LunarGlobal::s_mainwindow_width_ = 800;
size_t LunarGlobal::s_mainwindow_height_ = 600;
std::string LunarGlobal::s_lua_file_filter_ = "lua";
std::string LunarGlobal::s_octave_file_filter_ = "m";
std::string LunarGlobal::s_runner_lua_ = "luaexec";
std::string LunarGlobal::s_runner_octave_ = "octave";
std::string LunarGlobal::s_run_additional_args_ = "";
std::string LunarGlobal::s_lua_api_ = "/api/lua";
std::string LunarGlobal::s_octave_api_ = "/api/octave";
std::string LunarGlobal::s_file_type_default_ = kFileTypeLua;

LunarGlobal::LunarGlobal()
{

}

LunarGlobal::~LunarGlobal()
{

}

void LunarGlobal::Init(int argc, char* argv[])
{
    //input args process
    s_argc_ = argc;
    for (int i=0; i<argc; i++)
        s_argvec_.push_back(std::string(argv[i]));

    s_app_path_ = util::splitPathname(s_argvec_.at(0)).first;
    s_app_name_ = util::splitPathname(s_argvec_.at(0)).second;

    ReadCfg();
    WriteCfg();
}

void LunarGlobal::Quit()
{
    WriteCfg();
}

void LunarGlobal::ReadCfg()
{
    util::TextCfg text_cfg(LunarGlobal::get_app_path() + LunarGlobal::ks_cfg);

    s_autocompletion_threshold_ = text_cfg.getValue("AutoCompletion.Threshold", 2);
    s_autocompletion_wordtip_ = text_cfg.getValue("AutoCompletion.WordTip", 1);
    std::string font_type = text_cfg.getValue("Font.Type", std::string("Courier New"));
    int font_size = text_cfg.getValue("Font.Size", 10);
    s_font_ = QFont(StdStringToQString(font_type), font_size);
    s_mainwindow_width_ = text_cfg.getValue("MainWindow.Width", 800);
    s_mainwindow_height_ = text_cfg.getValue("MainWindow.Height", 600);
    s_runner_lua_ = text_cfg.getValue("Run.Runner.Lua", "luaexec");
    s_runner_octave_ = text_cfg.getValue("Run.Runner.Octave", "octave");
    s_run_additional_args_ = text_cfg.getValue("Run.Additional.Args", "");
    s_lua_file_filter_ = text_cfg.getValue("FileFilter.Lua", "lua");
    s_octave_file_filter_ = text_cfg.getValue("FileFilter.Octave", "m");
    s_file_type_default_ = text_cfg.getValue("FileFilter.DefaultType", kFileTypeLua);
    s_lua_api_ = text_cfg.getValue("Api.Lua", "/api/lua");
    s_octave_api_ = text_cfg.getValue("Api.Octave", "/api/octave");
}

void LunarGlobal::WriteCfg()
{
    util::TextCfg text_cfg(LunarGlobal::get_app_path() + LunarGlobal::ks_cfg);

    text_cfg.setValue("Font.Type", QStringToStdString(get_font().family()));
    text_cfg.setValue("Font.Size", get_font().pointSize());
    text_cfg.setValue("AutoCompletion.Threshold", s_autocompletion_threshold_);
    text_cfg.setValue("AutoCompletion.WordTip", s_autocompletion_wordtip_);
    text_cfg.setValue("MainWindow.Width", s_mainwindow_width_);
    text_cfg.setValue("MainWindow.Height", s_mainwindow_height_);
    text_cfg.setValue("FileFilter.Lua", s_lua_file_filter_);
    text_cfg.setValue("FileFilter.Octave", s_octave_file_filter_);
    text_cfg.setValue("FileFilter.DefaultType", s_file_type_default_);
    text_cfg.setValue("Run.Runner.Lua", s_runner_lua_);
    text_cfg.setValue("Run.Runner.Octave", s_runner_octave_);
    text_cfg.setValue("Run.Additional.Args", s_run_additional_args_);
    text_cfg.setValue("Api.Lua", s_lua_api_);
    text_cfg.setValue("Api.Octave", s_octave_api_);

    text_cfg.save();
}
