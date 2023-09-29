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
# Build chain for dependendies
#
# Make is used to run shell commands instead of bash to allow for parallel
# intermediary build stages.
#
# The primary build stages that can be specified on the command line are:
#
#   1. checkout
#   2. patch
#   3. build
#   4. install (default if no stage is given)
#
# Two stages are used for cleaning temoprary files:
#
#   1. clean
#   2. distclean
#
# "make clean" is used to remove temporary build artifacts. "make distclean" is
# used to remove all temporary files and reset the directory to an unused state.
#
# This Makefile depends on the following packages:
#
#   - curl
#   - patch
#   - python3
#   - unzip
#
################################################################################

# Build system setup
include setup_stages.mk
include setup_paths.mk

# Build parameter setup
include setup_environment.mk

# Define the shell used to execute commands
SHELL := /bin/bash

################################################################################
#
# Import package rules
#
################################################################################

include depends/native/emscripten/package.mk
include depends/wasm/ade/package.mk
include depends/wasm/opencv/package.mk

################################################################################
#
# Build system targets
#
# Defines the targets that are build when "make" is run.
#
################################################################################

#
# Define targets for "make checkout"
#

CHECKOUT_DEPENDS = \
  $(S)/checkout-ade \
  $(S)/checkout-emscripten \
  $(S)/checkout-opencv \
  $(S)/patch-opencv \

#
# Define targets for "make build"
#

BUILD_DEPENDS = \
  $(S)/build-opencv \

#
# Define targets for "make install"
#

INSTALL_DEPENDS = \
  $(S)/install-opencv \

#
# Inject targets
#

checkout: $(CHECKOUT_DEPENDS)
build: $(BUILD_DEPENDS)
install: $(INSTALL_DEPENDS)

#
# Define targets for building individual depends via "make <depend>"
#

.PHONY: ade
.PHONY: emscripten
.PHONY: opencv

ade: \
  $(S)/install-ade

emscripten: \
  $(S)/build-emscripten

opencv: \
  $(S)/install-opencv

################################################################################
#
# Build system procedures
#
################################################################################

#
# Clean stage
#

clean:
	rm -rf "$(BUILD_DIR)"
	rm -rf "$(S)"

#
# Distclean stage
#

distclean: clean
	rm -rf "$(REPO_DIR)"
