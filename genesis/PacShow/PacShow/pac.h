 
#include <thread>
#include "cntrgame.h"
using namespace std;


enum PacDirection
{
 Up,Right,Down,Left
} pacDirection,pacPrevDirection;

template <class C>
class Pac : public CntrGame
{
public:
    C* starter;
    int pacLife = 3;    
    Pac(sf::Texture* texture,C *_starter)
    {
        std::cout << "Pac class OK \n";
        starter = _starter;
        pacSpr.setTexture(*texture);
        lifeSpr.setTexture(*texture);        
        init();         
    };


    //---
    sf::Sprite getSprite()
    {
        return pacSpr;
    };

    //---
    sf::Sprite getLifeSpr()
    {
        return lifeSpr;
    }

    //---
    void rotation(sf::Keyboard::Key key)
    {
        if (checkKeyPress(key))
        {
            move(key);           
        }
         
    }

    ~Pac()
    {
        if (loopThread->joinable()) { loopThread->detach(); }
    }

  
    
    //---
    void reset()
    {
        CntrGame::pacIsDead = false;
        pacAnim = moveX = moveY = 0;
        pacRot = 0;             
        forwX = 1, forwY = 0;
        pacDirection = pacPrevDirection = Right;
        if (gameStatus == Demo)
        {
            setGridPosition(demoPos[0][0], demoPos[0][1]);
            pacDirection = pacPrevDirection = Left;
        }
        else {  setGridPosition(startX, startY); }
        pacSpr.setPosition(pacPos);
        stop();
    }

private:
    thread* loopThread;
    float pacAnim = 0;
    float pacRot = 0;   
    sf::Sprite pacSpr;  
    sf::Sprite lifeSpr;
    int forwX = 1,forwY=0;
    int startX =  11;
    int startY =  18;   
     
    //---
    void init()
    {       
        sprW = 30; sprH = 30; sprOfsX = 0; sprOfsY = 0;
        speed = 3;        
        pacSpr.setOrigin(sprW / 2, sprH / 2);
        pacSpr.setTextureRect(sf::IntRect(sprOfsX, sprOfsY, sprW, sprH));  
        lifeSpr.setTextureRect(sf::IntRect(30, 0, sprW, sprH));           
        lifeSpr.scale(.8,.8);
        reset();
        loopThread = new thread([this]() {loop(); });
    }

   

    //---
    void loop()
    {        
        while (true)
        {
            sleep( milliseconds  (delayLoop ) );                      
            pacAnim+=.4;
            if (pacAnim > 3) { pacAnim = 0; }          
            animation();    

        }        
    }

    //---
    void move(sf::Keyboard::Key key)
    {          
        if (gameStatus == Demo) { return; }
        //forwX = forwY = 0;
        switch (key)
        {
        case sf::Keyboard::Up :         
            pacDirection = Up;                             
            break;

        case sf::Keyboard::Right:           
            pacDirection = Right;
            break;

        case sf::Keyboard::Down:                                   
            pacDirection = Down;         
            break;

        case sf::Keyboard::Left:
            if (pacRot==std::abs(180) ) { return; }     
            pacDirection = Left;   
            break;      
        }              
    }

    //---
    void direction(PacDirection direct)
    {       
        forwX = forwY = 0;
        switch (direct)
        {
        case Up:
            moveY = -speed; moveX = 0;
            forwY = -1;
            pacRot = -90;         
            break;

        case Right:
            moveY = 0; moveX = speed;
            forwX = 1;
            pacRot = 0;
            break;

        case Down:
            moveY = speed; moveX = 0;
            forwY = 1;
            pacRot = 90;
            break;
        case Left:
            moveY = 0; moveX = -speed;
            forwX = -1;
            pacRot = 180;          
            break;
        }
        if (pacRot != pacSpr.getRotation())
        {
            pacRot -= pacSpr.getRotation();
            setRotation(pacRot);
        }
    }

    //---
    bool checkKeyPress(sf::Keyboard::Key key)
    {
        return key == sf::Keyboard::Up    ||
               key == sf::Keyboard::Right ||
               key == sf::Keyboard::Down  ||
               key == sf::Keyboard::Left;
    }

