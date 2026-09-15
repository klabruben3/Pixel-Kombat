# Pixel Kombat: Genesis

C++17 / SFML 3.0.2 maze game. Run `bin/pkg.exe`. Its SFML DLLs must remain beside
the executable; `assets` may be beside it or in its parent project folder.

## Controls

- Arrow keys: move; an early turn is buffered until a valid junction.
- R: restart the game with three lives and zero score.
- M: mute/unmute.
- Escape: quit.
- Losing window focus pauses gameplay and sound.

The existing title-bar UI shows score, lives, level, pellets, and game state.
Clear every pellet to advance on the same maze. A death preserves collected
pellets and score, resets the actors, and consumes one life. Three deaths end
the game. The first 10,000 points award one extra life.

Regular pellets award 10 points, power pellets 50. Power pellets frighten active
ghosts for six seconds on level one, decreasing to a two-second minimum on later
levels. Eating frightened ghosts awards 200, 400, 800, then 1,600 points per power
pellet. Eaten ghosts return to their own home, recover, and leave again.

## Build and verify

```powershell
.\build.ps1
.\build.ps1 -Tests
```

The default compiler matches the existing VS Code configuration. Override it
with `-Compiler 'path/to/g++.exe'` if necessary, using a MinGW compiler compatible
with the bundled SFML. The VS Code build task also links SFML audio.

Tests cover movement, the real maze's complete food route, ghost AI/state
transitions, lives, scoring, level progression, rendering and sound events.
Rendering tests create screenshots in `tests/visual/`, including wide and tall
window layouts and left/right tunnel fragments. Audio tests run muted.
`bin/pkg.exe --smoke-test` loads the game and audio, renders five frames muted,
then exits. It can be launched from outside the project to verify asset lookup.

See [the implementation report](docs/IMPLEMENTATION.md) for reference comparisons,
architecture, validation details, and remaining mechanics.
