/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-25 14:30
** Email        : caseyguan@posteritytech.com
** Filename     : ZeroMQHelper.cpp
** Description  : 
** ******************************************************/
#include    "ostype.h"
#include    "ZeroMQHelper.h"
#include    <zmq.h>

bool ZeroMQSendMessage(void* pSocket, const void* message, int msgLen, bool moreFlag)
{
    zmq_msg_t	    msg;
	int 		    ret;

    if(NULL == pSocket)
    {
        return false;
    }

    zmq_msg_init_size(&msg, msgLen);
	memcpy(zmq_msg_data(&msg), message, msgLen);
    if(moreFlag)
    {
	    ret = zmq_msg_send(&msg, pSocket, ZMQ_SNDMORE);
    }else
    {
        ret = zmq_msg_send(&msg, pSocket, 0);
    }
    zmq_msg_close(&msg);
    if(ret < 0)
    {
        return false;
    }else
    {
        return true;
    }
}


int ZeroMQReceiveMessage(void* pSocket, void* outPut, int len)
{
    zmq_msg_t	msg;
    void*       msgData;
    int         msgSize;
    int         ret;

    if( pSocket == NULL ||
        outPut == NULL 
        )
    {
        return -1;
    }

	zmq_msg_init(&msg);
	ret = zmq_msg_recv(&msg, pSocket, 0);
    if(ret < 0)
    {
        zmq_msg_close(&msg);
        return -1;
    }
	msgData = zmq_msg_data(&msg);	
	msgSize = zmq_msg_size(&msg);

    if(len < msgSize)
    {
        msgSize = len;
	    memcpy(outPut, msgData, msgSize);
    }else
    {
        memcpy(outPut, msgData, msgSize);
    }
     zmq_msg_close(&msg);

     return msgSize;
}

