// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <iostream>

#include "Logging.h"

//-------------------------------------------------------------------------------------------------
// Terminal colors
//

#define FAKE_TERMINAL_RED     "\033[0;31m"
#define FAKE_TERMINAL_GREEN   "\033[0;32m"
#define FAKE_TERMINAL_YELLOW  "\033[1;33m"
#define FAKE_TERMINAL_RESET   "\033[0m"
#define FAKE_TERMINAL_DEFAULT FAKE_TERMINAL_RESET
#define FAKE_TERMINAL_BOLD    "\033[1;1m"


namespace fake
{
    namespace logging
    {
        Stream::Stream(Level level)
            : level_(level)
        {
        }

        Stream::~Stream()
        {
            if (level_ == Level::Info)
                std::cout << FAKE_TERMINAL_GREEN << stream_.str() << FAKE_TERMINAL_RESET << '\n';
            else if (level_ == Level::Warning)
                std::cout << FAKE_TERMINAL_YELLOW << stream_.str() << FAKE_TERMINAL_RESET << '\n';
            else if (level_ == Level::Error)
                std::cout << FAKE_TERMINAL_RED << stream_.str() << FAKE_TERMINAL_RESET << '\n';
        }
    } // logging
} // fake
