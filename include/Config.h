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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

#include <portmidi.h>

#include "typedefs.h"
#include "SongIdNotifier.h"

/**
 * @brief   Parse and validate command line options/config files provided for
 *          the other parts of the program.
 */
class Config {
    public:
        /**
         * @brief   MIDI timecode frame rates
         */
        enum EMidiTimecodeFramerate
        {
            EMTF_NONE,      ///< do not emit any timecode
            EMTF_24,        ///< standard film (24 FPS)
            EMTF_25,        ///< PAL video (25 FPS)
            EMTF_2997,      ///< NTSC drop-frame (29.97 FPS)
            EMTF_30,        ///< NTSC non-drop (30 FPS)
        };

        /**
         * @brief   Constructor. Parse argc/argv, read config files and print usage message(s).
         * @param   argc
         * @param   argv
         */
        Config( int argc, char* argv[] );

        /**
         * @brief   Indicate if parsing succeeded
         * @return  True if parsing succeeded, otherwise false
         */
        operator bool() const
        {
            return _fOk;
        }

        /**
         * @brief   Get the MIDI timecode framerate
         * @return  Element of EMidiTimecodeFramerate
         */
        EMidiTimecodeFramerate getFPS() const
        {
            return _iFPS;
        }

        /**
         * @brief   Get the song begin notifier to send MIDI messages when a song begins
         * @return  Reference to a {@link SongIdNotifier}
         */
        const SongIdNotifier& beginNotifier() const
        {
            return _grIdNotifierBegin;
        }

        /**
         * @brief   Get the song end notifier to send MIDI messages when a song ends
         * @return  Reference to a {@link SongIdNotifier}
         */
        const SongIdNotifier& endNotifier() const
        {
            return _grIdNotifierEnd;
        }

        /**
         * @brief   Get the MIDI device ID
         * @return  Device ID for portmidi
         */
        PmDeviceID getMidiDevice() const
        {
            return _iDevice;
        }

        /**
         * @brief   Get the XMMS2 path
         * @return  Path to connect to (e.g. tcp path)
         *
         * If present, the environment variable XMMS_PATH is used. This can be overridden
         * by specifying the option on the command line. If neither the environment variable
         * nor the command line option is present, an empty string is returned to make XMMS2
         * use its default path.
         */
        const std::string& getXmmsPath() const
        {
            return _szXmmsPath;
        }

        /**
         * @brief   Indicate if we shall be verbose
         * @return  True if verbosity requested
         */
        bool beVerbose() const
        {
            return _fVerbose;
        }

    private:

        bool                    _fOk; // indicate if parsing succeeded

        bool                    _fVerbose;

        IdMap                   _mpllId;
        EMidiTimecodeFramerate  _iFPS;
        SongIdNotifier          _grIdNotifierBegin;
        SongIdNotifier          _grIdNotifierEnd;
        
        PmDeviceID              _iDevice;
        std::string             _szXmmsPath;
};

#endif // ifndef _CONFIG_H_

