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

#include "SongIdNotifier.h"

int main( int argc, char* argv[] )
{
    std::cout << "Version: " << VERSION << std::endl;

    IdMap map;
   
    SongIdNotifier grNote( map );

    std::cout << "Mapping as a/b input - maps a onto b (-1 to end input):\n";
    for( ; ; ) {
        int a,b;
        std::cout << "a=";
        std::cin >> a;
        if( a == -1 )
            break;
        std::cout << "b=";
        std::cin >> b;
        map[ a ] = b;
    }

    int dlId;
    std::cout << "Offset: ";
    std::cin >> dlId;
    grNote.setSongIdOffset( dlId );

    int bCmd;
    std::cout << "MIDI-Command to send (80, 90, A0, B0) [all hex]: ";
    std::cin >> std::hex >> bCmd;
    grNote.setMidiCommand( static_cast<SongIdNotifier::ESongIdNotifierCommand>( bCmd ) );

    MidiByte bChannel;
    int lT;
    std::cout << "MIDI-Channel [hex]: ";
    std::cin >> lT;
    bChannel = lT;
    grNote.setMidiChannel( bChannel );

    bool fLE;
    std::cout << "Little Endian: ";
    std::cin >> fLE;
    grNote.setEndian( fLE );

    std::cout << "Send song ids (-1 to exit) [hex]:\n";
    for( ; ; )
    {
        int ilId;
        std::cout << "ID=";
        std::cin >> ilId;
        std::cout << "MIDI=" << std::hex << grNote.getMsg( ilId ) << std::endl;
        if( ilId == -1 )
            break;
    }


    return 0;
}

