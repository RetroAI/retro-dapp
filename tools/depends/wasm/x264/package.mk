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
X264_REPO_NAME = x264
X264_VERSION = stable
X264_REMOTE_REPO = https://code.videolan.org/videolan/$(X264_REPO_NAME).git
X264_LIB = libx264.a

################################################################################
#
# Paths
#
################################################################################

# Checkout directory
REPO_DIR_X264 = $(REPO_DIR)/$(X264_REPO_NAME)

# Build directory
BUILD_DIR_X264 = $(BUILD_DIR)/$(X264_REPO_NAME)

# Build output
BUILD_FILE_X264 = $(BUILD_DIR_X264)/$(X264_LIB)

# Install output
INSTALL_FILE_X264 = $(DISTRIBUTION_DIR)/lib/$(X264_LIB)

################################################################################
#
# Configuration
#
################################################################################

X264_BUILD_DEPENDS = \
  $(S)/checkout-x264 \
  $(S)/build-emscripten \

X264_STANDARD_OPTIONS = \
  --prefix="$(DISTRIBUTION_DIR)" \

X264_CONFIGURATION_OPTIONS = \
  --disable-cli \
  --enable-shared \
  --disable-opencl \
  --disable-gpl \
  --disable-thread \

X264_ADVANCED_OPTIONS = \
  --disable-asm \

X264_CROSS_COMPILATION = \
  --host=i686-pc-linux-gnu \

X264_CONFIGURE_OPTIONS = \
  $(X264_STANDARD_OPTIONS) \
  $(X264_CONFIGURATION_OPTIONS) \
  $(X264_ADVANCED_OPTIONS) \
  $(X264_CROSS_COMPILATION) \

################################################################################
#
# Checkout
#
################################################################################

$(S)/checkout-x264: $(S)/.precheckout
	[ -d "$(REPO_DIR_X264)" ] || ( \
	  git clone -b $(X264_VERSION) "$(X264_REMOTE_REPO)" "$(REPO_DIR_X264)" \
	)

	@# TODO: Repository sync is delegated to the CI system.

	touch "$@"

################################################################################
#
# Build
#
################################################################################

$(BUILD_FILE_X264): $(S)/.prebuild $(X264_BUILD_DEPENDS)
	# Deep-copy repo to build folder for now
	[ -d "$(BUILD_DIR_X264)" ] || ( \
	  cp -r "$(REPO_DIR_X264)" "$(BUILD_DIR_X264)" \
	)

	# Activate PATH and other environment variables in the current terminal and
	# configure x264
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  cd "${BUILD_DIR_X264}" && \
	  emconfigure ./configure \
	    $(X264_CONFIGURE_OPTIONS)

	# Activate PATH and other environment variables in the current terminal and
	# build x264
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  emmake make -C "${BUILD_DIR_X264}" \
	    SHELL="/bin/bash" \
	    -j$(shell getconf _NPROCESSORS_ONLN) \

	touch "$@"

$(S)/build-x264: $(BUILD_FILE_X264)
	touch "$@"

################################################################################
#
# Install
#
################################################################################

$(INSTALL_FILE_X264): $(S)/.preinstall $(S)/build-x264
	mkdir -p "$(DISTRIBUTION_DIR)"

	# Activate PATH and other environment variables in the current terminal and
	# install x264
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  emmake make -C "${BUILD_DIR_X264}" install

	touch "$@"

$(S)/install-x264: $(INSTALL_FILE_X264)
	touch "$@"
