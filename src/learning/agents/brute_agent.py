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
current_dir: str = os.path.dirname(os.path.abspath(__file__))

# Get the parent directory of the current directory, which is the project root
project_root: str = os.path.dirname(current_dir)

# Add the project root to sys.path
sys.path.insert(0, project_root)


from typing import Optional

import gymnasium

import retroai.brute as brute_module
import retroai.enums
import retroai.retro_env


def main() -> None:
    game: str = "Airstriker-Genesis"
    state: retroai.enums.State = retroai.enums.State.DEFAULT
    scenario: Optional[str] = None
    max_episode_steps: int = 4500
    timestep_limit: float = 1e8

    retro_env: retroai.retro_env.RetroEnv = retroai.retro_env.retro_make(
        game,
        state,
        use_restricted_actions=retroai.enums.Actions.DISCRETE,
        scenario=scenario,
    )

    env: gymnasium.Wrapper = brute_module.Frameskip(retro_env)
    env = brute_module.TimeLimit(env, max_episode_steps=max_episode_steps)

    brute: brute_module.Brute = brute_module.Brute(
        env, max_episode_steps=max_episode_steps
    )
    timesteps: int = 0
    best_rew: float = float("-inf")
    while True:
        acts, rew = brute.run()
        timesteps += len(acts)

        if rew > best_rew:
            print("new best reward {} => {}".format(best_rew, rew))
            best_rew = rew

        if timesteps > timestep_limit:
            print("timestep limit exceeded")
            break


if __name__ == "__main__":
    main()
