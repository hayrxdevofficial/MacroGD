# MacroGD

Android-only Geometry Dash Geode mod for recording classic-mode inputs and exporting GDR2 replay files.

## Android builds

GitHub Actions builds two Android variants:

- **Android32** — ARMv7 (32-bit)
- **Android64** — ARM64 (64-bit)

Open the repository's **Actions** tab and select **Build MacroGD for Android**. Each successful job uploads its own `.geode` artifact.

## Features

- Adds a MacroGD button to the in-game pause menu.
- Starts recording relative to the current game command index.
- Records classic-mode jump press/release events for player 1 and player 2.
- Uses the upstream [GDReplayFormat](https://github.com/maxnut/GDReplayFormat) library for GDR2 serialization.
- Sorts inputs and writes a uniquely named replay file into the mod's Geode save directory after level completion.
- Uses Geode's cross-platform filesystem API for Android-compatible save paths.

## Compatibility

- Geometry Dash Android: `2.2074`
- Geode SDK: `4.0.0`
- C++23

## Current limitations

- Classic mode only; platformer-specific input encoding is not enabled.
- Only jump button events are recorded.
- Death frames, random seed, coin count and LDM state are not captured.
- This source has not yet passed a successful Android CI build or live replay compatibility test.
- Replay determinism depends on game version, physics tick timing and level state. 100% accuracy is not guaranteed.

## Project files

- `src/main.cpp` — pause menu, input capture and file export.
- `src/gdr2.hpp` — GDReplayFormat wrapper.
- `CMakeLists.txt` — Geode SDK and pinned GDReplayFormat dependency.
- `mod.json` — Android-only compatibility metadata.
- `.github/workflows/android.yml` — Android32 and Android64 CI builds.
