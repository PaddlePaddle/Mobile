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

if(ANDROID_ABI STREQUAL "armeabi-v7a")
    target_link_libraries(${PROJECT_NAME}
                          -Wl,--start-group -Wl,--whole-archive -lpaddle_capi_layers
                          -Wl,--no-whole-archive -lpaddle_capi_engine -Wl,--end-group
                          ${THIRD_PARTY_LIBRARYS})
elseif(ANDROID_ABI STREQUAL "arm64-v8a")
    target_link_libraries(${PROJECT_NAME}
                          -Wl,--start-group -Wl,--whole-archive -lpaddle_capi_layers
                          -Wl,--no-whole-archive -lpaddle_capi_engine -Wl,--end-group
                          ${THIRD_PARTY_LIBRARYS})
else()
    target_link_libraries(${PROJECT_NAME}
                    -Wl,--start-group -Wl,--whole-archive -lpaddle_capi_whole
                    -Wl,--no-whole-archive -Wl,--end-group
                    ${THIRD_PARTY_LIBRARYS} -lrt -ldl -lpthread)
endif()
