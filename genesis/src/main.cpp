#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace sf;
using namespace std;

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

const int TILESIZE = 20;
const int FRAME_SIZE = 32;
const float speed = 16;
const int PAC_FRAMES = FRAME_SIZE / 2;
const float tolerance = 1.f;
int pacCurrentFrame = 0;

// Defines pacman
Sprite pacman(Texture &atlas)
{
    Vector2f atpos;

    for (int y = 0; y < stage.size(); y++)
    {
        for (int x = 0; x < stage[y].size(); x++)
        {
            // create a position for the restart position

            if (stage[y][x] == '@')
            {
                atpos = Vector2f(FRAME_SIZE * (x + 1), FRAME_SIZE * (y + 1));
                break;
            }
        }
    }

    Sprite pac(atlas);
    pac.setOrigin({FRAME_SIZE / 2, FRAME_SIZE / 2});
    pac.setPosition(atpos);

    return pac;
}

bool runL = true;
bool runR = true;
bool runU = true;
bool runD = true;

// Pacman movements
void movePac(int &keycode, Sprite &pac)
{
    if (keycode == 73 && runU == true)
    {
        pac.move({0, -speed});
        pac.setRotation(degrees(270));
    }
    if (keycode == 74 && runD == true)
    {
        pac.move({0, speed});
        pac.setRotation(degrees(90));
    }

    if (keycode == 71 && runL == true)
    {
        pac.move({-speed, 0});
        pac.setRotation(degrees(180));
    }
    if (keycode == 72 && runR == true)
    {
        pac.move({speed, 0});
        pac.setRotation(degrees(0));
    }
}

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

struct brick
{
    Sprite block;
    char id;
    bool is_wall;
    bool is_food;

    brick(int x, int y, char c, Texture &stage_atlas, Texture &food_atlas) : block(stages(stage_atlas, food_atlas, c))
    {
        is_wall = c == '|' || c == '-' || c == '<' || c == '>' || c == '^' || c == 'v';
        is_food = c == '*' || c == '&';

        block.setPosition({(float)(FRAME_SIZE * (x + 1)), (float)(FRAME_SIZE * (y + 1))});
        id = c;
    }
};

// Making a replica of the stage array into walls with id and custom properties
std::vector<std::vector<brick>> wall;

void buildWall(Texture &stage_atlas, Texture &food_atlas)
{
    for (int y = 0; y < stage.size(); y++)
    {
        std::vector<brick> bRows;
        for (int x = 0; x < stage[y].size(); x++)
        {
            char tile = stage[y][x];
            brick b(x, y, tile, stage_atlas, food_atlas);

            bRows.push_back(b);
        }
        wall.push_back(bRows);
    }
} //

