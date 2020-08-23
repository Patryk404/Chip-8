#pragma once
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
class chip8 {
private:
	unsigned short opcode;
	unsigned char memory[4096]; // the chip 8 has 4k memory
	unsigned char V[16]; //CPU REGISTERS
	unsigned short I; //index register
	unsigned short pc; // program computer pc
	unsigned char gfx[64 * 32]; // screen 
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned short stack[16];
	unsigned short sp; // stack pointer
	unsigned char key[16]; // 0x0-0xF
	sf::SoundBuffer buffer;
	sf::Sound BEEP;
	void update_timers();
public: 
	chip8();
	void initialize();//  Initialize the Chip8 system and load the game into the memory 
	bool loadGame(const char * name_game); //loading a game into the memory
	void emulateCycle();// emulate one cycle of game
	void keyEvents(unsigned char key,bool pressed);// store key press state (Press and Release)
	void debugRender();
	void render(sf::RenderWindow& window);
};