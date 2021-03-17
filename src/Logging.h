// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#include <ostream>
#include <sstream>

namespace fake
{
    namespace logging
    {
        enum class Level
        {
            Error,
            Warning,
            Info,
        };

        class Stream
        {   
        public:
            Stream(Level level);
           ~Stream();

            inline std::ostream& stream()
            {
                return stream_;
            }

        private:
            std::ostringstream stream_;
            Level level_;

        };

        class NullStream
        {
        public:
            NullStream() = default;

            template <typename T>
            NullStream& operator<<(T) { return *this; }
        };

    } // logging
} // fake


#define FAKE_LOG(LEVEL) ::fake::logging::Stream(LEVEL).stream()

#ifndef NDEBUG
#define FAKE_LOG_DBG ::fake::logging::Stream(::fake::logging::Level::Info).stream()
#else
#define FAKE_LOG_DBG ::fake::logging::NullStream()
#endif
