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

#include <iostream>

#include <portmidi.h>

#include "Config.h"

int main( int argc, char* argv[] )
{
    // PortMidi warm up
    Pm_Initialize();
    std::cout << argv[ 0 ] << "Copyright (C) 2014 Maximilian Stein"
              << "\nVersion: " << VERSION << std::endl;
    
    int lRet = 0; // return code
    try {
        Config config( argc, argv );
        if( !config )
            throw 1;

        std::cout << "Send song ids (-1 to exit) [hex]:\n";
        for( ; ; )
        {
            int ilId;
            std::cout << "ID=";
            std::cin >> ilId;
            std::cout << "BEGIN=" << std::hex << config.beginNotifier().getMsg( ilId ) << '\n'
                      << "END=  " << std::hex << config.endNotifier().getMsg( ilId )
                      << std::endl;
            if( ilId == -1 )
                break;
        }
    }
    catch( int& l )
    {
        lRet = l;
    }

    std::cout << "Stop." << std::endl;


    // PortMidi shutdown
    Pm_Terminate();
    return lRet;
}

