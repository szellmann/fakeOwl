
#pragma once

#include <cstddef>

#include <visionaray/math/intersect.h>
#include <visionaray/math/matrix.h>
#include <visionaray/math/ray.h>

#include <owl/owl.h>
#include <fake/owl.h>

#include "Geom.h"

namespace fake
{
    struct Module;

    //--- Ray with payload --------------------------------

    struct Payload
    {
        // Registers for the "real" payload
        unsigned* p0;
        unsigned* p1;

        unsigned rayType;

        unsigned instID;

        OptixVisibilityMask visibilityMask;
        unsigned rayFlags;

        float tmax;

        visionaray::mat4x3 objectToWorldTransform;
        visionaray::mat4x3 worldToObjectTransform;

        visionaray::vec3 worldRayOrigin;
        visionaray::vec3 worldRayDirection;
    };

    struct Ray : visionaray::basic_ray<float>
    {
        Payload* payload;
    };

    struct HitRecord : visionaray::hit_record<Ray, visionaray::primitive<unsigned>>
    {
        unsigned instID = ~0U;

        visionaray::mat4x3 objectToWorldTransform;
        visionaray::mat4x3 worldToObjectTransform;

        visionaray::vec3 objectRayOrigin;
        visionaray::vec3 objectRayDirection;

        unsigned hitKind = 0U;

        const Geom* geom = nullptr;
        OWLGeomKind kind;
    };

    //--- Traversable -------------------------------------

    struct Traversable
    {
        TraversableHandle handle;
        const void* accessor = nullptr;
    };

    TraversableHandle registerTraversable(Traversable traversable);

    Traversable& getTraversable(TraversableHandle handle);

    void traversablesSetCurrentModule(Module* module);

    Traversable* getTraversableList();

    std::size_t getNumTraversables();

} // fake
