/*
 * dxr3unixserversocket.h: 
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __DXR3_UNIX_SERVER_SOCKET_H
#define __DXR3_UNIX_SERVER_SOCKET_H

#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>


#include "dxr3vdrincludes.h"

class cDxr3UnixServerSocket {
public:    
    cDxr3UnixServerSocket(const char* pFileName = "/tmp/.dxr3-ux-sock", int backlog = 5);
    virtual ~cDxr3UnixServerSocket();

    bool WaitForConnection(void);
    bool GetNextMessage(void);
    bool IsConnected(void);
protected:
    virtual void ProcessMessage(void) = 0;

    const char* m_pFileName;
    bool m_bConnected;
    int m_backlog;
    int m_fdServerSocket;
    int m_fdConnectionSocket;
    sockaddr_un m_addr;
    enum eDxr3socketMessageSize { MAX_REC_SIZE = 100 };
    uint8_t m_msg[MAX_REC_SIZE];
    uint8_t m_msgSize;
    
private:
    cDxr3UnixServerSocket(cDxr3UnixServerSocket&); // no copy constructor
};


class cDxr3StartStopSocket : public cDxr3UnixServerSocket {
public:    
    cDxr3StartStopSocket() : m_bSavedState(false) {};
    virtual ~cDxr3StartStopSocket() {};
protected:
    void ProcessMessage(void);
    void SendStatus(void);

    bool m_bSavedState;
    
private:
    cDxr3StartStopSocket(cDxr3StartStopSocket&); // no copy constructor    
};


class cDxr3StartStopThread : public cThread {
public:
    cDxr3StartStopThread() {};
    virtual ~cDxr3StartStopThread() {};    
protected:    
    virtual void Action(void);

private:
    cDxr3StartStopThread(cDxr3StartStopThread&); // no copy constructor          
};

#endif // __DXR3_UNIX_SERVER_SOCKET_H
