/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 23:44
** Email        : caseyguan@posteritytech.com
** Filename     : TaskWorker.h
** Description  : 
** ******************************************************/
#ifndef     _TASK_WORKER_HEADER_
#define     _TASK_WORKER_HEADER_
#include <string>
#include "HttpClient.h"

class CTaskWorker
{
public:
    CTaskWorker(int id, const string& ipcKey);
    ~CTaskWorker();
    static bool ProcessCbFunction(ThreadObj param);
    bool    Init();
    bool    Start();
    bool    Stop();
private:
    bool    processTask();
private:
    int             m_id;
    bool            m_quitFlag;
    string          m_ipcKey;
    void*           m_taskReceiveHandle;
    CBaseThread*    m_pThread;
    int             m_threadId;

};

#endif



