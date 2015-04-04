#include "processmsgthread.h"
#include "lunarcommon.h"
#include "gui/mainwindow.h"
#include "util/base.hpp"

////////////////////////////////////////////////////
// class name : MsgRecvThread
// description :
// author :
// time : 2012-02-02-16.08
////////////////////////////////////////////////////
MsgRecvThread::MsgRecvThread() :
    recv_msg_flag_(false),
    process_sock_(util::Family_IPv4)
{
}

MsgRecvThread::~MsgRecvThread()
{
}

void MsgRecvThread::Stop()
{
    recv_msg_flag_ = false;
    process_sock_.closeSocket();
}

bool MsgRecvThread::Start()
{
    unsigned short port = 0;
    bool ret = process_sock_.bind("", 0, &port);
    if(!ret)
    {
        LunarGlobal::set_process_sock_port(port);

        if(LunarGlobal::get_arg_cnt()>1)
        {
            for(int i=1; i<LunarGlobal::get_arg_cnt(); i++)
            {
                TMsg tmsg;
                tmsg.lunar_code = kLunarCode;
                tmsg.msg_id = MSG_OPEN_DOC;
                memcpy(tmsg.msg, LunarGlobal::get_arg(i).c_str(), LunarGlobal::get_arg(i).length());
                process_sock_.sendTo((char*)&tmsg, sizeof(tmsg), "127.0.0.1", LunarGlobal::get_process_sock_port());
            }
        }
        else
        {
            TMsg tmsg;
            tmsg.lunar_code = kLunarCode;
            tmsg.msg_id = MSG_OPEN_DOC;
            std::string str_empty = "";
            memcpy(tmsg.msg, str_empty.c_str(), str_empty.length());
            process_sock_.sendTo((char*)&tmsg, sizeof(tmsg), "127.0.0.1", LunarGlobal::get_process_sock_port());
        }
        return false;
    }

    recv_msg_flag_ = true;
    this->start();
    return true;
}

void MsgRecvThread::run()
{
    while(recv_msg_flag_)
    {
        std::string ip;
        unsigned short port;
        int len = process_sock_.recvFrom(&ip, &port);
        if(len > static_cast<int>(sizeof(int)*2) )
        {
            TMsg* pmsg = (TMsg*)(process_sock_.getRecvBuf());

            if(pmsg->lunar_code == kLunarCode)
            {
                MsgProc(pmsg);
            }
        }
    }
}

void MsgRecvThread::MsgProc(TMsg* pmsg)
{
    if (pmsg->lunar_code == kLunarCode)
    {
        switch (pmsg->msg_id)
        {
            case MSG_OPEN_DOC:
                OpenDoc(pmsg);
                break;

            default:
                break;
        }
    }
}

void MsgRecvThread::OpenDoc(TMsg* pmsg)
{
    std::string str(pmsg->msg);
    Q_EMIT OpenDoc(StdStringToQString(str));
}

