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
 * @brief   Test for class Exchange
 */

#include <thread>
#include <chrono>
#include <mutex>

#include <unittest++/UnitTest++.h>

#include "Exchange.h"

SUITE(ExchangeTest)
{
    class A
    {
        public:
            int x;
    };

    class B
    {
        public:
            int x;
            B( int i ) : x(i)
            {
            }
    };

    struct Fixture
    {
        Exchange<A> target;
    };

    TEST(ExplicitContructor1)
    {
        UNITTEST_TIME_CONSTRAINT(50);
        Exchange<B> target( B( 42 ) );

        // state should be ready => no lock
        CHECK_EQUAL( target.read().x, 42 );
    }

    TEST(ExplicitContructor2)
    {
        UNITTEST_TIME_CONSTRAINT(50);
        Exchange<B> target( B( 43 ), false );
        // now we should block, so create a thread to unblock
        std::thread thread( [&] ( )
                {
                    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
                    target.write( B( 44 ) );
                }
            );

        CHECK_EQUAL( target.read().x, 44 ); // if we get 43 we got something old
        thread.join();
    }

    TEST_FIXTURE( Fixture, WriteReadyFlag ) // test return value of write
    {
        UNITTEST_TIME_CONSTRAINT(50);
        CHECK_EQUAL( target.write( A() ), false );
        CHECK_EQUAL( target.write( A() ), true );
        target.read();
        CHECK_EQUAL( target.write( A() ), false );
    }

    TEST_FIXTURE( Fixture, ReadNonblocking )
    {
        UNITTEST_TIME_CONSTRAINT(50);
        // must not block
        target.read( false );
    }

    TEST_FIXTURE( Fixture, ReadBlockingNonblocking )
    {
        UNITTEST_TIME_CONSTRAINT(50);
        A a;
        a.x = 1;
        target.write( a );
        target.read(); // mark as read
        std::thread thread1( [&] ( )
                {
                    CHECK_EQUAL( target.read().x, 2 );
                }
            ); // thread should wait until a new message is written
        std::thread thread2( [&] ( )
                {
                    CHECK_EQUAL( target.read( false ).x, 1 );
                }
            ); // target.read() should return immediately

        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

        // now, also thread1 should return
        a.x = 2;
        target.write( a );

        thread1.join();
        thread2.join();
    }

    TEST_FIXTURE( Fixture, ReadConcurrent )
    {
        UNITTEST_TIME_CONSTRAINT(50);

        A a;
        a.x = 1;

        std::mutex mtx;
        int read = 0;

        auto fn = [&] ( )
                {
                    A ret = target.read();
                    mtx.lock();
                    if( read == 1 )
                        CHECK_EQUAL( ret.x, 2 );
                    else if( read == 2 )
                        CHECK_EQUAL( ret.x, 1 );
                    read = ret.x;
                    mtx.unlock();
                };

        std::thread thread1( fn  );
        std::thread thread2( fn  );

        target.write( a );
        a.x = 2;
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        target.write( a );

        thread1.join();
        thread2.join();
    }


}


