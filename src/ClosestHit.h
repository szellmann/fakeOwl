
#pragma once

#include "Program.h"

namespace fake
{
    struct ClosestHit : Program
    {
        std::string entryPointPrefix();

        int rayType = 0;

        // Symbol to prepare an intersection program call
        void* fakePrepareClosestHitSym = nullptr;

    };
} // fake
