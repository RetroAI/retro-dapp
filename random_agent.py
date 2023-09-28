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

import retroai.retro_env


def main():
    env = retroai.retro_env.retro_make(game="Airstriker-Genesis")

    env.reset()
    while True:
        obs, rew, done, info = env.step(env.action_space.sample())
        print(info)
        if done:
            env.reset()

    env.close()


if __name__ == "__main__":
    main()
