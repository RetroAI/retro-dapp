#!/bin/bash
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
#
# Helper for CI infrastructure. Sets the appropriate paths and calls CMake.
#
################################################################################

# Enable strict shell mode
set -o errexit
set -o nounset
set -o pipefail

################################################################################
# Environment paths
################################################################################

# Get the absolute path to the project root
PROJECT_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

# Directory for the engine sources
ENGINE_DIRECTORY="${PROJECT_DIRECTORY}/src/engine"

# Directory for tooling
TOOL_DIRECTORY="${PROJECT_DIRECTORY}/tools"

# Directory for intermediate build files
BUILD_DIRECTORY="${TOOL_DIRECTORY}/build"

# Directory of the Emscripten SDK
EMSDK_DIRECTORY="${TOOL_DIRECTORY}/emsdk"

# Directory to place the generated libraries
INSTALL_DIRECTORY="${TOOL_DIRECTORY}/install"

# Ensure directories exist
mkdir -p "${BUILD_DIRECTORY}"
mkdir -p "${INSTALL_DIRECTORY}"

################################################################################
# Setup environment
################################################################################

source "${EMSDK_DIRECTORY}/emsdk_env.sh"

################################################################################
# Call CMake
################################################################################

cd "${BUILD_DIRECTORY}"

emcmake cmake \
  "${ENGINE_DIRECTORY}" \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
  $(! command -v ccache &> /dev/null || echo "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache")

cmake \
  --build "${BUILD_DIRECTORY}" \
  --target install

# Copy the generated libraries to the public directory
cp -rv "${INSTALL_DIRECTORY}/wasm" "${PROJECT_DIRECTORY}/src/frontend/public"
