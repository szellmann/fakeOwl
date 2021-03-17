
set(HEADERS
    Bounds.h
    Buffer.h
    ClosestHit.h
    Context.h
    DataType.h
    Geom.h
    GeomGroup.h
    GeomType.h
    Group.h
    InstanceGroup.h
    Intersect.h
    Logging.h
    Miss.h
    Module.h
    Params.h
    Program.h
    RayGen.h
    Resource.h
    Texture.h
    Trace.h
    VarDecl.h
    Visionaray.h
    VisionarayTexture.h
)

set(SOURCES
    Bounds.cpp
    Buffer.cpp
    ClosestHit.cpp
    Context.cpp
    Geom.cpp
    GeomGroup.cpp
    GeomType.cpp
    Group.cpp
    InstanceGroup.cpp
    Intersect.cpp
    Logging.cpp
    Miss.cpp
    Module.cpp
    Params.cpp
    Program.cpp
    RayGen.cpp
    Resource.cpp
    Texture.cpp
    Trace.cpp
    VarDecl.cpp
    Visionaray.cpp
    VisionarayTexture.cpp
)

set(SOURCES
    ${SOURCES}
    fake_cuda.cpp
    owl_ext.cpp
    owl_host.cpp
)

set(fakeOwl_LIBRARY fakeOwl PARENT_SCOPE)
add_library(fakeOwl ${HEADERS} ${SOURCES})
