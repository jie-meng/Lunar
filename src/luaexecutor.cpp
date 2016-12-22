#include "luaexecutor.h"
#include "util/file.hpp"
#include <QMessageBox>

using namespace std;
using namespace util;

LuaExecutor::LuaExecutor(QObject* parent) :
    QObject(parent)
{
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
}

LuaExecutor::~LuaExecutor()
{
}

bool LuaExecutor::isRunning()
{
    return qprocess_->state() == QProcess::Running;
}

void LuaExecutor::stop()
{
    qprocess_->kill();
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

    std::string exec = executor;
    if ("" == exec)
        return false;

    std::string script = std::string("\"") + file + std::string("\"");
    qprocess_->setWorkingDirectory(StdStringToQString(path));
    qprocess_->start(StdStringToQString(exec + " " + script + " " + args), QProcess::ReadWrite);
    return true;
}

void LuaExecutor::output(const std::string& str)
{
    emit sendOutput(StdStringToQString(str));
}

void LuaExecutor::input(const QString& in)
{
    if (isRunning())
        qprocess_->write(QStringToStdString(in).c_str());
}
