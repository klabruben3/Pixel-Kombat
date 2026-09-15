#pragma once
#include "game.hpp"
#include <SFML/Audio.hpp>
#include <filesystem>
#include <iostream>
#include <memory>

namespace game {
class Audio {
public:
    enum Cue { Blue, Eat, Intro, MonsterEat, Death, Siren, ExtraLife, Count };
    explicit Audio(const std::filesystem::path& assets) {
        const std::array<const char*, Count> names{"blue", "eat", "intro", "monsterEat", "pacmanDeath", "siren", "winnLife"};
        for (int i = 0; i < Count; ++i) {
            if (!buffers[i].loadFromFile(assets / "sound" / (std::string(names[i]) + ".wav"))) {
                std::cerr << "Audio unavailable: " << names[i] << '\n';
                continue;
            }
            sounds[i] = std::make_unique<sf::Sound>(buffers[i]);
            sounds[i]->setVolume(i == Siren || i == Blue ? 18.f : 45.f);
            sounds[i]->setLooping(i == Siren || i == Blue);
        }
    }
    void toggleMute() {
        muted = !muted;
        for (int i = 0; i < Count; ++i) if (sounds[i])
            sounds[i]->setVolume(muted ? 0.f : (i == Siren || i == Blue ? 18.f : 45.f));
    }
    bool isMuted() const { return muted; }
    void setPaused(bool pause) {
        if (paused == pause) return;
        paused = pause;
        for (int i = 0; i < Count; ++i) if (sounds[i]) {
            if (pause) {
                resume[i] = sounds[i]->getStatus() == sf::SoundSource::Status::Playing;
                if (resume[i]) sounds[i]->pause();
            } else if (resume[i]) { sounds[i]->play(); resume[i] = false; }
        }
    }
    void sync(Game& game) {
        for (Event event : game.events) {
            switch (event) {
            case Event::Start: stopAll(); play(Intro); break;
            case Event::Pellet: play(Eat, false); break;
            case Event::Power: play(Eat, false); break;
            case Event::GhostEaten: play(MonsterEat); break;
            case Event::Death: stopAll(); play(Death); break;
            case Event::LevelClear: stopAll(); break;
            case Event::ExtraLife: play(ExtraLife); break;
            }
        }
        game.events.clear();
        const int desired = game.phase == Phase::Playing ? (game.frightenedTime > 0.f ? Blue : Siren) : -1;
        if (desired != background) {
            if (background >= 0) stop(background);
            background = desired;
            if (background >= 0) { stop(Intro); play(background); }
        }
    }
    const std::array<unsigned, Count>& starts() const { return playCounts; }
private:
    // Declaration order guarantees that sounds are destroyed before buffers.
    std::array<sf::SoundBuffer, Count> buffers;
    std::array<std::unique_ptr<sf::Sound>, Count> sounds;
    std::array<bool, Count> resume{};
    std::array<unsigned, Count> playCounts{};
    int background = -1;
    bool muted = false, paused = false;
    void play(int cue, bool interrupt = true) {
        if (!sounds[cue]) return;
        if (!interrupt && (resume[cue] || sounds[cue]->getStatus() == sf::SoundSource::Status::Playing)) return;
        sounds[cue]->play(); ++playCounts[cue];
        if (paused) { sounds[cue]->pause(); resume[cue] = true; }
    }
    void stop(int cue) {
        if (sounds[cue]) sounds[cue]->stop();
        resume[cue] = false;
    }
    void stopAll() {
        for (int i = 0; i < Count; ++i) stop(i);
        background = -1;
    }
};
} // namespace game
