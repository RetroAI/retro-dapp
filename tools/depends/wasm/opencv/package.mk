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
# OpenCV - The Open Computer Vision Library
#
# SPDX-License-Identifier: Apache-2.0
#
################################################################################

# Dependency name and version
OPENCV_REPO_NAME = opencv
OPENCV_VERSION = 4.5.0
OPENCV_REMOTE_REPO = https://github.com/opencv/$(OPENCV_REPO_NAME).git
OPENCV_LIB = libopencv_core.a
OPENCV_JS_LIB = opencv.js

################################################################################
#
# Paths
#
################################################################################

# Checkout directory
REPO_DIR_OPENCV = $(REPO_DIR)/$(OPENCV_REPO_NAME)

# Build directory
BUILD_DIR_OPENCV = $(BUILD_DIR)/$(OPENCV_REPO_NAME)

# Build output
BUILD_FILE_OPENCV = $(BUILD_DIR_OPENCV)/lib/$(OPENCV_LIB)
BUILD_FILE_OPENCV_JS = $(BUILD_DIR_OPENCV)/bin/$(OPENCV_JS_LIB)

# Install output
INSTALL_FILE_OPENCV = $(DISTRIBUTION_DIR)/lib/$(OPENCV_LIB)
INSTALL_FILE_OPENCV_JS = $(INSTALL_DIR)/$(OPENCV_JS_LIB)

################################################################################
#
# Configuration
#
################################################################################

OPENCV_GAPI_BUILD_DEPENDS = \
  $(S)/install-ade \

OPENCV_BUILD_DEPENDS = \
  $(S)/checkout-opencv \
  $(S)/build-emscripten \
  $(OPENCV_GAPI_BUILD_DEPENDS) \

################################################################################
#
# Checkout
#
################################################################################

$(S)/checkout-opencv: $(S)/.precheckout
	[ -d "$(REPO_DIR_OPENCV)" ] || ( \
	  git clone -b $(OPENCV_VERSION) "$(OPENCV_REMOTE_REPO)" "$(REPO_DIR_OPENCV)" && \
	  patch -p1 --forward --directory="$(REPO_DIR_OPENCV)" < \
	    "$(TOOL_DIR)/depends/wasm/opencv/0001-GAPI-Implement-RGBA2Gray-and-GBRA2Gray.patch" \
	)

	@# TODO: Repository sync is delegated to the CI system.

	touch "$@"

################################################################################
#
# Patch
#
################################################################################

$(S)/patch-opencv: $(S)/.prepatch $(S)/checkout-opencv \
  $(TOOL_DIR)/depends/wasm/opencv/opencv_js.config.py
	cp "$(TOOL_DIR)/depends/wasm/opencv/opencv_js.config.py" "$(REPO_DIR_OPENCV)/platforms/js"

	touch "$@"

################################################################################
#
# Build
#
################################################################################

