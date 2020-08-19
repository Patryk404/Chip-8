#pragma once
#include <string.h>
#include <iostream>
class chip8 {
public: 
	chip8();
	void initialize();//  Initialize the Chip8 system and load the game into the memory 
	void loadGame(std::string name_game); //loading a game into the memory
	void emulateCycle();// emulate one cycle of game
	void setKeys();// store key press state (Press and Release)
	bool drawFlag;
};