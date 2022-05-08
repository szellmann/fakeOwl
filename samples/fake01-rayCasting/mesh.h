// ======================================================================== //
// Copyright 2020-2020 The Authors                                          //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "owl/owl.h"
#include "owl/common/math/vec.h"

namespace cdf {

  using namespace owl;
  using namespace owl::common;
 
  struct Geometry {
    typedef std::shared_ptr<Geometry> SP;

    // Tag that helps describing the geometry a bit
    enum Tag {
      WholeMesh,     // indicates that the whole geometry is just one triangle soup
      CDFRow,        // indicates that the geometry represents a single CDF row
      CDFLastCol,    // indicates that the geometry represents the last cumulative column
      CDFRowSurface, // indicates that the geometry represents all CDF rows
      Model,         // indicates that the geometry belongs to a model, not a CDF
    };

    std::vector<vec3f> vertex;
    std::vector<vec3i> index;
    std::vector<float> slopes; // for CDF types

    Tag tag;
  };

  struct Mesh {
    typedef std::shared_ptr<Mesh> SP;

    enum Representation { Quads, Ribbons };

    std::vector<Geometry::SP> geoms;

    static Mesh::SP load(std::string objFileName);
  };

}
