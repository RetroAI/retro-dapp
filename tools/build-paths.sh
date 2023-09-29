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

#
# Paths for depends build scripts
#
# Import via:
#
#   source build-paths.sh
#

# Enable strict mode
set -o errexit
set -o pipefail
set -o nounset

#
# Directory definitions
#

# Get the absolute path to this script
SOURCE_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Root project directory
PROJECT_DIR="${SOURCE_DIRECTORY}/.."

# Typescript/Javascript source directory
SOURCE_DIR="${PROJECT_DIR}/frontend/src"

# Directory for generated source
GENERATED_SOURCE_DIR="${SOURCE_DIR}/generated"

# Tooling directory
TOOL_DIR="${PROJECT_DIR}/tools"

# Depends directory
DEPENDS_DIR="${TOOL_DIR}/depends"

# Build system stamp directory
STAMP_DIR="${TOOL_DIR}/stamps"

# Location of installed dependency files
DISTRIBUTION_DIR="${TOOL_DIR}/dist"

# Location of installed dependency libraries
DISTRIBUTION_LIB_DIR="${DISTRIBUTION_DIR}/lib"
