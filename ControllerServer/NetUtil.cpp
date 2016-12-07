/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 22:44
** Email        : caseyguan@posteritytech.com
** Filename     : NetUtil.cpp
** Description  : 
** ******************************************************/
bool	IsNumberIp(const char* ipAddress)
{
    int		i;
	char*	pStart;
	char*	pEnd;
	char	tmpString[128];

	strncpy(tmpString,ipAddress,127);
	tmpString[127]=0;

	pStart=tmpString;
	pEnd=strchr(pStart,'.');
	if(NULL==pEnd)
	{
		return(0);
	}

	*pEnd=0;
	i=atoi(pStart);
	if(i<0||i>255)
	{
		return(0);
	}

	pEnd++;
	pStart=pEnd;
	pEnd=strchr(pStart,'.');
	if(NULL==pEnd)
	{
		return(0);
	}

	*pEnd=0;
	i=atoi(pStart);
	if(i<0||i>255)
	{
		return(0);
	}
	pEnd++;
	pStart=pEnd;

	pEnd=strchr(pStart,'.');
	if(NULL==pEnd)
	{
		return(0);
	}

	*pEnd=0;
	i=atoi(pStart);
	if(i<0||i>255)
	{
		return(0);
	}
	pEnd++;
	pStart=pEnd;
	i=atoi(pStart);
	if(i<0||i>255)
	{
		return(0);
	}
	return(1);
}

int getHostByName(const char* hostName, struct sockaddr_in* outPut, int flag)
{
    	struct hostent	hostAddr;
	struct hostent	*result;
	char			buffer[1024];
	int				errorNo=-1;

	memset(outPut,0,sizeof(struct sockaddr_in));

	if(0==flag)
	{
		outPut->sin_addr.s_addr=inet_addr(hostName);
		return(0);
	}

	if(isNumericIp(hostName))
	{
		outPut->sin_addr.s_addr=inet_addr(hostName);
		return(0);
	}

	if(!strlen(hostName))
	{
		return(-1);
	}

	if(gethostbyname_r(hostName, &hostAddr, buffer, sizeof(buffer), &result, &errorNo))
	{
		return(-1);
	}
	else if(NULL==result)
	{
		return(-1);
	}
	else
	{
		memset(outPut,0,sizeof(struct sockaddr_in));
		bcopy(hostAddr.h_addr, (char *)&outPut->sin_addr, hostAddr.h_length);
		outPut->sin_family = hostAddr.h_addrtype;

		if(0xFFFFFFFF==outPut->sin_addr.s_addr)
		{
			outPut->sin_addr.s_addr=0;
			return(-1);
		}
		else
		{
			return(0);
		}
	}
}
