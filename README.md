# retro3

This repo contains the frontend, backend, and learning libraries for https://retro.ai. retro.ai is dedicated to preserving human gaming history, and then training an AI to beat it.

Each game integration has files listing memory locations for in-game variables, reward functions based on those variables, episode end conditions, savestates at the beginning of levels and a file containing hashes of ROMs that work with these files.

Please note that ROMs are not included and you must obtain them yourself. Most ROM hashes are sourced from their respective No-Intro SHA-1 sums.

## Dependencies

Nodejs, Docker, Sunodo ([Sunodo Docs](https://docs.sunodo.io/guide/introduction/installing)).

Sunodo is a build tool that simplifies the process of building and running Cartesi Machines. It is used to build the Cartesi Machine that runs the game emulator and the learning agent. It is also used to build the website frontend.

For frontend development, the pnpm tool is also used (`npm i -g pnpm`).

For the learning environment, the poetry tool is used (`python3 -m pip install poetry`).

## Quick Start

To build the backend, run the command:

```bash
sunodo build
```

To build and run the website frontend, enter the `src/frontend/` directory and use standard pnpm commands:

```bash
cd src/frontend
pnpm install
pnpm build
pnpm start
```

## Building OpenAI modules

To build the legacy OpenAI modules, enter the `openai` folder and run the build commands:

```bash
cd openai
cmake .
make -j$(nproc)
```

## Testing

Once `make` has completed in the `openai` folder, try running the two example learners:

```bash
cd src/learning/agents
./random_agent.py
./brute_agent.py
```

## Repo layout

The following subfolders compose the repo architecture:

* `src/backend/` - The Cartesi Machine logic
* `src/frontend/` - The website and frontend logic
* `src/learning/` - The Python code for AI agents
* `tools/` - Tooling for dependencies

The following subfolders were left over from OpenAI and will be adapted for our needs:

* `openai/cores/` - Emulator source code (will be moved to tools)
* `openai/retro/` - Emulator data and generated build files (will be moved somewhere else)
* `openai/src/` - OpenAI C++ Gym Retro environment
* `openai/test/roms/` - Test ROMs from OpenAI
* `openai/third-party/` - Third party libraries for Gym Retro (will be moved to depends)

## Emulated Systems

- Atari
	- Atari2600 (via Stella)
- NEC
	- TurboGrafx-16/PC Engine (via Mednafen/Beetle PCE Fast)
- Nintendo
	- Game Boy/Game Boy Color (via gambatte)
	- Game Boy Advance (via mGBA)
	- Nintendo Entertainment System (via FCEUmm)
	- Super Nintendo Entertainment System (via Snes9x)
- Sega
	- GameGear (via Genesis Plus GX)
	- Genesis/Mega Drive (via Genesis Plus GX)
	- Master System (via Genesis Plus GX)

See [LICENSES.md](openai/LICENSES.md) for information on the licenses of the individual cores.

## Included ROMs

The following non-commercial ROMs are included with Gym Retro for testing purposes:

- [the 128 sine-dot](http://www.pouet.net/prod.php?which=2762) by Anthrox
- [Sega Tween](https://pdroms.de/files/gamegear/sega-tween) by Ben Ryves
- [Happy 10!](http://www.pouet.net/prod.php?which=52716) by Blind IO
- [512-Colour Test Demo](https://pdroms.de/files/pcengine/512-colour-test-demo) by Chris Covell
- [Dekadrive](http://www.pouet.net/prod.php?which=67142) by Dekadence
- [Automaton](https://pdroms.de/files/atari2600/automaton-minigame-compo-2003) by Derek Ledbetter
- [Fire](http://privat.bahnhof.se/wb800787/gb/demo/64/) by dox
- [FamiCON intro](http://www.pouet.net/prod.php?which=53497) by dr88
- [Airstriker](https://pdroms.de/genesis/airstriker-v1-50-genesis-game) by Electrokinesis
- [Lost Marbles](https://pdroms.de/files/gameboyadvance/lost-marbles) by Vantage
