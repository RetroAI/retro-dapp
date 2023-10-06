################################################################################
#
#  Copyright (C) 2020-2023 retro.ai
#  This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
#
#  SPDX-License-Identifier: Apache-2.0
#  See the file LICENSE.md for more information.
#
################################################################################

# Dependency name and version
LIBVPX_REPO_NAME = libvpx
LIBVPX_VERSION = v1.9.0
LIBVPX_REMOTE_REPO = https://github.com/webmproject/$(LIBVPX_REPO_NAME).git
LIBVPX_LIB = libvpx.a

################################################################################
#
# Paths
#
################################################################################

# Checkout directory
REPO_DIR_LIBVPX = $(REPO_DIR)/$(LIBVPX_REPO_NAME)

# Build directory
BUILD_DIR_LIBVPX = $(BUILD_DIR)/$(LIBVPX_REPO_NAME)

# Build output
BUILD_FILE_LIBVPX = $(BUILD_DIR_LIBVPX)/$(LIBVPX_LIB)

# Install output
INSTALL_FILE_LIBVPX = $(DISTRIBUTION_DIR)/lib/$(LIBVPX_LIB)

################################################################################
#
# Configuration
#
################################################################################

LIBVPX_BUILD_DEPENDS = \
  $(S)/checkout-libvpx \
  $(S)/build-emscripten \

LIBVPX_BUILD_OPTIONS = \
  --prefix="$(DISTRIBUTION_DIR)" \
  --target=generic-gnu \
  --extra-cflags="-O3" \
#  --enable-ccache \ # TODO

LIBVPX_ADVANCED_OPTIONS = \
  --disable-examples \
  --disable-tools \
  --disable-docs \
  --disable-multithread \
  --disable-runtime-cpu-detect \

LIBVPX_CONFIGURE_OPTIONS = \
  $(LIBVPX_BUILD_OPTIONS) \
  $(LIBVPX_ADVANCED_OPTIONS) \

################################################################################
#
# Checkout
#
################################################################################

$(S)/checkout-libvpx: $(S)/.precheckout
	[ -d "$(REPO_DIR_LIBVPX)" ] || ( \
	  git clone -b $(LIBVPX_VERSION) "$(LIBVPX_REMOTE_REPO)" "$(REPO_DIR_LIBVPX)" \
	)

	@# TODO: Repository sync is delegated to the CI system.

	touch "$@"

################################################################################
#
# Build
#
################################################################################

$(BUILD_FILE_LIBVPX): $(S)/.prebuild $(LIBVPX_BUILD_DEPENDS)
	# Deep-copy repo to build folder for now
	[ -d "$(BUILD_DIR_LIBVPX)" ] || ( \
	  cp -r "$(REPO_DIR_LIBVPX)" "$(BUILD_DIR_LIBVPX)" \
	)

	# Activate PATH and other environment variables in the current terminal and
	# configure libvpx
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  cd "${BUILD_DIR_LIBVPX}" && \
	  emconfigure ./configure \
      $(LIBVPX_CONFIGURE_OPTIONS)

	# Activate PATH and other environment variables in the current terminal and
	# build libvpx
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  emmake make -C "${BUILD_DIR_LIBVPX}" \
      SHELL="/bin/bash" \
      -j$(shell getconf _NPROCESSORS_ONLN) \

	touch "$@"

$(S)/build-libvpx: $(BUILD_FILE_LIBVPX)
	touch "$@"

################################################################################
#
# Install
#
################################################################################

$(INSTALL_FILE_LIBVPX): $(S)/.preinstall $(S)/build-libvpx
	mkdir -p "$(DISTRIBUTION_DIR)"

	# Activate PATH and other environment variables in the current terminal and
	# install libvpx
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  emmake make -C "${BUILD_DIR_LIBVPX}" install

	touch "$@"

$(S)/install-libvpx: $(INSTALL_FILE_LIBVPX)
	touch "$@"
