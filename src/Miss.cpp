
#include "Miss.h"

namespace fake
{
    Miss::Miss(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars)
        : VarDecl(sizeOfVarStruct, vars, numVars)
    {
    }

    std::string Miss::entryPointPrefix()
    {
        return "__miss__";
    }
} // fake
