#include "../src/audio.hpp"
#include "../src/stage.hpp"
#include <cassert>
#include <iostream>
using namespace game;
int main() {
    Game game(stage);
    Audio audio("assets"); audio.toggleMute();
    audio.sync(game); assert(audio.starts()[Audio::Intro] == 1);
    audio.sync(game); assert(audio.starts()[Audio::Intro] == 1);
    game.phase = Phase::Playing;
    audio.sync(game); assert(audio.starts()[Audio::Siren] == 1);
    for (int i=0; i<100; ++i) audio.sync(game);
    assert(audio.starts()[Audio::Siren] == 1);
    game.events = {Event::Pellet,Event::Pellet,Event::Pellet};
    audio.sync(game); assert(audio.starts()[Audio::Eat] == 1);
    game.frightenedTime = 6;
    game.events = {Event::Power}; audio.sync(game);
    assert(audio.starts()[Audio::Blue] == 1);
    game.events = {Event::Power}; audio.sync(game);
    assert(audio.starts()[Audio::Blue] == 1);
    game.events = {Event::GhostEaten}; audio.sync(game);
    assert(audio.starts()[Audio::MonsterEat] == 1);
    audio.setPaused(true); audio.setPaused(false);
    game.frightenedTime = 0; audio.sync(game);
    assert(audio.starts()[Audio::Siren] == 2);
    game.phase = Phase::Dying; game.events = {Event::Death}; audio.sync(game);
    assert(audio.starts()[Audio::Death] == 1);
    audio.sync(game); assert(audio.starts()[Audio::Death] == 1);
    game.restart(); audio.sync(game); assert(audio.starts()[Audio::Intro] == 2);
    for (auto name : {"blue","eat","intro","monsterEat","pacmanDeath","siren","winnLife"}) {
        sf::SoundBuffer buffer;
        assert(buffer.loadFromFile(std::string("assets/sound/") + name + ".wav"));
        assert(buffer.getSampleCount() > 0);
        std::cout << name << ": " << buffer.getDuration().asSeconds() << " seconds\n";
    }
    std::cout << "Audio tests passed: resource loading, cues, bounded playback, loop transitions, pause\n";
}
