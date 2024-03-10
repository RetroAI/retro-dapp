################################################################################
#
#  Copyright (C) 2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  SPDX-License-Identifier: AGPL-3.0-or-later
#  See the file LICENSE.txt for more information.
#
################################################################################

"""
This file is used to set up the test environment for pytest.

It adds the project root to sys.path, so that the tests can import modules
from the project.
"""

import os
import sys

# Get the absolute path of the current script's directory
current_dir: str = os.path.dirname(os.path.abspath(__file__))

# Get the parent directory of the current directory, which is the learning environment root
project_root: str = os.path.dirname(current_dir)

# Add the project root to sys.path
sys.path.insert(0, project_root)
