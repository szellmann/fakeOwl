
#pragma once

#include <vector>

#include <owl/owl_host.h>

#include "Geom.h"
#include "Group.h"
#include "Visionaray.h"

namespace fake
{
    class GeomGroup : public Group
    {
    public:
        GeomGroup(OWLGeomKind kind, std::size_t numGeoms, OWLGeom* geoms);
       ~GeomGroup();

        void buildAccel();
        void refitAccel();

        const visionaray::GroupBVH& getAccel() const;

    private:
        OWLGeomKind kind;

        std::vector<Geom*> geoms;

        visionaray::GroupBVH bvh;
    };

} // fake
