#ifndef LUAEXECUTOR_H
#define LUAEXECUTOR_H

#include <string>
#include <QtCore/QObject>
#include <QtCore/QString>
#include "util/process.hpp"
#include "lunarcommon.h"

class LuaExecutor : public QObject
{
    Q_OBJECT
public:
    explicit LuaExecutor(QObject* parent = 0);
    virtual ~LuaExecutor();
    bool isRunning();
    void stop();
    bool execute(const std::string& file, const std::string& args, const std::string& path = "");
    bool executeInSysCmd(const std::string& file, const std::string& args, const std::string& path);
    void input(const QString& in);
Q_SIGNALS:
    void sendOutput(const QString&);
private:
    void output(const std::string& str);
    bool isFileInFileFilter(const std::string& file, const std::string& file_filter);
    std::string getScriptExecutor(const std::string& file);
private:
    util::Process process_;
private:
    DISALLOW_COPY_AND_ASSIGN(LuaExecutor)
};

#endif // LUAEXCUTOR_H
