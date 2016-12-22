#ifndef LUAEXECUTOR_H
#define LUAEXECUTOR_H

#include <string>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QProcess>
#include "lunarcommon.h"

class LuaExecutor : public QObject
{
    Q_OBJECT
public:
    explicit LuaExecutor(QObject* parent = 0);
    virtual ~LuaExecutor();
    bool isRunning();
    void stop();
    bool execute(const std::string& file, const std::string& args, const std::string& path, const std::string& executor = "");
    void input(const QString& in);
Q_SIGNALS:
    void sendOutput(const QString&);
private:
    void output(const std::string& str);
    bool isFileInFileFilter(const std::string& file, const std::string& file_filter);
private:
    QProcess* qprocess_;
private:
    DISALLOW_COPY_AND_ASSIGN(LuaExecutor)
};

#endif // LUAEXCUTOR_H
