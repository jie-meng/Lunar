#ifndef LUNAR_H
#define LUNAR_H

#include <vector>
#include <QtCore/QString>
#include <QtGui/QFont>
#include "util/net.hpp"

inline QString StdStringToQString(const std::string &s)
{
    return QString(QString::fromLocal8Bit(s.c_str()));
}

inline std::string QStringToStdString(const QString &s)
{
    return std::string((const char *)s.toLocal8Bit());
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
    void static readCfg();
    void static writeCfg();
    static const std::string ks_cfg;
    static void init(int argc, char* argv[]);
    static void quit();

    inline static std::string getAppPath() { return s_app_path_; }
    inline static std::string getAppName() { return s_app_name_; }
    inline static int getArgCnt() { return s_argc_; }
    inline static std::string getArg(size_t index) { return s_argvec_.at(index); }
    inline static int getAutocompletionThreshold() { return s_autocompletion_threshold_; }
    inline static int getAutocompletionWordtip() { return s_autocompletion_wordtip_; }
    inline static QFont getFont() { return s_font_; }
    inline static void setFont(const QFont& font) { s_font_ = font; }
    inline static unsigned short getProcessSockPort() { return s_process_sock_port_; }
    inline static void setProcessSockPort(unsigned short port) { s_process_sock_port_ = port; }
    inline static size_t getMainwindowWidth() { return s_mainwindow_width_; }
    inline static size_t getMainwindowHeight() { return s_mainwindow_height_; }
    inline static void setMainwindowWidth(size_t width) { s_mainwindow_width_ = width; }
    inline static void setMainwindowHeight(size_t height) { s_mainwindow_height_ = height; }
    inline static std::string getLuaFileFilter() { return s_lua_file_filter_; }
    inline static std::string getRunnerLua() { return s_runner_lua_; }
    inline static std::string getRunnerOctave() { return s_runner_octave_; }
    inline static std::string getRunAdditionalArgs() { return s_run_additional_args_; }
    inline static void setRunAdditionalArgs(const std::string& args) { s_run_additional_args_ = args; }
    inline static std::string getOctaveFileFilter() { return s_octave_file_filter_; }
    inline static std::string getLuaApi() { return s_lua_api_; }
    inline static std::string getOctaveApi() { return s_octave_api_; }
    inline static std::string getFileTypeDefault() { return s_file_type_default_; }
    inline static std::string getExtensionFile() { return s_extension_file_; }
    inline static std::string getExtensionFunc() { return s_extension_func_; }
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
    static std::string s_extension_file_;
    static std::string s_extension_func_;
private:
    DISALLOW_COPY_AND_ASSIGN(LunarGlobal)
};


#endif // LUNARCOMMON_H
