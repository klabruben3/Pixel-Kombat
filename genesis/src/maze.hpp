#pragma once

#include <algorithm>
#include <cmath>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace game {
constexpr float tileSize = 32.f;
constexpr float moveSpeed = 160.f;
enum class Direction { None, Left, Right, Up, Down };
struct Cell { int x = 0; int y = 0; };
inline bool operator==(Cell a, Cell b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(Cell a, Cell b) { return !(a == b); }
struct Position { float x; float y; };
inline Cell offset(Direction d) {
    switch (d) {
    case Direction::Left: return {-1, 0};
    case Direction::Right: return {1, 0};
    case Direction::Up: return {0, -1};
    case Direction::Down: return {0, 1};
    default: return {0, 0};
    }
}
inline bool isWall(char c) {
    return std::string("79T1-3|<>^v[]_+").find(c) != std::string::npos;
}
inline Direction opposite(Direction d) {
    switch (d) {
    case Direction::Left: return Direction::Right;
    case Direction::Right: return Direction::Left;
    case Direction::Up: return Direction::Down;
    case Direction::Down: return Direction::Up;
    default: return Direction::None;
    }
}
inline Position position(Cell cell, Direction direction, float progress) {
    const Cell delta = offset(direction);
    return {(cell.x + .5f) * tileSize + delta.x * progress,
            (cell.y + .5f) * tileSize + delta.y * progress};
}

class Maze {
public:
    std::vector<std::string> cells;
    Cell player;
    Cell spawn;
    Direction direction = Direction::None;
    Direction requested = Direction::None;
    float progress = 0.f;
    int score = 0;
    int remaining = 0;
    std::vector<char> collected;
    std::vector<std::vector<bool>> house;

    explicit Maze(std::vector<std::string> map) : cells(std::move(map)) {
        if (cells.empty() || cells.front().empty()) throw std::runtime_error("Empty maze");
        int spawns = 0;
        for (int y = 0; y < height(); ++y) {
            if (static_cast<int>(cells[y].size()) != width()) throw std::runtime_error("Uneven maze rows");
            for (int x = 0; x < width(); ++x) {
                char c = cells[y][x];
                if (c == '@') { player = {x, y}; ++spawns; }
                if (c == '*' || c == '&') ++remaining;
            }
        }
        if (spawns != 1) throw std::runtime_error("Maze requires one player spawn");
        spawn = player;
        house.assign(height(), std::vector<bool>(width(), false));
        // Flood the enclosed home behind '=' without changing its floor artwork.
        bool hasDoor = false;
        for (const auto& row : cells) hasDoor |= row.find('=') != std::string::npos;
        if (hasDoor) {
            std::queue<Cell> pending;
            for (int y = 0; y < height(); ++y) for (int x = 0; x < width(); ++x) {
                if (std::string("RPGC").find(cells[y][x]) != std::string::npos) {
                    house[y][x] = true;
                    pending.push({x, y});
                }
            }
            while (!pending.empty()) {
                Cell c = pending.front(); pending.pop();
                for (Direction d : {Direction::Up, Direction::Left, Direction::Down, Direction::Right}) {
                    Cell delta = offset(d), n{c.x + delta.x, c.y + delta.y};
                    if (inBounds(n) && !house[n.y][n.x] && !isWall(cells[n.y][n.x]) && cells[n.y][n.x] != '=') {
                        house[n.y][n.x] = true;
                        pending.push(n);
                    }
                }
            }
            if (house[spawn.y][spawn.x]) throw std::runtime_error("Ghost house must be enclosed by walls and a door");
        }
    }
    int width() const { return static_cast<int>(cells.front().size()); }
    int height() const { return static_cast<int>(cells.size()); }
    bool inBounds(Cell c) const { return c.y >= 0 && c.y < height() && c.x >= 0 && c.x < width(); }
    bool walkable(Cell c, bool homeAccess = false) const {
        return inBounds(c) && !isWall(cells[c.y][c.x]) &&
            (homeAccess || (cells[c.y][c.x] != '=' && !house[c.y][c.x]));
    }
    bool isTunnel(int row) const { return walkable({0, row}) && walkable({width() - 1, row}); }
    bool neighbor(Cell from, Direction d, Cell& to, bool homeAccess = false) const {
        if (d == Direction::None) return false;
        Cell delta = offset(d);
        to = {from.x + delta.x, from.y + delta.y};
        // Only rows with two open ends are horizontal tunnels.
        if (to.x < 0 || to.x >= width()) {
            if (!walkable({0, from.y}) || !walkable({width() - 1, from.y})) return false;
            to.x = (to.x + width()) % width();
        }
        return walkable(to, homeAccess);
    }
    void request(Direction d) {
        requested = d;
        Cell a = offset(direction), b = offset(d);
        if (progress > 0.f && a.x == -b.x && a.y == -b.y) {
            Cell next;
            if (neighbor(player, direction, next)) {
                player = next;
                progress = tileSize - progress;
                direction = d;
            }
        }
    }
    void collect() {
        char& c = cells[player.y][player.x];
        if (c == '*' || c == '&') {
            score += c == '*' ? 10 : 50;
            --remaining;
            collected.push_back(c);
            c = ' ';
        }
    }
    void update(float seconds) {
        if (!std::isfinite(seconds) || seconds <= 0.f) return;
        float distance = seconds * moveSpeed;
        while (distance > 0.f) {
            Cell next;
            if (progress == 0.f) {
                collect();
                if (neighbor(player, requested, next)) direction = requested;
                if (!neighbor(player, direction, next)) { direction = Direction::None; break; }
            }
            const float step = std::min(distance, tileSize - progress);
            progress += step;
            distance -= step;
            if (progress >= tileSize - .0001f) {
                neighbor(player, direction, next);
                player = next;
                progress = 0.f;
                collect();
            }
        }
    }
};
} // namespace game
