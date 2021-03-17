// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#include <string>

#include "Resource.h"

namespace fake
{
    // Program

    class Module;

    struct Program : ContextResource
    {
        enum Type { PtBounds, PtAnyHit, PtClosestHit, PtMiss, PtIntersect, PtRayGen, };

        virtual ~Program();

        virtual std::string entryPointPrefix() = 0;

        void build();

        bool isValid() const;

        // Module the program belongs to
        Module* module;

        // Type of the program
        Type type;

        // Name of the program
        const char* name;

        // Symbol handle of program entry point
        void* entryPointSym = nullptr;

        // Symbol handle of set_launchIndex function
        void* fakeSetLaunchIndexSym = nullptr;

        // Symbol handle of set_launchDims function
        void* fakeSetLaunchDimsSym = nullptr;

        // Symbol to reset the state to the previous program
        void* fakeResetPreviousProgramStateSym = nullptr;
    };

} // fake
