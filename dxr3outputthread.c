/*
 * dxr3outputthread.c
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

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "dxr3outputthread.h"
#include "dxr3log.h"

// ================================== 
const int AUDIO_OFFSET = 4500;
#define SCR m_dxr3Device.GetSysClock()
// ================================== 

// ================================== 
//! constructor
cDxr3OutputThread::cDxr3OutputThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer) :
cThread(), m_dxr3Device(dxr3Device), m_buffer(buffer), m_bStopThread(false), m_bNeedResync(false)
{
}

// ================================== 
// send stop signal
void cDxr3OutputThread::SetStopSignal() 
{
    Lock();
    m_bStopThread = true;
    Unlock();
}

// ==================================
// was stop signal send?
bool cDxr3OutputThread::GetStopSignal() 
{
    bool ret = false;
    Lock();
    ret = m_bStopThread;
    Unlock();

    return ret;
}

// ==================================
// constr.
cDxr3AudioOutThread::cDxr3AudioOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer) :
cDxr3OutputThread(dxr3Device, buffer) 
{
}

// ==================================
// thread action
void cDxr3AudioOutThread::Action() 
{ 
    bool resync = false;
    uint32_t pts = 0;
    
    cLog::Instance() << "cDxr3AudioOutThread::Action Thread started\n";

    sched_param temp;
    temp.sched_priority = 2;

    if (!pthread_setschedparam(pthread_self(), SCHED_RR, &temp)) 
	{
		cLog::Instance() << "cDxr3AudioOutThread::Action(): Error can't set priority\n";
    }  

    while (!GetStopSignal()) 
	{
		pts = 0;
        cFixedLengthFrame* pNext = m_buffer.Get();

        if (pNext) pts = pNext->GetPts();
        
        if (pts && abs((int)pts-(int)SCR) > 30000 || m_dxr3Device.IsExternalReleased()) 
		{
			if (cDxr3ConfigData::Instance().GetDebugEverything())
			{
				cLog::Instance() << "cDxr3AudioOutThread::Action " << "pts = " << pts << " scr = " << SCR << "\n";
			}
            m_buffer.Clear();
            m_bNeedResync = true;
        } 
		else if (pNext) 
		{
            if (!pts || pts < SCR) 
			{
				if (cDxr3ConfigData::Instance().GetDebugEverything())
				{
//					if (pts) cLog::Instance() << "cDxr3AudioOutThread::Action pts " << pNext->GetPts() << " scr " << SCR << " delta " << (pts - SCR) << "\n";
				}
                if (!pts && resync) 
				{
                    continue;
                } 
				else 
				{
                    resync = false;
                }
                
                if (pts && (pts < SCR) && ((SCR - pts) > 5000)) 
				{
					if (cDxr3ConfigData::Instance().GetDebugEverything())
					{
//						cLog::Instance() << "cDxr3AudioOutThread::Action pts audio too small " << (SCR - pts) << "\n";
					}
                    m_dxr3Device.SetSysClock(pts+ 1 * AUDIO_OFFSET);
                    m_dxr3Device.PlayAudioFrame(pNext);
                    if (m_buffer.IsPolled()) 
					{
                        m_buffer.Clear();
                        m_bNeedResync = true;
                    }
                } 
				else 
				{                
					if (cDxr3ConfigData::Instance().GetDebugEverything())
					{
//						cLog::Instance() << "cDxr3AudioOutThread::Action write audio\n";
					}
                    m_dxr3Device.PlayAudioFrame(pNext);
                    m_buffer.Pop();
                }                                   
            } 
			else 
			{
                if (abs((int)pts - (int)SCR) < (AUDIO_OFFSET )) 
				{
					if (cDxr3ConfigData::Instance().GetDebugEverything())
					{
//						if (pts) cLog::Instance() << "cDxr3AudioOutThread::Action pts " << pNext->GetPts() << " scr " << SCR << " delta " << (pts - SCR) << "\n";
					}
                    m_dxr3Device.PlayAudioFrame(pNext);
                    m_buffer.Pop();    
                } 
            }
        }
        
        if ((pts > SCR && abs((int)pts - (int)SCR) > AUDIO_OFFSET)) 
		{
			if (cDxr3ConfigData::Instance().GetDebugEverything())
			{
//				cLog::Instance() << "cDxr3AudioOutThread::Action Stopping audio " << SCR << " " << pts << "\n";
			}

            usleep(10000);

			if (cDxr3ConfigData::Instance().GetDebugEverything())
			{
//				cLog::Instance() << "cDxr3AudioOutThread::Action Starting audio " << SCR << " " << pts << "\n";
			}
        }
    }
}

// ==================================
// constr.
cDxr3VideoOutThread::cDxr3VideoOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer) :
cDxr3OutputThread(dxr3Device, buffer) 
{
}

// ==================================
// thread action
void cDxr3VideoOutThread::Action() 
{
    uint32_t pts = 0;
    static uint32_t lastPts = 0;
    
	cLog::Instance() << "cDxr3VideoOutThread::Action Thread started\n";

    sched_param temp;
    temp.sched_priority = 1;

    if (!pthread_setschedparam(pthread_self(), SCHED_RR, &temp)) 
	{
//        cLog::Instance() << "cDxr3VideoOutThread::Action(): Error can't set priority\n";
    }      

    while (!GetStopSignal()) 
	{
        cFixedLengthFrame* pNext = m_buffer.Get();
        if (pNext) 
		{
            pts = pNext->GetPts();
            if (pts == lastPts) pts = 0;
            
            if (pts > SCR && abs((int)pts - (int)SCR) < 7500) 
			{
                m_dxr3Device.SetPts(pts);
            }

            if (!pts || pts < SCR) 
			{
                if (m_buffer.Available()) 
				{
                   m_dxr3Device.PlayVideoFrame(pNext);
                   m_buffer.Pop();    
                }
            } 
			else 
			{
                if ((pts > SCR) && abs((int)pts - (int)SCR) < 7500) 
				{
					if (cDxr3ConfigData::Instance().GetDebugEverything())
					{
//						cLog::Instance() << "cDxr3VideoOutThread::Action pts " << pts << " scr " << SCR << " delta " << (pts - SCR) << "\n";
					}

                    m_dxr3Device.SetPts(pts);

					if (cDxr3ConfigData::Instance().GetDebugEverything())
					{
//						cLog::Instance() << "cDxr3VideoOutThread::Action done\n";
					}

                    if (m_buffer.Available() && pNext->GetData() && pNext->GetCount()) 
					{
                        m_dxr3Device.PlayVideoFrame(pNext);

						if (cDxr3ConfigData::Instance().GetDebugEverything())
						{
//							cLog::Instance() << "cDxr3VideoOutThread::Action write\n";
						}

                        m_buffer.Pop();    

						if (cDxr3ConfigData::Instance().GetDebugEverything())
						{
//							cLog::Instance() << "cDxr3VideoOutThread::Action done pop\n";
						}
                    }
					if (cDxr3ConfigData::Instance().GetDebug())
					{
//						cLog::Instance() << "cDxr3VideoOutThread::Action done complete\n";
					}
                } 
				else 
				{
                    if (pts < SCR) 
					{                        
						m_dxr3Device.PlayVideoFrame(pNext);
                        m_buffer.Pop();
                    }
                }
            }
        

            if (m_dxr3Device.IsExternalReleased()) 
			{
                m_bNeedResync = true;
                m_buffer.Clear();        
            }
        
            if ((pts > SCR && abs((int)pts - (int)SCR) > 7500 )) 
			{
				if (cDxr3ConfigData::Instance().GetDebugEverything())
				{
//					cLog::Instance() << "cDxr3VideoOutThread::Action Stopping video "  << SCR << " " << pts << "\n";
				}

                usleep(10000);

				if (cDxr3ConfigData::Instance().GetDebugEverything())
				{
//					cLog::Instance() << "cDxr3VideoOutThread::Action Starting video "  << SCR << " " << pts << "\n";
				}
            }
        }
    }
}


#undef SCR
