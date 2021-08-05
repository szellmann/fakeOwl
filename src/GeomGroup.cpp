#include "Visionaray.h" // keep on top!
#include "GeomGroup.h"
#include "Logging.h"

namespace fake
{
    GeomGroup::GeomGroup(OWLGeomKind kind, std::size_t numGeoms, OWLGeom* geoms)
        : kind(kind)
        , bvh(new visionaray::GroupBVH)
    {
        this->geoms.resize(numGeoms);

        traversableHandle = bvh->traversableHandle;

        for (std::size_t i = 0; i < numGeoms; ++i)
        {
            Geom* geom = (Geom*)geoms[i];

            if (kind == OWL_GEOMETRY_USER || kind == OWL_GEOM_USER)
            {
                if (dynamic_cast<UserGeom*>(geom) == nullptr)
                {
                    FAKE_LOG(fake::logging::Level::Error) << "User geom group cannot be created from non-user geom";
                    this->geoms.clear();
                    return;
                }
            }
            else if (kind == OWL_GEOMETRY_TRIANGLES || kind == OWL_GEOM_TRIANGLES)
            {
                if (dynamic_cast<TrianglesGeom*>(geom) == nullptr)
                {
                    FAKE_LOG(fake::logging::Level::Error) << "Triangles geom group cannot be created from non-triangles geom";
                    this->geoms.clear();
                    return;
                }
            }

            this->geoms[i] = geom;
        }
    }

    GeomGroup::~GeomGroup()
    {
    }

    void GeomGroup::buildAccel()
    {
        if (kind == OWL_GEOMETRY_USER || kind == OWL_GEOM_USER)
        {
            bvh->reset((UserGeom**)geoms.data(), geoms.size());
            bvh->build();
        }
        else if (kind == OWL_GEOMETRY_TRIANGLES || kind == OWL_GEOM_TRIANGLES)
        {
            bvh->reset((TrianglesGeom**)geoms.data(), geoms.size());
            bvh->build();
        }
    }

    void GeomGroup::refitAccel()
    {
        FAKE_LOG(fake::logging::Level::Warning) << "Refit not implemented yet, rebuilding instead!";
        // for (std::size_t i = 0; i < asUserGeom.bottomLevelBVHs.size(); ++i)
        // {
        //     asUserGeom.bottomLevelBVHs[i].refit();
        // }
    }

    visionaray::GroupBVH::SP GeomGroup::getAccel()
    {
        return bvh;
    }

} // fake
