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
# Helper to download and setup the Emscripten SDK
#
################################################################################

# Enable strict shell mode
set -o errexit
set -o nounset
set -o pipefail

# Emscripten configuration
EMSDK_VERSION="3.1.55"
EMSDK_REPO="https://github.com/emscripten-core/emsdk.git"

################################################################################
# Environment paths
################################################################################

# Get the absolute path to the tooling scripts
TOOL_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Target directory for the Emscripten SDK installation
EMSDK_DIRECTORY="${TOOL_DIRECTORY}/emsdk"

# Ensure directories exist
mkdir -p "${EMSDK_DIRECTORY}"

################################################################################
# Download and install Emscripten SDK
################################################################################

cd "${EMSDK_DIRECTORY}"

echo "Checking if Emscripten SDK is already installed..."
if [ ! -d "${EMSDK_DIRECTORY}/.git" ]; then
  echo "Emscripten SDK not found. Cloning..."
  git clone "${EMSDK_REPO}" "${EMSDK_DIRECTORY}"
else
  echo "Emscripten SDK found. Updating..."
  git fetch origin
  git reset --hard origin/main
fi

echo "Installing Emscripten SDK version ${EMSDK_VERSION}..."
./emsdk install ${EMSDK_VERSION}
./emsdk activate ${EMSDK_VERSION}

echo "Setting up the environment variables for the current session..."
source "./emsdk_env.sh"

echo "Emscripten SDK setup completed successfully."
