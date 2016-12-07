/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 23:37
** Email        : caseyguan@posteritytech.com
** Filename     : TaskReceiver.h
** Description  : 
** ******************************************************/
#ifndef _TASK_RECEIVER_HEADER_
#define _TASK_RECEIVER_HEADER_
#include "BaseThread.h"
#include "TcpSocket.h"
#include <string>

using namespace std;

class CTaskReceiver{
public:
    CTaskReceiver(unsigned int listenPort, const string& ipcKey);
    ~CTaskReceiver();
    static bool CallBackFunction(ThreadObj param);
    static int OnReceveHttpRequest(char* url, char* data);

    bool    Init();
    bool    Start();
    bool    Stop();
    bool    ReceiveProcess();
private:
    bool    processTaskMesssage(const char* buffer, QUERY_TASK_NODE* taskNode);
    bool    parseTaskJsonMessage(JSONNODE* node, QUERY_TASK_NODE* taskNode);
private:
    bool            m_quitFlag;
    unsigned  int   m_listenPort;
    int             m_receiverSockFd;
    CBaseThread*    m_pThread;
    int             m_threadId;
    CCTcpSocket     m_tcpSocket;
    string          m_ipcKey;
    void*           m_ipcHandle;
};

#endif

