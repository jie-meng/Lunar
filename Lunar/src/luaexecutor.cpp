#include "luaexecutor.h"
#include "util/file.hpp"
#include <QMessageBox>

using namespace std;
using namespace util;

LuaExecutor::LuaExecutor(QObject* parent) :
    QObject(parent)
{
    //ctor
}

LuaExecutor::~LuaExecutor()
{
    //dtor
}

bool LuaExecutor::isRunning()
{
    return process_.isRunning();
}

void LuaExecutor::stop()
{
    process_.kill();
}

std::string LuaExecutor::getScriptExecutor(const std::string& file)
{
    if (isFileInFileFilter(file, LunarGlobal::getLuaFileFilter()))
        return LunarGlobal::getRunnerLua();
    else if (isFileInFileFilter(file, LunarGlobal::getOctaveFileFilter()))
        return LunarGlobal::getRunnerOctave();
    else
        return "";
}

bool LuaExecutor::isFileInFileFilter(const std::string& file, const std::string& file_filter)
{
    std::vector<std::string> filterVec;
    util::strSplit(file_filter, ",", filterVec);
    for (std::vector<std::string>::iterator it = filterVec.begin(); it != filterVec.end(); ++it)
    {
        if (util::strEndWith(file, std::string(".") + *it, false))
            return true;
    }

    return false;
}

bool LuaExecutor::execute(const std::string& file, const std::string& args, const std::string& path)
{
    if ("" == file)
        return false;

    std::string executor = getScriptExecutor(file);
    if ("" == executor)
        return false;


#ifdef _PLATFORM_WINDOWS_
    std::string script = std::string("\"") + file + std::string("\"");
#endif
#ifdef _PLATFORM_LINUX_
    std::string script = file;
#endif
    QMessageBox::information(NULL, tr("Path"), StdStringToQString(executor + " " + script + " " + args));
    QMessageBox::information(NULL, tr("Path"), StdStringToQString(path));

    return process_.create(executor + " " + script + " " + args,
                           path,
                           true,
                           true,
                           UtilBind(&LuaExecutor::output, this, _1));
}

bool LuaExecutor::executeInSysCmd(const std::string& file, const std::string& args, const std::string& path)
{
    if ("" == file)
        return false;

    std::string executor = getScriptExecutor(file);
    if ("" == executor)
        return false;

#ifdef _PLATFORM_WINDOWS_
    std::string script = std::string("\"") + file + std::string("\"");
#endif
#ifdef _PLATFORM_LINUX_
    std::string script = file;
#endif

    if (path == currentPath())
    {
        ::system((executor + " " + script + " " + args).c_str());
    }
    else
    {
        string oldPath = currentPath();
        setCurrentPath(path);
        ::system((executor + " " + script + " " + args).c_str());
        setCurrentPath(oldPath);
    }

    return true;
}

void LuaExecutor::output(const std::string& str)
{
    emit sendOutput(StdStringToQString(str));
}

void LuaExecutor::input(const QString& in)
{
    process_.input(QStringToStdString(in));
}
