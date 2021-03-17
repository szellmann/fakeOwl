#include <cstring>

#include "DataType.h"
#include "Logging.h"
#include "Params.h"

namespace fake
{
    Params::Params(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars)
        : VarDecl(sizeOfVarStruct, vars, numVars)
    {
    }

} // fake
