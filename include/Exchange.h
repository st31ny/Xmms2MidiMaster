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

#ifndef _EXCHANGE_H_
#define _EXCHANGE_H_

#include <mutex>
#include <condition_variable>

/**
 * @brief   Class provinding a simple mechanism to exchange messages between threads.
 *
 * This is accomplished using a condition_variable. When trying to read the message, the call
 * is blocked until there is a message ready to read. Multiple senders/receivers are allowed,
 * but each message is considered new only once. Only one message can be cached at a time (no FIFO).
 */
template<class T>
class Exchange
{
    public:

        /**
         * @brief   Default Constructor
         */
        Exchange() : _fReady( false )
        {
        }

        /**
         * @brief   Constructor with explicit data
         * @param   Message
         * @param   True to mark message as ready to read
         */
        Exchange( const T& data, bool fReady = true ) : _data( data ), _fReady( fReady )
        {
        }
        
        Exchange( const Exchange& ) = delete;
        Exchange& operator=( const Exchange& ) = delete;

        /**
         * @brief   Set the message which will be delivered to a waiting reader or on the next
         *          call to {@link read()}
         * @param   Message
         * @return  Former state of the ready flag (if true the last message had not been delivered
         *          to anybody)
         */
        bool write( const T& data )
        {
            std::unique_lock<std::mutex> lock( _mutex );
            bool fReadyOld = _fReady;
            _fReady = true;
            _data = data;
            _monitor.notify_one();
            return fReadyOld;
        }

        /**
         * @brief   Read the current message
         * @param   If true only message marked as new is returned. Thus, the call might be 
         *          blocking if no new message is available yet.
         * @return  Current message
         */
        T read( bool fNew = true )
        {
            std::unique_lock<std::mutex> lock( _mutex );
            if( fNew )
                while( !_fReady ) _monitor.wait( lock );
            _fReady = false; // mark message as read
            return _data;
        }


    private:
        T                               _data;
        bool                            _fReady;
        std::mutex                      _mutex;
        std::condition_variable         _monitor;
};


#endif //ifndef _EXCHANGE_H_

