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

#include "SongIdNotifier.h"

SongIdNotifier::SongIdNotifier(
        ESongIdNotifierCommand   bCmd,
        MidiByte                 bChannel,
        bool                     fLE,
        const IdMap&             mpllId,
        int                      dlId
    ) : _mpllId( mpllId ), _dlId( dlId ), _rgbStatus( MIDI_STATUS_BYTE( bCmd, bChannel ) ), _fLE( fLE )
{
}

MidiMsg SongIdNotifier::getMsg( int ilSongId ) const
{
    if( _mpllId.count( ilSongId ) > 0 ) // direct mapping available
        ilSongId = _mpllId.at( ilSongId );
    else // no direct mapping, so use the offset
        ilSongId += _dlId;

    if( _fLE )
        return MIDI_MSG_SHORT( _rgbStatus, ilSongId, ilSongId >> 7 );
    return MIDI_MSG_SHORT( _rgbStatus, ilSongId >> 7, ilSongId );
}

