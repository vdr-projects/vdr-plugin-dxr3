/*
 * dxr3unixserversocket.c
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <cstring>
#include <string>

#include "dxr3unixserversocket.h"
#include "dxr3interface.h"
#include "dxr3log.h"

#ifndef SOCKET_CHMOD
#define SOCKET_CHMOD 0660
#endif

// ==================================
//! constructor
cDxr3UnixServerSocket::cDxr3UnixServerSocket(const char* pFileName, int backlog) 
{
    m_bConnected = false;
    m_backlog = backlog;
    m_addr.sun_family = AF_UNIX;
    m_msgSize = 0;

    m_pFileName = pFileName;
    strcpy(m_addr.sun_path, pFileName);
    unlink(pFileName);
    

    m_fdServerSocket = socket(PF_UNIX, SOCK_STREAM, 0);
    if (m_fdServerSocket > -1) 
	{
        if (bind(m_fdServerSocket, (sockaddr*)&m_addr, (socklen_t)sizeof(m_addr))) {
			cLog::Instance() << "cDxr3UnixServerSocket::cDxr3UnixServerSocket Error: binding socket failed\n";
        }

        if (listen(m_fdServerSocket, m_backlog)) 
		{
            cLog::Instance() << "cDxr3UnixServerSocket::cDxr3UnixServerSocket Error: Listen failed\n";
        }

        if (chmod(m_pFileName, SOCKET_CHMOD))  
		{
            cLog::Instance() << "cDxr3UnixServerSocket::cDxr3UnixServerSocket Error: Chmod failed\n";
        }
    } 
	else 
	{
       cLog::Instance() << "cDxr3UnixServerSocket::cDxr3UnixServerSocket Error: Unable to create socket\n";
    }    
}

// ==================================
bool cDxr3UnixServerSocket::WaitForConnection() 
{
    cLog::Instance() << "cDxr3UnixServerSocket::WaitForConnection Waiting ...\n";
    if (m_fdServerSocket > -1) 
	{
        m_fdConnectionSocket = accept(m_fdServerSocket, 0, 0);
        if (m_fdConnectionSocket > -1) 
		{
            m_bConnected = true;
            cLog::Instance() << "cDxr3UnixServerSocket::WaitForConnection Connected\n";
        } 
		else 
		{
            m_bConnected = false;
            cLog::Instance() << "cDxr3UnixServerSocket::WaitForConnection failed\n";
        }
    } 
	else 
	{
        m_bConnected = false;
    }
    return m_bConnected;
}

// ==================================
bool cDxr3UnixServerSocket::GetNextMessage() 
{
    bool ret = false;
    m_msgSize = 0;
    memset(m_msg, 0, MAX_REC_SIZE);
    
    if (m_bConnected) 
	{
        m_msgSize = read(m_fdConnectionSocket, m_msg, MAX_REC_SIZE - 1);
        if (m_msgSize <= 0) 
		{
            m_msgSize = 0;
            m_bConnected = 0;
            close(m_fdConnectionSocket);
            cLog::Instance() << "cDxr3UnixServerSocket::GetNextMessage failed/connection closed\n";
        } 
		else 
		{
            ret = true;
        }
    }

    if (ret) ProcessMessage();

    return ret;
}

// ==================================
bool cDxr3UnixServerSocket::IsConnected() 
{
    return m_bConnected;
}

// ==================================
cDxr3UnixServerSocket::~cDxr3UnixServerSocket() 
{
    close(m_fdConnectionSocket);
    close(m_fdServerSocket);
    unlink(m_pFileName);
}

// ==================================
void cDxr3StartStopSocket::SendStatus() 
{
    if (cDxr3Interface::Instance().IsExternalReleased()) 
	{
		std::string res("CloseDxr3DeviceRsp\n");
        write(m_fdConnectionSocket, res.c_str(), res.size());
    } 
	else 
	{
        std::string res("OpenDxr3DeviceRsp\n");
        write(m_fdConnectionSocket, res.c_str(), res.size());
    }
}

// ==================================
void cDxr3StartStopSocket::ProcessMessage(void) 
{
    cLog::Instance() << "cDxr3StartStopSocket::ProcessMessage Rec: " << (const char*) m_msg << "\n";
    
    if (std::string((const char*)m_msg) == std::string("OpenDxr3DeviceCmd")) 
	{
        cDxr3Interface::Instance().ExternalReopenDevices();
        SendStatus();
    } 
	else if (std::string((const char*)m_msg) == std::string("CloseDxr3DeviceCmd")) 
	{
        cDxr3Interface::Instance().ExternalReleaseDevices();
        SendStatus();
    }
	else if (std::string((const char*)m_msg) == std::string("StatusDxr3DeviceCmd")) 
	{
        SendStatus();
    } 
	else if (std::string((const char *)m_msg) == std::string("SaveDxr3DeviceCmd")) 
	{
        m_bSavedState = cDxr3Interface::Instance().IsExternalReleased();
        SendStatus();
    } 
	else if (std::string((const char *)m_msg) == std::string("RestoreDxr3DeviceCmd")) 
	{
        if (m_bSavedState) 
		{
            cDxr3Interface::Instance().ExternalReleaseDevices();
        } 
		else 
		{
            cDxr3Interface::Instance().ExternalReopenDevices();
        }
        SendStatus();
    } 
	else 
	{
        std::string res("Error\n");
        write(m_fdConnectionSocket, res.c_str(), res.size());
    }
}    

// ==================================
void cDxr3StartStopThread::Action() 
{
    cDxr3StartStopSocket mySocket;
    while (mySocket.WaitForConnection()) 
	{
        while (mySocket.GetNextMessage());
    }
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
