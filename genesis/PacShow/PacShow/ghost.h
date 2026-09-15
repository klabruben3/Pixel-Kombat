#pragma once
#include <SFML/Graphics.hpp>
#include <thread>
#include <iostream>
#include "targetCalc.h"
#include "gameText.h" 
using namespace std;
using namespace sf;

template <class C>
class Ghost : public CntrGame
{
public:

	C* starter;
	Ghost(Texture* texture, int id, C* _starter)
	{
		Id = id;
		Name = nameArr[id];
		starter = _starter;
		ghostSpr.setTexture(*texture);
		cout << "Ghost class OK Name: " << Name << endl;
		init();


	}
	~Ghost()
	{
		if (loopThread->joinable()) { loopThread->detach(); }
	}

	//---
	Sprite getSprite()
	{
		return ghostSpr;
	};

	//---
	Text getNameTxt() { return ghostNameTxt; }

	//---
	void reset()
	{
		ghostAnim = moveX = moveY = 0;
		speed = 3;
		if (gameStatus == Demo)
		{
			setGridPosition(demoPos[Id][0], demoPos[Id][1]);
			ghostNameTxt.setPosition(Vector2f(demoPos[Id][0] * 40, demoPos[Id][1] * 30));
			direction(GhostDirection::DOWN);
		}
		else
		{
			goHome();
		}
		thisGhostStatus = Scater;
		stop();
	}

private:
	Sprite ghostSpr;
	GameText gameText;
	Text ghostNameTxt;
	TargetCalc targetCalc;
	GhostDirection currGhostDir;
	thread* loopThread;
	sf::Vector2f ghostPos;
	float ghostAnim = 0;
	int startX;
	int startY;
	int sprOfsXX = 0;
	int startPosArr[4][2] = { {9,11},{10,11},{12,11},{13,11} };
	int edgePosArr[4][2] = { {-2,-5},{22,-2},{3,19},{18,19} };
	string nameArr[5] = { "","Blinky","Pinky","Inky","Clyde" };

	//---
	void init()
	{
		startX = startPosArr[Id - 1][0];
		startY = startPosArr[Id - 1][1];
		edgePos = sf::Vector2f(edgePosArr[Id - 1][0] * 30, edgePosArr[Id - 1][1] * 30);
		targetPos = &edgePos;
		sprW = 30; sprH = 30; sprOfsX = 0; sprOfsY = 30 + sprW * (Id - 1);
		ghostNameTxt = gameText.getGhostNameTxt(Name);		
		reset();		
		ghostSpr.setTextureRect(sf::IntRect(sprOfsX, sprOfsY, sprW, sprH));
		loopThread = new thread([this]() {loop(); });
	}


	//---
	void loop()
	{
		while (true)
		{
			sleep(milliseconds(delayLoop));	 	
			animation();
			if (!stopMove  )
			{
				if (xyModul())
				{
					checkCross();
				}
				ghostPos.x += moveX; ghostPos.y += moveY;
			}
			ghostAnim += .1;
			if (ghostAnim > 2) { ghostAnim = 0; }			

		}
	}

	//---
	void checkCross()
	{
		if (getPosGridX() > 21)
		{
			setGridPosition(0, 11);
		}
		else if (getPosGridX() < 0)
		{
			setGridPosition(21, 11);
		}

		string str = getMazeStr(getPosGridX(), getPosGridY());
		if (str == "=" || thisGhostStatus == Blue) { delayLoop = 20; }
		else { delayLoop = 15; }
		if (str == "-") { isInHome = false; }
		else if (str == "#") // Trap error out of maze
		{
			cout <<"Ghost error stoped. Name = " << Name << endl;
			stop();
		    
		}      
		const char* ch = str.c_str();
		GhostDirection dir = getDirection(ch,currGhostDir);
		if (dir == NONE)
		{
			//Blinky position for calculate Inky target
			if (Id == 1) { CntrGame::blinkyPosition = ghostPos; }
			dir = targetCalc.getToTargetDir(Id, ghostSpr.getPosition(),CntrGame::blinkyPosition,
					                        *targetPos, edgePos,  pacDirection, ch
			                               );
			if (dir == NONE) { dir = currGhostDir; }
		}           		
		direction(dir);
		 
	}

	//---
	GhostDirection getDirection(const char *crossCh,GhostDirection currDir)
	{
		switch (*crossCh)
		{
		case '*':
			return UP;
			break;

		case 'Q':          // RD
			if (currDir == LEFT) { return DOWN; }
			else { return RIGHT; }
			break;

		case 'W':         // W=LD                                
			if (currDir == RIGHT) { return DOWN; }
			else { return LEFT; }
			break;

		case 'E':         // E=RU
			if (currDir == LEFT) { return UP; }
			else { return RIGHT; }
			break;

		case 'R':         // R=LU
			if (currDir == RIGHT) { return UP; }
			else { return LEFT; }
			break;


		}
		return NONE;
	}

