/*
 * dxr3singleton.h
 *
 * Copyright (C) 2004 Christian Gmeiner
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

#ifndef _DXR3_SINGLETON_H_
#define _DXR3_SINGLETON_H_
	
// ==================================
//!  A singleton template. 
/*!
	Is a nice solution to use only
	one instance of a class.
*/
template<typename T>
class Singleton
{
	protected:
		Singleton() {}
		virtual ~Singleton() {}

	public:
		static T& Instance()  
		{
			static T m_Instance;
			return m_Instance;
		}
};

#endif /*_DXR3_SINGLETON_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
