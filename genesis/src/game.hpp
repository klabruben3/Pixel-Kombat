#pragma once
#include "maze.hpp"
#include <array>
#include <limits>
#include <random>

namespace game {
enum class Phase { Ready, Playing, Dying, LevelClear, GameOver };
enum class GhostMode { House, Leaving, Scatter, Chase, Frightened, Eaten };
enum class Event { Start, Pellet, Power, GhostEaten, Death, LevelClear, ExtraLife };
struct Ghost {
    Cell cell, home;
    Direction direction = Direction::None;
    float progress = 0.f;
    GhostMode mode = GhostMode::House;
    float release = 0.f;
    bool recovered = false;
};
inline constexpr std::array<Direction, 4> directions{
    Direction::Up, Direction::Left, Direction::Down, Direction::Right};
inline float distanceSquared(Cell a, Cell b) {
    const float x = static_cast<float>(a.x - b.x), y = static_cast<float>(a.y - b.y);
    return x * x + y * y;
}

class Game {
public:
    Maze maze;
    std::array<Ghost, 4> ghosts;
    Phase phase = Phase::Ready;
    int lives = 3, level = 1;
    float phaseTime = 4.5f;
    float frightenedTime = 0.f;
    float waveTime = 7.f;
    int wave = 0;
    float animationTime = 0.f;
    Direction facing = Direction::Right;
    std::vector<Event> events;
    Cell homeExit{};

    explicit Game(const std::vector<std::string>& map, unsigned seed = 0x504b47)
        : maze(map), original(map), random(seed) {
        findHome();
        resetActors();
        events.push_back(Event::Start);
    }
    GhostMode normalMode() const { return wave % 2 == 0 ? GhostMode::Scatter : GhostMode::Chase; }
    void request(Direction d) {
        if (phase == Phase::Ready) maze.requested = d;
        else if (phase == Phase::Playing) maze.request(d);
    }
    void restart() {
        maze = Maze(original);
        lives = 3; level = 1; extraLife = false;
        phase = Phase::Ready; phaseTime = 4.5f; accumulator = 0.f;
        resetActors();
        events = {Event::Start};
    }
    void update(float seconds) {
        if (!std::isfinite(seconds) || seconds <= 0.f) return;
        accumulator += seconds;
        constexpr float tick = 1.f / 120.f;
        while (accumulator + 0.000001f >= tick) {
            accumulator = std::max(0.f, accumulator - tick);
            step(tick);
        }
    }
    Position playerPosition() const { return position(maze.player, maze.direction, maze.progress); }
    Position ghostPosition(const Ghost& g) const { return position(g.cell, g.direction, g.progress); }
    Cell target(int id) const {
        const std::array<Cell, 4> corners{{{maze.width()-1, 0}, {0, 0},
                                         {maze.width()-1, maze.height()-1}, {0, maze.height()-1}}};
        if (ghosts[id].mode == GhostMode::Scatter) return corners[id];
        Cell pac = maze.player, heading = offset(facing);
        // Pacman-Main's short look-ahead distances fit this compact 23-cell maze.
        if (id == 1) return {pac.x + 2 * heading.x, pac.y + 2 * heading.y};
        if (id == 2) return {2 * (pac.x + heading.x) - ghosts[0].cell.x,
                             2 * (pac.y + heading.y) - ghosts[0].cell.y};
        if (id == 3 && distanceSquared(ghosts[id].cell, pac) < 16.f) return corners[id];
        return pac;
    }
    bool touching(Position a, Position b) const {
        float dx = std::abs(a.x - b.x);
        const int rowA = static_cast<int>(a.y / tileSize), rowB = static_cast<int>(b.y / tileSize);
        if (rowA == rowB && maze.isTunnel(rowA)) {
            const float width = maze.width() * tileSize;
            dx = std::fmod(dx, width);
            dx = std::min(dx, width - dx);
        }
        // Logical hit boxes, independent of atlas dimensions or display scaling.
        return dx < 20.f && std::abs(a.y - b.y) < 20.f;
    }

private:
    std::vector<std::string> original;
    std::mt19937 random;
    float accumulator = 0.f;
    int ghostChain = 0;
    bool extraLife = false;

