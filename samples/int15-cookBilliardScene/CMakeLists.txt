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

fake_owl_compile_and_embed(ptxCode
  deviceCode.cpp
  )
add_definitions(-DDATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_executable(int15-cookBilliardScene
  hostCode.cpp
  ${ptxCode}
  )
target_link_libraries(int15-cookBilliardScene
  ${fakeOwl_LIBRARY}
  ${OWL_VIEWER_LIBRARIES}
  )


