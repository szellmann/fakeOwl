
#pragma once

#include <vector>

#include <owl/owl_host.h>

#include "Geom.h"
#include "Group.h"

namespace fake
{
    class GeomGroup : public Group
    {
    public:
        GeomGroup(OWLGeomKind kind, std::size_t numGeoms, OWLGeom* geoms);
       ~GeomGroup();

        void buildAccel();
        void refitAccel();

        std::shared_ptr<visionaray::GroupBVH> getAccel();

    private:
        OWLGeomKind kind;

        std::vector<Geom*> geoms;

        std::shared_ptr<visionaray::GroupBVH> bvh;
    };

} // fake
