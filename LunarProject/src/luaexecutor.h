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
    bool setExecutor(const QString& executor_path);
    void setRunPath(const QString& runPath);
    bool isRunning();
    void stop();
    bool executeQ(const QString& args, const QString& path = "");
    bool execute(const std::string& args, const std::string& path = "");
    bool executeInSysCmd(const std::string& args, const std::string& path);
    void input(const QString& in);
    QString runPathQ();
    std::string runPath();
Q_SIGNALS:
    void sendOutput(const QString&);
private:
    void output(const std::string& str);
private:
    util::Process process_;
    QString executor_path_;
    QString run_path_;
private:
    DISALLOW_COPY_AND_ASSIGN(LuaExecutor)
};

#endif // LUAEXCUTOR_H
