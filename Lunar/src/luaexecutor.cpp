#include "luaexecutor.h"
#include "util/file.hpp"
#include <QMessageBox>

using namespace std;
using namespace util;

LuaExecutor::LuaExecutor(QObject* parent) :
    QObject(parent),
    run_path_("")
{
    //ctor
}

LuaExecutor::~LuaExecutor()
{
    //dtor
}

QString LuaExecutor::runPathQ()
{
    return run_path_;
}

std::string LuaExecutor::runPath()
{
    return QStringToStdString(run_path_);
}

bool LuaExecutor::setExecutor(const QString& executor_path)
{
    executor_path_ = "";

    if(!isPathFile(QStringToStdString(executor_path)))
        return false;

    executor_path_ = executor_path;
    return true;
}

void LuaExecutor::setRunPath(const QString& runPath)
{
    run_path_ = runPath;
}

bool LuaExecutor::isRunning()
{
    return process_.isRunning();
}

void LuaExecutor::stop()
{
    process_.kill();
}

bool LuaExecutor::executeQ(const QString& args, const QString& path)
{
    if ("" == executor_path_ || "" == args)
        return false;

    return process_.create(QStringToStdString(executor_path_ + " " + args),
                           QStringToStdString(path),
                           true,
                           true,
                           UtilBind(&LuaExecutor::output, this, _1));
}

bool LuaExecutor::execute(const std::string& args, const std::string& path)
{
    if ("" == executor_path_ || "" == args)
        return false;

    return process_.create(QStringToStdString(executor_path_) + " " + args,
                           path,
                           true,
                           true,
                           UtilBind(&LuaExecutor::output, this, _1));
}

bool LuaExecutor::executeInSysCmd(const std::string& args, const std::string& path)
{
    if ("" == executor_path_ || "" == args)
        return false;

    if (path == currentPath())
    {
        ::system((QStringToStdString(executor_path_) + " " + args).c_str());
    }
    else
    {
        string oldPath = currentPath();
        setCurrentPath(path);
        ::system((QStringToStdString(executor_path_) + " " + args).c_str());
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
