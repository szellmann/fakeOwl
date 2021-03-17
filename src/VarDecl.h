// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <owl/owl_host.h>

namespace fake
{
    class Buffer;
    class Group;
    class Texture;

    // Data type that manages variable declarations

    class VarDecl
    {
    public:
        VarDecl() = default;

        VarDecl(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars);

        virtual ~VarDecl();

        // Size of the variable declaration struct
        std::size_t sizeOfVarStruct = 0;

        // Copy of the input variable declarations
        std::vector<OWLVarDecl> inputVars;

        // Storage for the variable declarations
        void* dataPtr = nullptr;

    public:
        void set(const char* varName, bool b1);
        void set(const char* varName, bool b1, bool b2);
        void set(const char* varName, bool b1, bool b2, bool b3);
        void set(const char* varName, bool b1, bool b2, bool b3, bool b4);

        void set(const char* varName, int32_t i1);
        void set(const char* varName, int32_t i1, int32_t i2);
        void set(const char* varName, int32_t i1, int32_t i2, int32_t i3);
        void set(const char* varName, int32_t i1, int32_t i2, int32_t i3, int32_t i4);

        void set(const char* varName, uint32_t ui1);
        void set(const char* varName, uint32_t ui1, uint32_t ui2);
        void set(const char* varName, uint32_t ui1, uint32_t ui2, uint32_t ui3);
        void set(const char* varName, uint32_t ui1, uint32_t ui2, uint32_t ui3, uint32_t ui4);

        void set(const char* varName, uint64_t ui1);
        void set(const char* varName, uint64_t ui1, uint64_t ui2);
        void set(const char* varName, uint64_t ui1, uint64_t ui2, uint64_t ui3);
        void set(const char* varName, uint64_t ui1, uint64_t ui2, uint64_t ui3, uint64_t ui4);

        void set(const char* varName, float f1);
        void set(const char* varName, float f1, float f2);
        void set(const char* varName, float f1, float f2, float f3);
        void set(const char* varName, float f1, float f2, float f3, float f4);

        void set(const char* varName, const void* pointer);

        void set(const char* varName, Buffer* buffer);

        void set(const char* varName, Group* group);

        void set(const char* varName, Texture* texture);
    protected:
        OWLVarDecl findVariable(const char* name);

    };

} // fake
