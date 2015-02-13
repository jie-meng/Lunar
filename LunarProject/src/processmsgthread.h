#ifndef PROCESSMSGTHREAD_H
#define PROCESSMSGTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <deque>
#include "util/net.hpp"

const uint32_t kLunarCode = 0xA0B1C2D3;
const uint32_t kMsgBodySize = 1024;

#define MSG_OPEN_DOC    (0)

////////////////////////////////////////////////////
// class name : ProcessMsgThread
// description :
// author :
// time : 2012-02-01-14.58
////////////////////////////////////////////////////
struct TMsg
{
    TMsg() : lunar_code(0), msg_id(0) { memset(msg, 0, sizeof(msg)); }

    uint32_t lunar_code;
    uint32_t msg_id;
    char msg[kMsgBodySize];
};

////////////////////////////////////////////////////
// class name : MsgRecvThread
// description :
// author :
// time : 2012-02-02-16.08
////////////////////////////////////////////////////
class MsgRecvThread : public QThread
{
    Q_OBJECT
public:
    explicit MsgRecvThread();
    virtual ~MsgRecvThread();
    void Stop();
    bool Start();
Q_SIGNALS:
    void OpenDoc(const QString&);
protected:
    virtual void run();
private:
    void MsgProc(TMsg* pmsg);
    void OpenDoc(TMsg* pmsg);
    bool recv_msg_flag_;
    util::DgramSocket process_sock_;
private:
    DISALLOW_COPY_AND_ASSIGN(MsgRecvThread)
};

#endif // PROCESSMSGTHREAD_H
