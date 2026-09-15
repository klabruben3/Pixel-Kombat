# Pac-Man gameplay integration

## Inspection and system selection

| Existing Genesis system | Missing capability | Pacman-Main reference | PacShow reference | Integration |
| --- | --- | --- | --- | --- |
| 32px grid, buffered player movement | Ghost navigation and personalities | `Ghost.cpp`: target distance, four personalities, no ordinary U-turns | `targetCalc.h`, `ghost.h`: map-specific junction symbols | Keep Genesis grid; choose valid neighboring cells by target distance |
| R/C/G/P house markers | Release, return, recovery | Door permissions, eaten eyes returning to home and then exit | Immediate relocation home after eating | Timed release and BFS routes for leaving/returning; normal ghosts cannot re-enter |
| Food removal and score | Power effects, ghost contacts | Energizer timer, frightened and eaten states, world collision | Blue state, escalating ghost score | Power events, six-second initial duration, 200/400/800/1600 combo, logical hit boxes |
| Restart and food completion | Lives, death, level states | Death animation and level-dependent timing | Three lives, 10,000-point extra life, level reset | Ready/Playing/Dying/LevelClear/GameOver; score and remaining food survive death |
| No audio | Sound loading and transitions | No audio system in the supplied implementation | `gameSound.h`, `starter.h`, `pac.h`, `ghost.h` | Load once, own buffers before players, event cues and exclusive siren/frightened loops |
| Square source rectangles; outer-window clipping | Aspect stability and seamless tunnels | Different presentation/wrap logic | Different presentation/wrap logic | Keep Genesis graphics; uniform compositing and maze-sized clipping |

The supplied projects were inspected as engineering references. Their game
loops, UI, maps, threading, and asset-loading patterns were not imported.

## Visual fixes

**Stretching:** the previous resize handler set a fixed square view over the
entire window. A wide or tall window therefore applied different horizontal
and vertical scales. Pac-Man's source rectangles were already square; the
distortion happened in the view-to-window mapping.

`render.hpp::actorSprite` now requires a square in-bounds source rectangle and
uses one scale, `32 / sourceFrameSize`, on both axes. Pac-Man retains the original
32x32, six-frame atlas. Ghost frames are 30x30 and uniformly rendered at 32x32.
`present` fits the complete scene with one scale and centers it with black
letterboxing. The original 16px logical margin and map arrangement are retained.

**Tunnel seam:** the old projection offset was 736px (23 cells), but sprites
were clipped against a 768px window. That extra margin exposed too much of the
two images, and deciding to draw copies from cell/direction alone ignored the
actual sprite extent.

The map and actors now draw into a 736x736 render texture. Positions within
that surface use `(cell + 0.5) * 32 + direction * progress`. `drawActor` uses
`projections` to draw only sprite bounds intersecting the maze, at offsets of
exactly one maze width. The render texture clips complementary fragments at the
actual seam. Position normalization remains in grid movement when a segment
ends; the visual image remains identical under a one-world-width translation,
including immediate reversal during a crossing. Copies share the same sprite,
texture, frame, scale, color and rotation. There is one logical actor per ghost
and one logical Pac-Man. Returning eyes use the same projection helper.

## Gameplay

- `maze.hpp` remains the single map and player movement authority. It now emits
  collection events, distinguishes the ghost house/door, and snaps tiny floating
  errors at cell arrivals. Movement and collision use world coordinates only.
- `stage.hpp` contains the original map. The already blank house opening at
  column 11, row 10 is now `=` in its logical data; it still renders as the same
  blank floor. The house is flood-filled behind that door. Pac-Man and ordinary
  ghosts cannot enter it; leaving/returning ghosts can.
- `game.hpp` owns the four ghosts and gameplay transitions. Its 120Hz fixed
  simulation steps prevent two actors from skipping past each other between
  contact checks. Contact uses 20px center-distance thresholds on each axis and
  the shortest horizontal distance in tunnel rows, independent of textures.
