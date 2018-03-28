# Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserve.
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

set(PADDLE_FOUND OFF)
set(PADDLE_FLUID_FOUND OFF)

set(PADDLE_ROOT $ENV{PADDLE_ROOT} CACHE PATH "Paddle Path")
if(NOT PADDLE_ROOT)
  message(FATAL_ERROR "Set PADDLE_ROOT as your root directory installed PaddlePaddle")
endif()

find_path(PADDLE_INC_DIR NAMES paddle/fluid/inference/io.h PATHS ${PADDLE_ROOT})
find_library(PADDLE_FLUID_SHARED_LIB NAMES "libpaddle_fluid.so" PATHS
    ${PADDLE_ROOT}/paddle/fluid/inference)
find_library(PADDLE_FLUID_STATIC_LIB NAMES "libpaddle_fluid.a" PATHS
    ${PADDLE_ROOT}/paddle/fluid/inference)
if(PADDLE_INC_DIR AND PADDLE_FLUID_SHARED_LIB)
  set(PADDLE_FOUND ON)
  set(PADDLE_FLUID_FOUND ON)
  add_definitions(-DUSE_PADDLE_FLUID)
  add_library(paddle_fluid_shared SHARED IMPORTED)
  set_target_properties(paddle_fluid_shared PROPERTIES IMPORTED_LOCATION
                        ${PADDLE_FLUID_SHARED_LIB})
  set(PADDLE_LIBRARIES paddle_fluid_shared)
  message(STATUS "Found PaddlePaddle Fluid (include: ${PADDLE_INC_DIR}; "
          "library: ${PADDLE_FLUID_SHARED_LIB}")
else()
  set(PADDLE_FOUND OFF)
  set(PADDLE_FLUID_FOUND OFF)
  return()
endif()

include_directories(${PADDLE_INC_DIR})

find_path(PADDLE_GFLAGS_INC_DIR NAMES gflags/gflags.h PATHS
          ${PADDLE_ROOT}/third_party/install/gflags/include
          NO_DEFAULT_PATH)
find_path(PADDLE_GLOG_INC_DIR NAMES glog/logging.h PATHS
          ${PADDLE_ROOT}/third_party/install/glog/include
          NO_DEFAULT_PATH)
find_path(PADDLE_PROTOBUF_INC_DIR google/protobuf/message.h PATHS
          ${PADDLE_ROOT}/third_party/install/protobuf/include
          NO_DEFAULT_PATH)
find_path(PADDLE_EIGEN_INC_DIR NAMES unsupported/Eigen/CXX11/Tensor PATHS
          ${PADDLE_ROOT}/third_party/eigen3
          NO_DEFAULT_PATH)
if(PADDLE_GFLAGS_INC_DIR AND PADDLE_GLOG_INC_DIR AND PADDLE_PROTOBUF_INC_DIR AND PADDLE_EIGEN_INC_DIR)
  set(PADDLE_THIRD_PARTY_INC_DIRS
      ${PADDLE_GFLAGS_INC_DIR}
      ${PADDLE_GLOG_INC_DIR}
      ${PADDLE_PROTOBUF_INC_DIR}
      ${PADDLE_EIGEN_INC_DIR})
  message(STATUS "Paddle need to include these third party directories: ${PADDLE_THIRD_PARTY_INC_DIRS}")
  include_directories(${PADDLE_THIRD_PARTY_INC_DIRS})
endif()
