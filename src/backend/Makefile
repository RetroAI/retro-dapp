################################################################################
#
#  Copyright (C) 2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  This file is derived from Sunodo under the Apache 2.0 License
#  Copyright (C) 2023-2024 Sunodo (https://docs.sunodo.io)
#
#  SPDX-License-Identifier: AGPL-3.0-or-later AND Apache-2.0
#  See the file LICENSE.txt for more information.
#
################################################################################

CXX  := g++

.PHONY: clean

dapp: dapp.cpp
	$(CXX) -pthread -std=c++17 -I /opt/riscv/kernel/work/linux-headers/include -o $@ $^

clean:
	@rm -rf dapp
