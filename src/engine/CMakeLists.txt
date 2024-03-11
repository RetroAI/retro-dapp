################################################################################
#
#  Copyright (C) 2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  SPDX-License-Identifier: AGPL-3.0-or-later
#  See the file LICENSE.txt for more information.
#
################################################################################

################################################################################
# Project settings
################################################################################

project(retro_engine LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################################################################################
# Dependencies
################################################################################

# TODO

################################################################################
# Sources
################################################################################

set(SOURCE_FILES
  core/retro_engine.cpp
  core/retro_engine_embinder.cpp
)

################################################################################
# Libraries
################################################################################

# Add the executable based on the source files
add_executable(retro_engine ${SOURCE_FILES})

# Add flags for Empscripten builds
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set_target_properties(retro_engine PROPERTIES
    COMPILE_FLAGS " \
      -O3 \
    "
    # 26214400 is 25 MiB
    LINK_FLAGS " \
      -gsource-map \
      -s ALLOW_MEMORY_GROWTH=1 \
      -s INITIAL_MEMORY=26214400 \
      -s MODULARIZE=1 \
      -s USE_WEBGL2=1 \
      -s WASM=1 \
      --bind \
      --source-map-base https://retro.ai/ \
    "
  )
endif ()

################################################################################
# Install
################################################################################

INSTALL(
  FILES
    "${CMAKE_BINARY_DIR}/retro_engine.js"
    "${CMAKE_BINARY_DIR}/retro_engine.wasm"
    "${CMAKE_BINARY_DIR}/retro_engine.wasm.map"
  DESTINATION
    wasm
)
