#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

std::vector<std::string> stage = {
"7---T-------------T---9",
"|***|*************|***|",
"|*+*v*<-T->*<-T->*v*+*|",
"|*******|*****|*******|",
"[->*^*^*|*7-9*|*^*^*<-]",
"|***|*|*|*| |*|*|*|***|",
"|*+*|*|*v*1-3*v*|*|*+*|",
"|***|*|*********|*|***|",
"[---3*|*<----->*|*1---]",
"|*****|*********|*****|",
"v <---3*7-> <-9*1---> v",
"********|RC GP|********",
"^ <---9*1-----3*7---> ^",
"|*****|*********|*****|",
"[---9*|*<----->*|*7---]",
"|***|*|*********|*|***|",
"|*+*|*|*^*7-9*^*|*|*+*|",
"|***|*|*|*| |*|*|*|***|",
"[->*v*v*|*1-3*|*v*v*<-]",
"|*******|**@**|*******|",
"|*+*^*<-_->*<-_->*^*+*|",
"|***|*************|***|",
"1---_-------------_---3",

};

const int TILESIZE = 20;
const float speed = 25;
const int PAC_FRAMES = 6;
const int FRAME_SIZE = 32;
int pacCurrentFrame = 0;

// Defines pacman
Sprite pacman(Texture &atlas){    
    Vector2f atpos;

    for(int y = 0; y < stage.size(); y++){
        for(int x = 0; x < stage[y].size(); x++){
            // create a position for the restart position
            
            if(stage[y][x] == '@'){
                atpos = Vector2f(FRAME_SIZE * (x + 1), FRAME_SIZE * (y + 1));
                break;
            }
        }
    }
    
    Sprite pac(atlas);
    pac.setOrigin({FRAME_SIZE/2, FRAME_SIZE/2});
    pac.setPosition(atpos);

    return pac;
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

Sprite stages(Texture &atlas, char id){
    Sprite stage(atlas);    
    
    if(id == '7' || id == '9' || id == '3' || id == '1'){
        stage.setTextureRect(IntRect({0, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if(id == '7') stage.setRotation(degrees(0));
        else if(id == '9') stage.setRotation(degrees(90));
        else if(id == '3') stage.setRotation(degrees(180));
        else if(id == '1') stage.setRotation(degrees(270));
    }else if(id == '-' || id == '|'){
        stage.setTextureRect(IntRect({32, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if(id == '-') stage.setRotation(degrees(0));
        else if(id == '|') stage.setRotation(degrees(90));     
    }else if(id == 'T' || id == ']' || id == '_' || id == '['){
        stage.setTextureRect(IntRect({64, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if(id == 'T') stage.setRotation(degrees(0));
        else if(id == ']') stage.setRotation(degrees(90));
        else if(id == '_') stage.setRotation(degrees(180));
        else if(id == '[') stage.setRotation(degrees(270));        
    }else if(id == '>' || id == 'v' || id == '<' || id == '^'){
        stage.setTextureRect(IntRect({96, 0}, {FRAME_SIZE, FRAME_SIZE}));
        if(id == '>') stage.setRotation(degrees(0));
        else if(id == 'v') stage.setRotation(degrees(90));
        else if(id == '<') stage.setRotation(degrees(180));
        else if(id == '^') stage.setRotation(degrees(270));        
    }else if(id == '+'){
        stage.setTextureRect(IntRect({128, 0}, {FRAME_SIZE, FRAME_SIZE}));
    }else{
        stage.setTextureRect(IntRect({160, 0}, {FRAME_SIZE, FRAME_SIZE}));
    }

    stage.setOrigin(stage.getLocalBounds().getCenter());

    return stage;
}

struct brick{
    Sprite block;
    char id;

    brick(int x, int y, char c, Texture &atlas) : block(stages(atlas, c))
    {
        block.setPosition({(float)(FRAME_SIZE * (x + 1)), (float)(FRAME_SIZE * (y + 1))});

        id = c;
    }
};

// Making a replica of the stage array into walls with id and custom properties
std::vector<std::vector<brick>> wall;


void buildWall(Texture &atlas){
    for (int y = 0; y < stage.size(); y++) {
        std::vector<brick> bRows;
        for (int x = 0; x < stage[y].size(); x++) {
            char tile = stage[y][x];
            brick b(x, y, tile, atlas);
            
            bRows.push_back(b);
        }
        wall.push_back(bRows);
    }
}//

int main(){
    RenderWindow window(VideoMode({24 * FRAME_SIZE, 24 * FRAME_SIZE}), "Pixel Combat");
    window.setFramerateLimit(10);
    
    // defines the atlas for the characters
    Texture character_atlas;
    if (!character_atlas.loadFromFile("C:\\Users\\klabr\\Desktop\\Projects\\2d project\\Pixel Kombat\\genesis\\assets\\pacman.png")) return -1;
    character_atlas.setSmooth(false);
    
    // defines the atlas for the stage;
    Texture stage_atlas;
    if (!stage_atlas.loadFromFile("C:\\Users\\klabr\\Desktop\\Projects\\2d project\\Pixel Kombat\\genesis\\assets\\stage_atlas.png")) return -1;
    stage_atlas.setSmooth(false);        
    
    buildWall(stage_atlas);
    Sprite pac = pacman(character_atlas);
    
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
        for(auto &row : wall){
            for(auto col : row){
                // Vector2f brickCenter = col.brick.getGlobalBounds().getCenter();
                // Vector2f circleCenter = circle.getGlobalBounds().getCenter();
                
                // if(col.id == '*' && brickCenter.x == circleCenter.x || col.id == '*' && brickCenter.y == circleCenter.y){
                //     optional<FloatRect> intercept = col.brick.getGlobalBounds().findIntersection(circle.getGlobalBounds());
                //     if(intercept.has_value()){
                //         col.brick.setFillColor(Color::Green);
                //     }
                // }

                // if(col.id == '#' && abs(brickCenter.y - circleCenter.y) == TILESIZE){
                //     keycode = 0;
                //     reset(keycode);
                // }
                
                window.draw(col.block);
            }
        }

        window.draw(pac);
        window.display();
    }

    return 0;
}
