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
# Set properties based on the environment
#
################################################################################

#
# Operating system detection
#

PLATFORM =
ifeq ($(OS),Windows_NT)
	PLATFORM = windows
else
	KERNEL_NAME = $(shell uname -s)
	ifeq ($(KERNEL_NAME),Linux)
		PLATFORM = linux
	else ifeq ($(KERNEL_NAME),Darwin)
		PLATFORM = darwin
	else
		PROCESSOR = $(shell uname -p)
		ifneq ($(filter arm%,$(PROCESSOR)),)
			PLATFORM = arm
		endif
	endif
endif
