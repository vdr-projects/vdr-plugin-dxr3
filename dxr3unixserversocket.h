/*
 * dxr3unixsocket.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
