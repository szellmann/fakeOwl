
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



#--- samples ----------------------------------------------
if (FAKE_OWL_BUILD_SAMPLES)
  include(cmake/configure_fake_owl.cmake)
  add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/cuteeOwl)
  include_directories(${QT_OWL_INCLUDES})
  include_directories(samples/common/3rdParty)
  add_subdirectory(samples/fake01-rayCasting)
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
