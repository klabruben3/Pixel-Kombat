#include "stage.hpp"
#include "render.hpp"
#include "tiles.hpp"
#include "audio.hpp"
#include <iostream>

using namespace sf;
constexpr int FRAME_SIZE = game::pacFrameSize;
std::filesystem::path findAssets(const char* executable) {
    const auto executableDir = std::filesystem::absolute(executable).parent_path();
    for (const auto& root : {executableDir / "assets", executableDir.parent_path() / "assets",
                             std::filesystem::current_path() / "assets",
                             std::filesystem::current_path().parent_path() / "assets"}) {
        if (std::filesystem::exists(root / "pacman.png") &&
            std::filesystem::exists(root / "stage_atlas.png") &&
            std::filesystem::exists(root / "food.png")) return root;
    }
    throw std::runtime_error("Cannot locate assets next to the game or in its project folder");
}

const char* phaseLabel(game::Phase phase) {
    switch (phase) {
    case game::Phase::Ready: return "Ready";
    case game::Phase::Dying: return "Life lost";
    case game::Phase::LevelClear: return "Maze cleared!";
    case game::Phase::GameOver: return "Game over - R to restart";
    default: return "";
    }
}

int main(int argc, char** argv) {
    try {
        const auto assets = findAssets(argv[0]);
        Texture stageAtlas, foodAtlas;
        if (!stageAtlas.loadFromFile(assets / "stage_atlas.png") ||
            !foodAtlas.loadFromFile(assets / "food.png"))
            throw std::runtime_error("Failed to load maze textures");
        game::Game game(stage);
        game::Actors actors(assets);
        game::Audio audio(assets);
        const Vector2u worldSize{static_cast<unsigned>(game.maze.width() * FRAME_SIZE),
                                static_cast<unsigned>(game.maze.height() * FRAME_SIZE)};
        RenderTexture world(worldSize);
        world.setSmooth(false);
        const Vector2u windowSize{worldSize.x + FRAME_SIZE, worldSize.y + FRAME_SIZE};
        RenderWindow window(VideoMode(windowSize), "Pixel Kombat: Genesis");
        window.setFramerateLimit(60);
        window.setKeyRepeatEnabled(false);
        Clock clock;
        std::string shownTitle;
        bool focused = true;
        const bool smoke = argc > 1 && std::string(argv[1]) == "--smoke-test";
        unsigned frames = 0;
        if (smoke) audio.toggleMute();

        while (window.isOpen()) {
            while (auto event = window.pollEvent()) {
                if (event->is<Event::Closed>()) window.close();
                if (event->is<Event::FocusLost>()) focused = false;
                if (event->is<Event::FocusGained>()) focused = true;
                if (const auto* pressed = event->getIf<Event::KeyPressed>()) {
                    using Key = Keyboard::Key;
                    switch (pressed->code) {
                    case Key::Left: game.request(game::Direction::Left); break;
                    case Key::Right: game.request(game::Direction::Right); break;
                    case Key::Up: game.request(game::Direction::Up); break;
                    case Key::Down: game.request(game::Direction::Down); break;
                    case Key::Escape: window.close(); break;
                    case Key::R: game.restart(); break;
                    case Key::M: audio.toggleMute(); break;
                    default: break;
                    }
                }
            }
            if (!window.isOpen()) break;
            const float dt = std::min(clock.restart().asSeconds(), .1f);
            audio.setPaused(!focused);
            if (focused) game.update(dt);
            audio.sync(game);
            const std::string title = "Pixel Kombat: Genesis | Score: " + std::to_string(game.maze.score) +
                " | Lives: " + std::to_string(game.lives) + " | Level: " + std::to_string(game.level) +
                " | Pellets: " + std::to_string(game.maze.remaining) + " | " + phaseLabel(game.phase) +
                (!focused ? " Paused |" : "") + (audio.isMuted() ? " M: unmute | " : " M: mute | ") + "R: restart";
            if (title != shownTitle) { shownTitle = title; window.setTitle(title); }

            world.clear(Color::Black);
            for (int y = 0; y < game.maze.height(); ++y) {
                for (int x = 0; x < game.maze.width(); ++x) {
                    Sprite tile = game::tileSprite(stageAtlas, foodAtlas, game.maze.cells[y][x]);
                    tile.setPosition({(x + .5f) * game::tileSize, (y + .5f) * game::tileSize});
                    world.draw(tile);
                }
            }
            for (int i = 0; i < 4; ++i) actors.drawGhost(world, game, i);
            actors.drawPacman(world, game);
            world.display();
            window.clear(Color::Black);
            if (window.getSize().x > 0 && window.getSize().y > 0) game::present(window, world.getTexture());
            window.display();
            if (smoke && ++frames >= 5) window.close();
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Pixel Kombat: " << error.what() << '\n';
        return 1;
    }
}
