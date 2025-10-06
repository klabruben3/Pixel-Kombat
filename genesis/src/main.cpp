#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath> // for std::abs

using namespace sf;

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

int main() {
    // Create a window 800x600 using Vector2u
    RenderWindow window(VideoMode({800u, 600u}), "Controller Test");
    window.setFramerateLimit(60);

    const int TILESIZE = 20; // size of each tile in pixels
    const float speed = 5.f; // movement of characters

    // Circle with radius 10
    CircleShape circle(TILESIZE/2.f);
    circle.setFillColor(Color::Red);
    
    float radius =  circle.getRadius();
    
    // Set origin to center
    circle.setOrigin({radius, radius});
    circle.scale({.9, .9});

    Vector2f atpos;

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

    std::vector<std::vector<block>> wall;

    for (int y = 0; y < stage.size(); y++) {
        std::vector<block> bRows;
        for (int x = 0; x < stage[y].size(); x++) {
            char tile = stage[y][x];
            block b(x, y, tile);

            bRows.push_back(b);
        }
        wall.push_back(bRows);
    }

    bool run = true;

    int xPastD = -1;
    int yPastD = 0;
    
    while (window.isOpen()) {
        // Event handling (visitor style)
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
            window.close();
        }

        if(run){
            if(yPastD == 0){
                if(xPastD == -1) circle.move({-speed, 0});
                else if(xPastD == 1) circle.move({speed, 0});
            }else{
                if(yPastD -1) circle.move({0, -speed});
                else if(yPastD == 1) circle.move({0, speed});
            }
        }

        if (Joystick::isConnected(0)) {
            float x = Joystick::getAxisPosition(0, Joystick::Axis::X);
            float y = Joystick::getAxisPosition(0, Joystick::Axis::Y);

            if(std::abs(x) > 15){
                xPastD = x/std::abs(x);
                yPastD = 0;
                run = true;
            } else if(std::abs(y) > 15) {
                yPastD = y/std::abs(y);
                xPastD = 0;
            }
        } else {
            static bool printed = false;
            if (!printed) {
                std::cout << "No controller detected.\n";
                printed = true;
            }
        }

        window.clear();
        for(auto &row : wall){
            for (auto &col : row) {
                window.draw(col.brick);

                if(col.id == '#' && run == true){
                    std::optional<FloatRect> intersection = circle.getGlobalBounds().findIntersection(col.brick.getGlobalBounds());
                    if(intersection.has_value()){
                        run = false;
                        Vector2f brickCenter = col.brick.getGlobalBounds().getCenter();
                        Vector2f circleCenter = circle.getGlobalBounds().getCenter();
                        std::cout << brickCenter.x + radius << " " << circleCenter.x - radius + speed << std::endl;
                    }
                }
            }
        }

        std::cout << run << std::endl;

        // Draw
        window.draw(circle);
        window.display();
    }

    return 0;
}
