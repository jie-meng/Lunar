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
    SINGLETON(LunarGlobal)
    void init(int argc, char* argv[]);
    void quit();
    void readCfg();
    void writeCfg();
    inline std::string getAppPath() const { return app_path_; }
    inline std::string getAppName() const { return app_name_; }
    inline int getArgCnt() const { return argc_; }
    inline std::string getArg(size_t index) { return argvec_.at(index); }
    inline int getAutocompletionThreshold() const { return autocompletion_threshold_; }
    inline int getAutocompletionWordtip() const { return autocompletion_wordtip_; }
    inline QFont getFont() const { return font_; }
    inline void setFont(const QFont& font) { font_ = font; }
    inline unsigned short getProcessSockPort() const { return process_sock_port_; }
    inline void setProcessSockPort(unsigned short port) { process_sock_port_ = port; }
    inline size_t getMainwindowWidth() const { return mainwindow_width_; }
    inline size_t getMainwindowHeight() const { return mainwindow_height_; }
    inline void setMainwindowWidth(size_t width) { mainwindow_width_ = width; }
    inline void setMainwindowHeight(size_t height) { mainwindow_height_ = height; }
    inline std::string getRunAdditionalArgs() const { return run_additional_args_; }
    inline void setRunAdditionalArgs(const std::string& args) { run_additional_args_ = args; }
    inline std::string getExtensionFile() const { return "extension"; }
    inline std::string getExtensionFuncParseFileType() const { return extension_func_parsefiletype_; }
    inline std::string getExtensionFuncFilefilter() const { return extension_func_filefilter_; }
    inline std::string getFileFilter() const { return file_filter_; }
    void parseExtensionFileFilter();
private:
    LunarGlobal();
    ~LunarGlobal();
private:
    int argc_;
    std::vector<std::string> argvec_;
    std::string app_path_;
    std::string app_name_;
    int autocompletion_threshold_;
    int autocompletion_wordtip_;
    QFont font_;
    unsigned short process_sock_port_;
    size_t mainwindow_width_;
    size_t mainwindow_height_;
    std::string run_additional_args_;
    std::string extension_func_parsefiletype_;
    std::string extension_func_filefilter_;
    std::string file_filter_;
private:
    DISALLOW_COPY_AND_ASSIGN(LunarGlobal)
};


#endif // LUNARCOMMON_H
