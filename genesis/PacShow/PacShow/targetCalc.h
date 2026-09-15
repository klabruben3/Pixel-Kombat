#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

//---
enum GhostDirection
{
	UP, RIGHT, DOWN, LEFT, NONE
};

class TargetCalc
{
public:	
	 
	int targetX, targetY;

	TargetCalc()
	{
		cout << "Terge class OK " << endl;		 
	}

	~TargetCalc() {}
	

	//--dokimastiko gia na dixni target point
	Vector2f getTargetPoint()
	{
		return Vector2f(targetX/30, targetY/30);
	}

	//---
	GhostDirection getToTargetDir(int ghId,sf::Vector2f ghostPos,sf::Vector2f blinkyPos,  
		                          sf::Vector2f targetPos, sf::Vector2f edgePos, PacDirection pacDir,const char* crossCh)
	{		
		targetOfsX = targetOfsY = 0;
		targetPos = (Vector2f(targetPos.x - 15, targetPos.y - 15));
		if (ghId == 2 || ghId==3) // Pinky Inky
		{
			switch (pacDir)
			{
			case Up:
				 targetOfsY = -4;
				 if (ghId == 3) { targetOfsY = -2; }
				break;
			case Right:
				targetOfsX = 4;
				if (ghId == 3) { targetOfsX = 2; }
				break;
			case Down:
				targetOfsY = 4;
				if (ghId == 3) { targetOfsY = 2; }
				break;
			case Left:
				targetOfsX = -4;
				if (ghId == 3) { targetOfsX = -2; }
				break;
			default:
				break;
			}
		}
		targetOfsX *= 30;
		targetOfsY *= 30;
		if (ghId == 3) // Inky
		{
			targetOfsX += ( targetPos.x-blinkyPos.x) ;
			targetOfsY += ( targetPos.y-blinkyPos.y) ;
			//cout << "bx=" << blinkyPos.x << "  by =" << blinkyPos.y << endl;
			//cout << "bx=" << targetPos.x << "  by =" << targetPos.y << endl;
		}
		 
		//--***
		targetX = targetPos.x +targetOfsX;
		targetY = targetPos.y +targetOfsY;
		//*************
			
		if (noCondition(crossCh)) { return NONE; }
		int dX = abs(ghostPos.x - (targetPos.x+ targetOfsX) );
		int dY = abs( ghostPos.y -(targetPos.y+targetOfsY) );
			
		string str = string(1, *crossCh);  //to_string(*crossCh);
		if (ghId == 4)// Clyde
		{
			if (ghostStatus == Attack)
			{				
				if (dX/30 < 8 && (dY/30) < 8) {targetPos = edgePos; }
				else {targetPos = (*CntrGame::pacPosition);}
				
			}
	    }
		if (dX > dY)
		{

			if ( ghostPos.x > targetPos.x)
			{
				if ( str != "D") { return LEFT; } // D=UDR  
				else if ( ghostPos.y  > targetPos.y ) { return UP ; }
				else { return DOWN; }
			}
			else
			{
				if ( str != "F") { return RIGHT; } // F=UDL                     
				else if (ghostPos.y > targetPos.y) {  return UP; }
				else {   return DOWN; }
			}


		}
		else
		{
			
			if ( ghostPos.y  > targetPos.y)
			{
				if ( str != "S" && str != "G") { return UP; }    // S=LRD , G=LR
				else if ( ghostPos.x < targetPos.x) {  return RIGHT; }
				else {  return LEFT; }
			}
			else
			{
				//cout << "ghost x=" << ghostPos.x << " target x= " << targetPos.x << endl;
				if ( str != "A" && str != "G") { return DOWN; } // A=LRU, G=LR
				else if (ghostPos.x < targetPos.x) {   return RIGHT; }
				else {  return LEFT; }
			}
		}
		 
		return NONE;
	}

private:
	int targetOfsX = 0;
	int targetOfsY = 0;
	//---
	bool noCondition(const char* ch)
	{
		return *ch == 'B' || *ch == 'P' || *ch == 'I' || *ch == 'C' ||
			   *ch == '0' || *ch == '=' || *ch == '1' || *ch == '@' ||
			   *ch == '#' || *ch == '-' || *ch == ' ' || *ch == 'c';
	}

};
 