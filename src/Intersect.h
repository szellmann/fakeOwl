
#pragma once

#include "Program.h"

namespace fake
{
    struct Intersect : Program
    {
        std::string entryPointPrefix();

        int rayType = 0;

        // Symbol to prepare an intersection program call
        void* fakePrepareIntersectionSym = nullptr;

        // Symbo to obtain result from intersection program call
        void* fakeGetIntersectionResultSym = nullptr;

    };
} // fake
