project(inference CXX C)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/")
include(FindPaddle)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type
    of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

target_link_libraries(${PROJECT_NAME} 
                -Wl,--whole-archive -lpaddle_capi_whole -Wl,--no-whole-archive 
                ${THIRD_PARTY_LIBRARYS} -lstdc++ -lm -lrt -lpthread -ldl)

