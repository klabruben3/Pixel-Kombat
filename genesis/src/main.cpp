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

    bool isIntersect = false;

    int xD = -1;
    int xPastD;

    int yD;
    int yPastD;
    
    while (window.isOpen()) {
        // Event handling (visitor style)
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
            window.close();
        }

        if(xD == -1){
            circle.move({-speed, 0});
            xPastD = xD;
        } else if(xD == 1){
            circle.move({speed, 0});
            xPastD = xD;
        }
        
        if(yD == -1){
            circle.move({0, -speed});
            yPastD = yD;
        }else if(yD == 1) {
            circle.move({0, speed});
            yPastD = yD;
        }


        if (Joystick::isConnected(0)) {
            float x = Joystick::getAxisPosition(0, Joystick::Axis::X);
            float y = Joystick::getAxisPosition(0, Joystick::Axis::Y);

            xD = (std::abs(x) > 15) ? x/std::abs(x) : xPastD;
            yD = (std::abs(y) > 15) ? y/std::abs(y) : yPastD;

            std::cout << xD << " " << yD << std::endl;
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

                // std::optional<FloatRect> intersection = circle.getGlobalBounds().findIntersection(col.brick.getGlobalBounds());
                // if(intersection.has_value()){
                //     std::cout << "Contact made." << std::endl;
                //     if(col.id == '#'){
                //         isIntersect = true;
                //         std::cout << "# activated" << std::endl;
                //     }
                // }else{
                //     isIntersect = false;
                //     // intersection.reset();
                // }
                // intersection.reset();
            }
        }

        // Draw
        window.draw(circle);
        window.display();
    }

    return 0;
}
