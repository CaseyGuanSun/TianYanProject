/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 23:38
** Email        : caseyguan@posteritytech.com
** Filename     : TaskReceiver.cpp
** Description  : 
** ******************************************************/
#include "TaskReceiver"
#include "GlobalObject.h"
#include "ostype.h"
#include "libjson.h"

#define THIS_FILE "CTaskReceiver"

CTaskReceiver::CTaskReceiver(unsigned int listenPort, const string & ipcKey):
    m_quitFlag(false)
    ,m_listenPort(listenPort)
    ,m_pThread(NULL)
    ,m_ipcKey(ipcKey)
    ,m_ipcHandle(NULL)
{

}

CTaskReceiver::~CTaskReceiver()
{

}

bool CTaskReceiver::CallBackFunction(ThreadObj param)
{
    CTaskReceiver* ptr = (CTaskReceiver*)param;
    return ptr->ReceiveProcess();
}

int CTaskReceiver::OnReceveHttpRequest(char* url, char* data)
{
    
}

bool   CTaskReceiver::Init()
{
    m_receiverSockFd = m_tcpSocket.CreateTcpSocket("", m_listenPort, 0, 0);
    if(m_receiverSockFd< 0)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "Create Task Receiver Tcp Socket failed, port:%d"， m_listenPort);
        return false;
    }

    m_ipcHandle = zmq_socket(ZeroMqCtxHandle, ZMQ_REQ);
    if(NULL == m_ipcHandle) 
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Create task ipc handle failed");
        return false;
    }

    m_pThread = CBaseThread::Create(CallBackFunction,this, PRIORITY_NORMAL, "TaskReceiver");
    if(NULL == m_pThread)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "Create Task Receiver Thread failed");
        return false;
    }

    DebugHandle->DebugMsg(LOG_LEVEL_INFO, THIS_FILE, "Task Receiver Init success");
    return true;
}

bool   CTaskReceiver::Start()
{
    if(m_pThread)
	{
		return m_pThread->Start(m_threadId);
	}
	return false;
}

bool   CTaskReceiver::Stop()
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

bool   CTaskReceiver::ReceiveProcess()
{
    char    buffer[MAX_TCP_MESSAGE_LENGTH+ 1];
    int     ret;
    bool    result;
    QUERY_TASK_NODE  taskNode;

    while(!m_quitFlag)
    {
        memset(buffer, 0x00, sizeof(buffer));
        ret =  m_tcpSocket.ReceiveTcpMessage(buffer, 1000);
        if(ret <= 0)
        {
            
        }else
        {
            memset(&taskNode, 0x00, sizeof(QUERY_TASK_NODE));
            result = processTaskMesssage(buffer, &taskNode);
            if(!result)
            {
                DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "process Task Message error");
            }else{
                //send task node to TaskWorker by ipc
            }
        }
    }
}

bool    CTaskReceiver::processTaskMesssage(const char* buffer, QUERY_TASK_NODE* taskNode)
{
    bool    result;

    if(NULL == buffer ||
       NULL == taskNode)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "process task message error, param is not right");
        return false;
    }

    JSONNODE *node = json_parse(buffer);
    if(NULL == node)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Parse task message failed, buffer:\n%s", buffer);
        return false;
    }

    result = parseTaskJsonMessage(node, taskNode);
    if(!result)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Parse task JSON message failed");
        json_delete(node);
        return false;
    }

    json_delete(node);
    return true;
}

bool    CTaskReceiver::parseTaskJsonMessage(JSONNODE* node, QUERY_TASK_NODE* taskNode)
{
    if(node == NULL)
    {
        DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Parse task JSON message failed");
        return false;
    }

    JSONNODE_ITERATOR iter = json_begin(node);
    while(iter != json_end(node))
    {
        if(*iter == NULL)
        {
            DebugHandle->DebugMsg(LOG_LEVEL_ERROR, THIS_FILE, "Parse Task JSNO message 2 failed");
            return false;
        }

        if(json_type(*iter) == JSON_ARRAY ||
           json_type(*iter) == JSON_NODE)
        {
            parseTaskJsonMessage(*iter, response);
        }

        json_char *node_name = json_name(*iter);

        if(!strcmp(node_name, "timestamp"))
        {
            taskNode->timestamp = json_as_int(*iter);
        }else if(!strcmp(node_name, "taskId"))
        {
            taskNode->taskId = json_as_int(*iter);
        }else if(!strcmp(node_name, "tiebaUserName"))
        {
            json_char *node_value = json_as_string(*iter);
            taskNode->tiebaUserName = node_value;
            json_free(node_value);
        }else if(!strcmp(node_name, "mobileWithAsterisk"))
        {
            json_char *node_value = json_as_string(*iter);
            taskNode->mobileWithAsterisk = node_value;
            json_free(node_value);
        }else if(!strcmp(node_name, "mobilePartA"))
        {
            json_char *node_value = json_as_string(*iter);
            taskNode->mobilePartA = node_value;
            json_free(node_value);
        }else if(!strcmp(node_name, "mobilePartB"))
        {
            json_char *node_value = json_as_string(*iter);
            taskNode->mobilePartB = node_value;
            json_free(node_value);
        }else if(!strcmp(node_name, "areaCode"))
        {
            json_char *node_value = json_as_string(*iter);
            taskNode->areaCode = node_value;
            json_free(node_value);
        }

        json_free(node_name);
        ++iter;
    }

    return 0;
}

