#include "../src/game.hpp"
#include "../src/stage.hpp"
#include <cassert>
#include <iostream>

using namespace game;
Game playing() {
    Game g(stage);
    g.phase = Phase::Playing;
    g.events.clear();
    for (auto& ghost : g.ghosts) ghost.release = 100000.f;
    return g;
}
Direction nearestFood(const Maze& maze) {
    std::queue<std::pair<Cell, Direction>> pending;
    std::vector<bool> seen(maze.width() * maze.height());
    pending.push({maze.player, Direction::None});
    seen[maze.player.y * maze.width() + maze.player.x] = true;
    while (!pending.empty()) {
        auto [cell, first] = pending.front(); pending.pop();
        if (first != Direction::None && (maze.cells[cell.y][cell.x] == '*' || maze.cells[cell.y][cell.x] == '&')) return first;
        for (Direction d : directions) {
            Cell next;
            if (!maze.neighbor(cell, d, next)) continue;
            int key = next.y * maze.width() + next.x;
            if (seen[key]) continue;
            seen[key] = true;
            pending.push({next, first == Direction::None ? d : first});
        }
    }
    return Direction::None;
}
int main() {
    constexpr float dt = 1.f / 120.f;
    Game g(stage);
    assert(g.maze.width() == 23 && g.maze.height() == 23);
    assert(g.homeExit == (Cell{11,9}));
    assert(!g.maze.walkable({11,10}) && g.maze.walkable({11,10}, true));
    for (const auto& ghost : g.ghosts) {
        assert(!g.maze.walkable(ghost.home));
        assert(g.maze.walkable(ghost.home, true));
    }
    g.request(Direction::Left); g.update(4.6f);
    assert(g.phase == Phase::Playing && g.maze.direction == Direction::Left);

    // Actual maze: every pellet reachable, score exact, next level preserves lives/score.
    g = playing();
    int expected = 0;
    for (auto& row : stage) for (char c : row) expected += c == '*' ? 10 : c == '&' ? 50 : 0;
    int steps = 0;
    while (g.phase == Phase::Playing && steps++ < 10000) {
        Direction d = nearestFood(g.maze);
        assert(d != Direction::None);
        g.request(d); g.update(.2f); g.events.clear();
        assert(g.maze.progress < .001f && g.maze.walkable(g.maze.player));
    }
    assert(g.phase == Phase::LevelClear && g.maze.remaining == 0 && g.maze.score == expected);
    g.update(2.1f);
    assert(g.level == 2 && g.phase == Phase::Ready && g.maze.remaining > 0 && g.maze.score == expected);

    // Repeated collision consumes one life, pauses, preserves food, then respawns.
    g = playing();
    int food = g.maze.remaining;
    for (int life = 2; life >= 0; --life) {
        g.ghosts[0].cell = g.maze.player; g.ghosts[0].mode = GhostMode::Chase;
        g.ghosts[0].direction = Direction::None; g.ghosts[0].progress = 0;
        g.update(dt);
        assert(g.phase == Phase::Dying && g.lives == life);
        g.update(.5f); assert(g.lives == life);
        g.update(1.6f);
        if (life > 0) {
            assert(g.phase == Phase::Ready && g.maze.remaining == food);
            assert(g.maze.player == g.maze.spawn);
            g.update(2.1f);
        }
    }
    assert(g.phase == Phase::GameOver);
    g.update(10.f); assert(g.phase == Phase::GameOver && g.lives == 0);
    g.restart(); assert(g.phase == Phase::Ready && g.lives == 3 && g.maze.score == 0 && g.level == 1);

    // Power pellet wins against a same-tick contact and ghost chain is 200/400/800/1600.
    g = playing(); g.maze.player = {3,3};
    for (auto& ghost : g.ghosts) { ghost.cell = g.maze.player; ghost.mode = GhostMode::Chase; }
    g.update(dt);
    assert(g.phase == Phase::Playing && g.lives == 3 && g.frightenedTime > 5.f);
    assert(g.maze.score == 50 + 200 + 400 + 800 + 1600);
    for (auto& ghost : g.ghosts) assert(ghost.mode == GhostMode::Eaten);
    g.update(dt); assert(g.maze.score == 3050);
    assert(g.waveTime == 7.f - dt); // Wave clock pauses while frightened.

    // Returning eyes traverse walls/door correctly, wait at home, and leave again.
    g = playing(); g.ghosts[0].cell = {1,1}; g.ghosts[0].mode = GhostMode::Eaten;
    bool home = false, left = false;
    for (int i = 0; i < 120 * 60; ++i) {
        g.update(dt); g.events.clear();
        const auto& ghost = g.ghosts[0];
        assert(g.maze.walkable(ghost.cell, true));
        if (ghost.mode == GhostMode::House) home = true;
        if (home && (ghost.mode == GhostMode::Scatter || ghost.mode == GhostMode::Chase)) { left = true; break; }
    }
    assert(home && left);

    // All four release and navigate real corridors without cutting wall corners.
    g = Game(stage); g.phase = Phase::Playing;
    g.maze.player = {10,5}; // An enclosed empty tile keeps this path test collision-free.
    std::array<bool,4> released{};
    for (int tick = 0; tick < 120 * 40; ++tick) {
        g.update(dt); g.events.clear();
        assert(g.phase == Phase::Playing);
        for (int id = 0; id < 4; ++id) {
            const auto& ghost = g.ghosts[id];
            assert(g.maze.walkable(ghost.cell, true));
            if (ghost.mode == GhostMode::Chase || ghost.mode == GhostMode::Scatter) {
                released[id] = true;
                assert(g.maze.walkable(ghost.cell));
            }
            if (ghost.progress > 0.f) {
                Cell next;
                assert(g.maze.neighbor(ghost.cell, ghost.direction, next,
                    ghost.mode == GhostMode::Leaving || ghost.mode == GhostMode::Eaten));
            }
        }
    }
    for (bool value : released) assert(value);
    assert(g.wave >= 2);

    // Distinct targets and tunnel-aware collision independent of sprite size.
    g = playing(); g.facing = Direction::Right;
    for (auto& ghost : g.ghosts) ghost.mode = GhostMode::Chase;
    assert(g.target(0) == g.maze.player);
    assert(g.target(1) == (Cell{g.maze.player.x+2,g.maze.player.y}));
    assert(g.target(2) != g.target(1));
    g.ghosts[3].cell = g.maze.player;
    assert(g.target(3) == (Cell{0,22}));
    assert(g.touching({2,368},{734,368}));
    assert(!g.touching({2,48},{734,48}));

    // Frightened expires, refreshed pellets extend it, bonus life is awarded once.
    g = playing(); g.maze.player = {3,3}; g.update(dt);
    g.update(3.f); const float oldTimer = g.frightenedTime;
    g.maze.player = {1,13}; g.update(dt); assert(g.frightenedTime > oldTimer);
    g.update(6.1f); assert(g.frightenedTime == 0.f);
    g.maze.score = 10000; g.update(dt); assert(g.lives == 4);
    g.update(dt); assert(g.lives == 4);

    std::cout << "Gameplay tests passed: full maze, ghosts, power, recovery, lives, levels, scoring, tunnels\n";
}
