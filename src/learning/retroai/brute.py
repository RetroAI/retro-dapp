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

"""
Implementation of the Brute from "Revisiting the Arcade Learning Environment:
Evaluation Protocols and Open Problems for General Agents" by Machado et al.
https://arxiv.org/abs/1709.06009

This is an agent that uses the determinism of the environment in order to do
pretty well at a number of retro games.  It does not save emulator state but
does rely on the same sequence of actions producing the same result when played
back.
"""

import random
from typing import Any, Optional, SupportsFloat

import gymnasium
import numpy as np

EXPLORATION_PARAM: float = 0.005


class Frameskip(gymnasium.Wrapper):
    def __init__(self, env, skip: int = 4) -> None:
        super().__init__(env)
        self._skip: int = skip

    def reset(self):
        return self.env.reset()

    def step(self, act):
        total_rew: float = 0.0
        done: Optional[bool] = None
        for i in range(self._skip):
            obs, rew, done, info, _ = self.env.step(act)
            total_rew += float(rew)
            if done:
                break

        return obs, total_rew, done, info


class TimeLimit(gymnasium.Wrapper):
    def __init__(self, env, max_episode_steps: Optional[int] = None) -> None:
        super().__init__(env)
        self._max_episode_steps: Optional[int] = max_episode_steps
        self._elapsed_steps: int = 0

    def step(self, ac) -> tuple[Any, SupportsFloat, bool, bool, dict[str, Any]]:
        observation, reward, terminated, truncated, info = self.env.step(ac)
        self._elapsed_steps += 1
        if (
            self._max_episode_steps is not None
            and self._elapsed_steps >= self._max_episode_steps
        ):
            truncated = True
            info["TimeLimit.truncated"] = True
        return observation, reward, terminated, truncated, info

    def reset(self, **kwargs) -> tuple[Any, dict[str, Any]]:
        self._elapsed_steps = 0
        return self.env.reset(**kwargs)


class Node:
    def __init__(self, value=-np.inf, children=None) -> None:
        self.value = value
        self.visits = 0
        self.children = {} if children is None else children

    def __repr__(self) -> str:
        return "<Node value=%f visits=%d len(children)=%d>" % (
            self.value,
            self.visits,
            len(self.children),
        )


def select_actions(root, action_space, max_episode_steps):
    """
    Select actions from the tree

    Normally we select the greedy action that has the highest reward
    associated with that subtree.  We have a small chance to select a
    random action based on the exploration param and visit count of the
    current node at each step.

    We select actions for the longest possible episode, but normally these
    will not all be used.  They will instead be truncated to the length
    of the actual episode and then used to update the tree.
    """
    node = root

    acts = []
    steps = 0
    while steps < max_episode_steps:
        if node is None:
            # We've fallen off the explored area of the tree, just select
            # random actions
            act = action_space.sample()
        else:
            epsilon = EXPLORATION_PARAM / np.log(node.visits + 2)
            if random.random() < epsilon:
                # Random action
                act = action_space.sample()
            else:
                # Greedy action
                act_value = {}
                for act in range(action_space.n):
                    if node is not None and act in node.children:
                        act_value[act] = node.children[act].value
                    else:
                        act_value[act] = -np.inf
                best_value = max(act_value.values())
                best_acts = [
                    act
                    for act, value in act_value.items()
                    if value == best_value
                ]
                act = random.choice(best_acts)

            if act in node.children:
                node = node.children[act]
            else:
                node = None

        acts.append(act)
        steps += 1

    return acts


def rollout(env, acts):
    """
    Perform a rollout using a preset collection of actions
    """
    total_rew = 0
    env.reset()
    steps = 0
    for act in acts:
        _obs, rew, done, _info = env.step(act)
        steps += 1
        total_rew += rew
        if done:
            break

    return steps, total_rew


def update_tree(root, executed_acts, total_rew):
    """
    Given the tree, a list of actions that were executed before the game ended,
    and a reward, update the tree so that the path formed by the
    actions are all updated to the new reward.
    """
    root.value = max(total_rew, root.value)
    root.visits += 1
    new_nodes = 0

    node = root
    for step, act in enumerate(executed_acts):
        if act not in node.children:
            node.children[act] = Node()
            new_nodes += 1
        node = node.children[act]
        node.value = max(total_rew, node.value)
        node.visits += 1

    return new_nodes


class Brute:
    """
    Implementation of the Brute

    Creates and manages the tree storing game actions and rewards
    """

    def __init__(self, env, max_episode_steps: Optional[int]) -> None:
        self.node_count: int = 1
        self._root: Node = Node()
        self._env = env
        self._max_episode_steps: Optional[int] = max_episode_steps

    def run(self):
        acts = select_actions(
            self._root, self._env.action_space, self._max_episode_steps
        )
        steps, total_rew = rollout(self._env, acts)
        executed_acts = acts[:steps]
        self.node_count += update_tree(self._root, executed_acts, total_rew)
        return executed_acts, total_rew
