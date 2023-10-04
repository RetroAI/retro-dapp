################################################################################
#
#  Copyright (C) 2020-2023 retro.ai
#  This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
#
#  SPDX-License-Identifier: Apache-2.0
#  See the file LICENSE.md for more information.
#
################################################################################

################################################################################
#
# Build system for C++ libraries
#
# Required CMake variables:
#
#   CMAKE_FIND_ROOT_PATH - Point this to dependencies compiled with Emscripten
#   CMAKE_INSTALL_PREFIX - Point this to the "public" folder
#
################################################################################

################################################################################
#
# Project settings
#
################################################################################

project(retroai)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

################################################################################
#
# Dependencies
#
################################################################################

find_package(OpenCV REQUIRED)

################################################################################
#
# Define sources
#
################################################################################

#
# Motion tracker
#

set(MOTION_TRACKER_SOURCES
  api/imgproc.cpp
  api/scene.cpp
  api/video.cpp
  kernels/cpu/cpu_imgproc.cpp
  kernels/cpu/cpu_scene.cpp
  kernels/cpu/cpu_video.cpp
  motion_tracker/motion_tracker.cpp
  motion_tracker/motion_tracker_embinder.cpp
  motion_tracker/vision_graph.cpp
  utils/emscripten_utils.cpp
  utils/frame_pool.cpp
  utils/image_utils.cpp
  utils/math_utils.cpp
)

################################################################################
#
# Build libraries
#
# TODO:
#
#   * Build properly instead of shelling out
#   * Could refactor this into macros
#
################################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}
)

string(APPEND EMSCRIPTEN_LINK_FLAGS
  "--bind "
  # "-o dist/engine.js "
  # " -std=c++11 "
  # " -O2 "
  # " --preload-file textures "
  # " --preload-file shaders "
  # " --preload-file fonts "
  # " --pre-js pre-module.j "
  # " --post-js post-module.j "
  "-s ALLOW_MEMORY_GROWTH=1 "
  "-s ASSERTIONS=1 "
  # " -s DEMANGLE_SUPPORT=1 "
  # " -s DISABLE_EXCEPTION_CATCHING=0 "
  "-s ERROR_ON_UNDEFINED_SYMBOLS=0 "
  # " -s FULL_ES3=1 "
  # " -s GL_ASSERTIONS=1 "
  # " -s GL_UNSAFE_OPTS=0 "
  # " -s INVOKE_RUN=0 "
  # " -s LEGACY_GL_EMULATION=0 "
  #"-s LLD_REPORT_UNDEFINED "
  # " -s OFFSCREENCANVAS_SUPPORT=1 "
  # " -s SAFE_HEAP=1 "
  #"-s TOTAL_MEMORY=67108864 "
  # " -s USE_FREETYPE=1 "
  # " -s USE_GLFW=3 "
  # " -s USE_WEBGL2=1 "
  "-s USE_ZLIB=1 "
  # " -s WASM=1 "
)

#
# Motion tracker
#

add_executable(motion_tracker
  ${MOTION_TRACKER_SOURCES}
)

target_include_directories(motion_tracker PRIVATE
  ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(motion_tracker PRIVATE
  ${OpenCV_LIBS}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set_target_properties(motion_tracker PROPERTIES
    COMPILE_FLAGS " \
      -O0 \
      -g4 \
      -s DISABLE_EXCEPTION_CATCHING=0 \
      -s INITIAL_MEMORY=26214400 \
    "
    # 26214400 is 25 MiB
    LINK_FLAGS " \
      --bind \
      --source-map-base https://retro.ai/ \
      -O0 \
      -g4 \
      -s DISABLE_EXCEPTION_CATCHING=0 \
      -s INITIAL_MEMORY=26214400 \
    "
  )
endif ()

################################################################################
#
# Install libraries
#
################################################################################

#
# Motion tracker
#

INSTALL(
  FILES
    "${CMAKE_BINARY_DIR}/motion_tracker.js"
    "${CMAKE_BINARY_DIR}/motion_tracker.wasm"
    "${CMAKE_BINARY_DIR}/motion_tracker.wasm.map"
  DESTINATION
    motion_tracker
)
