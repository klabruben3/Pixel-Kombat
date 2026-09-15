 
#include <iostream>
#include <SFML/Graphics.hpp>
#include <thread>

using namespace std;
using namespace sf;
class Fruit
{
public:
	 

	Fruit( Texture *texture )
	{
		cout << "Fruit class OK" << endl;
		fruitSpr.setTexture(*texture);		 
		setGridPos(11, 13);
		setLevel(1);
	}

	~Fruit() {}

	//---
	Sprite getSrpite()
	{
		return fruitSpr;
	}

	//---
	void setVisible(bool vis)
	{
		visible = vis;
	}

	//---
	void start( )
	{		
		if (!fruitThr.joinable())
		{
			fruitThr = thread(&Fruit::loop, this);
			cout << "Fruit thread start"<<endl;
		}		
	}

	//---
	void stop()
	{
		if (fruitThr.joinable())
		{
			fruitThr.detach();
			cout << "Fruit thread stop" << endl;
		}
	}

	//---
	void setLevel(int lvl)
	{
		switch (lvl)
		{
		case 3:
		case 4:
			level = 3;
			break;
		case 5:
		case 6:
			level = 4;
			break;
		case 7:
		case 8:
			level = 5;
			break;

		case 9:
		case 10:
			level = 6;
			break;

		case 11:
		case 12:
			level = 7;
			break;

		 default:
			 level = lvl;
			break;
		}
		
		if (lvl >= 13) { level = 8;}
		fruitSpr.setTextureRect(IntRect(24 * (level - 1), ofsY, 24, 28));
	}

	//---
	bool getVisible() { return visible; }
	
	//---
	Vector2f getPosition() { return fruitSpr.getPosition(); }

private:
	Sprite fruitSpr;
	int ofsX=0, ofsY=6*30;
	int level=0;
	int delay = 30;// wait 30 sec for visible fruit
	bool visible = false;
	int curTime;
	thread fruitThr;
   	
	//---
	void loop()
	{
		curTime = time(0);
		curTime += delay;
		while ( fruitThr.joinable())
		{
			if (curTime < time(0))
			{
				setVisible(!visible);
				curTime = time(0);
				curTime += delay;
			}	
			sleep(milliseconds(10));
		}
		setVisible(false);
	}

	//---
	void setGridPos(int x, int y)
	{
		fruitSpr.setPosition(Vector2f(x * 30, y * 30));
	}

};