
#include "Context.h"
#include "Geom.h"
#include "GeomType.h"
#include "Logging.h"

namespace fake
{
    GeomType::GeomType(OWLGeomKind kind, std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars)
        : VarDecl(sizeOfVarStruct, vars, numVars)
        , kind(kind)
    {
    }

    Geom* GeomType::createGeom()
    {
        if (kind == OWL_GEOMETRY_USER || kind == OWL_GEOM_USER)
            return new UserGeom(sizeOfVarStruct,
                                inputVars.data(),
                                inputVars.size(),
                                bounds,
                                anyHit,
                                closestHit,
                                intersect);

        else if (kind == OWL_GEOMETRY_TRIANGLES || kind == OWL_GEOM_TRIANGLES)
            return new TrianglesGeom(sizeOfVarStruct,
                                     inputVars.data(),
                                     inputVars.size(),
                                     anyHit,
                                     closestHit);

        return nullptr;
    }

    void GeomType::releaseGeom(Geom* geom)
    {
        FAKE_LOG_DBG << "Releasing geom";

        delete geom;
    }

    void GeomType::setBoundsProg(Bounds* bo)
    {
        bounds = bo;
    }

    Bounds* GeomType::getBoundsProg() const
    {
        return bounds;
    }

    void GeomType::setIntersectProg(Intersect* is, int rayType)
    {
        if (rayType >= context->getRayTypeCount())
        {
            FAKE_LOG(fake::logging::Level::Error) << "rayType " << rayType
                                                  << " exceeds rayType count:"
                                                  << context->getRayTypeCount();
            return;
        }

        intersect.resize(context->getRayTypeCount());

        intersect[rayType] = is;
    }

    void GeomType::setClosestHitProg(ClosestHit* ch, int rayType)
    {
        if (rayType >= context->getRayTypeCount())
        {
            FAKE_LOG(fake::logging::Level::Error) << "rayType " << rayType
                                                  << " exceeds rayType count:"
                                                  << context->getRayTypeCount();
            return;
        }

        closestHit.resize(context->getRayTypeCount());

        closestHit[rayType] = ch;
    }
}
