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
# Depends build scripting entry point
#
# Call via:
#
#   build-depends.sh <task>
#
# See the function dispatch() for the available tasks that can be run.
#

# Enable strict mode
set -o errexit
set -o pipefail
set -o nounset

# Absolute path to this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

#
# Source includes
#

source "${SCRIPT_DIR}/build-paths.sh"

#
# Dispatch function
#
# This function contains the available tasks. The first argument identifies
# which task to jump to.
#
function dispatch() {
  case "${1:-}" in
  all)
    depends-all
    ;;
  checkout)
    depends-checkout
    ;;
  build)
    depends-build
    ;;
  install)
    depends-install
    ;;
  *)
    echo "Invalid task: ${1:-}"
    exit 1
    ;;
  esac
}

function depends-all() {
  # Dependencies to build
  BUILD_DEPENDS="emscripten "

  # Build OpenCV
  if [ ! -f "${GENERATED_SOURCE_DIR}/opencv.js" ] \
    || [ ! -f "${DISTRIBUTION_LIB_DIR}/libopencv_core.a" ]; then
    rm -f "${STAMP_DIR}/build-opencv"
    BUILD_DEPENDS+="opencv "
  fi

  make -C "${TOOL_DIR}" -j$(getconf _NPROCESSORS_ONLN) ${BUILD_DEPENDS}
}

function depends-checkout() {
  make -C "${TOOL_DIR}" checkout -j10
}

function depends-build() {
  make -C "${TOOL_DIR}" build -j$(getconf _NPROCESSORS_ONLN)
}

function depends-install() {
  make -C "${TOOL_DIR}" install
}

# Perform the dispatch
dispatch ${1:-}
