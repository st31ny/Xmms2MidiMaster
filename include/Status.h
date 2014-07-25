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
 * @brief   Class provided to exchange time, playback status and song id
 *          between the XMMS2 client and the MIDI master
 *
 * An instance of this class represents a complete xmms2 status. Thus, it should not
 * be fragmented nor contain only updated fields.
 * The status object holds two time points enabling linear extrapolation of the local time
 * (i.e. "When will xyz ms have passed?"). Local time = system time on this machine.
 */
class Status
{
    public:
        /**
         * @brief   Playback status
         */
        enum EPlaybackStatus
        {
            EPS_STOPPED,            ///< Playback stopped
            EPS_PAUSED,             ///< Playback paused
            EPS_PLAYING,            ///< Playing
        };


        /**
         * @brief   Default Constructor
         */
        Status() : _iState( EPS_STOPPED ), _ilSongId( 0 ), 
                   _grTime1( TimePointInvalid ), _grTime2( TimePointInvalid )
                {}

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
         *              Current xmms2 playing position in ms.
         * @param   lLocalTime
         *              Time point when playing position notification was received
         *
         * There are always to time points stored, so invoking this method replaces
         * the oldest one.
         */
        void setTime( XTimePoint lXTime, LTimePoint lLocalTime )
        {
            // push back the former newest time pair
            _grTime2 = std::move( _grTime1 );
            // store new time point
            _grTime1 = TimePoint( lXTime, lLocalTime );
        }

        /**
         * @brief   Get the older time point
         * @return  TimePoint holding the xmms2 playback position ("first") and the
         *          corresponding local time ("second")
         */
        TimePoint getTimeOld() const
        {
            return _grTime2;
        }

        /**
         *  @brief  Get the newer time point
         *  @return TimePoint. See {@link Status::getTimeOld()} for details.
         */
        TimePoint getTimeNew() const
        {
            return _grTime1;
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

        // time points for linear extrapolation
        // _grTime1 >= _grTime2
        TimePoint               _grTime1;
        TimePoint               _grTime2;
};

#endif // ifndef _STATUS_H_

