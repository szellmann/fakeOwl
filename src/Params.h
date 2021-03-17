
#pragma once

#include <cstdint>

#include <owl/owl_host.h>

#include "Resource.h"
#include "VarDecl.h"

namespace fake
{
    struct Params : ContextResource, VarDecl
    {
        Params(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars);
    };

} // fake
