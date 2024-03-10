################################################################################
#
#  Copyright (C) 2023-2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  This file is derived from OpenAI's Gym Retro under the MIT license
#  Copyright (C) 2017-2018 OpenAI (http://openai.com)
#
#  SPDX-License-Identifier: AGPL-3.0-or-later AND MIT
#  See the file LICENSE.txt for more information.
#
################################################################################

from enum import Enum


class State(Enum):
    """
    Special values for setting the restart state of the environment.  You can
    also specify a string that is the name of the ``.state`` file
    """

    # Start the game at the default savestate from ``metadata.json``
    DEFAULT = -1

    # Start the game at the power on screen for the emulator
    NONE = 0


class Observations(Enum):
    """
    Different settings for the observation space of the environment
    """

    # Use RGB image observations
    IMAGE = 0

    # Use RAM observations where you can see the memory of the game instead of
    # the screen
    RAM = 1


class Actions(Enum):
    """
    Different settings for the action space of the environment
    """

    # MultiBinary action space with no filtered actions
    ALL = 0

    # MultiBinary action space with invalid or not allowed actions filtered out
    FILTERED = 1

    # Discrete action space for filtered actions
    DISCRETE = 2

    # MultiDiscete action space for filtered actions
    MULTI_DISCRETE = 3
