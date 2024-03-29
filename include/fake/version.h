
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
#define FAKE_OWL_BASE_OWL_VERSION_PATCH 7
#define FAKE_OWL_BASE_OWL_VERSION_COMMIT "8c4d5f1ea0cc2a1974ed8111381c1d26e6417af8"


