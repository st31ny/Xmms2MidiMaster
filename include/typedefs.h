/*  Xmms2MidiMaster - XMMS2-Client emitting MIDI timecode to synchronize arbitrary MIDI-capable devices
 *  Copyright (C) 2014  Maximilian Stein
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

/**
 * @file    typedefs.h
 * @brief   Global type and macro definitions
 */

#include <map>
#include <utility>

#ifndef _PORTTIME_H_
#define _PORTTIME_H_
#include <porttime.h>
#endif

/**
 * @brief   xmms2 song id
 */
typedef int                                     XSongId;

/**
 * @brief   Invalid xmms2 song id
 */
static const XSongId                            XSongIdInvalid = 0;

/**
 * @brief   song id sent over midi
 */
typedef int                                     MSongId;

/**
 * @brief   Type for mapping XMMS2 song ids to MIDI notes to emit on
 *          song change.
 */
typedef std::map<XSongId, MSongId>              IdMap;

/**
 * @brief   Entry of a {@link IdMap}
 */
typedef std::pair<XSongId, MSongId>             IdMapEntry;

/**
 * @brief   Complete MIDI message
 */
typedef unsigned long                           MidiMsg;

/**
 * @brief   A MIDI data byte
 */
typedef unsigned char                           MidiByte;

/**
 * @brief   xmms2 playback time
 */
typedef int                                     XTimePoint;

/**
 * @brief   Invalid xmms2 playback time value
 */
static const XTimePoint                         XTimePointInvalid = 0;

/**
 * @brief   System clock to use
 */
//typedef std::chrono::high_resolution_clock      Clock;

/**
 * @brief   Representation of a local time
 */
typedef PtTimestamp                             LTimePoint;

/**
 * @brief   Get current local time
 * @return  LTimePoint representing the current local time
 */
static inline LTimePoint Now()
{
    return Pt_Time();
}

/**
 * @brief   Invalid local time value
 */
static const LTimePoint                         LTimePointInvalid = 0;

/**
 * @brief   Time pair holding the xmms2 time and the local time point
 */
struct TimePoint {
    /**
     * @brief   Constructor
     * @param   x
     *              Xmms2 time
     * @param   l
     *              Local time
     */
    TimePoint( XTimePoint x = XTimePointInvalid, LTimePoint l = LTimePointInvalid ) :
        xtime( x ), ltime( l ) {}

    /**
     * @brief   Xmms2 time (arithmetic type)
     */
    XTimePoint                                  xtime;
    
    /**
     * @brief   Local time (arithmetic type)
     */
    LTimePoint                                  ltime;

    /**
     * @brief   Comparison operator
     * @param   other
     *              Another TimePoint
     * @return  True if this object and other represent the same time point
     */
    bool operator==( const TimePoint& other ) const
    {
        return ( xtime == other.xtime ) && ( ltime == other.ltime );
    }
};

/**
 * @brief   Invalid time point
 */
static const TimePoint                          TimePointInvalid = TimePoint( XTimePointInvalid,
                                                                        LTimePointInvalid );

/**
 * @brief   Build a status byte
 * @param   cmd
 *              MIDI command (0x80, 0x90, ...)
 * @param   channel
 *              MIDI channel
 */
#define MIDI_STATUS_BYTE(cmd,channel)           ( ((cmd)&0xF0) | ((channel)&0x0F) )

/**
 * @brief   Build a short MIDI message
 * @param   status
 *              Status byte
 * @param   data1
 *              Data byte 1
 * @param   data2
 *              Data byte 2
 */
#define MIDI_MSG_SHORT(status,data1,data2)      ((((data2) << 16) & 0x7F0000) | \
                                                    (((data1) << 8) & 0x7F00) | \
                                                    ((status) & 0xFF))

/**
 * @brief   Check if a MIDI short message is valid
 */
#define MIDI_MSG_SHORT_VALID(msg)               ( ((msg)&0x80) && ((~(msg))&0x8000) && \
                                                    ((~(msg))&0x800000) && ((~(msg))&0xFF000000) )

#endif // ifndef _TYPEDEFS_H_

