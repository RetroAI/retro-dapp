#!/usr/bin/env python3
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


import os
import sys

# Get the absolute path of the current script's directory
current_dir = os.path.dirname(os.path.abspath(__file__))

# Get the parent directory of the current directory, which is the project root
project_root = os.path.dirname(current_dir)

# Add the project root to sys.path
sys.path.insert(0, project_root)


import retroai.retro_env


def main() -> None:
    env: retroai.retro_env.RetroEnv = retroai.retro_env.retro_make(
        game="Airstriker-Genesis"
    )

    env.reset()
    while True:
        obs, rew, done, info = env.step(env.action_space.sample())
        print(info)
        if done:
            env.reset()

    env.close()


if __name__ == "__main__":
    main()
