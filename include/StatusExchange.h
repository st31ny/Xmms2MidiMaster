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

#ifndef _STATUSEXCHANGE_H_
#define _STATUSEXCHANGE_H_

/**
 * @brief   Class provided to exchange time, playback status and song id
 *          between the XMMS2 client and the MIDI master
 *
 * This class is also responsible for thread synchronization. However, if one
 * thread only writes the data while the others only read the data we do not
 * need to block any thread using a mutex. This can be achieved by only giving one
 * class write access. Race conditions are not problematic to the MIDI protocol.
 */
class StatusExchange
{
    public:
        /**
         * @brief   Playback status
         */
        enum EPlaybackStatus
        {
            EPS_STOPPED = 0,        ///< Playback stopped
            EPS_PAUSED,             ///< Playback paused
            EPS_PLAYING,            ///< Playing
        };

        /**
         * @brief   Default Constructor
         */
        StatusExchange() : _iState( EPS_STOPPED ), _lTime( 0 ), _ilSongId( 0 ) {}

        /**
         * @brief   Set playback status
         * @param   iState
         *              Current playback status
         */
        void setPlaybackStatus( EPlaybackStatus iState ) volatile
        {
            _iState = iState;
        }

        /**
         * @brief   Get the playback status
         * @return  Current playback status
         */
        EPlaybackStatus getPlaybackStatus() const volatile
        {
            return _iState;
        }

        /**
         * @brief   Set the playing position
         * @param   lTime
         *              Current playing position in ms.
         */
        void setTime( int lTime ) volatile
        {
            _lTime = lTime;
        }

        /**
         * @brief   Get the playing position
         * @return  Milliseconds passed since beginning of the song
         */
        int getTime() const volatile
        {
            return _lTime;
        }

        /**
         * @brief   Set the XMMS2 song id
         * @param   ilSongId
         *              Current XMMS2 song id
         */
        void setSongId( int ilSongId ) volatile
        {
            _ilSongId = ilSongId;
        }

        /**
         * @brief   Get the XMMS2 song id
         * @return  Current XMMS2 song id
         */
        int getSongId() const volatile
        {
            return _ilSongId;
        }

    private:
        EPlaybackStatus         _iState;
        int                     _lTime;
        int                     _ilSongId;
};

#endif // ifndef _STATUSEXCHANGE_H_

