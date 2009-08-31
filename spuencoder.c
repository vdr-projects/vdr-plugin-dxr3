/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2009 Christian Gmeiner
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

#include "spuencoder.h"
#include "dxr3interface.h"

static const uint8_t CMD_FORCE_DISPLAYING   = 0x00;
static const uint8_t CMD_STOP_DISPLAYING    = 0x02;
static const uint8_t CMD_SET_COLOR          = 0x03;
static const uint8_t CMD_SET_ALPHA          = 0x04;
static const uint8_t CMD_SET_DISPLAYAREA    = 0x05;
static const uint8_t CMD_SET_PIXEL_ADDRESES = 0x06;
static const uint8_t CMD_CHG_COLCON         = 0x07;

void cSpuEncoder::clearOsd()
{
    uint8_t d[10];

    // packet size
    d[0] = 0;
    d[1] = 10;

    // pointer to the SP_DCSQT
    d[2] = 0;
    d[3] = 4;

    // display duration...
    d[4] = 0x00;
    d[5] = 0x00;    // duration before turn on command occurs (will not be used)

    // pointer to next command block
    d[6] = 0;       // since this is the last command block, this
    d[7] = 4;       // points back to itself

    // stop displaying
    d[8] = CMD_STOP_DISPLAYING;

    // end sequence
    d[9] = 0xFF;

    // TODO: osd button handling

    cDxr3Interface::instance()->WriteSpu((uchar *)&d, 10);
}
