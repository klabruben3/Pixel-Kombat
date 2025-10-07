#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <typeinfo>

using namespace sf;
using namespace std;

std::vector<std::string> stage = {
"#########################",
"#***********************#",
"#*###*#####*##*#####*###*#",
"#***********@***********#",
"#*###*##*########*##*###*#",
"#****##****##****##****#",
"###*##*##*##*##*##*##*###",
"#****##****##****##****#",
"#*###*##*########*##*###*#",
"#***********************#",
"#*###*##*########*##*###*#",
"#****##****##****##****#",
"###*##*##*##*##*##*##*###",
"#****##****##****##****#",
"#*###*#####*##*#####*###*#",
"#***********************#",
"#########################"
};

const int TILESIZE = 20;
const float speed = 2.f;

// Creates the circle
CircleShape circle(TILESIZE/2.f);

// Gives properties to the circle
void setCircleProperties(){
    float radius =  circle.getRadius();
    
    // sets attributes
    circle.setFillColor(Color::Red);
    circle.setOrigin({radius, radius});
    
    Vector2f atpos;

    // Makes sure
    for(int y = 0; y < stage.size(); y++){
        for(int x = 0; x < stage[y].size(); x++){
            // create a position for the restart position
            if(stage[y][x] == '@'){
                atpos = Vector2f(x * TILESIZE + radius, y * TILESIZE + radius);
                break;
            }
        }        
    }
    
    circle.setPosition(atpos);
}

// Pacman movements
void movePac(int &keycode){
    if(keycode == 71) circle.move({-speed, 0});
    else if(keycode == 72) circle.move({speed, 0});
    else if(keycode == 73) circle.move({0, -speed});
    else if(keycode == 74) circle.move({0, speed});
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
    // window.setFramerateLimit(60);

    setCircleProperties();
    buildWall();

    while (window.isOpen()) {
        // Captures the code of the keyboard key being pressed and initializes it to keycode
        int keycode;

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

        movePac(keycode);

        // Puts the objects on screen
        window.clear(Color::Black);
        for(auto &row : wall){
            for(auto col : row){
                window.draw(col.brick);
            }
        }
        window.draw(circle);
        window.display();
    }

    return 0;
}