int main()
{
    RenderWindow window(VideoMode({24 * FRAME_SIZE, 24 * FRAME_SIZE}), "Pixel Combat");
    window.setFramerateLimit(10);

    // defines the atlas for the characters
    Texture character_atlas;
    if (!character_atlas.loadFromFile("C:\\Users\\klabr\\Desktop\\Projects\\2d project\\Pixel Kombat\\genesis\\assets\\pacman.png"))
        return -1;
    character_atlas.setSmooth(false);

    // defines the atlas for the stage;
    Texture stage_atlas;
    if (!stage_atlas.loadFromFile("C:\\Users\\klabr\\Desktop\\Projects\\2d project\\Pixel Kombat\\genesis\\assets\\stage_atlas.png"))
        return -1;
    stage_atlas.setSmooth(false);

    // defines the atlas for the food pallette
    Texture food_atlas;
    if (!food_atlas.loadFromFile("C:\\Users\\klabr\\Desktop\\Projects\\2d project\\Pixel Kombat\\genesis\\assets\\food.png"))
        return -1;
    food_atlas.setSmooth(false);

    // Builds everything
    buildWall(stage_atlas, food_atlas);
    Sprite pac = pacman(character_atlas);
    //

    // Captures the code of the keyboard key being pressed and initializes it to keycode
    int keycode;

    while (window.isOpen())
    {
        // Listens for events
        while (auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();

            // Triggers when a keyboard button is pressed
            if (auto pressed = event->getIf<Event::KeyPressed>())
            {
                int code = static_cast<int>(pressed->code);

                // Insures keycode is altered only when directional keys are pressed
                if (code == 71 || code == 72 || code == 73 || code == 74)
                    keycode = code;
                // cout << runU << " " << runD << " " << runL << " " << runR << endl;
            }
        }

        movePac(keycode, pac);

        // Iterates throught the pacman frames in atlas
        pac.setTextureRect(IntRect({pacCurrentFrame, 0}, {FRAME_SIZE, FRAME_SIZE}));
        pacCurrentFrame = (!PAC_FRAMES == pacCurrentFrame) ? pacCurrentFrame + FRAME_SIZE : 0;

        Vector2f pacCenter = pac.getGlobalBounds().getCenter();
        int pacAngle = pac.getRotation().asDegrees();

        if (pacCenter.x < -16 && pacAngle == 180)
        {
            pac.setPosition({768, pacCenter.y});
        }
        else if (pacCenter.x >= 784 && pacAngle == 0)
        {
            pac.setPosition({-16, pacCenter.y});
        }

        // Puts the objects on screen
        window.clear(Color::Black);
        for (vector<brick> &row : wall)
        {
            for (brick &col : row)
            {
                Vector2f blockCenter = col.block.getGlobalBounds().getCenter();
                float xDifference = pacCenter.x - blockCenter.x;
                float yDifference = pacCenter.y - blockCenter.y;

                // Wall collision
                if (col.is_wall && abs(yDifference) < tolerance)
                {
                    if (abs(xDifference - FRAME_SIZE) < tolerance)
                    {
                        runL = false;
                    }

                    if (abs(xDifference + FRAME_SIZE) < tolerance)
                    {
                        runR = false;
                    }
                }

                if (col.is_wall && abs(xDifference) < tolerance)
                {
                    if (abs(yDifference - FRAME_SIZE) < tolerance)
                    {
                        runU = false;
                    }

                    if (abs(yDifference + FRAME_SIZE) < tolerance)
                    {
                        runD = false;
                    }
                } //

                // Runs for every food tile
                if (col.is_food)
                {
                    // Handles the food collection
                    if (abs(xDifference) < tolerance && abs(yDifference) < tolerance)
                    {
                        col.block.setColor(Color::Transparent);
                    }

                    // Handles the X-axis collision
                    if (abs(yDifference) < tolerance)
                    { // meaning if pac is at the same row as the block
                        if (xDifference > 0)
                        { // 👈 AKA pac is on the right of the block
                            if (xDifference <= FRAME_SIZE)
                            {
                                runL = true;
                            }
                            if (xDifference < FRAME_SIZE)
                            {
                                runU = false;
                                runD = false;
                            }
                        }

                        if (xDifference < 0)
                        { // 👈 AKA pac is on the left of the block
                            if (xDifference >= -FRAME_SIZE)
                            {
                                runR = true;
                            }
                            if (xDifference > -FRAME_SIZE)
                            {
                                runU = false;
                                runD = false;
                            }
                        }
                    }

                    // Handles the Y-axis collision
                    if (abs(xDifference) < tolerance)
                    { // Meaning if pac is at the same column as the block
                        if (yDifference > 0)
                        { // 👈aka when pacman is below the food tile center
                            if (yDifference <= FRAME_SIZE)
                            {
                                runU = true;
                            }
                            if (yDifference < FRAME_SIZE)
                            {
                                runL = false;
                                runR = false;
                            }
                        }

                        if (yDifference < 0)
                        { // 👈 when pacman is pn top of the food tile center
                            if (yDifference >= -FRAME_SIZE)
                            {
                                runD = true;
                            }
                            if (yDifference > -FRAME_SIZE)
                            {
                                runL = false;
                                runR = false;
                            }
                        }
                    }
                }

                window.draw(col.block);
            }
        }

        window.draw(pac);
        window.display();
    }

    return 0;
}
