################################################################################
#
#  Copyright (C) 2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  SPDX-License-Identifier: AGPL-3.0-or-later
#  See the file LICENSE.txt for more information.
#
################################################################################

import subprocess


def run_command(command) -> None:
    result: subprocess.CompletedProcess[bytes] = subprocess.run(
        command, shell=True
    )
    if result.returncode != 0:
        exit(result.returncode)


def main() -> None:
    print("Running Black for code formatting...")
    run_command("black --check .")

    print("Running isort for import sorting...")
    run_command("isort --check .")

    print("Running flake8 for linting...")
    run_command("flake8 .")

    print("Running mypy for type checking...")
    run_command("mypy . --check-untyped-defs")

    print("Running pytest for testing...")
    run_command("pytest")


def format() -> None:
    print("Running Black for code formatting...")
    run_command("black .")

    print("Running isort for import sorting...")
    run_command("isort .")


if __name__ == "__main__":
    main()