    void findHome() {
        std::array<int, 4> found{};
        bool exitFound = false;
        const std::string ids = "RPGC";
        for (int y = 0; y < maze.height(); ++y) for (int x = 0; x < maze.width(); ++x) {
            auto id = ids.find(maze.cells[y][x]);
            if (id != std::string::npos) { ghosts[id].home = {x,y}; ++found[id]; }
            if (maze.cells[y][x] == '=') {
                for (Direction d : directions) {
                    Cell delta = offset(d), n{x + delta.x, y + delta.y};
                    if (maze.walkable(n)) { homeExit = n; exitFound = true; }
                }
            }
        }
        if (!exitFound || std::any_of(found.begin(), found.end(), [](int n) { return n != 1; }))
            throw std::runtime_error("Game requires R/P/G/C ghost spawns and a house exit");
        for (const Ghost& g : ghosts) {
            if (route(g.home, homeExit) == Direction::None)
                throw std::runtime_error("Ghost home is not connected to its exit");
        }
    }
    void resetActors() {
        maze.player = maze.spawn; maze.direction = maze.requested = Direction::None;
        maze.progress = 0.f; maze.collected.clear();
        facing = Direction::Right; animationTime = 0.f;
        frightenedTime = 0.f; ghostChain = 0; wave = 0; waveTime = 7.f;
        for (int i = 0; i < 4; ++i) {
            Ghost& g = ghosts[i];
            g.cell = g.home; g.progress = 0.f; g.direction = Direction::None;
            g.mode = GhostMode::House; g.release = i * 1.5f; g.recovered = false;
        }
    }
    void reverse(Ghost& g) {
        if (g.direction == Direction::None) return;
        if (g.progress > 0.f) {
            Cell next;
            if (!maze.neighbor(g.cell, g.direction, next, true)) return;
            g.cell = next; g.progress = tileSize - g.progress;
        }
        g.direction = opposite(g.direction);
    }
    void energize() {
        frightenedTime = std::max(2.f, 6.f - (level - 1) * .4f);
        ghostChain = 0;
        events.push_back(Event::Power);
        for (Ghost& g : ghosts) {
            if (g.mode == GhostMode::Eaten) continue;
            g.recovered = false;
            if (g.mode == GhostMode::Chase || g.mode == GhostMode::Scatter || g.mode == GhostMode::Frightened) {
                g.mode = GhostMode::Frightened;
                reverse(g);
            }
        }
    }
    // Breadth-first routes are reserved for leaving/returning, so eyes cannot
    // become trapped in a greedy loop when heading back to the house.
    Direction route(Cell from, Cell to) const {
        if (from == to) return Direction::None;
        std::vector<int> visited(maze.width() * maze.height(), 0);
        std::queue<std::pair<Cell, Direction>> pending;
        pending.push({from, Direction::None}); visited[from.y * maze.width() + from.x] = 1;
        while (!pending.empty()) {
            auto current = pending.front(); pending.pop();
            for (Direction d : directions) {
                Cell next;
                if (!maze.neighbor(current.first, d, next, true)) continue;
                int key = next.y * maze.width() + next.x;
                if (visited[key]) continue;
                Direction first = current.second == Direction::None ? d : current.second;
                if (next == to) return first;
                visited[key] = 1; pending.push({next, first});
            }
        }
        return Direction::None;
    }
    Direction choose(int id) {
        Ghost& g = ghosts[id];
        if (g.mode == GhostMode::Eaten) return route(g.cell, g.home);
        if (g.mode == GhostMode::Leaving) return route(g.cell, homeExit);
        std::vector<Direction> options;
        for (Direction d : directions) {
            Cell next;
            if (d != opposite(g.direction) && maze.neighbor(g.cell, d, next)) options.push_back(d);
        }
        if (options.empty()) {
            Cell next;
            return maze.neighbor(g.cell, opposite(g.direction), next) ? opposite(g.direction) : Direction::None;
        }
        if (g.mode == GhostMode::Frightened)
            return options[std::uniform_int_distribution<std::size_t>(0, options.size()-1)(random)];
        const Cell goal = target(id);
        float best = std::numeric_limits<float>::max();
        Direction result = options.front();
        for (Direction d : options) {
            Cell next; maze.neighbor(g.cell, d, next);
            float distance = distanceSquared(next, goal);
            if (distance < best) { best = distance; result = d; }
        }
        return result;
    }
    void moveGhost(int id, float dt) {
        Ghost& g = ghosts[id];
        if (g.mode == GhostMode::House) {
            g.release -= dt;
            if (g.release > 0.f) return;
            g.mode = GhostMode::Leaving;
        }
        float speed = std::min(150.f, 120.f + (level - 1) * 4.f);
        if (g.mode == GhostMode::Frightened) speed = 75.f;
        else if (g.mode == GhostMode::Eaten) speed = 260.f;
        else if (maze.isTunnel(g.cell.y) && (g.cell.x < 4 || g.cell.x >= maze.width() - 4)) speed *= .75f;
        float distance = dt * speed;
        while (distance > 0.f) {
            if (g.progress == 0.f) {
                if (g.mode == GhostMode::Eaten && g.cell == g.home) {
                    g.mode = GhostMode::House; g.release = 1.f; g.direction = Direction::None;
                    g.recovered = true;
                    return;
                }
                if (g.mode == GhostMode::Leaving && g.cell == homeExit)
                    g.mode = frightenedTime > 0.f && !g.recovered ? GhostMode::Frightened : normalMode();
                g.direction = choose(id);
                if (g.direction == Direction::None) return;
            }
            const float step = std::min(distance, tileSize - g.progress);
            g.progress += step; distance -= step;
            if (g.progress >= tileSize - .0001f) {
                Cell next;
                if (!maze.neighbor(g.cell, g.direction, next, g.mode == GhostMode::Eaten || g.mode == GhostMode::Leaving))
                    throw std::logic_error("Ghost attempted to enter a blocked cell");
                g.cell = next; g.progress = 0.f;
            }
        }
    }
    void collisions() {
        for (Ghost& g : ghosts) {
            if (g.mode == GhostMode::House || g.mode == GhostMode::Eaten) continue;
            if (!touching(playerPosition(), ghostPosition(g))) continue;
            if (g.mode == GhostMode::Frightened) {
                maze.score += 200 * (1 << std::min(ghostChain++, 3));
                g.mode = GhostMode::Eaten;
                events.push_back(Event::GhostEaten);
            } else {
                --lives; phase = Phase::Dying; phaseTime = 2.f;
                frightenedTime = 0.f;
                events.push_back(Event::Death);
                return;
            }
        }
    }
    void step(float dt) {
        if (phase == Phase::GameOver) return;
        if (phase != Phase::Playing) {
            phaseTime = std::max(0.f, phaseTime - dt);
            if (phaseTime > .0001f) return;
            if (phase == Phase::Ready) { phase = Phase::Playing; return; }
            if (phase == Phase::Dying && lives == 0) { phase = Phase::GameOver; return; }
            const bool nextLevel = phase == Phase::LevelClear;
            if (nextLevel) {
                int score = maze.score;
                maze = Maze(original); maze.score = score; ++level;
                events.push_back(Event::Start);
            }
            resetActors(); phase = Phase::Ready; phaseTime = nextLevel ? 4.5f : 2.f;
            return;
        }
        animationTime += dt;
        if (frightenedTime > 0.f) {
            frightenedTime = std::max(0.f, frightenedTime - dt);
            if (frightenedTime == 0.f) for (Ghost& g : ghosts)
                if (g.mode == GhostMode::Frightened) g.mode = normalMode();
        } else if (wave < 7) {
            waveTime -= dt;
            if (waveTime <= 0.f) {
                ++wave; waveTime = wave % 2 ? 20.f : (wave < 4 ? 7.f : 5.f);
                for (Ghost& g : ghosts) if (g.mode == GhostMode::Chase || g.mode == GhostMode::Scatter) {
                    g.mode = normalMode(); reverse(g);
                }
            }
        }
        maze.update(dt);
        if (maze.direction != Direction::None) facing = maze.direction;
        for (char item : maze.collected) {
            if (item == '&') energize(); else events.push_back(Event::Pellet);
        }
        maze.collected.clear();
        if (maze.remaining == 0) {
            phase = Phase::LevelClear; phaseTime = 2.f;
            frightenedTime = 0.f; events.push_back(Event::LevelClear);
        } else {
            collisions();
            if (phase == Phase::Playing) {
                for (int i = 0; i < 4; ++i) moveGhost(i, dt);
                collisions();
            }
        }
        if (!extraLife && maze.score >= 10000) {
            extraLife = true; ++lives; events.push_back(Event::ExtraLife);
        }
    }
};
} // namespace game
