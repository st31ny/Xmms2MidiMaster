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

#ifndef _MIDIMASTER_H_
#define _MIDIMASTER_H_

#include <string>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>

#include <portmidi.h>
#ifndef _PORTTIME_H_
#define _PORTTIME_H_
#include <porttime.h>
#endif

#include "typedefs.h"
#include "Exchange.h"
#include "Config.h"
#include "Status.h"

/**
 * @brief   Responsible for emitting MIDI commands
 *
 * This includes all calculation necessary to produce correct MIDI time code
 * according to the configuration.
 * Basic strategy: Measure ticks when playtime is refreshed and extrapolate the playtime
 */
class MidiMaster
{
    private:
        /**
         * @brief   Struct for BSD time
         */
        struct BSDTime
        {
            MidiByte hour;                  ///< Hour including frame rate information
            MidiByte minute;                ///< Minute
            MidiByte second;                ///< Second
            MidiByte frame;                 ///< Frame
        };

    public:
        /**
         * @brief   Constructor
         * @param   config
         *              Config object
         * @param   ex
         *              Status exchange object to get playback information from
         * @throws  std::runtime_error
         *
         * The constructor will also open the midi device.
         */
        MidiMaster( const Config& config, Exchange<Status>& ex );

        /**
         * @brief   Main loop. Start sending midi packets and runs infinitely
         */
        void run();

    private:
        /**
         * @brief   Update time extrapolation values
         *
         * Time points are directly extracted from _grStatus*
         */
        void updateTimeInt();

        /**
         * @brief   Adapt time extrapolation values to a changed local clock (e.g. after a pause)
         * 
         * Neccessary data is red from _grStatusNew. Speed of time advancing is not changed
         * but the y-intercept only
         */
        void updateTimeYIntercept();

        /**
         * @brief   Do time extrapolation
         * @param   xtime
         *              xmms2 time to extrapolate
         * @return  local time corresponding to xtime
         */
        LTimePoint timeInt( XTimePoint xtime );

        /**
         * @brief   Convert xmms2 time points to midi frame numbers
         * @param   xtime
         *              xmms2 time point
         * @return  midi time code frame number corresponding to a time <= xtime
         */
        int frameNrAt( XTimePoint xtime );

        /**
         * @brief   Send absolute playing position (MIDI Full time code)
         * @param   iFrame
         *              Number of frame to encode
         *
         * Absolute time positions are sent immediateley.
         */
        void sendAbs( int iFrame );

        /**
         * @brief   Send midi song stop signal
         * @param   iXSongId
         *              xmms2 song id
         */
        void sendStopId( XSongId iXSongId );

        /**
         * @brief   Send midi song start signal
         * @param   iXSongId
         *              xmms2 song id
         */
        void sendStartId( XSongId iXSongId );

        /**
         * @brief   Enqueue quarter frames
         *
         * According to frequency this function enqueues more or less frames
         * based on the time points in _grStatus*. If the functions detects a jump (i.e.
         * if it is impossible to send time codes fast enough without collision), an absolute
         * time position is enqueued before.
         */
        void enqueueFrames();

        /**
         * @brief   Do song start sequence.
         *
         *  The start sequence consists of sending a song start id, refreshing the frame counter
         *  and sending an absolute time position.
         *  All data is read from _grStatusNew.
         */
        void songStart();

        /**
         * @brief   Convert a frame number into a BSD time
         * @param   iFrame
         *              Absolute frame number to convert
         * @return  Time position as BSDTime. Hour contains frame rate information
         */
        BSDTime getBSDTime( int iFrame ) const;

    private:
        const Config&               _config;

        // save status to compare with on status updates
        Status                      _grStatusOld;
        Status                      _grStatusNew;
        int                         _cStatusValid; // number of valid statuses; after a stop/song change
                                                   // interpolation would change otherwise
        Exchange<Status>&           _grStatusExchange;
    
        // midi device
        PortMidiStream*             _hMidiOut;

        // connection parameters
        PtTimestamp                 _iNextTimeSlot; // ensure non-decreasing time stamps

        LTimePoint                  _cScheduleTime; // time to wake up before the next time code enqueuing is
                                                    // necessary. Should be about 10 ms

        int                         _FPS; // midi time code fps
        /**
         * @brief   FPS representation in bits for time code messages
         *
         * Format: 0rr00000
         * 
         * rr
         * 00 = 24 frames/s
         * 01 = 25 frames/s
         * 10 = 29.97 frames/s
         * 11 = 30 frames/s
         */
        MidiByte                    _bFPS;
        XTimePoint                  _QXTimeT; // duration of a quarter frame in xmms2 time
        int                         _cFrame; // index of next midi time code frame
        XTimePoint                  _lTimepoint; // next encoded time point

        // linear time extrapolation parameters (=dL/dX*x+n)
        LTimePoint                  _lTimeInt_dL; // local time delta (nominator of slope)
        XTimePoint                  _lTimeInt_dX; // xmms2 time delta (denominator of slope)
        LTimePoint                  _lTimeInt_n; // constant offset (y-intercept)
        
};

#endif // ifndef _MIDIMASTER_H_

