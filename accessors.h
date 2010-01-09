/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2010 Christian Gmeiner
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

#ifndef ACCESSORS_H
#define ACCESSORS_H

// based on a very good idea from
//  * http://www.kirit.com/C%2B%2B%20killed%20the%20get%20%26%20set%20accessors
//  * http://www.kirit.com/C%2B%2B%20killed%20the%20get%20%26%20set%20accessors/A%20simple%20meta-accessor

enum Accessor_type { rw, ro };

template<typename T, Accessor_type = rw >
class Accessors;

template<typename T>
class Accessors<T, rw> {
public:
    Accessors<T, rw>() {}
    explicit Accessors(const T &t) : m_t(t) {}

    const T &operator() () const    { return m_t; }
    T operator() (const T &t)       { return m_t = t; }

private:
    T m_t;
};

template<typename T>
class Accessors<T, ro> {
public:
    Accessors<T, ro>() {}
    explicit Accessors(const T &t) : m_t(t) {}

    const T &operator() () const    { return m_t; }
    T operator= (const T &t)        {return m_t = t; }

private:
    T m_t;
};

#endif /*ACCESSORS_H*/
