#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
//#include <time.h>
#include <thread>
#include <chrono>


using namespace std;
using namespace sf;
class GameText
{
public:
	 
	    Font fnt;
		Text enterTxt;
		Text gameOverTxt;
		Text scoreTxt;
		Text bonusTxt;
		Text levelTxt;
		Text countTxt;
		Text targetPointTxt;
		RenderWindow* window;
		thread bonusThr;
		int *totalTime;
		int curTime;
		int delay = 2;// seconds
	GameText()//RenderWindow *win)
	{
		cout << "TextGame class OK" << endl;
		//window = win;
		fnt.loadFromFile("font/comic.ttf");
		setText();		 
	}

	~GameText() 
	{
		if (bonusThr.joinable()) { bonusThr.detach(); }
	}

	//---
	void showBonus(int bonus,Vector2f pos,bool fruit)
	{		
		cout << "Bonus ***** " << bonus << endl;
		if (fruit) { bonusTxt.setFillColor(Color::Green); }
		else
		{
			bonusTxt.setFillColor(Color::Blue);
		}
		bonusTxt.setString(to_string(bonus));
		bonusTxt.setPosition(pos);
		curTime = time(0);
		curTime += delay;
		totalTime = &curTime;
		sleep(milliseconds(100));
		if (!bonusThr.joinable())
		{
			cout << "TextThread created " << endl;			
			bonusThr = thread(&GameText::show, this);
		}				
	}

	//---
	void stopThread()
	{
		if (bonusThr.joinable()) { bonusThr.detach(); }
	}

	//---
	Text getGhostNameTxt(string name)
	{
		Text* txt = new Text();
		txt->setFont(fnt);
		txt->setString(name);
		txt->setFillColor(Color::White);
		txt->setCharacterSize(25);
		return *txt;
	}

private:
	

	//---
	void show()
	{		
		while (true )
		{
			int x = time(0);
			if (*totalTime < x)
			{

				bonusTxt.setString("");			
			}
			this_thread::sleep_for(chrono::milliseconds(10));			
			//cout << "TotalTime= " << *totalTime << endl;
		}// wait for delay 
				
		
	}


	//---
	void setText()
	{
		enterTxt.setFont(fnt);
		enterTxt.setString("PRESS ENTER");
		enterTxt.setCharacterSize(35);
		enterTxt.setPosition(Vector2f(300, 500));

	   


		scoreTxt.setFont(fnt);
		scoreTxt.setString("SCORE:");
		scoreTxt.setCharacterSize(22);
		scoreTxt.setPosition( getGridPosXY(1,0));

		levelTxt.setFont(fnt);
		levelTxt.setString("LEVEL:");
		levelTxt.setCharacterSize(18);
		levelTxt.setPosition(getGridPosXY(19, 24));
		
		countTxt.setFont(fnt);
		countTxt.setFillColor(Color::Red);
		countTxt.setString("");
		countTxt.setCharacterSize(35);
		countTxt.setOutlineColor(Color::White);
		countTxt.setOutlineThickness(2);
		countTxt.setPosition(getGridPosXY(11, 14));
	
		bonusTxt.setFont(fnt);
		bonusTxt.setString("");
		bonusTxt.setCharacterSize(26);
		bonusTxt.setFillColor(Color::Blue);
		bonusTxt.setOutlineColor(Color::White);
		bonusTxt.setOutlineThickness(2);

		gameOverTxt.setFont(fnt);
		gameOverTxt.setString("");
		gameOverTxt.setCharacterSize(35);
		gameOverTxt.setFillColor(Color::Red);
		gameOverTxt.setOutlineColor(Color::White);
		gameOverTxt.setOutlineThickness(2);
		gameOverTxt.setPosition(getGridPosXY(8, 14));


	 targetPointTxt.setFont(fnt);
	 targetPointTxt.setString("*");
	 targetPointTxt.setCharacterSize(25);
	 targetPointTxt.setFillColor(Color::Green);
	 //targetPointTxt.setOutlineColor(Color::White);
	 targetPointTxt.setOutlineThickness(1);
	}


	//---
	Vector2f getGridPosXY(float x,float y)
	{
		return Vector2f(x*30, y*30);
	}

	
};