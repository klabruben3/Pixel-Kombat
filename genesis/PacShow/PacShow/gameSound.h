#pragma once
#include <iostream>
#include <string>
#include <SFML/Audio.hpp>
 

using namespace std;

class GameSound
{
public:	
	static GameSound *instance;
	enum PlSound
	{
		Blue, Eat, EatFruit, Intro, MonsterEat,
		PacmanDeath, PacmanEaten, Siren, WinnLife
	};

	GameSound()
	{
		cout << "Sound class OK" << endl;
		instance = this;
		init();
	}
	~GameSound() {}

	//---
	void play(PlSound snd)
	{				
		sndPlayer[snd].play();	
		if (snd ==PlSound::Intro )// if intro.wav, wait for end and start game  || snd == PlSound::MonsterEat
		{
			while (sndPlayer[snd].getStatus() == SoundSource::Status::Playing) {};			 
		}
		if (snd == PlSound::Blue || snd==PlSound::Siren) { sndPlayer[snd].setLoop(true); }
	}
	
	//---
	void stop(PlSound snd)
	{
		sndPlayer[snd].stop();		
	}	 

	//---
	void stopAll()
	{
		for (int i = 0; i < 9;i++)
		{
			sndPlayer[i].stop();
	    }
		cout << "Stop All Sound" << endl;
	}

	//---
	void setPich( PlSound snd, float pich)
	{
		sndPlayer[snd].setPitch(pich);		
	}

	//---
	static GameSound *getInstance()
	{
		if (!instance) { instance = new GameSound(); }
		return instance;
	}

private:
	sf::SoundBuffer sndBuf[9];
	sf::Sound sndPlayer[9];
	string wavArr[9] = { "blue","eat","eatFruit","intro","monsterEat",
						 "pacmanDeath","pacmanEaten","siren","winnLife"
	                   };

	//---
	void init()
	{
		for (int i = 0;i<9; i++)
		{			
		   bool load=sndBuf[i].loadFromFile("sound/" + wavArr[i] + ".wav");
		   while (!load)
		   {
			   cout << "Sound not found " << wavArr[i]<<".wav";
		   }     
		   sndPlayer[i].setBuffer(sndBuf[i]);		
		}
	}

	


};
GameSound *GameSound::instance;