
#pragma once

#include <cstdint>
#include <vector>

#include <owl/owl_host.h>

#include "Bounds.h"
#include "ClosestHit.h"
#include "Intersect.h"
#include "Resource.h"
#include "VarDecl.h"

namespace fake
{
    class Geom;

    class GeomType : public ContextResource, private VarDecl // VarDecl is only a template
    {
    public:
        GeomType(OWLGeomKind kind, std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars);

        Geom* createGeom();
        void releaseGeom(Geom* geom);

        void setBoundsProg(Bounds* bo);
        Bounds* getBoundsProg() const;

        void setAnyHitProg(Program* ah, int rayType);

        void setClosestHitProg(ClosestHit* ch, int rayType);

        void setIntersectProg(Intersect* is, int rayType);
        Intersect* getIntersectProg() const;

    private:
        OWLGeomKind kind;

        Bounds* bounds = nullptr;
        std::vector<Program*> anyHit; // TODO: prob. store derived classes here!
        std::vector<ClosestHit*> closestHit; // TODO: prob. store derived classes here!
        std::vector<Intersect*> intersect;

    };

} // fake
