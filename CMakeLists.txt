
project(fakeOwl)

cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build all libraries as shared libraries instead of static" ON)
option(FAKE_OWL_BUILD_SAMPLES "Build the Samples?" OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/submodules/visionaray/include)

add_subdirectory(src)

set(fakeOwl_LIBRARY ${fakeOwl_LIBRARY} PARENT_SCOPE)

#--- test application -------------------------------------



#--- samples ----------------------------------------------
if (FAKE_OWL_BUILD_SAMPLES)
  include(cmake/configure_fake_owl.cmake)
  include_directories(${QT_OWL_INCLUDES})
  include_directories(${PROJECT_SOURCE_DIR})
  add_subdirectory(submodules/glfw)
  include_directories(samples/common)
  include_directories(samples/common/3rdParty)
  add_subdirectory(samples)
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
