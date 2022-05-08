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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "mesh.h"

namespace cdf {

  Mesh::SP Mesh::load(std::string objFileName) {
    Mesh::SP res = std::make_shared<Mesh>();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err = "";

    std::string modelDir = objFileName.substr(0, objFileName.rfind('/') + 1);

    bool readOK
      = tinyobj::LoadObj(&attrib,
                        &shapes,
                        &materials,
                        &err,
                        &err,
                        objFileName.c_str(),
                        modelDir.c_str(),
                        /* triangulate */true,
                        /* default vertex colors fallback*/ false);

    if (!readOK)
      throw std::runtime_error("Could not read OBJ model from " + objFileName + " : " + err);

    for (int shapeID = 0; shapeID < (int)shapes.size(); shapeID++) {
      tinyobj::shape_t& shape = shapes[shapeID];
      Geometry::SP geom = std::make_shared<Geometry>();
      geom->tag = Geometry::Model;
      // Just lazily copy _all_ the vertices into this geom..
      for (std::size_t i=0; i<attrib.vertices.size(); i+=3) {
        float *v = attrib.vertices.data() + i;
        geom->vertex.push_back({v[0],v[1],v[2]});
      }
      for (int faceID = 0; faceID < shape.mesh.material_ids.size(); faceID++) {
        if (shape.mesh.num_face_vertices[faceID] != 3)
          throw std::runtime_error("not properly tessellated"); // while this would actually be rather easy...
        tinyobj::index_t idx0 = shape.mesh.indices[3*faceID];
        tinyobj::index_t idx1 = shape.mesh.indices[3*faceID+1];
        tinyobj::index_t idx2 = shape.mesh.indices[3*faceID+2];
        geom->index.push_back({idx0.vertex_index,
                               idx1.vertex_index,
                               idx2.vertex_index});
      }
      res->geoms.push_back(geom);
    }

    return res;
  }
}
