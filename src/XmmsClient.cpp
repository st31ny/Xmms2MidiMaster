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


XmmsClient::XmmsClient( const Config& config, volatile StatusExchange& grEx )
    : _client( "XmmsMidiMaster" ), _config( config ), _grEx( grEx )
{
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
    _client.playback.signalPlaytime()( Xmms::bind( &XmmsClient::signalPlaytime, this ) );
    _client.playback.broadcastCurrentID()( Xmms::bind( &XmmsClient::broadcastId, this ) );
    _client.playback.broadcastStatus()( Xmms::bind( &XmmsClient::broadcastStatus, this ) );

    if( _config.beVerbose() )
        std::cout << "enter XMMS2 main loop" << std::endl;

    _client.getMainLoop().run();

    if( _config.beVerbose() )
        std::cout << "leave XMMS2 main loop" << std::endl;
}

bool XmmsClient::signalPlaytime( const int& lTime )
{

    _grEx.setTime( lTime );

    return true;
}

bool XmmsClient::broadcastId( const int& ilSongId )
{
    _grEx.setSongId( ilSongId );

    if( _config.beVerbose() )
        std::cout << "new song id: " << ilSongId << std::endl;

    return true;
}

bool XmmsClient::broadcastStatus( const Xmms::Playback::Status& iState )
{
    switch( iState )
    {
        case Xmms::Playback::STOPPED:
            _grEx.setPlaybackStatus( StatusExchange::EPS_STOPPED );
            if( _config.beVerbose() )
                std::cout << "new status: STOPPED" << std::endl;
            break;
        case Xmms::Playback::PLAYING:
            _grEx.setPlaybackStatus( StatusExchange::EPS_PLAYING );
            if( _config.beVerbose() )
                std::cout << "new status: PLAYING" << std::endl;
            break;
        case Xmms::Playback::PAUSED:
            _grEx.setPlaybackStatus( StatusExchange::EPS_PAUSED );
            if( _config.beVerbose() )
                std::cout << "new status: PAUSED" << std::endl;
            break;
    }

    return true;
}

bool XmmsClient::errorHandler( const std::string& szMsg )
{
    std::cerr << "XMMS2 Error: " << szMsg << std::endl;
    return false;
}

