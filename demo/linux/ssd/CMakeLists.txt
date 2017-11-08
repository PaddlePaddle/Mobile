# Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License

cmake_minimum_required(VERSION 3.0)
project(vgg_ssd_demo CXX C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/")
find_package(Paddle)
find_package(OpenCV)

set(SRC_FILES main.cpp image_reader.cpp paddle_image_recognizer.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11")

target_link_libraries(${PROJECT_NAME}
                      ${PADDLE_LIBRARIES}
                      ${PADDLE_THIRD_PARTY_LIBRARIES})
if(NOT ANDROID_ABI)
  target_link_libraries(${PROJECT_NAME} -lrt -ldl -lpthread)
endif()
if(OPENCV_FOUND)
  target_link_libraries(${PROJECT_NAME} ${OPENCV_LIBRARIES})
endif()
