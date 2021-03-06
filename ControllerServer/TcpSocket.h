/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-14 23:16
** Email        : caseyguan@posteritytech.com
** Filename     : TcpSocket.h
** Description  : 
** ******************************************************/
#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_
#include <string>

using namespace std;
#define MAXTCPPACKETLEN  2000

enum TCP_SOCKET_CODE
{
	TCP_SOCKET_SUCCESS = 0, 
	TCP_SOCKET_CREATEFAILED  = -1,		
	TCP_SOCKET_SETNONBLOCK_FAILED = -2,
	TCP_SOCKET_SETLINGER_FAILED = -3,
	TCP_SOCKET_SETSENDTIMEOUT_FAILED = -4,
	TCP_SOCKET_SETREUSEADDR_FAILED = -5
};

class CCTcpSocket
{
public:
	CCTcpSocket();
	~CCTcpSocket();

	int		CreateTcpSocket(string bindIp, unsigned short port, bool blockFlag, unsigned int sendTimeOut);	
	int		ReceiveTcpMessage(char* recvBuf, int timeout);
	int		ReceiveNRawMessage(char* buffer, int recvLen, int timeout);
private:

	int					m_sockFd;
	string				m_bindIp;
	unsigned short   m_localPort;
	bool				m_blockFlag;
	unsigned int		m_sendTimeOut;
};

#endif