	//---
	void direction(GhostDirection direct)
	{
		switch (direct)
		{
		case UP:
			moveY = -speed; moveX = 0;
			break;

		case RIGHT:
			moveY = 0; moveX = speed;
			break;

		case DOWN:
			moveY = speed; moveX = 0;
			break;
		case LEFT:
			moveY = 0; moveX = -speed;
			break;
		}
		currGhostDir = direct;

	}
	//---
	void animation()
	{

		if (thisGhostStatus != Blue)
		{
			switch (currGhostDir)
			{
			case UP:
				sprOfsXX = sprW * 4;
				break;
			case RIGHT:
				sprOfsXX = 0;
				break;
			case DOWN:
				sprOfsXX = sprW * 2;
				break;
			case LEFT:
				sprOfsXX = sprW * 6;
				break;

			}
		}
		else
		{
			sprOfsY = 5 * sprH;
			sprOfsXX = 0;
		}

		sprOfsX = (sprW * (int)ghostAnim) + sprOfsXX;
		ghostSpr.setTextureRect(sf::IntRect(sprOfsX, sprOfsY, sprW, sprH));
		ghostSpr.setPosition(ghostPos);
		if (gameStatus == Play && !CntrGame::pacIsDead)
		{
			if (collidToPac())
			{
				if (CntrGame::isBlueGhost)//thisGhostStatus==Blue) 
				{
					gameSound.play(GameSound::PlSound::MonsterEat);
					if (CntrGame::ghostBonus > 1600) { CntrGame::ghostBonus = 100; }
					CntrGame::ghostBonus *= 2;
					CntrGame::score += CntrGame::ghostBonus;
					starter->gameText->showBonus(CntrGame::ghostBonus, ghostSpr.getPosition(),false);
					starter->gameText->scoreTxt.setString("SCORE: "+to_string(CntrGame::score));
					goHome();
				}
				else { starter->collidToPac(); }
				cout << "Collided to Pac " << endl;
			}
		}


		//-----------****** trap Error -> Out of Maze
		//if ( gameStatus==Play)
		//{
		//	//cout << "posX " << getPosGridX() << endl;
		//	//cout << "posY " << getPosGridY() << endl;
		//}
		/*if (isInHome &&  getPosGridY()!=11 && getPosGridY() != 10 && !CntrGame::isBlueGhost)
		{			
			stop();
			cout << "**** OUPS ****" << endl;
			cout << "isInHome=" << isInHome << endl;
			cout << "Ghost Name=" << Name << endl;
		}*/

		if (getPosGridX()>23 || getPosGridX()<-5 ||
			getPosGridY()>24 || getPosGridY()<-5
		   )
		{
			cout << "************* Ghost out of Maze.  Name=" << Name<<endl;
			cout << "isInHome=" << isInHome << endl;
			cout << "Direction=" << currGhostDir << endl;
			cout << "Status=" << thisGhostStatus << endl;
			stop();	
			goHome();
		}

		//--------------------------------------
	}
 

	 

	//---
	void goHome()
	{	   
		setGridPosition(startX, startY);		
		isInHome = true;
		if (Id < 3) { direction(GhostDirection::RIGHT); }
		else
		{
			{direction(GhostDirection::LEFT); }
		}
		sprOfsY = 30 + sprW * (Id - 1);
		targetPos = &edgePos;
		thisGhostStatus = Scater;
		stop();		
	}

	//---
	void setGridPosition(int x, int y)
	{		
		ghostPos = sf::Vector2f((x * sprW) , (y * sprH) );
		

	}

	//---
	int getPosGridX()
	{
		return  (ghostSpr.getPosition().x) / sprW;
	}

	//---
	int getPosGridY()
	{
		return  (ghostSpr.getPosition().y) / sprH;
	}

	//---
	bool collidToPac()
	{
			int dx = abs((ghostSpr.getPosition().x+sprW/2) - CntrGame::pacPosition->x);
			int dy = abs((ghostSpr.getPosition().y+sprH/2) - CntrGame::pacPosition->y);		
			return  dx < sprW/2 && dy < sprH/2;		
	}

	//---
	bool xyModul()
	{
		modulX = (int)ghostSpr.getPosition().x % sprW;
		modulY = (int)ghostSpr.getPosition().y % sprH;	
		return modulX == 0  && modulY == 0  ;
	}

	

};