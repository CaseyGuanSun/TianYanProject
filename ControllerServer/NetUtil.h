/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 22:44
** Email        : caseyguan@posteritytech.com
** Filename     : NetUtil.h
** Description  : 
** ******************************************************/
#include "ostype.h"
bool	IsNumberIp(const char* ipAddress);
int getHostByName(const char* hostName, struct sockaddr_in* outPut, int flag);

