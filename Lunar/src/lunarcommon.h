#ifndef LUNAR_H
#define LUNAR_H

#include <QtCore/QString>
#include <QtGui/QFont>
#include <vector>
#include "util/net.hpp"
#include "processmsgthread.h"

inline QString StdStringToQString(const std::string &s)
{
    return QString(QString::fromLocal8Bit(s.c_str()));
    //return QString::fromStdString(s);
}

inline std::string QStringToStdString(const QString &s)
{
    return std::string((const char *)s.toLocal8Bit());
    //return s.toStdString();
}

void InitLunarCommon(int argc, char* argv[]);
void LunarMsgBox(const std::string& str);
void LunarMsgBoxQ(const QString& str);

////////////////////////////////////////////////////
// class name : LogSocket
// description :
// author :
// time : 2012-01-17-10.47
////////////////////////////////////////////////////
class LogSocket
{
public:
    LogSocket();
    ~LogSocket();
    void SendLog(const std::string& log);
private:
    util::DgramSocket log_sock_;
private:
    DISALLOW_COPY_AND_ASSIGN(LogSocket)
};

const std::string kFileTypeLua = "lua";
const std::string kFileTypeOctave = "octave";

////////////////////////////////////////////////////
// class name : LunarGlobal
// description :
// author :
// time : 2012-01-19-09.16
////////////////////////////////////////////////////
class LunarGlobal
{
public:
    LunarGlobal();
    ~LunarGlobal();
    void static ReadCfg();
    void static WriteCfg();
    static const std::string ks_cfg;
    static void Init(int argc, char* argv[]);
    static void Quit();

    inline static std::string get_app_path() { return s_app_path_; }
    inline static std::string get_app_name() { return s_app_name_; }
    inline static int get_arg_cnt() { return s_argc_; }
    inline static std::string get_arg(size_t index) { return s_argvec_.at(index); }
    inline static int get_autocompletion_threshold() { return s_autocompletion_threshold_; }
    inline static int get_autocompletion_wordtip() { return s_autocompletion_wordtip_; }
    inline static QFont get_font() { return s_font_; }
    inline static void set_font(const QFont& font) { s_font_ = font; }
    inline static unsigned short get_process_sock_port() { return s_process_sock_port_; }
    inline static void set_process_sock_port(unsigned short port) { s_process_sock_port_ = port; }
    inline static size_t get_mainwindow_width() { return s_mainwindow_width_; }
    inline static size_t get_mainwindow_height() { return s_mainwindow_height_; }
    inline static void set_mainwindow_width(size_t width) { s_mainwindow_width_ = width; }
    inline static void set_mainwindow_height(size_t height) { s_mainwindow_height_ = height; }
    inline static std::string getLuaFileFilter() { return s_lua_file_filter_; }
    inline static std::string getRunnerLua() { return s_runner_lua_; }
    inline static std::string getRunnerOctave() { return s_runner_octave_; }
    inline static std::string getRunAdditionalArgs() { return s_run_additional_args_; }
    inline static void setRunAdditionalArgs(const std::string& args) { s_run_additional_args_ = args; }
    inline static std::string getOctaveFileFilter() { return s_octave_file_filter_; }
    inline static std::string getLuaApi() { return s_lua_api_; }
    inline static std::string getOctaveApi() { return s_octave_api_; }
    inline static std::string getFileTypeDefault() { return s_file_type_default_; }
private:
    static int s_argc_;
    static std::vector<std::string> s_argvec_;
    static std::string s_app_path_;
    static std::string s_app_name_;
    static int s_autocompletion_threshold_;
    static int s_autocompletion_wordtip_;
    static QFont s_font_;
    static unsigned short s_process_sock_port_;
    static size_t s_mainwindow_width_;
    static size_t s_mainwindow_height_;
    static std::string s_lua_file_filter_;
    static std::string s_octave_file_filter_;
    static std::string s_runner_lua_;
    static std::string s_runner_octave_;
    static std::string s_run_additional_args_;
    static std::string s_lua_api_;
    static std::string s_octave_api_;
    static std::string s_file_type_default_;
private:
    DISALLOW_COPY_AND_ASSIGN(LunarGlobal)
};


#endif // LUNARCOMMON_H