    //---
    void setRotation(float rot)
    {         
        pacSpr.rotate(rot);
    }

    //---
    int getPosGridX()
    {
      return  (pacSpr.getPosition().x ) / sprW;
    }

    //---
    int getPosGridY()
    {
        return  (pacSpr.getPosition().y ) / sprH;
    }

    //---
    void animation()
    {    
        //string str = getMazeStr(getPosGridX(),getPosGridY());
        if (getPosGridX() > 21)
        {
            setGridPosition(0, 11);
        }
        else if (getPosGridX() < 0)
        {
            moveX = 0;
            setGridPosition(21, 11);
        }
        if (xyModul())
        {
            
            direction(pacDirection);
            if (collidWall())
            {   
               if (pacDirection == pacPrevDirection) 
                {
                   moveX = moveY = 0;
                   pacPrevDirection = pacDirection;
                   //   if (abs(forwX) == 1) {  moveX = 0; }
                   //   if (abs(forwY) == 1) {  moveY = 0; }
                }
               else 
               {     
                   direction(pacPrevDirection);
               }             
            }
             
            pacPrevDirection = pacDirection;
        }

        if ( xyModul() && collidWall()  )
        {
            moveX = moveY = 0;
        }       
        else if (!stopMove  )
        {
            pacPos.x += moveX; pacPos.y += moveY;
            pacSpr.setPosition(pacPos);
            if (xyModul() ) { eatDot(getPosGridX(), getPosGridY()); }
        }
        //else { pacAnim = 0; }        
        // fruit 
        if (getMazeStr(getPosGridX(), getPosGridY() )=="1" && starter->fruit->getVisible() )
        {
            int bonus = 100 * CntrGame::level;
            gameSound.play(GameSound::PlSound::EatFruit);
            setScore(bonus);
            starter->gameText->showBonus(bonus ,  starter->fruit->getPosition(),true );
            starter->fruit->setVisible(false);
        }


        sprOfsX = (sprW * (int)pacAnim);
        pacSpr.setTextureRect(sf::IntRect(sprOfsX, sprOfsY, sprW, sprH));
        CntrGame::pacPosition=&pacPos;
    }

    //---
    void eatDot(int pacX, int pacY)
    {
        for (int i = 0; i < maze->dotsCount; i++)
        {
            sf::CircleShape dot = maze->dotsArr[i];
            sf::Vector2f pos = dot.getPosition();
            int dotX = (int)(pos.x / sprW);
            int dotY = (int)(pos.y / sprH);
            if (dotX == pacX && dotY == pacY && !starter->isCollid)
            {
                if (dot.getFillColor() != sf::Color::Black)
                {
                    
                    if (dot.getRadius() == 10)// big dot
                    {
                        CntrGame::score += 20;                       
                        if (starter->blueInterval > 0)
                        {
                            CntrGame::isBlueGhost = true;
                            starter->setBlueGhost();
                        }
                    }
                    dot.setFillColor(sf::Color::Black);
                    maze->dotsArr[i] = dot;
                    gameSound.play(GameSound::PlSound::Eat);
                    CntrGame::dotsEat++;         
                    setScore(dotBonus);
                    break;
                }

            }
        }
    }

    //---
    void setScore(int score)
    {
        CntrGame::score += score;
        starter->gameText->scoreTxt.setString("SCORE: " + to_string(CntrGame::score));
    }

    //---
    bool collidWall()
    {
       return checkMazeGrid(getPosGridX() + forwX, getPosGridY() + forwY);
    }

    

    //---
    bool xyModul()
    {
        modulX = (int)pacSpr.getPosition().x % sprW;
        modulY = (int)pacSpr.getPosition().y % sprH;
        return modulX == 15 && modulY == 15;
    }

    //---
    void stopLoop()
    {
        loopThread->detach();
    }

    //---
    void setGridPosition(int x, int y)
    {
        pacPos = sf::Vector2f((x * 30) + (sprW / 2), (y * 30) + (sprH / 2));        
    }     

};