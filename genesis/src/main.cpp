#include <SFML/Graphics.hpp>
#include "maze.hpp"
#include <filesystem>
#include <iostream>

using namespace sf;
constexpr int FRAME_SIZE = 32;
std::vector<std::string> stage = {
    "7---T-------------T---9",
    "|***|*************|***|",
    "|*+*v*<-T->*<-T->*v*+*|",
    "|**&****|*****|*******|",
    "[->*^*^*|*7-9*|*^*^*<-]",
    "|***|*|*|*| |*|*|*|***|",
    "|*+*|*|*v*1-3*v*|*|*+*|",
    "|***|*|*********|*|***|",
    "[---3*|*<----->*|*1---]",
    "|*****|*********|*****|",
    "v*<---3*7-> <-9*1--->*v",
    "********|RC GP|****&***",
    "^*<---9*1-----3*7--->*^",
    "|&****|*********|*****|",
    "[---9*|*<----->*|*7---]",
    "|***|*|*********|*|***|",
    "|*+*|*|*^*7-9*^*|*|*+*|",
    "|***|*|*|*| |*|*|*|***|",
    "[->*v*v*|*1-3*|*v*v*<-]",
    "|*******|**@**|***&***|",
    "|*+*^*<-_->*<-_->*^*+*|",
    "|***|*************|***|",
    "1---_-------------_---3",

};

