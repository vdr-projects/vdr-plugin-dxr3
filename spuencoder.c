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
#include "dxr3tools.h"

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

void cSpuEncoder::encode(cBitmap *bmap, int top, int left)
{
    // store internaly
    bitmap = bmap;
    this->top = top;
    this->left = left;

    // set initial value
    data = NULL;

    // prepare datastructures
    memset(rleData.top, 0, sizeof(rleData.top));
    memset(rleData.bottom, 0, sizeof(rleData.bottom));
    memset(spu, 0, sizeof(spu));

    // free previouse used regions
    clearRegions();

    // get needed informations about used colors
    colors = bitmap->Colors(numColors);

    dsyslog("[dxr3-spuencoder] num colors %d", numColors);

    // generate and upload color palette
    generateColorPalette();

    // if the bitmap uses only 4 colors, we can
    // simply define a region, else we need to
    // calculate the reginos and do some remapping.
    if (numColors <= 4) {
        // add one region
        cSpuRegion *reg = new cSpuRegion(0);

        for (int i = 0; i < numColors; i++) {
            reg->addIndex(bitmap->Index(colors[i]));
        }
        regions.push_back(reg);
    } else {
        calculateRegions();
    }

    dsyslog("[dxr3-spuencoder] rle data");

    // encode whole bitmap with rle and store top and bottom lines
    rle4colors();

    // as we have only small space for all our spu data, we do here
    // a little trick. If we run out of space, when using
    // top and odd bottom, we try to use two times the top lines.
    // this will gain some more free space. the osd may not look
    // as good as with top and bottom lines, but its better to
    // have a working and maybe not so sexy looking osd instead of a
    // a broken osd.

    try {
        generateSpuData(true);
    } catch (char const* e) {

        dsyslog("[dxr3-spuencoder] %s", e);

        // make a try with only even lines
        generateSpuData(false);
    }

    // we are ready to send generated spu data
    dsyslog("[dxr3-spuencoder] spu packet size %d (bytes). %d left", written, (MAX_SPU_DATA - written));
    cDxr3Interface::instance()->WriteSpu((uchar *)&spu, written);

    if (data) {
        delete data;
        data = NULL;
    }
}

void cSpuEncoder::writeNibble(uint8_t val)
{
    // look if we have an overflow
    if (written == MAX_SPU_DATA) {
        throw "overflow";
    }

    if (ncnt++ & 1) {
        *p++ = nholder | ((val) & 0x0f);
        written++;
    } else {
        nholder = (val << 4);
    }
}

void cSpuEncoder::writeColorAndAlpha(sSection *sec, bool withCMD)
{
    if (withCMD) {
        spu[written++] = CMD_SET_COLOR;
    }
    spu[written++] = (sec->colIndex[3] << 4) | (sec->colIndex[2] & 0x0f);
    spu[written++] = (sec->colIndex[1] << 4) | (sec->colIndex[0] & 0x0f);

    if (withCMD) {
        spu[written++] = CMD_SET_ALPHA;
    }
    spu[written++] = (opacity[sec->colIndex[3]] << 4) | (opacity[sec->colIndex[2]] & 0x0f);
    spu[written++] = (opacity[sec->colIndex[1]] << 4) | (opacity[sec->colIndex[0]] & 0x0f);
}

void cSpuEncoder::writeRegionInformation()
{
    dsyslog("[dxr3-spuencoder] writing region definitions");

    spu[written++] = CMD_CHG_COLCON;

    int32_t size_fill = written;

    spu[written++] = 0x00;  // total size of parameter area
    spu[written++] = 0x00;  // will be filled later

    for (size_t i = 0; i < regions.size(); i++) {
        cSpuRegion *reg = regions[i];

        // we need to modify start and end line
        // because both are values for the whole screen and
        // we are working only on the bitmap to show.
        // se simply add top coordinate to startLine and endLine
        reg->startLine += this->top;
        reg->endLine += this->top - 1;

        // define region (LN_CTLI)
        spu[written++] = (reg->startLine >> 8);
        spu[written++] = reg->startLine & 0xff;
        spu[written++] = (((reg->openSections() & 0x0f) << 4)| ((reg->endLine >> 8) & 0x0f));
        spu[written++] = reg->endLine & 0xff;

        for (uint8_t j = 0; j < reg->openSections(); j++) {

            sSection *sec = reg->section(j);

            // we also need to modify startColumn value
            sec->startColumn += this->left;

            // define section (PXCTLI)
            spu[written++] = (sec->startColumn >> 8) & 0x0f;
            spu[written++] = sec->startColumn & 0xff;

            // write color and alpha
            writeColorAndAlpha(sec, false);
        }
    }

    spu[written++] = 0x0f;  // end of parameter area
    spu[written++] = 0xff;
    spu[written++] = 0xff;
    spu[written++] = 0xff;

    // update size of 0x07 command
    int32_t s = written - size_fill - 1;

    spu[size_fill++] = s >> 8;
    spu[size_fill++] = s & 0xff;
}

