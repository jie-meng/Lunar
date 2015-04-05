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

bool LuaExecutor::execute(const std::string& file,
                          const std::string& args,
                          const std::string& path,
                          const std::string& executor)
{
    if ("" == file)
        return false;

    std::string exec = ("" == executor) ? getScriptExecutor(file) : executor;
    if ("" == exec)
        return false;

    std::string script = std::string("\"") + file + std::string("\"");

    return process_.create(exec + " " + script + " " + args,
                           path,
                           true,
                           true,
                           UtilBind(&LuaExecutor::output, this, _1));
}

bool LuaExecutor::executeInSysCmd(const std::string& file,
                                  const std::string& args,
                                  const std::string& path,
                                  const std::string& executor)
{
    if ("" == file)
        return false;

    std::string exec = ("" == executor) ? getScriptExecutor(file) : executor;
    if ("" == exec)
        return false;

    std::string script = std::string("\"") + file + std::string("\"");

    if (path == currentPath())
    {
        ::system((exec + " " + script + " " + args).c_str());
    }
    else
    {
        string oldPath = currentPath();
        setCurrentPath(path);
        ::system((exec + " " + script + " " + args).c_str());
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
