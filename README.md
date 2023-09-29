# retro-dapp

This repo contains the frontend, backend, and learning libraries for retro.ai.

Each game integration has files listing memory locations for in-game variables, reward functions based on those variables, episode end conditions, savestates at the beginning of levels and a file containing hashes of ROMs that work with these files.

Please note that ROMs are not included and you must obtain them yourself. Most ROM hashes are sourced from their respective No-Intro SHA-1 sums.

## Building

In the root folder, execute the following commands:

```bash
cmake .
make -j$(nproc)
```

To build the Cartesi backend, enter the `backend/` directory and run the following command:

```bash
docker buildx bake -f docker-bake.hcl -f docker-bake.override.hcl --load
```

## Testing

Once `make` has completed in the root folder, try running the two example learners:

```bash
./random_agent.py
./brute_agent.py
```

## Repo layout

The following subfolders compose the repo architecture:

* `backend/` - The Cartesi Machine logic
* `frontend/` - The website and frontend logic
* `lib/` - The machine-independent emulator logic
* `retroai/` - The experimental AI logic
* `tools/` - Tooling for dependencies

The following subfolders were left over from OpenAI and will be adapted for our needs:

* `cores/` - Emulator source code (will be moved to depends)
* `retro/` - Emulator data and generated build files (will be moved somewhere else)
* `src/` - OpenAI C++ Gym Retro environment
* `test/roms/` - Test ROMs from OpenAI
* `third-party/` - Third party libraries for Gym Retro (will be moved to depends)

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

See [ThirdParty.md](LICENSES/ThirdParty.md) for information on the licenses of the individual cores.

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
