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
#include <stdexcept>

#include <thread>

#include <portmidi.h>

#include "Exchange.h"
#include "Config.h"
#include "Status.h"
#include "XmmsClient.h"
#include "MidiMaster.h"

int main( int argc, char* argv[] )
{
    // PortMidi warm up
    Pm_Initialize();
    std::cout << argv[ 0 ] << " Copyright (C) 2014 Maximilian Stein"
              << "\nVersion: " << VERSION << std::endl;
    
    int lRet = 0; // return code
    try {
        Config config( argc, argv );
        if( !config )
            throw 1;

        Exchange<Status> grStatusExchange;
        try {
            XmmsClient client( config, grStatusExchange );
            MidiMaster master( config, grStatusExchange );
            std::thread thMaster( &MidiMaster::run, std::ref( master ) );
            thMaster.detach();
            client.run(); // blocking
        }
        catch( std::runtime_error& err )
        {
            std::cerr << err.what() << std::endl;
            throw 2;
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

