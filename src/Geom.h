
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <owl/owl_host.h>

#include "Buffer.h"
#include "Resource.h"
#include "VarDecl.h"

namespace fake
{
    class Bounds;
    class ClosestHit;
    class Intersect;
    class Miss;
    class Program;

    class Geom : public ContextResource, public VarDecl
    {
    public:
        friend class GeomType; // can create

    protected:
        Geom(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars,
             const std::vector<Program*>& anyHit,
             const std::vector<ClosestHit*>& closestHit);
        virtual ~Geom();

    public:
        void setPrimCount(std::size_t primCount);

        std::size_t getPrimCount() const;

        ClosestHit* getClosestHitProg(int rayType) const;

    protected:
        std::size_t primCount = 0;

        std::vector<Program*> anyHit;
        std::vector<ClosestHit*> closestHit;
    };


    class UserGeom : public Geom
    {
    public:
        friend class GeomType; // can create

        Bounds* getBoundsProg() const;
        Program* getAnyHitProg(int rayType) const;
        Intersect* getIntersectProg(int rayType) const;

    private:
        UserGeom(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars,
                 Bounds* bounds,
                 const std::vector<Program*>& anyHit,
                 const std::vector<ClosestHit*>& closestHit,
                 const std::vector<Intersect*>& intersect);
       ~UserGeom();

        Bounds* bounds = nullptr;
        std::vector<Intersect*> intersect;
    };


    class TrianglesGeom : public Geom
    {
    public:
        friend class GeomType; // can create

        void setVertices(const Buffer* buf,
                         std::size_t count,
                         std::size_t stride,
                         std::size_t offset);

        void setIndices(const Buffer* buf,
                        std::size_t count,
                        std::size_t stride,
                        std::size_t offset);

        std::size_t getIndexCount() const;
        std::size_t getVertexCount() const;

        owl::vec3i getIndex(std::size_t index);
        owl::vec3f getVertex(std::size_t indirectIndex);

    private:
        TrianglesGeom(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars,
                      const std::vector<Program*>& anyHit,
                      const std::vector<ClosestHit*>& closestHit);
       ~TrianglesGeom();

        const Buffer* vertices = nullptr;
        std::size_t vertexCount = 0;
        std::size_t vertexStride = 0;
        std::size_t vertexOffset = 0;

        const Buffer* indices = nullptr;
        std::size_t indexCount = 0;
        std::size_t indexStride = 0;
        std::size_t indexOffset = 0;

    };
} // fake