void cSpuEncoder::generateColorPalette()
{
    // we need to convert the color we get from
    // vdr, because it is stored in AARRGGBB
    // and we need AA and RRGGBB separated to
    // be able to convert color to yuv and set
    // wanted opacity vales later on.

    memset(&palcolors, 0, sizeof(palcolors));

    for (int i = 0; i < numColors; i++) {
        // separate AA and RRGGBB values
        tColor opac = (colors[i] & 0xff000000) >> 24;
        tColor color = (colors[i] & 0x00ffffff);

        opacity[i] = opac * 0xf / 0xff;

        // convert RRGGBB to BBGGRR
        color = ((color & 0x0000ff) << 16) | (color & 0x00ff00) | ((color & 0xff0000) >> 16);

        // convert to YCrCb
        palcolors[i] = Tools::Rgb2YCrCb(color);
    }

    // upload color palette
    cDxr3Interface::instance()->SetPalette(palcolors);
}

void cSpuEncoder::generateSpuData(bool topAndBottom) throw (char const* )
{
    // reset pointer
    p = &spu[4];

    int bottomStart;

    // copy rle encoded data into spu buffer
    if (topAndBottom) {

        memcpy(p, rleData.top, rleData.topLen);
        p += rleData.topLen;

        memcpy(p, rleData.bottom, rleData.bottomLen);
        p += rleData.bottomLen;

        bottomStart = 4 + rleData.topLen;
        written = bottomStart + rleData.bottomLen;

    } else {

        memcpy(p, rleData.top, rleData.topLen);
        p += rleData.topLen;

        bottomStart = 4;
        written = bottomStart + rleData.topLen;
    }

    // update size of pixel block
    spu[2] = written >> 8;
    spu[3] = written & 0xff;

    int x1 = written;

    // display duration
    spu[written++] = 0x00;
    spu[written++] = 0x00;      // duration before turn on command occurs (will not be used)

    // x1
    spu[written++] = x1 >> 8;   // since this is the last command block, this
    spu[written++] = x1 & 0xff; // points back to itself

    // 0x00: force displaying
    spu[written++] = CMD_FORCE_DISPLAYING;

    // 0x05: coordinates
    uint32_t right = left + bitmap->Width() - 1;
    uint32_t bottom = top + bitmap->Height() - 1;

    dsyslog("[dxr3-spuencoder] top %d left %d", top, left);

    spu[written++] = CMD_SET_DISPLAYAREA;
    spu[written++] = left >> 4;
    spu[written++] = ((left & 0xf) << 4) + (right >> 8);
    spu[written++] = (right & 0xff);
    spu[written++] = top >> 4;
    spu[written++] = ((top & 0xf) << 4) + (bottom >> 8);
    spu[written++] = (bottom & 0xff);

    // 0x06: both fields' offsets
    spu[written++] = CMD_SET_PIXEL_ADDRESES;
    spu[written++] = 0x00;      // even start at index 4
    spu[written++] = 0x04;
    spu[written++] = bottomStart >> 8;
    spu[written++] = bottomStart & 0xff;

    // write informations for other regions
    if (regions.size() > 1) {
        writeRegionInformation();
    } else {
        // write color-> palette index and alpha data for the first region
        writeColorAndAlpha(regions.front()->section(0), true);
    }

    // 0xff: end sequence
    spu[written++] = 0xff;
    if (!(written & 1)) {
        spu[written++] = 0xff;
    }

    // write overall packet size
    spu[0] = written >> 8;
    spu[1] = written & 0xff;
}

void cSpuEncoder::clearRegions()
{
    for (size_t i = 0; i < regions.size(); i++) {
        delete regions[i];
    }
    regions.clear();
}

