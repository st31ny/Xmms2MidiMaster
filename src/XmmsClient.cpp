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

#include "XmmsClient.h"


XmmsClient::XmmsClient( const Config& config, Exchange<Status>& ex ) 
    : _client( "XmmsMidiMaster" ), _config( config ), _grStatusExchange( ex )
{
    // connect to xmms2
    if( config.getXmmsPath().size() == 0 )
        _client.connect(); // try to connect at default path
    else
        _client.connect( config.getXmmsPath().c_str() );

    if( config.beVerbose() )
        std::cout <<  "XMMS2 connection successful\n";
}

void XmmsClient::run()
{
    if( _config.beVerbose() )
        std::cout << "request XMMS2 signals and broadcasts\n";
    // register for broadcasts and request initial values (the latter is important: otherwise we won't
    // have a valid state until something changes)
    _client.playback.signalPlaytime()( Xmms::bind( &XmmsClient::signalPlaytime, this ) );
    _client.playback.getPlaytime()( Xmms::bind( &XmmsClient::signalPlaytime, this ) );
    _client.playback.broadcastCurrentID()( Xmms::bind( &XmmsClient::broadcastId, this ) );
    _client.playback.currentID()( Xmms::bind( &XmmsClient::broadcastId, this ) );
    _client.playback.broadcastStatus()( Xmms::bind( &XmmsClient::broadcastStatus, this ) );
    _client.playback.getStatus()( Xmms::bind( &XmmsClient::broadcastStatus, this ) );

    if( _config.beVerbose() )
        std::cout << "enter XMMS2 main loop" << std::endl;

    _client.getMainLoop().run();

    if( _config.beVerbose() )
        std::cout << "leave XMMS2 main loop" << std::endl;
}

bool XmmsClient::signalPlaytime( const int& lTime )
{
    // get localtime
    LTimePoint ltp = Now();

    _grStatus.setTime( lTime, ltp );

    // send status update
    _grStatusExchange.write( _grStatus );

    return true;
}

bool XmmsClient::broadcastId( const int& ilSongId )
{
    _grStatus.setSongId( ilSongId );

    // send status update
    //_grStatusExchange.write( _grStatus );

    if( _config.beVerbose() )
        std::cout << "new song id: " << ilSongId << std::endl;

    return true;
}

bool XmmsClient::broadcastStatus( const Xmms::Playback::Status& iState )
{
    Status::EPlaybackStatus iStatusOld = _grStatus.getPlaybackStatus();
    switch( iState )
    {
        case Xmms::Playback::STOPPED:
            _grStatus.setPlaybackStatus( Status::EPS_STOPPED );
            if( _config.beVerbose() )
                std::cout << "new status: STOPPED" << std::endl;
            break;
        case Xmms::Playback::PLAYING:
            _grStatus.setPlaybackStatus( Status::EPS_PLAYING );
            if( _config.beVerbose() )
                std::cout << "new status: PLAYING" << std::endl;
            break;
        case Xmms::Playback::PAUSED:
            _grStatus.setPlaybackStatus( Status::EPS_PAUSED );
            if( _config.beVerbose() )
                std::cout << "new status: PAUSED" << std::endl;
            break;
    }

    // send status update
    if( iStatusOld != Status::EPS_STOPPED )
        // after a stop, first the playback state, then the song id and finally the time broadcast is sent
        // => we cannot write the status now as long as the id and time are unknown
        _grStatusExchange.write( _grStatus );

    return true;
}

bool XmmsClient::errorHandler( const std::string& szMsg )
{
    std::cerr << "XMMS2 Error: " << szMsg << std::endl;
    return false;
}

