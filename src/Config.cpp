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

#include "Config.h"

namespace std {
    /**
     * @brief   Read IdMap items from an istream
     * @param   in
     *              istream object to read from
     * @param   mpall
     *              IdMap object to write (key,value) pairs to
     * @return  in
     */
    std::istream& operator>>( std::istream& in, IdMapEntry& mapll );
}

/**
 * @brief   Parser for response file option
 * @see     http://www.boost.org/doc/libs/1_36_0/doc/html/program_options/howto.html#id3453181
 */
std::pair<std::string, std::string> at_option_parser( std::string const& sz );

namespace po = ::boost::program_options;

/**
 * @brief   Helper function to parse options for the SongIdNotifier
 * @param   mpszgr
 *              Map with read options to parse
 * @param   szName
 *              Name of the SongIdNotifier. Either 'begin' or 'end'
 * @param   gr
 *              Reference to the SongIdNotifier to write to.
 * @return  True if successful, otherwise false
 */
bool _parseSongIdNotifierOptions( po::variables_map mpszgr, std::string szName, SongIdNotifier& gr );

Config::Config( int argc, char* argv[] ) :
    _fOk( false ),
    _fVerbose( false ),
    _grIdNotifierBegin( _mpllId ),
    _grIdNotifierEnd( _mpllId )
{
    po::options_description grDesc( "Available options" );
    grDesc.add_options()
        ( "help,h", "Show this message and exit" )
        ( "verbose,v", "Show more detailed messages" )
        ( "list,l", "Show available MIDI output devices and their IDs, and exit" )
        ( "response-file", po::value<std::string>(), "Load response file with \"@file\".\nAttention: Short options in response files must not be followed by a whitespace. However, long options are always followed by a whitespace." )
        
        ( "device,d", po::value<PmDeviceID>(&_iDevice)->default_value( Pm_GetDefaultOutputDeviceID() ), "Set the MIDI device number to use. This must be an output device. See also option \"-l\"." )
        ( "xmms-path,x", po::value<std::string>( &_szXmmsPath )->default_value( std::getenv( "XMMS_PATH" ) ? : "" ), "Override the environment variable XMMS_PATH. If neither the environment variable nor this option is present, connect to XMMS2's default path." )

        ( "fps,f", po::value<std::string>()->default_value( "film" ), "Set frame rate. One of \n \"film\" (24 fps)\n \"pal\" (25 fps)\n \"ntscd\" (29.97 fps)\n \"ntsc\" (30 fps)" )
        
        ( "map,m", po::value< std::vector<IdMapEntry> >()->composing(), "<XMMS2 ID>:<custom ID>\nMap a XMMS2 song ID onto a custom ID emitted when a song begins or ends" )
        ( "offset,o", po::value<int>()->default_value( 0 ), "Add this offset to the XMMS2 song ID if no direct mapping (\"-m\") is available" )
        
        ( "begin-status,s", po::value<std::string>()->default_value( "none" ), "Set the MIDI status to send when a song begins.\nOne of:\n \"none\" \t(do not send these messages)\n \"noteoff\" \t(send NOTE OFF messages)\n \"noteon\" \t(send NOTE ON messages)\n \"pa\" \t(send POLYPHONIC AFTERTOUCH messages)\n \"cc\" \t(send CONTROL CHANGE messages)" )
        ( "end-status,S", po::value<std::string>()->default_value( "none" ), "Set the MIDI status to send when a song ends (stop or song change). See \"-s\" for details." )
        ( "begin-channel,c", po::value<int>()->default_value( 1 ), "Set the MIDI channel to send when a song begins. Between 1 and 16." )
        ( "end-channel,C", po::value<int>()->default_value( 1 ), "Set the MIDI channel to send when a song ends. Between 1 and 16.")
        ( "begin-littleendian,e", "Use little endian for song ID encoding in song begin messages." )
        ( "end-littleendian,E", "Use little endian for song ID encoding in song end messages." )
        
        ;

    // parse options
    po::variables_map mpszgr;
    try {
        po::store( po::command_line_parser( argc, argv ).options( grDesc ).
                extra_parser( at_option_parser ).run(), mpszgr );
    }
    catch( po::error& e )
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    // load response file
    if( mpszgr.count( "response-file" ) )
    {
        std::ifstream fl( mpszgr[ "response-file" ].as<std::string>().c_str() );
        if( !fl )
        {
            std::cerr << "Could not open response file." << std::endl;
            return;
        }

        std::stringstream rgch;
        rgch << fl.rdbuf();
        
        // split string at tokens
        boost::char_separator<char> rgchSep( " \n\t\r" );
        boost::tokenizer< boost::char_separator<char> > grTok( rgch.str(), rgchSep );
        
        std::vector<std::string> rgszArgs;
        std::copy( grTok.begin(), grTok.end(), std::back_inserter( rgszArgs ) );

        // finally parse arguments
        try {
            po::store( po::command_line_parser( rgszArgs ).options( grDesc ).
                    extra_parser( at_option_parser ).run(), mpszgr );
        }
        catch( po::error& e )
        {
            std::cerr << e.what() << std::endl;
            return;
        }
    }

    
    po::notify( mpszgr );

    // read and verify passed options
    if( mpszgr.count( "help" ) )
    {
        std::cout << grDesc << std::endl;
        return;
    }

    if( mpszgr.count( "verbose" ) )
        _fVerbose = true;
    
    if( mpszgr.count( "list" ) )
    {
        // print all output devices
        int cl = Pm_CountDevices();
        for( int il = 0; il < cl; ++il )
        {
            const PmDeviceInfo* pgrInfo;
            pgrInfo = Pm_GetDeviceInfo( il );

            if( pgrInfo->output )
            {
                std::cout << "[" << il << "] " << pgrInfo->name << " (" << pgrInfo->interf << ")\n";
            }
        }
        std::cout << std::endl;
        return;
    }

    if( mpszgr.count( "fps" ) )
    {
        std::string szFps = mpszgr[ "fps" ].as<std::string>();
        if( szFps == "film" )
        {
            _iFPS = EMTF_24;
        } else
        if( szFps == "pal" )
        {
            _iFPS = EMTF_25;
        } else
        if( szFps == "ntscd" )
        {
            _iFPS = EMTF_2997;
        } else
        if( szFps == "ntsc" )
        {
            _iFPS = EMTF_30;
        } else
        {
            std::cerr << "Frame rate invalid." << std::endl;
            return;
        }
    }

    // verfiy MIDI port (must be an output device)
    {
        const PmDeviceInfo* pgrInfo;
        if( ( _iDevice >= Pm_CountDevices() ) ||
                ( !( pgrInfo = Pm_GetDeviceInfo( _iDevice ) )->output ) )
        {
            std::cerr << "No valid MIDI ouput device selected.\n"
                      << "Call \"" << argv[ 0 ] << " -l\" to get a list of valid device IDs."
                      << std::endl;
            return;
        }
       
        if( _fVerbose )
            std::cout << "select MIDI device [" << _iDevice << "] " << pgrInfo->name
                      << " (" << pgrInfo->interf << ")\n";
    }

    // print XMMS_PATH if requested
    if( _fVerbose )
    {
        if( _szXmmsPath.size() > 0 )
            std::cout << "select XMMS2 path \"" << _szXmmsPath << "\"\n";
        else
            std::cout << "select default XMMS2 path\n";
    }

    // build ID map
    if( mpszgr.count( "map" ) )
    {
        std::vector< IdMapEntry > rggrMpllId = mpszgr[ "map" ].as< std::vector< IdMapEntry > >();
        for( std::vector< IdMapEntry >::const_iterator igr = rggrMpllId.begin(),
                igrMax = rggrMpllId.end(); igr != igrMax; ++igr )
            _mpllId[ igr->first ] = igr->second;
    }

    // parse SongIdNotifiers
    if( ! ( _parseSongIdNotifierOptions( mpszgr, "begin", _grIdNotifierBegin ) &&
            _parseSongIdNotifierOptions( mpszgr, "end", _grIdNotifierEnd ) ) )
    {
        std::cerr << "See \"" << argv[ 0 ] << " -h\" for details." << std::endl;
        return;
    }

    if( _fVerbose )
    {
        std::cout << "song ID mapping:\n";
        for( typename IdMap::const_iterator i = _mpllId.begin(); i != _mpllId.end(); ++i )
        {
            std::cout << i->first << " => " << i->second << '\n';
        }
        std::cout << std::flush;
    }

    _fOk = true;
}

