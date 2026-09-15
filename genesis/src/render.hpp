#pragma once
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <filesystem>

namespace game {
constexpr int pacFrameSize = 32;
constexpr int ghostFrameSize = 30;
constexpr float actorSize = 32.f;
static_assert(pacFrameSize == static_cast<int>(tileSize));

// All actors have square source frames and one uniform scale. The same sprite
// instance is used for every projection; only its position changes.
inline sf::Sprite actorSprite(const sf::Texture& texture, sf::IntRect frame) {
    if (frame.size.x <= 0 || frame.size.x != frame.size.y || frame.position.x < 0 || frame.position.y < 0 ||
        frame.position.x + frame.size.x > static_cast<int>(texture.getSize().x) ||
        frame.position.y + frame.size.y > static_cast<int>(texture.getSize().y))
        throw std::runtime_error("Actor frame must be square and inside its texture");
    sf::Sprite sprite(texture, frame);
    sprite.setOrigin({frame.size.x / 2.f, frame.size.y / 2.f});
    const float scale = actorSize / frame.size.x;
    sprite.setScale({scale, scale});
    return sprite;
}

template<class Draw>
void projections(Position center, float halfWidth, float worldWidth, bool tunnel, Draw draw) {
    // Clip against the same period used by logical movement, not the outer
    // window (which includes a margin). This remains continuous on reversal too.
    for (int copy = -1; copy <= 1; ++copy) {
        if (copy != 0 && !tunnel) continue;
        const float x = center.x + copy * worldWidth;
        if (x + halfWidth > 0.f && x - halfWidth < worldWidth) draw(sf::Vector2f{x, center.y});
    }
}
inline void drawActor(sf::RenderTarget& target, sf::Sprite& sprite, Position center, float width, bool tunnel) {
    const float halfWidth = sprite.getGlobalBounds().size.x / 2.f;
    projections(center, halfWidth, width, tunnel, [&](sf::Vector2f p) {
        sprite.setPosition(p);
        target.draw(sprite);
    });
}
struct Fit { float scale; sf::Vector2f offset; };
inline Fit fitCanvas(sf::Vector2u window, sf::Vector2f canvas) {
    const float scale = std::min(window.x / canvas.x, window.y / canvas.y);
    return {scale, {(window.x - canvas.x * scale) / 2.f, (window.y - canvas.y * scale) / 2.f}};
}
inline void present(sf::RenderTarget& target, const sf::Texture& world) {
    const auto size = world.getSize();
    const sf::Vector2f canvas{size.x + tileSize, size.y + tileSize};
    const Fit fit = fitCanvas(target.getSize(), canvas);
    // Use window pixels here; never stretch a fixed logical view to fit a resize.
    target.setView(sf::View(sf::FloatRect({0.f, 0.f},
        {static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y)})));
    sf::Sprite scene(world);
    scene.setScale({fit.scale, fit.scale});
    scene.setPosition(fit.offset + sf::Vector2f{tileSize / 2.f, tileSize / 2.f} * fit.scale);
    target.draw(scene);
}

class Actors {
public:
    sf::Texture pacman, ghost;
    unsigned frameCount = 0;
    explicit Actors(const std::filesystem::path& assets) {
        if (!pacman.loadFromFile(assets / "pacman.png") || !ghost.loadFromFile(assets / "ghost_atlas.png"))
            throw std::runtime_error("Failed to load actor textures");
        if (pacman.getSize().y != pacFrameSize || pacman.getSize().x % pacFrameSize != 0)
            throw std::runtime_error("Pac-Man atlas must contain a single row of square 32px frames");
        frameCount = pacman.getSize().x / pacFrameSize;
        if (frameCount == 0 || ghost.getSize().x < 8 * ghostFrameSize || ghost.getSize().y < 6 * ghostFrameSize)
            throw std::runtime_error("Invalid actor atlas dimensions");
        pacman.setSmooth(false); ghost.setSmooth(false);
    }
    void drawPacman(sf::RenderTarget& target, const Game& game) const {
        int frame = static_cast<int>(game.animationTime / .1f) % frameCount;
        if (game.phase == Phase::Dying)
            frame = std::min(static_cast<int>((2.f - game.phaseTime) * 6.f), static_cast<int>(frameCount) - 1);
        auto sprite = actorSprite(pacman, {{frame * pacFrameSize, 0}, {pacFrameSize, pacFrameSize}});
        switch (game.facing) {
        case Direction::Left: sprite.setRotation(sf::degrees(180)); break;
        case Direction::Up: sprite.setRotation(sf::degrees(270)); break;
        case Direction::Down: sprite.setRotation(sf::degrees(90)); break;
        default: break;
        }
        if (game.phase == Phase::Dying)
            sprite.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(255.f * std::min(1.f, game.phaseTime))));
        if (game.phase != Phase::GameOver)
            drawActor(target, sprite, game.playerPosition(), game.maze.width() * tileSize, game.maze.isTunnel(game.maze.player.y));
    }
    void drawGhost(sf::RenderTarget& target, const Game& game, int id) const {
        const Ghost& g = game.ghosts[id];
        const Position p = game.ghostPosition(g);
        const float width = game.maze.width() * tileSize;
        const bool tunnel = game.maze.isTunnel(g.cell.y);
        if (g.mode == GhostMode::Eaten) {
            // Eyes have the same world-space footprint and clipping as the body.
            const Cell look = offset(g.direction);
            projections(p, actorSize / 2.f, width, tunnel, [&](sf::Vector2f center) {
                for (float x : {-6.f, 6.f}) {
                    sf::CircleShape eye(5.f); eye.setOrigin({5.f,5.f});
                    eye.setPosition(center + sf::Vector2f{x, -3.f}); target.draw(eye);
                    sf::CircleShape pupil(2.5f); pupil.setOrigin({2.5f,2.5f});
                    pupil.setFillColor(sf::Color(40, 80, 255));
                    pupil.setPosition(center + sf::Vector2f{x + look.x * 2.f, -3.f + look.y * 2.f}); target.draw(pupil);
                }
            });
            return;
        }
        int directionFrame = 0;
        switch (g.direction) {
        case Direction::Down: directionFrame = 2; break;
        case Direction::Up: directionFrame = 4; break;
        case Direction::Left: directionFrame = 6; break;
        default: break;
        }
        const int frame = static_cast<int>(game.animationTime / .15f) % 2;
        const bool scared = g.mode == GhostMode::Frightened;
        auto sprite = actorSprite(ghost, {{(scared ? frame : directionFrame + frame) * ghostFrameSize,
            (scared ? 5 : id + 1) * ghostFrameSize}, {ghostFrameSize, ghostFrameSize}});
        if (scared && game.frightenedTime < 2.f && static_cast<int>(game.animationTime * 8.f) % 2 == 0)
            sprite.setColor(sf::Color(180, 180, 255));
        drawActor(target, sprite, p, width, tunnel);
    }
};
} // namespace game
