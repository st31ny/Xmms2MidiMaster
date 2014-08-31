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

#include "MidiMaster.h"


MidiMaster::MidiMaster( const Config& config, Exchange<Status>& ex ) :
    _config( config ), _grStatusExchange( ex )
{
    _cStatusValid = 0;
    
    _cScheduleTime = 150;
    
    if( config.getFPS() == Config::EMTF_NONE )
    {
        _FPS = 0;
        _bFPS = 0;
    } else
    if( config.getFPS() == Config::EMTF_25 )
    {
        _FPS = 25;
        _bFPS = 0x20;
    }
    else
        throw std::runtime_error( std::string( "Sorry, only 25 FPS supported yet." ) );

    if( _FPS )
        _QXTimeT = ( 1000 / _FPS ) / 4;
    _cFrame = 0;
    _lTimepoint = 0;
    
    // start PortMidi
    PmError iErr;
    if( ( iErr = Pm_OpenOutput( &_hMidiOut, config.getMidiDevice(), 0, 100, 0, 0, 1 ) ) != pmNoError )
        throw std::runtime_error( std::string( "Unable to open midi device: " ) + Pm_GetErrorText( iErr ) );

    _lTimeInt_dL = 1;
    _lTimeInt_dX = 1;
    _lTimeInt_n = Now();

    _iNextTimeSlot = Now();

}

void MidiMaster::run()
{
    while( 1 )
    {
        // read status packets
        _grStatusOld = std::move( _grStatusNew );
        _grStatusNew = _grStatusExchange.read();
        _cStatusValid++;

        // detect state transistion 
        const Status::EPlaybackStatus& iStateOld = _grStatusOld.getPlaybackStatus(),
            iStateNew = _grStatusNew.getPlaybackStatus();

        if( iStateOld == Status::EPS_INVALID &&
                ( iStateNew == Status::EPS_PLAYING || iStateNew == Status::EPS_PAUSED ) )
        { // init
            if( _config.beVerbose() )
                std::cout << "send init" << std::endl;
            
            songStart();
            updateTimeYIntercept();
            // enqueueFrames();
        } else
        if( iStateOld == Status::EPS_PLAYING &&
                iStateNew == Status::EPS_PAUSED )
        { // play -> pause

        } else
        if( iStateOld == Status::EPS_PAUSED &&
                iStateNew == Status::EPS_PLAYING )
        { // pause -> play
            updateTimeYIntercept(); // xmms2 time was paused
        } else
        if( ( iStateOld == Status::EPS_PLAYING || iStateOld == Status::EPS_PAUSED ) &&
                iStateNew == Status::EPS_STOPPED )
        { // play/pause -> stop
            if( _config.beVerbose() )
                std::cout << "play->stop" << std::endl;
            sendStopId( _grStatusOld.getSongId() );
            _cFrame = 0;
            _cStatusValid = 0;
            sendAbs( 0 );
        } else
        if( iStateOld == Status::EPS_STOPPED &&
                iStateNew == Status::EPS_PLAYING )
        { // stop -> play
            if( _config.beVerbose() )
                std::cout << "stop->play" << std::endl;
            songStart();
            updateTimeYIntercept(); // xmms2 was paused
            enqueueFrames();
            _cStatusValid = 1;
        } else
        if( iStateNew == Status::EPS_PLAYING )
        { // playing
            int cFrame;
            // song id changed?
            if( _grStatusNew.getSongId() != _grStatusOld.getSongId() )
            {
                sendStopId( _grStatusOld.getSongId() );
                songStart();
                updateTimeYIntercept();
            } else
            if( ( cFrame = frameNrAt( _grStatusNew.getTime().xtime ) ) > _cFrame ||
                    cFrame < frameNrAt( _grStatusOld.getTime().xtime ) ) // jump detection
            {
                if( _config.beVerbose() )
                    std::cout << "Jump detected: " << _cFrame << "->" << cFrame << std::endl;
                sendAbs( cFrame );
                _cFrame = cFrame;
                updateTimeYIntercept();
                _cStatusValid = 1; // first valid package after jump received
            }

            // update time extrapolation if enough valid packages have arrived
            if( _cStatusValid > 1 )
                updateTimeInt();
            
            // enqueue Q-frames if neccessary
            enqueueFrames();
        }
    }

}
 
void MidiMaster::updateTimeInt()
{
    const TimePoint& t1 = _grStatusOld.getTime();
    const TimePoint& t2 = _grStatusNew.getTime();

    if( t1 == TimePointInvalid && t2 == TimePointInvalid )
        return;

    // only update if times a valid
    if( t2.xtime <= t1.xtime )
        return;

    _lTimeInt_dL = t2.ltime - t1.ltime;
    _lTimeInt_dX = t2.xtime - t1.xtime;

    updateTimeYIntercept();
}

