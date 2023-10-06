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
# Set paths for build system early setup
#
################################################################################

# Directory for tooling
TOOL_DIR = $(shell pwd)

# Directory for dependency definitions
DEPENDS_DIR = $(TOOL_DIR)/depends

# Directory of stamps for tracking build progress
STAMP_DIR = $(TOOL_DIR)/stamps

# Shorten variable name for Makefile stamp idiom
S = $(STAMP_DIR)

# Directory for storing dependency repositories
REPO_DIR = $(TOOL_DIR)/repos

# Directory of building dependencies
BUILD_DIR = $(TOOL_DIR)/build

# Directory to place generated files
INSTALL_DIR = $(TOOL_DIR)/../frontend/src/generated

# Directory to place C++ library dependencies
DISTRIBUTION_DIR = $(TOOL_DIR)/dist

# Stamp files for build system stages
CHECKOUT_DONE = $(STAMP_DIR)/.checkout-done
BUILD_DONE = $(STAMP_DIR)/.build-done
INSTALL_DONE = $(STAMP_DIR)/.install-done
