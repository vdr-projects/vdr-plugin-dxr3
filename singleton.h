/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2004-2009 Christian Gmeiner
 *
 * This file is part of vdr-plugin-dxr3.
 *
 * vdr-plugin-dxr3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2.
 *
 * vdr-plugin-dxr3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dxr3-plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <memory>
#include <vdr/thread.h>

template <class T>
class Singleton {
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

    virtual ~Singleton() { }

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

#endif /*_SINGLETON_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
