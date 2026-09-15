#pragma once
#include <iostream>
#include <string>
#include <array>
#include <map>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Maze
{
public:     
    static Maze* instance;
      string mazePattern[25] =
    { "#######################" , // 0   // #= Bricks
      "#Q   S    W#Q    S   W#" , // 1   // X= Ghosts direction condition
      "#@### #### # #### ###@#" , // 2   // c= Pacman char
      "# ### #### # #### ### #" , // 3   // Q=RD
      "#D   X S  A A  S X   F#" , // 4   // W=LD
      "# ### # ####### # ### #" , // 5   // E=RU
      "#E   F# ####### #D   R#" , // 6   // R=LU
      "##### #E  W#Q  R# #####" , // 7   //-------
      "##### #### # #### #####" , // 8   // A=LRU 
      "##### #Q  AGA  W# #####" , // 9   // S=LRD
      "##### # ###-### # #####" , // 12  // D=UDR
      "0====X F#BP*IC#D X====0" , // 11  // F=UDL
      "##### # ####### # #####" , // 12  // G=LR   
      "##### #D0001000F# #####" , // 13  // 1=Fruit location
      "##### # ####### # #####" , // 14  // L=Low speed in tunel for ghosts
      "##### # ####### # #####" , // 15  // BPIC=Ghosts char
      "#Q   X A  W#Q  A X   W#" , // 16
      "# ### #### # #### ### #" , // 17
      "#E W#D S  AcA  S F#Q R#" , // 18
      "### # # ####### # # ###" , // 19
      "#Q A R#E  W#Q  R#E A W#" , // 20
      "# ######## # ######## #" , // 24
      "#@######## # ########@#" , // 22
      "#E        A A        R#" , // 23
      "#######################" };// 24
    // 01234567890123456789012 
     
      string mazeGrid[23][25];     
      CircleShape  dotsArr[222]; // 222 dots
      int dotsCount = 0;

    Maze()
    {
        cout << "Maze class OK"<<endl;
        instance = this;         
    }
    ~Maze()
    {
     
    }

    //----
    void initMaze()
    {      
        int colum = 0; int row = 0;      
        for (int i = 0; i < (Maze::mazeW*Maze::mazeH) ; i++)
        {              
            if (colum >= Maze::mazeW)
            {
                colum = 0; row++;
               // cout << mazePattern[row] << endl;
            }
            string str = mazePattern[row].substr(colum, 1);
            mazeGrid[colum][row] = str;         
            if (str == "#")
            {
               // wallShapeArr[wallShapeCount] = getWallShape(colum,row);
                wallShapeCount++;
            }
            else
            {
                if (isDot(str) )                   
                {                
                 dotsArr[dotsCount] = getDots(colum, row,str);                 
                 dotsCount++;
                }                
            }
            colum++;
        }       
    }

    //---
    void drawWall( RenderWindow *window )
    {
        int i = 0;
        for ( i = 0; i < wallShapeCount; i++)
        {          
            // window->draw (wallShapeArr[i]);
            if (i < dotsCount)
            {
                window->draw(dotsArr[i]);
            }            
        }        
    }

    //
    void redrawDot()
    {
        for (int i = 0; i < dotsCount; i++)
        {           
            CircleShape dot = dotsArr[i];
            dot.setFillColor(Color::Yellow);
            dotsArr[i] = dot;          
        }
    }
    

private:
    sf::RectangleShape wallShapeArr[23*24];    
    static int mazeW, mazeH;
    int wallShapeCount = 0;

    //---
    bool isDot(string str)
    {
        return str != "0" && str != "=" &&
               str != "B" && str != "P" &&
               str != "I" && str != "C" && 
               str != "G" && str != "*" && 
               str != "1" && str != "-" && str!="c";
    }

    //---
    RectangleShape getWallShape(int col,int row)
    {
        RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(26, 26));// 30 28
        rectangle.setFillColor(Color::Blue);
        rectangle.setOutlineColor(sf::Color::Black);
        rectangle.setOutlineThickness(.5);
        rectangle.setPosition(col*30, row*30);
        return rectangle;
    }     

    //---
    CircleShape getDots(int col, int row, string str)
    {
        float radius = 3;  int offset = 12;
        Color color = Color::Yellow;
        if (str == "@")
        {
            radius = 10; offset = 6;          
        }
        CircleShape _dot;
        _dot.setRadius(radius);
        _dot.setFillColor(color);
        _dot.setPosition(col * 30 + offset, row * 30 + offset);      
        return _dot;
    }

};

Maze *Maze::instance;
int Maze::mazeW = 23;
int Maze::mazeH = 25;
 