$(BUILD_FILE_OPENCV): $(S)/.prebuild $(OPENCV_BUILD_DEPENDS)
	mkdir -p "$(BUILD_DIR_OPENCV)"

	# Activate PATH and other environment variables in the current terminal and
	# build OpenCV
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  cd "${BUILD_DIR_OPENCV}" && \
	  emcmake cmake "$(REPO_DIR_OPENCV)" \
	    -DCMAKE_FIND_ROOT_PATH="$(DISTRIBUTION_DIR)" \
	    -DCMAKE_INSTALL_PREFIX="$(DISTRIBUTION_DIR)" \
	    -DCMAKE_BUILD_PARALLEL_LEVEL=$(shell getconf _NPROCESSORS_ONLN) \
	    $(shell ! command -v ccache &> /dev/null || echo "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache") \
	    -DBUILD_SHARED_LIBS=OFF \
	    -DWITH_1394=OFF \
	    -DWITH_ADE=ON \
	    -DWITH_VTK=OFF \
	    -DWITH_EIGEN=OFF \
	    -DWITH_FFMPEG=OFF \
	    -DWITH_GSTREAMER=OFF \
	    -DWITH_GTK=OFF \
	    -DWITH_GTK_2_X=OFF \
	    -DWITH_IPP=OFF \
	    -DWITH_JASPER=OFF \
	    -DWITH_JPEG=OFF \
	    -DWITH_WEBP=OFF \
	    -DWITH_OPENEXR=OFF \
	    -DWITH_OPENGL=OFF \
	    -DWITH_OPENVX=OFF \
	    -DWITH_OPENNI=OFF \
	    -DWITH_OPENNI2=OFF \
	    -DWITH_PNG=OFF \
	    -DWITH_TBB=OFF \
	    -DWITH_TIFF=OFF \
	    -DWITH_V4L=OFF \
	    -DWITH_OPENCL=OFF \
	    -DWITH_OPENCL_SVM=OFF \
	    -DWITH_OPENCLAMDFFT=OFF \
	    -DWITH_OPENCLAMDBLAS=OFF \
	    -DWITH_GPHOTO2=OFF \
	    -DWITH_LAPACK=OFF \
	    -DWITH_ITT=OFF \
	    -DWITH_QUIRC=OFF \
	    -DBUILD_ZLIB=ON \
	    -DBUILD_opencv_apps=OFF \
	    -DBUILD_opencv_calib3d=ON \
	    -DBUILD_opencv_dnn=ON \
	    -DBUILD_opencv_features2d=ON \
	    -DBUILD_opencv_flann=ON \
	    -DBUILD_opencv_gapi=ON \
	    -DBUILD_opencv_ml=OFF \
	    -DBUILD_opencv_photo=ON \
	    -DBUILD_opencv_imgcodecs=ON \
	    -DBUILD_opencv_shape=OFF \
	    -DBUILD_opencv_videoio=OFF \
	    -DBUILD_opencv_videostab=OFF \
	    -DBUILD_opencv_highgui=OFF \
	    -DBUILD_opencv_superres=OFF \
	    -DBUILD_opencv_stitching=OFF \
	    -DBUILD_opencv_java=OFF \
	    -DBUILD_opencv_java_bindings_generator=OFF \
	    -DBUILD_opencv_js=ON \
	    -DBUILD_opencv_js_bindings_generator=ON \
	    -DBUILD_opencv_python2=OFF \
	    -DBUILD_opencv_python3=OFF \
	    -DBUILD_opencv_python_bindings_generator=OFF \
	    -DBUILD_EXAMPLES=OFF \
	    -DBUILD_PACKAGE=OFF \
	    -DBUILD_TESTS=OFF \
	    -DBUILD_PERF_TESTS=OFF \
	    -DBUILD_DOCS=OFF \
	    -DWITH_PTHREADS_PF=OFF \
	    -DCV_ENABLE_INTRINSICS=OFF \
	    -DBUILD_WASM_INTRIN_TESTS=OFF \
	    -DCMAKE_C_FLAGS=" \
	      -s WASM=1 \
	      -s USE_PTHREADS=0 \
	      -s DISABLE_EXCEPTION_CATCHING=1 \
	    " \
	    -DCMAKE_CXX_FLAGS=" \
	      -s WASM=1 \
	      -s USE_PTHREADS=0 \
	      -s DISABLE_EXCEPTION_CATCHING=1 \
	    " \

	OPENCV_JS_WHITELIST="$(TOOL_DIR)/depends/wasm/opencv/opencv_js.config.py" \
	  make -C "${BUILD_DIR_OPENCV}" -j$(shell getconf _NPROCESSORS_ONLN)

	touch "$@"

$(S)/build-opencv: $(BUILD_FILE_OPENCV)
	touch "$@"

################################################################################
#
# Install
#
################################################################################

$(INSTALL_FILE_OPENCV): $(S)/.preinstall $(S)/build-opencv
	mkdir -p "$(DISTRIBUTION_DIR)"

	# Install headers and libraries
	cmake \
	  --build "$(BUILD_DIR_OPENCV)" \
	  --target install

	touch "$@"

$(INSTALL_FILE_OPENCV_JS): $(S)/.preinstall $(S)/build-opencv \
  $(TOOL_DIR)/depends/wasm/opencv/0001-temp-Hack-opencv.js-to-ES6.patch
	mkdir -p "$(INSTALL_DIR)"

	# Copy generated files
	cp "$(BUILD_FILE_OPENCV_JS)" "$(INSTALL_DIR)"

	# Hack in ES6 support
	patch --no-backup-if-mismatch -d "$(INSTALL_DIR)" < "$(TOOL_DIR)/depends/wasm/opencv/0001-temp-Hack-opencv.js-to-ES6.patch"

$(S)/install-opencv: $(INSTALL_FILE_OPENCV) $(INSTALL_FILE_OPENCV_JS)
	touch "$@"
