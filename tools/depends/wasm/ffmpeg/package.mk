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
FFMPEG_REPO_NAME = ffmpeg
FFMPEG_VERSION = n4.4
FFMPEG_REMOTE_REPO = https://github.com/FFmpeg/FFmpeg.git
FFMPEG_LIB = libavutil.a

################################################################################
#
# Paths
#
################################################################################

# Checkout directory
REPO_DIR_FFMPEG = $(REPO_DIR)/$(FFMPEG_REPO_NAME)

# Build directory
BUILD_DIR_FFMPEG = $(BUILD_DIR)/$(FFMPEG_REPO_NAME)

# Build output
BUILD_FILE_FFMPEG = $(BUILD_DIR_FFMPEG)/libavutil/$(FFMPEG_LIB)

# Install output
INSTALL_FILE_FFMPEG = $(DISTRIBUTION_DIR)/lib/$(FFMPEG_LIB)

################################################################################
#
# Configuration
#
################################################################################

# TODO: Remove codecbox.js dependency
FFMPEG_BUILD_DEPENDS = \
  $(S)/checkout-codecbox.js \
  $(S)/checkout-ffmpeg \
  $(S)/build-emscripten \
  $(S)/install-codecbox.js \
  #$(S)/install-libvpx \
  #$(S)/install-x264 \

# You can change items in FF_DECODERS, FF_DEMUXERS, FF_PARSERS, FF_ENCODERS,
# FF_MUXERS and FF_FILTERS to select components of ffmpeg
FF_DECODERS = \
  aac \
  aac_latm \
  ac3 \
  ac3_fixed \
  cook \
  h263 \
  h263i \
  h263p \
  h264 \
  hevc \
  libvpx_vp8 \
  libvpx_vp9 \
  mjpeg \
  mjpegb \
  mp2 \
  mp2float \
  mp3 \
  mp3adu \
  mp3adufloat \
  mp3float \
  mp3on4 \
  mp3on4float \
  mpeg1video \
  mpeg2video \
  mpeg4 \
  mpegvideo \
  msmpeg4v1 \
  msmpeg4v2 \
  msmpeg4v3 \
  opus \
  pcm_alaw \
  pcm_bluray \
  pcm_dvd \
  pcm_f32be \
  pcm_f32le \
  pcm_f64be \
  pcm_f64le \
  pcm_lxf \
  pcm_mulaw \
  pcm_s16be \
  pcm_s16be_planar \
  pcm_s16le \
  pcm_s16le_planar \
  pcm_s24be \
  pcm_s24daud \
  pcm_s24le \
  pcm_s24le_planar \
  pcm_s32be \
  pcm_s32le \
  pcm_s32le_planar \
  pcm_s8 \
  pcm_s8_planar \
  pcm_u16be \
  pcm_u16le \
  pcm_u24be \
  pcm_u24le \
  pcm_u32be \
  pcm_u32le \
  pcm_u8 \
  ra_144 \
  ra_288 \
  ralf \
  rv10 \
  rv20 \
  rv30 \
  rv40 \
  sipr \
  vc1 \
  vc1image \
  vorbis \
  vp6 \
  vp6a \
  vp6f \
  wmalossless \
  wmapro \
  wmav1 \
  wmav2 \
  wmavoice \
  wmv1 \
  wmv2 \
  wmv3 \
  wmv3image \
  zlib \

FF_DEMUXERS = \
  aac \
  ac3 \
  asf \
  avi \
  flac \
  flv \
  m4v \
  matroska \
  mjpeg \
  mov \
  mp3 \
  mpegps \
  mpegts \
  mpegtsraw \
  mpegvideo \
  ogg \
  pcm_alaw \
  pcm_f32be \
  pcm_f32le \
  pcm_f64be \
  pcm_f64le \
  pcm_mulaw \
  pcm_s16be \
  pcm_s16le \
  pcm_s24be \
  pcm_s24le \
  pcm_s32be \
  pcm_s32le \
  pcm_s8 \
  pcm_u16be \
  pcm_u16le \
  pcm_u24be \
  pcm_u24le \
  pcm_u32be \
  pcm_u32le \
  pcm_u8 \
  rm \
  vc1 \
  vc1t \

FF_PARSERS = \
  aac \
  aac_latm \
  h264 \
  hevc \
  opus \

FF_ENCODERS = \
  aac \
  libopus \
  libvpx_vp8 \
  libvpx_vp9 \
  libx264 \
  libx264rgb \
  vorbis \
  libmp3lame \
  libopenh264 \

FF_MUXERS = \
  mp3 \
  mp4 \
  oga \
  ogg \
  webm \

FF_FILTERS = \
  adelay \
  aformat \
  aresample \
  ashowinfo \
  ass \
  atrim \
  concat \
  copy \
  crop \
  cropdetect \
  format \
  join \
  pad \
  rotate \
  scale \
  select \
  setdar \
  setsar \
  showinfo \
  trim \
  vflip \
  volume \
  volumedetect \

FFMPEG_STANDARD_OPTIONS = \
  --disable-logging \
  --prefix="$(TOOL_DIR)/dist" \

FFMPEG_LICENSING_OPTIONS = \
  --enable-gpl \
  --enable-version3 \

FFMPEG_CONFIGURATION_OPTIONS = \
  --disable-runtime-cpudetect \