std::istream& std::operator>>( std::istream& in, IdMapEntry& empll )
{
    namespace po = ::boost::program_options;
    
    // match a single map entry
    boost::regex entry( "(\\d*):(\\d*)" );

    // get the string to parse
    std::string sz;
    in >> sz;

    // match regexp, extract ints and put them into the map
    boost::smatch grMatch;
    if( boost::regex_match( sz, grMatch, entry ) )
    {
        // convert parts to int using program option's conversion function
        int lKey = boost::lexical_cast<int>( grMatch[ 1 ] );
        int lVal = boost::lexical_cast<int>( grMatch[ 2 ] );

        // write key & value
        empll.first = lKey;
        empll.second = lVal;
    } else
    {
        throw po::validation_error( po::validation_error::invalid_option_value );
    }

    return in;
}

bool _parseSongIdNotifierOptions( po::variables_map mpszgr, std::string szName, SongIdNotifier& gr )
{
    if( mpszgr.count( szName + "-status" ) )
    {
        std::string szVal = mpszgr[ szName + "-status" ].as<std::string>();
        if( szVal == "none" )
        {
            gr.setMidiCommand( SongIdNotifier::ESINC_NONE );
            return true; // sending messages is not requested, so we do not need the rest
        }
        if( szVal == "noteoff" )
        {
            gr.setMidiCommand( SongIdNotifier::ESINC_NOTEOFF );
        } else
        if( szVal == "noteon" )
        {
            gr.setMidiCommand( SongIdNotifier::ESINC_NOTEON );
        } else
        if( szVal == "pa" )
        {
            gr.setMidiCommand( SongIdNotifier::ESINC_PA );
        } else
        if( szVal == "cc" )
        {
            gr.setMidiCommand( SongIdNotifier::ESINC_CC );
        } else
        {
            std::cerr << "Invalid MIDI status byte passed." << std::endl;
            return false;
        }
    }
    
    if( mpszgr.count( "offset" ) )
    {
        // use the same offset for both, the song begin and the song end notifier
        int dl = mpszgr[ "offset" ].as<int>();
        gr.setSongIdOffset( dl );
    }
    
    if( mpszgr.count( szName + "-channel" ) )
    {
        // SongIdNotifier wants physical channels, so subtract 1
        int b = mpszgr[ szName + "-channel" ].as<int>() - 1;
        if( b < 0 || b > 15 )
        {
            std::cerr << "MIDI channel must be between 1 and 16." << std::endl;
            return false;
        }
        gr.setMidiChannel( b );
    }
    
    if( mpszgr.count( szName + "-littleendian" ) )
    {
        gr.setEndian( true );
    }

    return true;
}

std::pair<std::string, std::string> at_option_parser( std::string const& sz )
{
    if( '@' == sz[ 0 ] )
        return std::make_pair( std::string( "response-file" ), sz.substr( 1 ) );
    return std::pair< std::string, std::string >();
}

