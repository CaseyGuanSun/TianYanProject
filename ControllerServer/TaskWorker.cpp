/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 23:44
** Email        : caseyguan@posteritytech.com
** Filename     : TaskWorker.cpp
** Description  : 
** ******************************************************/
#include    "TaskWorker.h"
#include    "GlobalObject.h"
#include    <zmq.h>

#define THIS_FILE "CTaskWorker"

CTaskWorker::CTaskWorker(int id, const string& ipcKey)
    m_id(id)
    ,m_quitFlag(false)
    ,m_ipcKey(ipcKey)
    ,m_taskReceiveHandle(NULL)
    ,m_pThread(NULL)
{
}

CTaskWorker::~CTaskWorker()
{

}

bool CTaskWorker::CallBackFunction(ThreadObj param)
{
    CTaskWorker* ptr = (CTaskWorker*)param;
    retur ptr->processTask();
}

bool CTaskWorker::Init()
{
    m_taskReceiveHandle = zmq_socket(ZeroMqCtxHandle, ZMQ_REQ);
    if(NULL == m_taskReceiveHandle)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Create task receiver handle failed");
        return false;
    }

    m_pThread = CBaseThread::Create(CallBackFunction, this, PRIORITY_NORMAL, "TaskWorker");
    if(m_pthread == NULL)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Create task process thread failed");
        zmq_close(m_taskReceiveHandle);
        return false;
    }
    DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "Init Task process success");

    return true;
}

bool CTaskWorker::Start()
{
    if(m_pThread)
    {
        return m_pThread->Start(m_threadId);
    }
    return false;
}

bool CTaskWorker::Stop()
{
	if(m_quitFlag)
	{
		return true;
	}
	m_quitFlag = true;
	if(m_pThread)
	{
		return m_pThread->Stop();
	}
	return false;
}

bool CTaskWorker::processTask()
{
    char                identity [MAX_IDENTITY_LENGTH+1];
    int                 ret;
	bool  		        result;
    QUERY_TASK_NODE     taskNode;

	memset(identity, 0x00, sizeof(identity));
    sprintf (identity, "TASKWORKER:%d", m_id);
    zmq_setsockopt (m_taskReceiveHandle, ZMQ_IDENTITY, identity, strlen (identity));
	DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "ipc key:%s", m_ipcKey.c_str());

    zmq_connect(m_taskReceiveHandle, m_ipcKey.c_str());

    ZeroMQSendMessage(m_taskReceiveHandle, identity, strlen(identity));

    while(!m_quitFlag)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "Task Preocess begin to receive Task");

        memset(identity, 0x00, sizeof(identity));
		ret = ZeroMQReceiveMessage(m_taskReceiveHandle, identity, MAX_IDENTITY_LENGTH);
		if(ret < 0)
		{
			DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Task Worker ID:%d receive identity failed, errno:%d, error:%s",
			m_id, errno, zmq_strerror(errno));
			break;
		}

		DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "Task Worker ID:%d Receive Identity:%s", m_id, identity);

		ret = ZeroMQReceiveMessage(m_zeroMqReceiveHandle, buffer, MAX_BUFFER_LENGTH);
		if(ret < 0)
		{			
			DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Task Worker ID:%d receive empty frame failed, errno:%d, error:%s",
			m_id, errno, zmq_strerror(errno));
			break;
		}

        memset(&taskNode, 0x00, sizeof(QUERY_TASK_NODE));
		ret = ZeroMQReceiveMessage(m_zeroMqReceiveHandle, &taskNode, sizeof(QUERY_TASK_NODE));
		if(ret < 0)
		{			
			DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Task Worker ID:%d receive Task Node frame failed, errno:%d, error:%s",
			m_id, errno, zmq_strerror(errno));
			break;
		}

        result = ZeroMQSendMessage(m_zeroMqReceiveHandle, "OK", strlen("OK"), false);
        DebugHandle->DebugMsg(m_zeroMqReceiveHandle, THIS_FILE, "Receive Task, timestamp:%d, taskId:%d, tiebaUserName:%s, mobilePartA:%s, mobilePartB:%s, areaCode:%s", 
        taskNode.timestamp, taskNode.taskId, taskNode.tiebaUserName.c_str(), taskNode.mobilePartA.c_str(), taskNode.mobilePartB.c_str(), taskNode.areaCode.c_str());
    }
}


