#pragma once
#include <SFML/Graphics.hpp>
namespace game {
inline sf::Sprite tileSprite(sf::Texture &stage_atlas, sf::Texture &food_atlas, char id)
{
    using namespace sf;
    constexpr int FRAME_SIZE = 32;
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


}

