/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-25 14:30
** Email        : caseyguan@posteritytech.com
** Filename     : ZeroMQHelper.h
** Description  : 
** ******************************************************/
#ifndef _ZEROMQ_HELPER_HEADER_
#define _ZEROMQ_HELPER_HEADER_

bool ZeroMQSendMessage(void* pSocket, const void* message, int msgLen, bool moreFlag);
int ZeroMQReceiveMessage(void* pSocket, void* outPut, int len);

#endif
