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

#ifndef _XMMSCLIENT_H_
#define _XMMSCLIENT_H_

#include <string>
#include <iostream>

#include <xmmsclient/xmmsclient++.h>

#include "Config.h"
#include "StatusExchange.h"

/**
 * @brief   Class receiving all required XMMS2 events (song id, playback status, time)
 */
class XmmsClient
{
    public:
        /**
         * @brief   Constructor
         * @param   config
         *              Config object
         * @param   grEx
         *              StatusExchange object to write results to
         * @throws  std::runtime_error
         */
        XmmsClient( const Config& config, volatile StatusExchange& grEx );

        /**
         * @brief   Main loop. Register all signals and broadcasts and loop indefinitely
         */
        void run();

        /**
         * @brief   Receive current playtime
         * @param   lTime
         *              Current playback position in ms
         * @return  True to receive the signal again (will always be true)
         */
        bool signalPlaytime( const int& lTime );

        /**
         * @brief   Receive current song id
         * @param   ilSongId
         *              Current XMMS2 song id
         * @return  True to continue receiving this broadcast (will always be true)
         */
        bool broadcastId( const int& ilSongId );

        /**
         * @brief   Receive current playback status
         * @param   iState
         *              New playback state
         * @return  True to continue receiving this broadcast (will always be true)
         */
        bool broadcastStatus( const Xmms::Playback::Status& iState );

        /**
         * @brief   Error handler for XMMS2 calls
         * @param   szMsg
         *              Error message
         * @return  False (has no effect)
         */
        bool errorHandler( const std::string& szMsg );

    private:
        Xmms::Client                _client;
        const Config&               _config;
        volatile StatusExchange&    _grEx;
};

#endif // ifndef _XMMSCLIENT_H_

