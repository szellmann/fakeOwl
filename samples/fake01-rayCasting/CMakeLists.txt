# ======================================================================== #
# Copyright 2020 Ingo Wald                                                 #
#                                                                          #
# Licensed under the Apache License, Version 2.0 (the "License");          #
# you may not use this file except in compliance with the License.         #
# You may obtain a copy of the License at                                  #
#                                                                          #
#     http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                          #
# Unless required by applicable law or agreed to in writing, software      #
# distributed under the License is distributed on an "AS IS" BASIS,        #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
# See the License for the specific language governing permissions and      #
# limitations under the License.                                           #
# ======================================================================== #

fake_owl_compile_and_embed(embedded_deviceCode
  deviceCode.cpp
  )
add_executable(fake01-rayCasting
  Renderer.cpp
  viewer.cpp
  mesh.cpp
  ${embedded_deviceCode}
  )
target_link_libraries(fake01-rayCasting
  ${fakeOwl_LIBRARY}
  ${OWL_VIEWER_LIBRARIES}
  )


