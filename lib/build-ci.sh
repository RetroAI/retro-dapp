#!/bin/bash
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
# Helper for CI infrastructure. Sets the appropriate paths and calls CMake.
#
################################################################################

# Enable strict shell mode
set -o errexit
set -o nounset
set -o pipefail

#
# Environment paths
#

# Get the absolute path to this script
SOURCE_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Directory of the depends build system
TOOL_DIRECTORY="${SOURCE_DIRECTORY}/../tools"

# Directory for intermediate build files
BUILD_DIRECTORY="${TOOL_DIRECTORY}/build/cpp-libs"

# Directory of the Emscripten SDK
EMSDK_DIRECTORY="${TOOL_DIRECTORY}/repos/emsdk"

# Directory of the installed dependency files
DEPENDS_DIRECTORY="${TOOL_DIRECTORY}/dist"

# Directory to place the generated libraries
INSTALL_DIRECTORY="${SOURCE_DIRECTORY}/../frontend/public"

# Ensure directories exist
mkdir -p "${BUILD_DIRECTORY}"
mkdir -p "${INSTALL_DIRECTORY}"

#
# Setup environment
#

source "${EMSDK_DIRECTORY}/emsdk_set_env.sh"

#
# Call CMake
#

cd "${BUILD_DIRECTORY}"

emcmake cmake \
  "${SOURCE_DIRECTORY}" \
  -DCMAKE_FIND_ROOT_PATH="${DEPENDS_DIRECTORY}" \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_DIRECTORY}" \
  $(! [ -x "$(command -v ccache)" ] || echo "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache") \

cmake --build "${BUILD_DIRECTORY}" -j$(shell getconf _NPROCESSORS_ONLN) --target install
