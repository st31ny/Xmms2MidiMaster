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

/**
 * @brief   Test for class Status
 */

#include <unittest++/UnitTest++.h>

#include "Status.h"

SUITE(StatusTest)
{
    struct Fixture
    {
        Fixture()
        {

        }

        ~Fixture() {}

        Status target;
    };

    TEST_FIXTURE( Fixture, PlaybackStatus )
    {
        // initial state: stopped
        CHECK_EQUAL( target.getPlaybackStatus(), Status::EPS_STOPPED );

        target.setPlaybackStatus( Status::EPS_PLAYING );
        CHECK_EQUAL( target.getPlaybackStatus(), Status::EPS_PLAYING );

        target.setPlaybackStatus( Status::EPS_PAUSED );
        CHECK_EQUAL( target.getPlaybackStatus(), Status::EPS_PAUSED );
    }

}