void MidiMaster::updateTimeYIntercept()
{
    const TimePoint& t2 = _grStatusNew.getTime();
    if( t2 == TimePointInvalid )
        return;
    // n = localtime + m * (-xmms2time)
    _lTimeInt_n = t2.ltime - ( _lTimeInt_dL * t2.xtime + _lTimeInt_dX / 2 ) / _lTimeInt_dX;
}



LTimePoint MidiMaster::timeInt( XTimePoint xtime )
{
    return _lTimeInt_n + ( _lTimeInt_dL * xtime + _lTimeInt_dX / 2 ) / _lTimeInt_dX;
}

int MidiMaster::frameNrAt( XTimePoint xtime )
{
    return ( xtime * _FPS ) / 1000; // xtime is in milliseconds
}

void MidiMaster::sendAbs( int iFrame )
{
    if( !_FPS ) return;
    BSDTime grBSD = getBSDTime( iFrame );
    MidiByte rgbMsg[] = { 0xF0, 0x7F, 0x7F, 0x01, 0x01, grBSD.hour,
        grBSD.minute, grBSD.second, grBSD.frame, 0xF7, 0x00 };
    
    Pm_WriteSysEx( _hMidiOut, _iNextTimeSlot, rgbMsg );
}

void MidiMaster::sendStopId( XSongId iXSongId )
{
    if( _config.beVerbose() )
        std::cout << "send stop id of song #" << iXSongId << std::endl;
    MidiMsg rgb = _config.endNotifier().getMsg( iXSongId );
    if( rgb )
    {
        Pm_WriteShort( _hMidiOut, _iNextTimeSlot, rgb );
    }
}

void MidiMaster::sendStartId( XSongId iXSongId )
{
    if( _config.beVerbose() )
        std::cout << "send start id of song #" << iXSongId << std::endl;
    MidiMsg rgb = _config.beginNotifier().getMsg( iXSongId );
    if( rgb )
    {
        Pm_WriteShort( _hMidiOut, _iNextTimeSlot, rgb );
    }
}

void MidiMaster::enqueueFrames()
{
    if( !_FPS ) return;
    while( 1 )
    {
        // enqueue a complete timestamp = 8 quaterframes = 2 frames

        // start time of this frame
        XTimePoint xtime = ( _cFrame * 1000 + _FPS / 2 ) / _FPS;
        if( timeInt( xtime ) -  Now() > _cScheduleTime )
            // there is still enough time to schedule the frames later
            return;
        // ensure non-decreasing times (neccessary for jumps) ???
        while( timeInt( xtime ) < _iNextTimeSlot ) ++xtime;

        BSDTime grBSD = getBSDTime( _cFrame );
        // quater frames: data pieces
        MidiByte rgbMsg[] = { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70 };
        rgbMsg[ 0 ] |= grBSD.frame & 0x0F;
        rgbMsg[ 1 ] |= grBSD.frame >> 4;
        rgbMsg[ 2 ] |= grBSD.second & 0x0F;
        rgbMsg[ 3 ] |= grBSD.second >> 4;
        rgbMsg[ 4 ] |= grBSD.minute & 0x0F;
        rgbMsg[ 5 ] |= grBSD.minute >> 4;
        rgbMsg[ 6 ] |= grBSD.hour & 0x0F;
        rgbMsg[ 7 ] |= ( grBSD.hour >> 4 & 0x01 ) | ( _bFPS >> 4 );

        PtTimestamp when;
        for( unsigned int i = 0; i < 8; ++i, xtime += _QXTimeT )
        {
            when = timeInt( xtime );
            Pm_WriteShort( _hMidiOut, when, 0xF1 | ( rgbMsg[ i ] << 8 ) );
        }

        // increase frame counter
        _cFrame += 2;
        // remember latest time sent to PortMIDI to ensure non-decreasing timestamps
        _iNextTimeSlot = when;
    }
}

void MidiMaster::songStart()
{
    sendStartId( _grStatusNew.getSongId() );
    _cFrame = ( _grStatusNew.getTime().xtime * _FPS ) / 1000;
    //_cFrame = 0;
    sendAbs( _cFrame );
}



MidiMaster::BSDTime MidiMaster::getBSDTime( int iFrame ) const
{
    BSDTime grBSD;
    grBSD.frame = iFrame % _FPS;
    grBSD.second = iFrame / _FPS;
    grBSD.minute = grBSD.second / 60;
    grBSD.second %= 60;
    grBSD.hour = grBSD.minute / 60;
    grBSD.minute %= 60;
    grBSD.hour %= 24;
    grBSD.hour |= _bFPS; // add frame rate information

    return grBSD;
}


