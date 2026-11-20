# MacroGD

A Geometry Dash Geode mod prototype for recording frame-based inputs and exporting GDR2 replay files.

## Status

Early development. The recorder/exporter is not yet validated in Geometry Dash or against third-party replay players. Do not treat generated files as guaranteed 100% deterministic.

## Build prerequisites

- Geometry Dash version matching the Geode SDK configuration
- Geode SDK checkout in `geode/`
- Geode CLI and a supported CMake toolchain

## Build

```sh
geode build
```

## Current limitations

- Classic mode input recording is the initial target.
- Platformer-specific button encoding and death-frame capture are not implemented yet.
- GDR2 export follows the published binary layout and still needs interoperability testing.
- Frame timing and replay determinism depend on the game build and runtime environment.
