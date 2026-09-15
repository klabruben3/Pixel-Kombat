#include "../src/maze.hpp"
#include <cassert>
#include <iostream>

using namespace game;

int main() {
    for (char c : std::string("79T1-3|<>^v[]_+")) assert(isWall(c));
    for (char c : std::string(" *@&RCGP")) assert(!isWall(c));

    Maze idle({"-----", "|@**|", "-----"});
    idle.update(1.f);
    assert(idle.player.x == 1 && idle.progress == 0.f);
    idle.request(Direction::Right);
    idle.update(10.f);
    assert(idle.player.x == 3 && idle.progress == 0.f);
    assert(idle.score == 20 && idle.remaining == 0);
    idle.update(1.f);
    assert(idle.score == 20);
    idle.request(Direction::Left);
    idle.update(10.f);
    assert(idle.player.x == 1); // Collected cells remain traversable.

    Maze turn({"-----", "|@ *|", "---*|", "-----"});
    turn.request(Direction::Right);
    turn.update(0.1f);
    assert(turn.progress == 16.f);
    turn.request(Direction::Down);
    turn.update(0.3f);
    assert(turn.player.x == 3 && turn.player.y == 1);
    turn.update(0.2f);
    assert(turn.player.x == 3 && turn.player.y == 2);

    Maze reverse({"-----", "|@ *|", "-----"});
    reverse.request(Direction::Right);
    reverse.update(0.05f);
    reverse.request(Direction::Left);
    assert(reverse.player.x == 2 && reverse.progress == 24.f);
    reverse.update(0.05f);
    assert(reverse.player.x == 1 && reverse.progress == 0.f);

    Maze tunnel({"-----", "@  & ", "-----"});
    tunnel.request(Direction::Left);
    tunnel.update(0.2f);
    assert(tunnel.player.x == 4);
    tunnel.request(Direction::Right);
    tunnel.update(0.2f);
    assert(tunnel.player.x == 0);
    tunnel.request(Direction::Left);
    tunnel.update(0.4f);
    assert(tunnel.player.x == 3 && tunnel.score == 50);

    Maze closed({"-----", "@   |", "-----"});
    closed.request(Direction::Left);
    closed.update(1.f);
    assert(closed.player.x == 0 && closed.progress == 0.f);
    closed.request(Direction::Up);
    closed.update(1.f);
    assert(closed.player.y == 1);

    Maze oneStep({"------", "|@ **|", "------"});
    Maze manySteps = oneStep;
    oneStep.request(Direction::Right);
    manySteps.request(Direction::Right);
    oneStep.update(0.5f);
    for (int i = 0; i < 50; ++i) manySteps.update(0.01f);
    assert(oneStep.player.x == manySteps.player.x);
    assert(std::abs(oneStep.progress - manySteps.progress) < 0.001f);
    assert(oneStep.score == manySteps.score);

    bool rejected = false;
    try { Maze invalid({"@ ", " "}); } catch (const std::runtime_error&) { rejected = true; }
    assert(rejected);
    std::cout << "Maze collision and movement tests passed\n";
}
