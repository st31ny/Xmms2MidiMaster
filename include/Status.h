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

#ifndef _STATUS_H_
#define _STATUS_H_

#include <utility>

#include "typedefs.h"

/**
 * @brief   Class to exchange time, playback status and song id
 *          between the XMMS2 client and the MIDI master
 *
 * An instance of this class represents a complete xmms2 status. Thus, it should not
 * be fragmented nor contain only updated fields.
 */
class Status
{
    public:
        /**
         * @brief   Playback status
         */
        enum EPlaybackStatus
        {
            EPS_INVALID = -1,       ///< Invalid state
            EPS_STOPPED,            ///< Playback stopped
            EPS_PAUSED,             ///< Playback paused
            EPS_PLAYING,            ///< Playing
        };


        /**
         * @brief   Default Constructor
         */
        Status() : _iState( EPS_INVALID ), _ilSongId( XSongIdInvalid ), 
                   _grTime( TimePointInvalid )
        {
        }

        /**
         * @brief   Set playback status
         * @param   iState
         *              Current playback status
         */
        void setPlaybackStatus( EPlaybackStatus iState )
        {
            _iState = iState;
        }

        /**
         *  @brief  Get playback status
         *  @return Playback status. One element of {@link EPlaybackStatus}.
         */
        EPlaybackStatus getPlaybackStatus() const
        {
            return _iState;
        }

        /**
         * @brief   Set the current playing position
         * @param   lXTime
         *              Current xmms2 playing position in ms
         * @param   lLocalTime
         *              Time point when playing position notification was received
         */
        void setTime( XTimePoint lXTime, LTimePoint lLocalTime )
        {
            _grTime = TimePoint( lXTime, lLocalTime );
        }

        /**
         * @brief   Get the time point pair
         * @return  TimePoint holding the xmms2 playback position ("xtime") and the
         *          corresponding local time ("ltime")
         */
        const TimePoint& getTime() const
        {
            return _grTime;
        }

        /**
         * @brief   Set the xmms2 song id
         * @param   ilSongId
         *              Current xmms2 song id
         */
        void setSongId( XSongId ilSongId )
        {
            _ilSongId = ilSongId;
        }

        /**
         * @brief   Get the xmms2 song id
         * @return  Current xmms2 song id
         */
        XSongId getSongId() const
        {
            return _ilSongId;
        }

    private:
        // save playback state and song id
        EPlaybackStatus         _iState;
        XSongId                 _ilSongId;

        TimePoint               _grTime;
};

#endif // ifndef _STATUS_H_

