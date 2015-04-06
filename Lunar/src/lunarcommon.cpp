#include "lunarcommon.h"
#include <QMessageBox>
#include <vector>
#include "util/file.hpp"
#include "util/cfg.hpp"
#include "util/process.hpp"
#include "extension.h"

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
\tif name == \"extension\" then\n\
\t\tif file.isPathFile(path .. \"/Lunar\") or file.isPathFile(path .. \"/Lunar.exe\") then\n\
\t\t\treturn \"lua\", \"api/lua\"\n\
\t\tend\n\
\tend\n\
\n\
\t-- lua\n\
\tif string.lower(file.fileExtension(name)) == \"lua\" then\n\
\t\treturn \"lua\", \"api/lua\", \"luaexec\"\n\
\tend\n\
end\n\
\n\
function fileFilter()\n\
\treturn \"Lua Files(*.lua);;\"\n\
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
    string font_type = text_cfg.getValue("Font.Type", string("Courier New"));
    int font_size = text_cfg.getValue("Font.Size", 10);
    font_ = QFont(StdStringToQString(font_type), font_size);
    mainwindow_width_ = text_cfg.getValue("MainWindow.Width", 800);
    mainwindow_height_ = text_cfg.getValue("MainWindow.Height", 600);
    run_additional_args_ = text_cfg.getValue("Run.Additional.Args", "");
    extension_func_parsefiletype_ = text_cfg.getValue("Extension.Func.ParseFileType", "parseFileType");
    extension_func_filefilter_ = text_cfg.getValue("Extension.Func.FileFilter", "fileFilter");
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
    text_cfg.setValue("Run.Additional.Args", run_additional_args_);
    text_cfg.setValue("Extension.Func.ParseFileType", extension_func_parsefiletype_);
    text_cfg.setValue("Extension.Func.FileFilter", extension_func_filefilter_);

    text_cfg.save();
}
