
#pragma once

#define FAKE_OWL_VERSION_MAJOR 0
#define FAKE_OWL_VERSION_MINOR 1
#define FAKE_OWL_VERSION_PATCH 0

#define FAKE_OWL_VERSION (FAKE_OWL_VERSION_MAJOR * 100 + FAKE_OWL_VERSION_MINOR * 10 + FAKE_OWL_VERSION_PATCH)

//-------------------------------------------------------------------------------------------------
// Convenience macros to obtain version info
// FAKE_OWL_VERSION_{EQ|NEQ|LT|LE|GT|GE} (==|!=|<|<=|>|>=)
//
// Use like:
//
// #if FAKE_OWL_VERSION_GE(0,0,1)
//     // Use features that require fakeOwl version 0.0.1 or newer
// #endif
//

#define FAKE_OWL_VERSION_EQ(MAJOR, MINOR, PATCH)                    \
       (FAKE_OWL_VERSION_MAJOR == MAJOR)                            \
    && (FAKE_OWL_VERSION_MINOR == MINOR)                            \
    && (FAKE_OWL_VERSION_PATCH == PATCH)

#define FAKE_OWL_VERSION_NEQ(MAJOR, MINOR, PATCH)                   \
     !( FAKE_OWL_VERSION_EQ(MAJOR, MINOR, PATCH) )

#define FAKE_OWL_VERSION_LT(MAJOR, MINOR, PATCH)                    \
       (FAKE_OWL_VERSION_MAJOR < MAJOR)                             \
    || ( (FAKE_OWL_VERSION_MAJOR == MAJOR)                          \
      && (FAKE_OWL_VERSION_MINOR < MINOR) )                         \
    || ( (FAKE_OWL_VERSION_MAJOR == MAJOR)                          \
      && (FAKE_OWL_VERSION_MINOR == MINOR)                          \
      && (FAKE_OWL_VERSION_PATCH < PATCH) )

#define FAKE_OWL_VERSION_LE(MAJOR, MINOR, PATCH)                    \
       ( FAKE_OWL_VERSION_LT(MAJOR, MINOR, PATCH) )                 \
    || ( FAKE_OWL_VERSION_EQ(MAJOR, MINOR, PATCH) )

#define FAKE_OWL_VERSION_GT(MAJOR, MINOR, PATCH)                    \
    !( FAKE_OWL_VERSION_LE(MAJOR, MINOR, PATCH) )

#define FAKE_OWL_VERSION_GE(MAJOR, MINOR, PATCH)                    \
    !( FAKE_OWL_VERSION_LT(MAJOR, MINOR, PATCH) )


//-------------------------------------------------------------------------------------------------
// Base version of Optix-OWL, i.e., the version that the headers are based off
//

#define FAKE_OWL_BASE_OWL_VERSION_MAJOR 1
#define FAKE_OWL_BASE_OWL_VERSION_MINOR 1
#define FAKE_OWL_BASE_OWL_VERSION_PATCH 5
#define FAKE_OWL_BASE_OWL_VERSION_COMMIT "5d901c86e567e6a55f910d0d6188e86f0c303eb9"


