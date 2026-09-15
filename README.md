# Pixel Kombat: Genesis

**Pixel Kombat: Genesis** is a retro-inspired arcade maze game built in **C++ with SFML**.

The project began from the fundamental structure of classic Pac-Man-style gameplay — grid movement, maze navigation, pellets, tunnels, and collision — with the longer-term goal of evolving that foundation into a more combat-oriented arcade game.

Rather than simply recreating Pac-Man, Pixel Kombat is intended to experiment with what happens when classic maze mechanics are combined with abilities, enemy encounters, progression, and more aggressive player mechanics.

> **Classic maze movement. Arcade combat. Pixel-scale chaos.**

---

## Current State

Pixel Kombat is currently an **early playable prototype**.

The implemented game loop includes:

* Grid-based player movement
* Buffered directional input
* Maze wall collision
* Horizontal tunnels
* Pellet collection
* Regular and large pellets
* Score tracking
* Level completion
* Restarting
* Focus-aware movement pausing
* SFML-based rendering and input

The following systems belong to the larger Pixel Kombat vision but are **not yet implemented**:

* Enemy AI
* Combat
* Player lives
* Damage
* Rechargeable laser
* Power-ups
* Multiple levels
* Progression systems
* Audio and music systems

This README separates those planned systems from the current implementation intentionally.

---

## Gameplay

The current prototype follows a traditional arcade-maze loop:

```text
Navigate maze
     ↓
Collect pellets
     ↓
Increase score
     ↓
Clear remaining food
     ↓
Complete maze
```

### Movement

The player moves through the maze using the arrow keys.

Movement is grid-oriented rather than unrestricted.

Directional input is buffered, allowing the player to request a turn before reaching an intersection.

For example:

```text
Moving right
     │
     │ player presses ↑
     ▼
Continue right until
upward path is valid
     │
     ▼
Turn upward
```

This produces movement closer to traditional arcade maze games than immediately changing direction whenever a key is pressed.

---

## Maze Collision

Maze geometry determines where the player is allowed to move.

Movement checks the intended direction against the surrounding maze structure before advancing the player.

This prevents the player's sprite from becoming the authority over collision.

Conceptually:

```text
Player input
     ↓
Requested direction
     ↓
Maze collision check
     │
     ├── blocked → remain on current path
     │
     └── valid   → apply movement
```

Horizontal tunnel connections allow the player to travel from one side of the maze to the other where supported by the level.

---

## Scoring

The current scoring model is deliberately simple.

| Item           | Score |
| -------------- | ----: |
| Regular pellet |    10 |
| Large pellet   |    50 |

The current score is displayed in the game window title.

When all food has been collected, player movement stops and the level-completion state is displayed.

---

## Controls

### Movement

```text
↑ ↓ ← →
Arrow keys
```

### Restart

```text
R
```

### Exit

```text
Escape
```

Movement automatically pauses when the game window loses focus.

---

## The Larger Pixel Kombat Idea

The current Pac-Man-like foundation is intended to become the base for a more combat-oriented arcade game.

One of the central planned mechanics is a **rechargeable laser ability**.

The concept is that normal maze gameplay remains the primary way of earning points.

As the player collects food:

```text
Collect pellets
      │
      ├── Score increases
      │
      └── Laser charge increases
```

At sufficient charge, the player would be able to release a powerful beam as an emergency offensive ability.

The laser itself would not generate additional score.

This preserves the maze game's original incentive:

> **Food creates score. Combat creates survival opportunities.**

The ability should therefore be tactical rather than replacing the core gameplay loop.

---

## Planned Gameplay Model

The larger game concept currently looks roughly like:

```text
               Maze Navigation
                      │
                      ▼
               Pellet Collection
                 │          │
                 │          └────► Score
                 │
                 ▼
             Ability Charge
                 │
                 ▼
              Combat
                 │
        ┌────────┴────────┐
        ▼                 ▼
      Escape          Enemy control
        │                 │
        └────────┬────────┘
                 ▼
              Survive
```

Future development may change individual mechanics as the prototype evolves.

---

## Why C++ / SFML?

Pixel Kombat is also a learning project in lower-level game development.

