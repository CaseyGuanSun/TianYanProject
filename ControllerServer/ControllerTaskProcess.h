//
// Created by posteritytech on 2016/10/23.
//

#ifndef CONTROLLERSERVER_CONTROLLERTASKPROCESS_H
#define CONTROLLERSERVER_CONTROLLERTASKPROCESS_H

#include <string>
#include "BaseThread.h"
#include "Debug.h"

using  namespace std;

class CControllerTaskProcess
{
public:
    CControllerTaskProcess();
    ~CControllerTaskProcess();

    bool    Init();
    bool    Start();
    bool    Stop();
private:
    bool            m_quitFlag;
    CBaseThread*    m_pReceiveThread;
    int             m_listenPort;
    string          m_listenIp;

    unsigned int    m_receiveThreadId;
    int             m_receiveSockFd;

};

#endif //CONTROLLERSERVER_CONTROLLERTASKPROCESS_H
