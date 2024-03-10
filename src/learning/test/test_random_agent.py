################################################################################
#
#  Copyright (C) 2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  SPDX-License-Identifier: AGPL-3.0-or-later
#  See the file LICENSE.txt for more information.
#
################################################################################

import os
import sys

# Get the absolute path of the current script's directory
current_dir: str = os.path.dirname(os.path.abspath(__file__))

# Get the parent directory of the current directory, which is the project root
project_root: str = os.path.dirname(current_dir)

# Add the project root to sys.path
sys.path.insert(0, project_root)


import retroai.retro_env


def test_random_agent() -> None:
    env: retroai.retro_env.RetroEnv = retroai.retro_env.retro_make(
        game="Airstriker-Genesis"
    )

    env.reset()
    for _ in range(1000):
        obs, rew, done, info = env.step(env.action_space.sample())
        print(info)
        if done:
            env.reset()

    env.close()
