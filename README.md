# MacroGD

Geode mod prototype for recording Geometry Dash classic-mode jump inputs and exporting `.gdr2` replay files.

## Current status

- Pause-menu button starts a recording.
- Input events are collected from `GJBaseGameLayer::handleButton`.
- The frame counter is tied to `GJBaseGameLayer::processCommands` calls.
- On level completion, MacroGD writes a replay into the mod's save directory using the level name.

**Important:** this is an early prototype. I have committed the source, but I have not successfully compiled it or tested its replay files in Geometry Dash or another replay player. Treat the GDR2 writer as experimental until interoperability tests pass.

## Compatibility target

- Geometry Dash: `2.2074`
- Geode SDK: `4.0.0`
- C++20

## Build on Windows

1. Install the Geode CLI and the matching Geode 4.0.0 SDK.
2. Open PowerShell in the repository folder.
3. Set `GEODE_SDK` to the SDK directory:

```powershell
$env:GEODE_SDK = 'C:\path\to\GeodeSDK'
geode build --config Release
```

## Known limitations

- Classic mode only; the current writer records the jump button.
- Platformer input encoding is not implemented.
- Death frames, seed, coin count and LDM metadata are not captured yet.
- GDR2 serialization is based on the published format layout and still needs validation against GDReplayFormat and real players.
- Exact replay determinism depends on the game build, physics tick timing and level state.

## Project layout

- `src/main.cpp` — Geode hooks, pause button, recording and file export.
- `src/gdr2.hpp` — isolated experimental GDR2 serializer.
- `mod.json` — Geode and Geometry Dash compatibility metadata.
- `CMakeLists.txt` — standard Geode SDK CMake setup.