Unlike my web projects, much less of the runtime is provided by application frameworks.

The project requires direct handling of concepts such as:

* Game loops
* Delta time
* Input
* Movement
* Collision
* Rendering
* Sprite positioning
* Asset loading
* Native compilation
* Executable distribution
* Runtime DLL dependencies

SFML provides the multimedia layer while C++ controls the game logic itself.

This makes Pixel Kombat useful not only as a game project but as an exploration of how interactive software works below the abstractions I normally use on the web.

---

## Technology

Pixel Kombat currently uses:

* **C++17**
* **SFML**
* **MinGW / GCC**
* **VS Code**
* Native Windows executables

---

## Project Structure

The current project is centered around the `genesis` implementation.

At a high level:

```text
Pixel-Kombat/
│
└── genesis/
    │
    ├── src/
    │   └── main.cpp
    │
    ├── assets/
    │   ├── pacman.png
    │   ├── stage_atlas.png
    │   └── food.png
    │
    └── bin/
        └── pkg.exe
```

Additional test and development files may exist outside this simplified view.

The executable expects the game assets to remain available at runtime.

Asset lookup supports expected locations around the executable and project working directory so the game can be run from normal development and distribution layouts.

---

## Building

The project is currently developed on **Windows** using GCC/MinGW and SFML.

The configured VS Code build task can be used to compile the main game.

The resulting executable is:

```text
pkg.exe
```

The executable must be distributed with:

* The required `assets` directory
* Required SFML runtime DLLs

The current build environment is Windows-focused rather than a portable cross-platform build system.

---

## Tests

Movement and maze logic have standalone tests that do not require SFML.

Using the current Windows development toolchain:

```powershell
& 'C:\g++\g++ 14.2\mingw64\bin\g++.exe' -std=c++17 -Wall -Wextra tests/maze_tests.cpp -o bin/maze_tests.exe

& .\bin\maze_tests.exe
```

Separating game logic from rendering dependencies allows core maze behavior to be tested without opening an SFML window.

As the project develops, more gameplay rules should follow the same pattern where practical.

---

## Architecture Direction

The current prototype is small, but several architectural principles are already useful.

### Game state should not depend on rendering

Movement, maze rules, collision, scoring, and future combat logic should remain as independent from SFML presentation as practical.

The renderer should display game state rather than define it.

### Input expresses intent

Keyboard input requests movement.

The maze and gameplay rules determine whether that requested movement can actually occur.

### Gameplay systems should remain composable

Future systems such as:

```text
movement
collision
enemies
combat
abilities
score
lives
power-ups
```

should interact without forcing all game logic into a single monolithic update function.

### Test rules without the window

Where a gameplay rule does not fundamentally require rendering or audio, it should be possible to test it separately from SFML.

---

## Planned Systems

Future development may explore:

* Enemy movement and AI
* Player lives and death states
* Rechargeable laser combat
* Power-ups
* Multiple maze layouts
* Increasing difficulty
* Improved level progression
* Animation
* Sound effects
* Music
* Additional pixel art
* Better game-state architecture
* Menu and pause systems
* Native release packaging
* Potential cross-platform builds

The exact design remains open while the fundamental systems are developed.

---

## Inspiration

Pixel Kombat is heavily inspired by the gameplay foundations established by **Pac-Man**.

The intention is not to claim the original maze-game mechanics as unique.

The project uses those familiar mechanics as a starting point for experimenting with different systems, particularly combat and player abilities.

Original gameplay additions, code, artwork, level design, and presentation can evolve independently from that inspiration as the project grows.

---

## Status

**Prototype / Active experiment**

Pixel Kombat: Genesis should currently be viewed as a playable engineering prototype rather than a finished game.

The most important work at this stage is establishing strong foundations:

```text
movement
   ↓
collision
   ↓
world rules
   ↓
entities
   ↓
gameplay systems
   ↓
combat
   ↓
polish
```

Building those foundations correctly matters more than rapidly layering features onto a fragile core.

---

## License

Pixel Kombat: Genesis is a personal game-development project.

Unless a separate license is provided, the source code, original assets, and original game content are not licensed for redistribution or reuse.
