#include "../src/render.hpp"
#include "../src/tiles.hpp"
#include "../src/stage.hpp"
#include <cassert>
#include <iostream>

using namespace game;
void checkWrap(sf::Sprite sprite) {
    sf::RenderTexture surface({160,64});
    surface.clear(sf::Color::Black);
    drawActor(surface, sprite, {80,32}, 160, true);
    surface.display();
    const auto reference = surface.getTexture().copyToImage();
    for (int center : {-16,-12,-8,-4,0,4,8,12,16,144,148,152,156,160,164,168,172,176}) {
        surface.clear(sf::Color::Black);
        drawActor(surface, sprite, {static_cast<float>(center),32}, 160, true);
        surface.display();
        auto actual = surface.getTexture().copyToImage();
        for (unsigned y = 0; y < 64; ++y) for (int x = 0; x < 160; ++x) {
            int source = ((x - (center - 80)) % 160 + 160) % 160;
            assert(actual.getPixel({static_cast<unsigned>(x),y}) == reference.getPixel({static_cast<unsigned>(source),y}));
        }
    }
}
int main() {
    Actors actors("assets");
    for (unsigned frame = 0; frame < actors.frameCount; ++frame) {
        for (int angle : {0,90,180,270}) {
            auto pac = actorSprite(actors.pacman, {{static_cast<int>(frame)*32,0},{32,32}});
            pac.setRotation(sf::degrees(static_cast<float>(angle)));
            assert(pac.getScale().x == pac.getScale().y);
            checkWrap(pac);
        }
    }
    for (int id = 0; id < 5; ++id) for (int frame = 0; frame < (id == 4 ? 2 : 8); ++frame)
        checkWrap(actorSprite(actors.ghost, {{frame*30,(id+1)*30},{30,30}}));

    sf::Texture tiles, food;
    assert(tiles.loadFromFile("assets/stage_atlas.png"));
    assert(food.loadFromFile("assets/food.png"));
    Game game(stage); game.phase = Phase::Playing;
    sf::RenderTexture world({736,736});
    world.clear(sf::Color::Black);
    for (int y=0; y<23; ++y) for (int x=0; x<23; ++x) {
        auto tile = tileSprite(tiles, food, game.maze.cells[y][x]);
        tile.setPosition({(x+.5f)*32,(y+.5f)*32}); world.draw(tile);
    }
    for (int i=0; i<4; ++i) actors.drawGhost(world,game,i);
    actors.drawPacman(world,game);
    world.display();
    for (auto size : {sf::Vector2u{768,768}, sf::Vector2u{1024,600}, sf::Vector2u{480,900}}) {
        sf::RenderTexture display(size);
        display.clear(sf::Color::Black); present(display,world.getTexture()); display.display();
        const Fit fit = fitCanvas(size,{768,768});
        assert(std::abs(fit.offset.x * 2 + 768 * fit.scale - size.x) < .001f);
        assert(std::abs(fit.offset.y * 2 + 768 * fit.scale - size.y) < .001f);
        assert(display.getTexture().copyToImage().saveToFile("tests/visual/game-" + std::to_string(size.x) + "x" + std::to_string(size.y) + ".png"));
    }
    // Compact evidence: equal fragments across both seams at quarter-tile steps.
    sf::RenderTexture strips({160,64});
    sf::RenderTexture sheet({640,8*64});
    sheet.clear(sf::Color(30,30,30));
    for (int row=0; row<8; ++row) for (int column=0; column<4; ++column) {
        strips.clear(sf::Color::Black);
        const float x = (row < 4 ? 16.f - row*8.f : 144.f + (row-4)*8.f);
        auto sprite = column < 2 ? actorSprite(actors.pacman, {{64,0},{32,32}}) :
            actorSprite(actors.ghost, {{0,column==2 ? 30 : 150},{30,30}});
        if (column==0) sprite.setRotation(sf::degrees(180));
        drawActor(strips,sprite,{x,32},160,true); strips.display();
        sf::Sprite strip(strips.getTexture()); strip.setPosition({column*160.f,row*64.f}); sheet.draw(strip);
    }
    sheet.display(); assert(sheet.getTexture().copyToImage().saveToFile("tests/visual/tunnel-seams.png"));
    bool rejected = false;
    try { actorSprite(actors.pacman, {{0,0},{32,16}}); } catch (const std::runtime_error&) { rejected = true; }
    assert(rejected);
    std::cout << "Rendering tests passed: all actor frames, orientations, clipped seam pixels, wide/tall layouts\n";
}
