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
        Fixture() : consttarget( target )
        {

        }

        ~Fixture() {}

        Status target;
        const Status& consttarget;
    };

    TEST_FIXTURE( Fixture, PlaybackStatus )
    {

        // initial state: stopped
        CHECK_EQUAL( consttarget.getPlaybackStatus(), Status::EPS_STOPPED );

        target.setPlaybackStatus( Status::EPS_PLAYING );
        CHECK_EQUAL( consttarget.getPlaybackStatus(), Status::EPS_PLAYING );

        target.setPlaybackStatus( Status::EPS_PAUSED );
        CHECK_EQUAL( consttarget.getPlaybackStatus(), Status::EPS_PAUSED );
    }

    TEST_FIXTURE( Fixture, SongID )
    {
        // initial song id: 0
        CHECK_EQUAL( consttarget.getSongId(), 0 );

        target.setSongId( 42 );
        CHECK_EQUAL( consttarget.getSongId(), 42 );
    }

    TEST_FIXTURE( Fixture, Time )
    {
        // initial time: TimePointInvalid
        CHECK( consttarget.getTimeNew() == TimePointInvalid );
        CHECK( consttarget.getTimeOld() == TimePointInvalid );

        // set time => old time stays invalid, new time is updated
        LTimePoint ltp = std::chrono::high_resolution_clock::now();
        target.setTime( 0, ltp );
        CHECK( consttarget.getTimeNew() == TimePoint( 0, ltp ) );
        CHECK( consttarget.getTimeOld() == TimePointInvalid );

        // set time again
        LTimePoint ltp2 = std::chrono::high_resolution_clock::now();
        target.setTime( 1, ltp2 );
        CHECK( consttarget.getTimeNew() == TimePoint( 1, ltp2 ) );
        CHECK( consttarget.getTimeOld() == TimePoint( 0, ltp ) );
    }

}

