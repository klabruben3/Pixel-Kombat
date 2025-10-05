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
    const int speed = 5.f; // size of each tile in pixels

    // Circle with radius 50
    CircleShape circle(20/2.5f);
    circle.setFillColor(Color::Red);
    
    float radius =  circle.getRadius();
    
    // Set origin to center
    circle.setOrigin({radius, radius});

    Vector2f atpos;

    for(size_t y = 0; y < stage.size(); y++){
        for(size_t x = 0; x < stage[y].size(); x++){
            // create a position for the restart position
            if(stage[y][x] == '@'){
                atpos = Vector2f(x * TILESIZE + radius, y * TILESIZE + radius);
                break;
            }
        }        
    }

    circle.setPosition(atpos);
    
    while (window.isOpen()) {
        // Event handling (visitor style)
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
            window.close();
        }

        struct walls{
            std::vector<RectangleShape> blocks;
            char id;
        };

        std::vector<walls> w;
        
        for (size_t y = 0; y < stage.size(); y++) {
            for (size_t x = 0; x < stage[y].size(); x++) {
                char tile = stage[y][x];
                walls wall;
                
                RectangleShape block({(float)TILESIZE, (float)TILESIZE});
                block.setPosition({(float)(x * TILESIZE), (float)(y * TILESIZE)});
            
                if (tile == '#') block.setFillColor(Color::Blue);  wall.id = tile; // wall
                if (tile == '*') block.setFillColor(Color::Black); wall.id = tile; // floor
                if (tile == '@') block.setFillColor(Color::Black); wall.id = tile; // floor

                wall.blocks.push_back(block);
                w.push_back(wall);
            }
        }

        if (Joystick::isConnected(0)) {
            float x = Joystick::getAxisPosition(0, Joystick::Axis::X);
            float y = Joystick::getAxisPosition(0, Joystick::Axis::Y);

            int xD = (std::abs(x) > 15) ? x/std::abs(x) : 0;
            int yD = (std::abs(y) > 15) ? y/std::abs(y) : 0;

            if(xD == -1) circle.move({-speed, 0});
            else if(xD == 1) circle.move({speed, 0});

            if(yD == -1) circle.move({0, -speed});
            else if(yD == 1) circle.move({0, speed});                
        } else {
            static bool printed = false;
            if (!printed) {
                std::cout << "No controller detected.\n";
                printed = true;
            }
        }

        for(size_t y = 0; y < w.size(); y++){
            for (size_t x = 0; x < w[y].blocks.size(); x++) {
                RectangleShape tile = w[y].blocks[x];
                window.draw(tile);

                if(circle.getGlobalBounds().findIntersection(tile.getGlobalBounds())){
                    std::cout << "finally" << std::endl; 
                }
            }
        }

        // Draw
        window.draw(circle);
        window.display();
        window.clear();
    }
}
