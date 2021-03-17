#include "Bounds.h"
#include "ClosestHit.h"
#include "Geom.h"
#include "Intersect.h"
#include "Logging.h"
#include "Program.h"

namespace fake
{

    // Private constructor
    Geom::Geom(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars,
               const std::vector<Program*>& anyHit,
               const std::vector<ClosestHit*>& closestHit)
        : VarDecl(sizeOfVarStruct, vars, numVars)
        , anyHit(anyHit)
        , closestHit(closestHit)
    {
    }

    Geom::~Geom()
    {
    }

    void Geom::setPrimCount(std::size_t pc)
    {
        primCount = pc;
    }

    std::size_t Geom::getPrimCount() const
    {
        return primCount;
    }

    ClosestHit* Geom::getClosestHitProg(int rayType) const
    {
        if (rayType >= closestHit.size())
        {
            FAKE_LOG(fake::logging::Level::Error) << "rayType " << rayType
                                                  << " exceeds rayType count:"
                                                  << closestHit.size();
            return nullptr;
        }

        return closestHit[rayType];
    }


    // Private constructor
    UserGeom::UserGeom(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars,
                       Bounds* bounds,
                       const std::vector<Program*>& anyHit,
                       const std::vector<ClosestHit*>& closestHit,
                       const std::vector<Intersect*>& intersect)
        : Geom(sizeOfVarStruct, vars, numVars, anyHit, closestHit)
        , bounds(bounds)
        , intersect(intersect)
    {
    }

    UserGeom::~UserGeom()
    {
    }

    Bounds* UserGeom::getBoundsProg() const
    {
        return bounds;
    }

    Intersect* UserGeom::getIntersectProg(int rayType) const
    {
        if (rayType >= intersect.size())
        {
            FAKE_LOG(fake::logging::Level::Error) << "rayType " << rayType
                                                  << " exceeds rayType count:"
                                                  << intersect.size();
            return nullptr;
        }

        return intersect[rayType];
    }



    TrianglesGeom::TrianglesGeom(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars,
                                 const std::vector<Program*>& anyHit,
                                 const std::vector<ClosestHit*>& closestHit)
        : Geom(sizeOfVarStruct, vars, numVars, anyHit, closestHit)
    {
    }

    TrianglesGeom::~TrianglesGeom()
    {
    }

    void TrianglesGeom::setVertices(const Buffer* buf,
                                    std::size_t count,
                                    std::size_t stride,
                                    std::size_t offset)
    {
        vertices = buf;
        vertexCount = count;
        vertexStride = stride;
        vertexOffset = offset;
    }

    void TrianglesGeom::setIndices(const Buffer* buf,
                                   std::size_t count,
                                   std::size_t stride,
                                   std::size_t offset)
    {
        primCount = count;

        indices = buf;
        indexCount = count;
        indexStride = stride;
        indexOffset = offset;
    }

    std::size_t TrianglesGeom::getIndexCount() const
    {
        return indexCount;
    }

    std::size_t TrianglesGeom::getVertexCount() const
    {
        return vertexCount;
    }

    owl::vec3i TrianglesGeom::getIndex(std::size_t index)
    {
        return *(const owl::vec3i*)((const uint8_t*)indices->dataPtr + indexOffset + index * indexStride); // offset * stride?
    }

    owl::vec3f TrianglesGeom::getVertex(std::size_t indirectIndex)
    {
        return *(const owl::vec3f*)((const uint8_t*)vertices->dataPtr + vertexOffset + indirectIndex * vertexStride); // offset * stride?
    }
} // fake
