#include "luaexecutor.h"
#include "util/file.hpp"
#include <QMessageBox>

using namespace std;
using namespace util;

LuaExecutor::LuaExecutor(QObject* parent) :
    QObject(parent)
{
#ifdef __APPLE__
    qprocess_ = new QProcess(parent);
    connect(qprocess_, &QProcess::readyReadStandardOutput, this, [this]()
    {
       QByteArray data = qprocess_->readAllStandardOutput();
       emit sendOutput(StdStringToQString(data.toStdString()));
    });

    connect(qprocess_, &QProcess::readyReadStandardError, this, [this]()
    {
       QByteArray data = qprocess_->readAllStandardError();
       emit sendOutput(StdStringToQString(data.toStdString()));
    });
#endif
}

LuaExecutor::~LuaExecutor()
{
    //dtor
}

bool LuaExecutor::isRunning()
{
#ifdef __APPLE__
    return qprocess_->state() == QProcess::Running;
#else
    return process_.isRunning();
#endif
}

void LuaExecutor::stop()
{
#ifdef __APPLE__
    qprocess_->kill();
#else
    process_.kill();
#endif
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

//    std::string exec = ("" == executor) ? getScriptExecutor(file) : executor;
    std::string exec = executor;
    if ("" == exec)
        return false;

    std::string script = std::string("\"") + file + std::string("\"");
#ifdef __APPLE__
    qprocess_->setWorkingDirectory(StdStringToQString(path));
    qprocess_->start(StdStringToQString(exec + " " + script + " " + args), QProcess::ReadWrite);
    return true;
#else
    return process_.create(exec + " " + script + " " + args,
                           path,
                           true,
                           true,
                           UtilBind(&LuaExecutor::output, this, _1));
#endif
}

void LuaExecutor::output(const std::string& str)
{
    emit sendOutput(StdStringToQString(str));
}

void LuaExecutor::input(const QString& in)
{
#ifdef __APPLE__
    if (isRunning())
        qprocess_->write(QStringToStdString(in).c_str());
#else
    process_.input(QStringToStdString(in));
#endif
}