void cSpuEncoder::calculateRegions()
{
    // create temporay bitmap
    data = new cBitmap(bitmap->Width(), bitmap->Height(), bitmap->Bpp(), bitmap->X0(), bitmap->Y0());

    // we go through the whole bitmap and define
    // spuregions and do remapping.

    for (uint16_t y = 0; y < bitmap->Height(); /* y gets incremented in algorithgm */) {

        cSpuRegion *reg = new cSpuRegion(y);
        regions.push_back(reg);

        tIndex prevIndex = 0xff;

        // in this part of the algorithm, we go through one
        // line of the osd and define sections for the current
        // region.

        for (uint16_t x = 0; x < bitmap->Width(); x++) {

            // get current color
            tIndex index = *(bitmap->Data(x, y));

            if (prevIndex != index) {

                // try to add it to current section
                if (!reg->addIndex(index)) {

                    // its time to create a new section
                    // but before we continoue bring old
                    // section into a well defined state
                    reg->section(reg->openSections() - 1)->endColumn = x;

                    // add a new section
                    if (!reg->newSection()) {
                        // TODO: what now?
                        dsyslog("out of sections");
                    }

                    // set some start values for new section
                    reg->section(reg->openSections() - 1)->startColumn = x;
                    reg->addIndex(index);
                }

                prevIndex = index;
            }
        }

        // go for sure that we have set an endColumn value. this might happen
        // if only one section was needed.
        reg->section(reg->openSections() - 1)->endColumn = bitmap->Width();

        // next we need to find the optimal endLine for the current
        // region. this is done by growing each section and then use
        // the minimal value for the endLine

        uint16_t endLine = 0;
        bool doneLoop = false;

        for (uint8_t i = 0; i < reg->openSections(); i++) {
            sSection *sec = reg->section(i);
            //dsyslog("section %d", i);

            for (uint16_t y = reg->startLine; !doneLoop && y < bitmap->Height(); y++) {
                for (uint16_t x = sec->startColumn; !doneLoop && x < sec->endColumn; x++) {

                    tIndex idx = *(bitmap->Data(x, y));

                    if (!reg->containsIndex(idx)) {

                        if (endLine == 0) {
                            endLine = y;
                        } else {
                            endLine = std::min(endLine, y);
                        }

                        doneLoop = true;
                    }
                }
            }

            if (endLine == reg->startLine) {
                endLine++;
            }

            // we reached the end of the bitmap
            if (!doneLoop) {
                endLine = bitmap->Height();
            }
        }

        reg->endLine = endLine;

        // in the last step we need to map old index values
        // to new one. this is needed as the index value in the
        // bitmap can only be 2 bits big (see rle method).

        //dsyslog("opensections %d", reg->openSections());
        for (uint8_t i = 0; i < reg->openSections(); i++) {
            sSection *sec = reg->section(i);

            for (uint16_t y = reg->startLine; y < reg->endLine; y++) {
                for (uint16_t x = sec->startColumn; x < sec->endColumn; x++) {
                    tIndex idx = *(bitmap->Data(x, y));
                    data->SetIndex(x, y, sec->cmap[idx]);
                }
            }

            // we reached the end of the bitmap
            if (!doneLoop) {
                endLine = bitmap->Height();
            }
        }

        // update y
        y = endLine;
    }

    bitmap = data;
}

void cSpuEncoder::rle4colors()
{
    int len;
    p = rleData.top;

    // first encode all top lines (0, 2, 4, ...) followed by
    // all bottom lines (1, 3, 5, ...)

    for (int i = 0; i < 2; i++) {

        nholder = 0;
        ncnt = 0;
        written = 0;

        if (i == 1) {
            p = rleData.bottom;
        }

        for (int y = (0 + i); y < bitmap->Height(); y += 2) {

            for (int x = 0; x < bitmap->Width(); x += len) {
                tIndex color = *(bitmap->Data(x, y));

                for (len = 1; x+len < bitmap->Width(); ++len) {
                    if (*(bitmap->Data(x+len, y)) != color) {
                        break;
                    }
                }

                if (len < 0x04) {
                    writeNibble((len << 2) | color);
                } else if (len < 0x10) {
                    writeNibble(len >> 2);
                    writeNibble((len << 2) | color);
                } else if (len < 0x40) {
                    writeNibble(0);
                    writeNibble(len >> 2);
                    writeNibble((len << 2) | color);
                } else if (x + len == bitmap->Width()) {
                    writeNibble(0);
                    writeNibble(0);
                    writeNibble(0);
                    writeNibble(color);
                } else {
                    if (len > 0xff) {
                        len = 0xff;
                    }
                    writeNibble(0);
                    writeNibble(len >> 6);
                    writeNibble(len >> 2);
                    writeNibble((len << 2) | color);
                }
            }

            // end of line
            if (ncnt & 1) {
                writeNibble(0);
            }
        }

        if (i == 0) {
            rleData.topLen = written;
        } else {
            rleData.bottomLen = written;
        }
    }
}

