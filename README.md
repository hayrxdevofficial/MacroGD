# MacroGD

A Geometry Dash Geode mod for recording classic-mode inputs and exporting standard GDR2 replay files.

## Current features

- Adds a **MacroGD** button to the pause menu.
- Starts recording from the current game command index, so the exported replay begins at frame zero.
- Records classic-mode jump press/release events for both players.
- Uses the upstream [GDReplayFormat](https://github.com/maxnut/GDReplayFormat) library for GDR2 serialization instead of a handwritten binary writer.
- Sorts inputs before export and saves a uniquely named `.gdr2` file in the mod save directory after level completion.
- Builds Windows, macOS, iOS, Android 32-bit and Android 64-bit variants through the official Geode build action.

## Compatibility target

- Geometry Dash: `2.2074`
- Geode SDK: `4.0.0`
- C++23 (required by the pinned GDReplayFormat revision)

## Build

The GitHub Actions workflow uses the official `geode-sdk/build-geode-mod` action. Push a commit or manually start **Build Geode Mod** from the repository's Actions tab. The combined `.geode` package is uploaded as the `MacroGD-Build-Output` artifact when all platform jobs succeed.

For a local build, install the matching Geode SDK and Geode CLI, set `GEODE_SDK` to the SDK directory, then run:

```sh
geode build --config Release
```

## Current limitations

- Classic mode only; platformer-specific input encoding is not enabled.
- Only jump button events are recorded.
- Death frames, random seed, coin count and LDM state are not captured.
- The recorder has not yet been validated in a live Geometry Dash session or against external replay players.
- Replay determinism depends on the game version, physics tick rate, level state and runtime environment. A 100% guarantee is not claimed.

## Project layout

- `src/main.cpp` — Geode hooks, pause button, input collection and file export.
- `src/gdr2.hpp` — thin wrapper around upstream GDReplayFormat types.
- `CMakeLists.txt` — Geode SDK and pinned GDReplayFormat dependency.
- `mod.json` — mod metadata and supported Geometry Dash versions.
- `.github/workflows/multi-platform.yml` — multi-platform CI build.
