/*
 * dxr3singleton.h
 *
 * Copyright (C) 2004-2009 Christian Gmeiner
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

#ifndef _DXR3_SINGLETON_H_
#define _DXR3_SINGLETON_H_

#include <memory>
#include <vdr/thread.h>

template <class T>
class Singleton
{
public:
    static T *instance() {

        // use double-checked looking
        // see http://en.wikipedia.org/wiki/Double-checked_locking
        if (inst.get() == 0) {
            m.Lock();
            if (inst.get() == 0) {
                inst = std::auto_ptr<T>(new T);
            }
            m.Unlock();
        }

        return inst.get();
    }

    virtual ~Singleton() { };

protected:
    Singleton() { }

private:
    static std::auto_ptr<T> inst;
    static cMutex m;
};

template<class T>
std::auto_ptr<T> Singleton<T>::inst(0);

template<class T>
cMutex Singleton<T>::m;

#endif /*_DXR3_SINGLETON_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