- Blinky targets Pac-Man. Pinky targets two cells ahead. Inky doubles the vector
  from Blinky through one cell ahead of Pac-Man. Clyde chases when at least four
  cells away and retreats to his corner nearby. These compact-map distances
  follow Pacman-Main's actual constants. Each has a separate scatter corner.
- Normal ghosts choose legal neighbors with the smallest squared target
  distance, excluding reversal except at dead ends. Mode changes reverse them.
  The scatter/chase schedule is 7/20/7/20/5/20/5 seconds, then continuous chase.
  The schedule pauses during frightened mode. Frightened ghosts slow down and
  choose random legal turns. Tunnels also slow ordinary ghosts.
- Eaten ghosts become harmless eyes and use breadth-first paths through the
  maze to their own spawn. They wait one second, leave again, and are no longer
  vulnerable to the old power pellet. A newly collected power pellet can
  frighten them again. No ghost is removed permanently.
- Ready and death pauses are nonblocking. Death displays the original Pac-Man
  frames with a fade; the square dimensions never change. Completing food
  advances to the next level with score/lives retained. Ghost speed gradually
  increases, capped below Pac-Man's speed; frightened time has a safe minimum.

## Audio

`audio.hpp` adapts PacShow's useful audio behavior into one owner with fixed
sound channels. Sounds are loaded once. Collection, power, ghost-eaten, start,
death, level-clear, and extra-life events are consumed once. Pellet playback
does not restart an already playing cue. Multiple ghost-eaten events share one
bounded channel. Only one siren/frightened ambience loop is active, changed on
state transitions. Death, completion and restart stop outdated audio. Focus
loss pauses/resumes active voices; M changes volume without resetting playback.

Missing sound files log a diagnostic without hanging gameplay. The actual
seven supplied WAV files are included under `assets/sound/`. Their origins and
the new ghost atlas are recorded in `assets/REFERENCE-ASSETS.md`.

## Files

Changed: `src/main.cpp`, `src/maze.hpp`, `.vscode/tasks.json`, rebuilt `bin/pkg.exe`.

Added: `src/game.hpp`, `src/render.hpp`, `src/tiles.hpp`, `src/stage.hpp`,
`src/audio.hpp`, `build.ps1`, `.gitignore`, this report, local README, asset
provenance, `tests/game_tests.cpp`, `tests/render_tests.cpp`, and
`tests/audio_tests.cpp`. The original movement regression tests remain.

New runtime assets: one ghost atlas, seven WAV files, SFML audio DLLs. Existing
Pac-Man, food and wall artwork is unchanged. Reference folders are untouched.

## Deliberate limits

- PacShow's attract/demo screen, fonts, background maps, UI, detached actor
  threads, busy-wait audio, fruit bonuses and unrelated sprites were not
  integrated. Fruit is optional and outside the required core gameplay loop.
- The two references differ from the original arcade game's exact rules. This
  is a playable adaptation, not a cycle-exact arcade emulation: no original
  targeting bugs, pellet-counter house release or exact late-level speed tables.
- Level progression currently repeats the existing Genesis maze with difficulty
  changes. There are no additional map designs or high-score persistence.
- The project's proposed laser is not implemented by either reference and
  remains a separate future mechanic.

## Validation

`build.ps1 -Tests` runs four suites: existing movement regressions, full real-map
food traversal and gameplay transitions, pixel-for-pixel tunnel rendering for
every actor frame/direction, and sound loading/transition counters. Render
captures cover the original square layout, a wide window, a tall window and
progressive seam fragments. `--smoke-test` exercises real window creation,
asset/audio loading, compositing, and normal shutdown.

The captures are generated using the same rendering functions as the game.
Automated sound checks run muted; they validate decoding and playback behavior,
not a subjective listening assessment. A human playthrough remains useful for
difficulty and sound-volume tuning.
