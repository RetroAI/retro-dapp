#!/usr/bin/env python3
################################################################################
#
#  Copyright (C) 2023 retro.ai
#  This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
#
#  This file is derived from OpenAI's Gym Retro under the MIT license
#  Copyright (c) 2017-2018 OpenAI (http://openai.com)
#
#  SPDX-License-Identifier: Apache-2.0 AND MIT
#  See the file LICENSE for more information.
#
################################################################################

import retroai.brute as brute_module
import retro.data
import retroai.enums
import retroai.retro_env


def main():
    game = "Airstriker-Genesis"
    state = retroai.enums.State.DEFAULT
    scenario = None
    max_episode_steps = 4500
    timestep_limit = 1e8

    env = retroai.retro_env.retro_make(
        game,
        state,
        use_restricted_actions=retroai.enums.Actions.DISCRETE,
        scenario=scenario,
    )

    env = brute_module.Frameskip(env)
    env = brute_module.TimeLimit(env, max_episode_steps=max_episode_steps)

    brute = brute_module.Brute(env, max_episode_steps=max_episode_steps)
    timesteps = 0
    best_rew = float("-inf")
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
