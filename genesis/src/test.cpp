#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

std::vector<std::string> stage = {
"#######################",
"#***#*************#***#",
"#*#*#*#####*#####*#*#*#",
"#*******#*****#*******#",
"###*#*#*#*###*#*#*#*###",
"#***#*#*#*#_#*#*#*#***#",
"#*#*#*#*#*###*#*#*#*#*#",
"#***#*#*********#*#***#",
"#####*#*#######*#*#####",
"#*****#*********#*****#",
"#_#####*###_###*#####_#",
"********#RC_GP#********",
"#_#####*#######*#####_#",
"#*****#*********#*****#",
"#####*#*#######*#*#####",
"#***#*#*********#*#***#",
"#*#*#*#*#*###*#*#*#*#*#",
"#***#*#*#*#_#*#*#*#***#",
"###*#*#*#*###*#*#*#*###",
"#*******#**@**#*******#",
"#*#*#*#####*#####*#*#*#",
"#***#*************#***#",
"#######################",

};

const int TILESIZE = 20;
const float speed = 50;

// Gives properties to the circle
Vector2f atPos(Sprite pac){    
    Vector2f atpos;

    for(int y = 0; y < stage.size(); y++){
        for(int x = 0; x < stage[y].size(); x++){
            // create a position for the restart position

            if(stage[y][x] == '@'){
                atpos = Vector2f(x * TILESIZE + pac.getGlobalBounds().getCenter().x, y * TILESIZE - pac.getGlobalBounds().getCenter().y);
                break;
            }
        }
    }

    return atpos;
}

// Pacman movements
void movePac(int &keycode, Sprite &pac){
    if(keycode == 71){
        pac.move({-speed, 0});
        pac.setRotation(degrees(180));
    } else if(keycode == 72){
        pac.move({speed, 0});
         pac.setRotation(degrees(0));
    } else if(keycode == 73){
        pac.move({0, -speed});
        pac.setRotation(degrees(270));
    } else if(keycode == 74){
        pac.move({0, speed});
        pac.setRotation(degrees(90));
    }
}

void reset(int &keycode, Sprite &pac){
    if(keycode == 71) pac.move({speed, 0});
    else if(keycode == 72) pac.move({-speed, 0});
    else if(keycode == 73) pac.move({0, speed});
    else if(keycode == 74) pac.move({0, -speed});
}

struct block{
    RectangleShape brick;
    char id;

    block(int x, int y, char c){
        brick.setSize({(float)TILESIZE, (float)TILESIZE});
        brick.setPosition({(float)(x * TILESIZE), (float)(y * TILESIZE)});
                
        if(c == '#') brick.setFillColor(Color::Blue);
        if(c == '*') brick.setFillColor(Color::Black);
        if(c == '@') brick.setFillColor(Color::Black);
        if(c == '_') brick.setFillColor(Color::Black);
        if(c == 'R') brick.setFillColor(Color::Black);
        if(c == 'C') brick.setFillColor(Color::Black);
        if(c == 'G') brick.setFillColor(Color::Black);
        if(c == 'P') brick.setFillColor(Color::Black);
        id = c;
    }
};

// Making a replica of the stage array into walls with id and custom properties
std::vector<std::vector<block>> wall;


void buildWall(){
    for (int y = 0; y < stage.size(); y++) {
        std::vector<block> bRows;
        for (int x = 0; x < stage[y].size(); x++) {
            char tile = stage[y][x];
            block b(x, y, tile);
            
            bRows.push_back(b);
        }
        wall.push_back(bRows);
    }
}//

int main(){
    RenderWindow window(VideoMode({800u, 600u}), "Controller Test");
    window.setFramerateLimit(10);

    buildWall();

    const int PAC_FRAMES = 3;
    const int FRAME_SIZE = 30;
    int pacCurrentFrame = 0;

    Texture atlas;
    if (!atlas.loadFromFile("C:\\Users\\klabr\\Desktop\\Projects\\2d project\\Pixel Kombat\\genesis\\assets\\PMSprites.png")) return -1;
    atlas.setSmooth(false);
    Sprite sprites(atlas);
    Sprite pac = sprites;
    pac.setOrigin({FRAME_SIZE/2, FRAME_SIZE/2});
    pac.setPosition(atPos(pac));
    
    
    
    // Captures the code of the keyboard key being pressed and initializes it to keycode
    int keycode;

    while (window.isOpen()) {
        // Listens for events
        while (auto event = window.pollEvent()){
            if (event->is<Event::Closed>()) window.close();

            // Triggers when a keyboard button is pressed
            if (auto pressed = event->getIf<Event::KeyPressed>()){
                int code = static_cast<int>(pressed->code);

                // Insures keycode is altered only when directional keys are pressed
                if(code == 71 || code == 72 || code == 73 || code == 74) keycode = code;
            }
        }
        
        // Iterates throught the pacman frames in atlas
        pac.setTextureRect(IntRect({pacCurrentFrame, 0}, {FRAME_SIZE, FRAME_SIZE}));
        pacCurrentFrame = (!PAC_FRAMES == pacCurrentFrame) ? pacCurrentFrame + FRAME_SIZE : 0;

        movePac(keycode, pac);
        
        // Puts the objects on screen
        window.clear(Color::Black);
        // for(auto &row : wall){
        //     for(auto col : row){
        //         Vector2f brickCenter = col.brick.getGlobalBounds().getCenter();
        //         Vector2f circleCenter = circle.getGlobalBounds().getCenter();
                
        //         if(col.id == '*' && brickCenter.x == circleCenter.x || col.id == '*' && brickCenter.y == circleCenter.y){
        //             optional<FloatRect> intercept = col.brick.getGlobalBounds().findIntersection(circle.getGlobalBounds());
        //             if(intercept.has_value()){
        //                 col.brick.setFillColor(Color::Green);
        //             }
        //         }

        //         if(col.id == '#' && abs(brickCenter.y - circleCenter.y) == TILESIZE){
        //             keycode = 0;
        //             reset(keycode);
        //         }
        //         window.draw(col.brick);
        //     }
        // }

        window.draw(pac);
        window.display();
    }

    return 0;
}