FFMPEG_PROGRAM_OPTIONS = \
  --disable-programs \
  --disable-ffplay \
  --disable-ffprobe \

FFMPEG_DOCUMENTATION_OPTIONS = \
  --disable-doc \

FFMPEG_COMPONENT_OPTIONS = \
  --disable-pthreads \
  --disable-w32threads \
  --disable-network \

FFMPEG_EXTERNAL_LIBRARIES = \
  --enable-libvpx \
  --enable-libx264 \
  --enable-libmp3lame \
  --enable-libopus \
  --enable-libopenh264 \
  --disable-xlib \
  --disable-iconv \

FFMPEG_TOOLCHAIN_OPTIONS = \
  --arch=x86_32 \
  --cpu=generic \
  --enable-cross-compile \
  --target-os=none \
  --cc="emcc" \
  --pkg-config="$(BUILD_DIR_CODECBOX_JS)/pkg_config" \
  --ranlib="emranlib" \
  --extra-cflags="-I$(BUILD_DIR_CODECBOX_JS)/build/dist/include -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 -fno-stack-protector" \
  --extra-ldflags="-L$(BUILD_DIR_CODECBOX_JS)/build/dist/lib" \
  --optflags="-O3" \

FFMPEG_OPTIMIZATION_OPTIONS = \
  --disable-asm \

FFMPEG_DEVELOPER_OPTIONS = \
  --disable-debug \
  --disable-stripping \

FFMPEG_CONFIG_SHARED = \
  $(FFMPEG_STANDARD_OPTIONS) \
  $(FFMPEG_LICENSING_OPTIONS) \
  $(FFMPEG_CONFIGURATION_OPTIONS) \
  $(FFMPEG_PROGRAM_OPTIONS) \
  $(FFMPEG_DOCUMENTATION_OPTIONS) \
  $(FFMPEG_COMPONENT_OPTIONS) \
  $(FFMPEG_EXTERNAL_LIBRARIES) \
  $(FFMPEG_TOOLCHAIN_OPTIONS) \
  $(FFMPEG_OPTIMIZATION_OPTIONS) \
  $(FFMPEG_DEVELOPER_OPTIONS) \

FFMPEG_FULL_CONFIG = \
  $(FFMPEG_CONFIG_SHARED) \
  --enable-protocol=file \

FFMPEG_CUSTOM_CONFIG = \
  $(FFMPEG_CONFIG_SHARED) \
  --disable-everything \
  --enable-protocol=file \
  $(foreach decoder,$(FF_DECODERS),--enable-decoder=$(decoder) ) \
  $(foreach demuxer,$(FF_DEMUXERS),--enable-demuxer=$(demuxer) ) \
  $(foreach parser,$(FF_PARSERS),--enable-parser=$(parser) ) \
  $(foreach muxer,$(FF_MUXERS),--enable-muxer=$(muxer) ) \
  $(foreach encoder,$(FF_ENCODERS),--enable-encoder=$(encoder) ) \
  $(foreach filter,$(FF_FILTERS),--enable-filter=$(filter) ) \

# Select your FFmpeg config
#FFMPEG_CONFIGURE_OPTIONS = $(FFMPEG_FULL_CONFIG)
FFMPEG_CONFIGURE_OPTIONS = $(FFMPEG_CUSTOM_CONFIG)

################################################################################
#
# Checkout
#
################################################################################

$(S)/checkout-ffmpeg: $(S)/.precheckout
	[ -d "$(REPO_DIR_FFMPEG)" ] || ( \
	  git clone -b $(FFMPEG_VERSION) "$(FFMPEG_REMOTE_REPO)" "$(REPO_DIR_FFMPEG)" \
	)

	@# TODO: Repository sync is delegated to the CI system.

	touch "$@"

################################################################################
#
# Build
#
################################################################################

$(BUILD_FILE_FFMPEG): $(S)/.prebuild $(FFMPEG_BUILD_DEPENDS)
	# Deep-copy repo to build folder for now
	[ -d "$(BUILD_DIR_FFMPEG)" ] || ( \
	  cp -r "$(REPO_DIR_FFMPEG)" "$(BUILD_DIR_FFMPEG)" \
	)

	# Activate PATH and other environment variables in the current terminal and
	# configure FFmpeg
	[ -f "$(BUILD_DIR_FFMPEG)/ffbuild/.config" ] || ( \
	  . "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	    cd "${BUILD_DIR_FFMPEG}" && \
	    emconfigure ./configure $(FFMPEG_CONFIGURE_OPTIONS) \
	)

	# Activate PATH and other environment variables in the current terminal and
	# build FFmpeg
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  emmake make -C "${BUILD_DIR_FFMPEG}" -j$(shell getconf _NPROCESSORS_ONLN)

	touch "$@"

$(S)/build-ffmpeg: $(BUILD_FILE_FFMPEG)
	touch "$@"

################################################################################
#
# Install
#
################################################################################

$(INSTALL_FILE_FFMPEG): $(S)/.preinstall $(S)/build-ffmpeg
	mkdir -p "$(DISTRIBUTION_DIR)"

	# Activate PATH and other environment variables in the current terminal and
	# install FFmpeg
	. "$(REPO_DIR_EMSDK)/emsdk_set_env.sh" && \
	  emmake make -C "${BUILD_DIR_FFMPEG}" install

	touch "$@"

$(S)/install-ffmpeg: $(INSTALL_FILE_FFMPEG)
	touch "$@"
