/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-17 21:34
** Email        : caseyguan@posteritytech.com
** Filename     : TcpSocket.cpp
** Description  : 
** ******************************************************/
#include "TcpSocket.h"

CCTcpSocket::CCTcpSocket()
{
}

CCTcpSocket::~CCTcpSocket()
{
}


/**
 *This function will create the socket then bind it to local bindIp:port
 *If port == 0, will not bind it
 *return negative for failed
 *NON_BLOCK:False,BLOCK:True
 */
int CCTcpSocket::CreateTcpSocket(const string& bindIp, unsigned short port, bool blockFlag, unsigned int sendTimeOut)
{
	int			flags;
	int			ret;

	m_sockFd = socket(AF_INET, SOCK_STREAM, 0);

	if(-1 == m_sockFd)
	{
		return TCP_SOCKET_CREATE_FAILED;
	}

	if(!blockFlag)
	{
		flags = fcntl(m_sockFd, F_GETFL, 0);
		if(flags < 0)
		{
			close(m_sockFd);
			return  TCP_SOCKET_SETNONBLOCK_FAILED;
		}
		flags |= O_NONBLOCK;
		if((ret = fcntl(m_sockFd, F_SETFL, flags)) < 0)
		{
			close(m_sockFd);
			return TCP_SOCKET_SETNONBLOCK_FAILED;
		}
	}else
	{
		flags = fcntl(m_sockFd, F_GETFL, 0);
		if(flags < 0)
		{
			close(m_sockFd);
			return TCP_SOCKET_SETNONBLOCK_FAILED;
		}
		flags &= ~O_NONBLOCK;
		if((ret = fcntl(m_sockFd, F_SETFL, flags)) < 0)
		{
			close(m_sockFd);
			return TCP_SOCKET_SETNONBLOCK_FAILED;
		}
	}

	struct linger	lingers;
	lingers.l_onoff = 1;
	lingers.l_linger = 0;
	if((ret = setsockopt(m_sockFd, SOL_SOCKET, SO_LINGER, (const char *) &lingers, sizeof(struct linger))) < 0)
	{
		close(m_sockFd);	
		return TCP_SOCKET_SETLINGER_FAILED;
	}

	if(sendTimeOut > 0)
	{
		struct	timeval	timeValue;
		timeValue.tv_sec = sendTimeOut;
		timeValue.tv_usec = 0;
		if((ret = setsockopt(m_sockFd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeValue, sizeof(struct timeval))) < 0)
		{
			close(m_sockFd);
			return  TCP_SOCKET_SETSENDTIMEOUT_FAILED;
		}
	}
	if(port == 0)
	{
		return m_sockFd;
	}

	int value = 1;
	ret = setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, (void *)&value,sizeof(value));
	if(ret < 0)
	{
		close(m_sockFd);
		return TCP_SOCKET_SETREUSEADDR_FAILED;
	}

	struct sockaddr		sockAddrLocal;
	struct sockaddr_in  *sockAddrPtr;
	struct sockaddr_in  tmpAddr;

	memset(&sockAddrLocal, 0x00, sizeof(struct sockaddr));
	sockAddrPtr = (struct sockaddr_in*)sockAddrLocal;
	memset(&sockAddrLocal,0,sizeof(sockAddrLocal));
	sockAddrPtr->sin_family=AF_INET;
	if(bindIp.length()<=0)
	{
		sockAddrPtr->sin_addr.s_addr=INADDR_ANY;
	}else{
		if("INADDR_ANY"==bindIp)
		{
			if(-1==getHostByName(bindIp.c_str(), &tmpAddr,1))
			{
				close(sockFd);
				return(-5);
			}
			sockAddrPtr->sin_addr.s_addr=tmpAddr.sin_addr.s_addr;
		}
		else
		{
			sockAddrPtr->sin_addr.s_addr=INADDR_ANY;
		}
	}
	sockAddrPtr->sin_port=htons(port);

	if(-1==bind(sockFd,&sockAddrLocal,sizeof(sockAddrLocal)))
	{
	//	writeIpcErrorLog("Bind socket failed(%s)\n",strerror(errno));
		close(sockFd);
		return(-6);
	}

	return(sockFd);
}

