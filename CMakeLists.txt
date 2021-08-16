
project(fakeOwl)

cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FAKE_OWL_BUILD_SAMPLES "Build the Samples?" ON)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/submodules/visionaray/include)

add_subdirectory(src)

set(fakeOwl_LIBRARY ${fakeOwl_LIBRARY} PARENT_SCOPE)

#--- test application -------------------------------------

include(cmake/configure_fake_owl.cmake)
set(owl_dir ${PROJECT_SOURCE_DIR})
add_subdirectory(cuteeOwl)
include_directories(${QT_OWL_INCLUDES})
# find_package(CUDA)
include_directories(${CUDA_INCLUDE_DIRS})
fake_owl_compile_and_embed(
  embedded_deviceCode deviceCode.cpp
  )
add_executable(test Renderer.cpp viewer.cpp mesh.cpp ${embedded_deviceCode})
target_link_libraries(test ${fakeOwl_LIBRARY})
QT_OWL_LINK(test)


#--- samples ----------------------------------------------
if (FAKE_OWL_BUILD_SAMPLES)
  include_directories(samples/common/3rdParty)
  add_subdirectory(samples/int01-simpleTriangles)
  add_subdirectory(samples/int06-mixedGeometries)
  add_subdirectory(samples/int10-texturedTriangles)
  add_subdirectory(samples/int11-rotatingBoxes)
  add_subdirectory(samples/int15-cookBilliardScene)
  # add_subdirectory(samples/s05-rtow)
  add_subdirectory(samples/s08-sierpinski)
  # add_subdirectory(samples/owlDVR)
  # add_subdirectory(samples/owlPhotonMapping)
  # add_subdirectory(samples/owlptdvr)
endif()