Sprite stages(Texture &stage_atlas, Texture &food_atlas, char id)
{
    Sprite stage(stage_atlas);
    Sprite food(food_atlas);

    if (id == '7' || id == '9' || id == '3' || id == '1')
    {
        stage.setTextureRect(IntRect({0, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if (id == '7')
            stage.setRotation(degrees(0));
        else if (id == '9')
            stage.setRotation(degrees(90));
        else if (id == '3')
            stage.setRotation(degrees(180));
        else if (id == '1')
            stage.setRotation(degrees(270));
    }
    else if (id == '-' || id == '|')
    {
        stage.setTextureRect(IntRect({32, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if (id == '-')
            stage.setRotation(degrees(0));
        else if (id == '|')
            stage.setRotation(degrees(90));
    }
    else if (id == 'T' || id == ']' || id == '_' || id == '[')
    {
        stage.setTextureRect(IntRect({64, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if (id == 'T')
            stage.setRotation(degrees(0));
        else if (id == ']')
            stage.setRotation(degrees(90));
        else if (id == '_')
            stage.setRotation(degrees(180));
        else if (id == '[')
            stage.setRotation(degrees(270));
    }
    else if (id == '>' || id == 'v' || id == '<' || id == '^')
    {
        stage.setTextureRect(IntRect({96, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if (id == '>')
            stage.setRotation(degrees(0));
        else if (id == 'v')
            stage.setRotation(degrees(90));
        else if (id == '<')
            stage.setRotation(degrees(180));
        else if (id == '^')
            stage.setRotation(degrees(270));
    }
    else if (id == '+')
    {
        stage.setTextureRect(IntRect({128, 0}, {FRAME_SIZE, FRAME_SIZE}));
    }
    else if (id == '*')
    {
        food.setTextureRect(IntRect({0, 0}, {FRAME_SIZE, FRAME_SIZE}));
        stage = food;
    }
    else if (id == '&')
    {
        food.setTextureRect(IntRect({32, 0}, {FRAME_SIZE, FRAME_SIZE}));
        stage = food;
    }
    else
    {
        stage.setTextureRect(IntRect({160, 0}, {FRAME_SIZE, FRAME_SIZE}));
    }

    stage.setOrigin(stage.getLocalBounds().getCenter());

    return stage;
}

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

int main(int, char** argv) {
    try {
        const auto assets = findAssets(argv[0]);
        Texture characterAtlas, stageAtlas, foodAtlas;
        if (!characterAtlas.loadFromFile(assets / "pacman.png") ||
            !stageAtlas.loadFromFile(assets / "stage_atlas.png") ||
            !foodAtlas.loadFromFile(assets / "food.png")) {
            throw std::runtime_error("Failed to load game textures");
        }
        const unsigned frameCount = characterAtlas.getSize().x / FRAME_SIZE;
        if (frameCount == 0 || characterAtlas.getSize().y < FRAME_SIZE)
            throw std::runtime_error("Invalid player sprite sheet");

        game::Maze maze(stage);
        const Vector2u windowSize{static_cast<unsigned>((maze.width() + 1) * FRAME_SIZE),
                                 static_cast<unsigned>((maze.height() + 1) * FRAME_SIZE)};
        RenderWindow window(VideoMode(windowSize), "Pixel Kombat: Genesis");
        window.setFramerateLimit(60);
        window.setKeyRepeatEnabled(false);
        Sprite pac(characterAtlas, IntRect({0, 0}, {FRAME_SIZE, FRAME_SIZE}));
        pac.setOrigin({16.f, 16.f});
        Clock clock;
        float animationTime = 0.f;
        int shownScore = -1;
        bool focused = true;

        while (window.isOpen()) {
            while (auto event = window.pollEvent()) {
                if (event->is<Event::Closed>()) window.close();
                if (event->is<Event::FocusLost>()) focused = false;
                if (event->is<Event::FocusGained>()) focused = true;
                if (event->is<Event::Resized>()) {
                    // Preserve the logical map coordinates when resizing the window.
                    window.setView(View(FloatRect({0.f, 0.f},
                        {static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)})));
                }
                if (const auto* pressed = event->getIf<Event::KeyPressed>()) {
                    using Key = Keyboard::Key;
                    switch (pressed->code) {
                    case Key::Left: maze.request(game::Direction::Left); break;
                    case Key::Right: maze.request(game::Direction::Right); break;
                    case Key::Up: maze.request(game::Direction::Up); break;
                    case Key::Down: maze.request(game::Direction::Down); break;
                    case Key::Escape: window.close(); break;
                    case Key::R:
                        maze = game::Maze(stage);
                        animationTime = 0.f;
                        pac.setRotation(degrees(0));
                        shownScore = -1;
                        break;
                    default: break;
                    }
                }
            }
            if (!window.isOpen()) break;
            // Cap long stalls so restoring the window cannot leap through the maze.
            const float dt = std::min(clock.restart().asSeconds(), 0.1f);
            if (focused && maze.remaining > 0) {
                maze.update(dt);
                if (maze.direction != game::Direction::None) animationTime += dt;
            }
            if (shownScore != maze.score) {
                shownScore = maze.score;
                window.setTitle("Pixel Kombat: Genesis | Score: " + std::to_string(maze.score) +
                    " | Pellets: " + std::to_string(maze.remaining) +
                    (maze.remaining == 0 ? " | Maze cleared! R to restart" : " | R to restart"));
            }
            const int frame = static_cast<int>(animationTime / 0.1f) % frameCount;
            pac.setTextureRect(IntRect({frame * FRAME_SIZE, 0}, {FRAME_SIZE, FRAME_SIZE}));
            switch (maze.direction) {
            case game::Direction::Left: pac.setRotation(degrees(180)); break;
            case game::Direction::Right: pac.setRotation(degrees(0)); break;
            case game::Direction::Up: pac.setRotation(degrees(270)); break;
            case game::Direction::Down: pac.setRotation(degrees(90)); break;
            default: break;
            }
            const game::Cell delta = game::offset(maze.direction);
            Vector2f position{(maze.player.x + 1) * game::tileSize + delta.x * maze.progress,
                              (maze.player.y + 1) * game::tileSize + delta.y * maze.progress};
            window.clear(Color::Black);
            for (int y = 0; y < maze.height(); ++y) {
                for (int x = 0; x < maze.width(); ++x) {
                    Sprite tile = stages(stageAtlas, foodAtlas, maze.cells[y][x]);
                    tile.setPosition({(x + 1) * game::tileSize, (y + 1) * game::tileSize});
                    window.draw(tile);
                }
            }
            pac.setPosition(position);
            window.draw(pac);
            // Draw the other end of a tunnel crossing for continuous wrapping.
            if ((maze.player.x == 0 && delta.x < 0) ||
                (maze.player.x == maze.width() - 1 && delta.x > 0)) {
                position.x -= delta.x * maze.width() * game::tileSize;
                pac.setPosition(position);
                window.draw(pac);
            }
            window.display();
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Pixel Kombat: " << error.what() << '\n';
        return 1;
    }
}