int		CCTcpSocket::ReceiveNRawMessage(char* buffer, int recvLen, int timeout)
{
	int		tmpLen=0;
	int		needLen=rcvLen;
	int		ret;
	fd_set  portFds;
	struct  timeval tv;
	int     selectN;
	int		restartSelectCounter;
	time_t	startTimer;

	if(timeout>0)
	{
		startTimer=time(NULL);
		selectN=sockFd+1;

		if(selectN>=1023 || selectN<1)
		{
	//		writeIpcErrorLog("ERROR: selectN %d is error\n", selectN);
			return(-1);
		}
		tv.tv_sec=timeout;
		tv.tv_usec=0;
	}

	if(timeout>0)
	{
		restartSelectCounter=0;
RESTART_SELECT_ACTION:
		FD_ZERO(&portFds);
		FD_SET(sockFd, &portFds);
		ret = select(selectN, &portFds, NULL, NULL, &tv);

		if(ret<0)
		{
			if((restartSelectCounter++)<10)//changed by casey from (restartSelectCounter<10) to (restartSelectCounter++<10)
			{
				goto RESTART_SELECT_ACTION;
			}

			writeIpcErrorLog("Select Error!!! for more than 10 times\n");
			return(-1);
		}
		else if(0==ret)
		{
			// timeout
			if((time(NULL)-startTimer)<timeout)
			{
				tv.tv_sec=timeout-(time(NULL)-startTimer);
				tv.tv_usec=0;
				goto RESTART_SELECT_ACTION;
			}
			return(-1);
		}

		ret=recv(sockFd,&buffer[tmpLen],needLen,MSG_WAITALL);
	}
	else
	{
		ret=recv(sockFd,&buffer[tmpLen],needLen,MSG_WAITALL);
	}

	if(ret==needLen)
	{
		return(ret);
	}
	else
	{
		return(-1);
	}
}

int		CCTcpSocket::ReceiveTcpMessage(char* recvBuf, int timeout)
{
	unsigned char	tcp_header[12];
	int				rcvLen;
	int				msgBodyLen;
	int				tmpLen;

	memset(&tcp_header,0,sizeof(tcp_header));
	if(sizeof(tcp_header)!=(rcvLen=ReceiveNRawMessage(sockFd,(char*) tcp_header,sizeof(tcp_header),timeout)))
	{
		return(-1);
	}

	if( tcp_header[0]!=0xFF ||
		tcp_header[1]!=0xFE ||
		tcp_header[2]!=0xFF ||
		tcp_header[3]!=0xFE ||
		tcp_header[4]!=0xFF ||
		tcp_header[5]!=0xFE ||
		tcp_header[6]!=0xFF ||
		tcp_header[7]!=0xFE
		)
	{
		// sync not right, should disconnect the socket
		printf("Sync not right\n");
		return(-2);
	}

	memcpy(&tmpLen,&tcp_header[8],sizeof(int));
	msgBodyLen=ntohl(tmpLen);
	if(msgBodyLen>=MAXTCPPACKETLEN || msgBodyLen<0)
	{
		return(-3);
	}

	if(0==msgBodyLen)
	{
		return(0);
	}

	memset(rcvBuf,0,sizeof(rcvBuf));
	if(0==timeout)
	{
		rcvLen=ReceiveNRawMessage(sockFd,rcvBuf,msgBodyLen,timeout);
	}
	else
	{
		rcvLen=ReceiveNRawMessage(sockFd,rcvBuf,msgBodyLen,timeout+1);
	}

	if(-1==rcvLen)
	{
		return(-4);
	}

	rcvBuf[rcvLen]=0;
	return(rcvLen);
}
