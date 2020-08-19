#define emulator_chip8 

#include <iostream>

unsigned short opcode;
unsigned char memory[4096]; // the chip 8 has 4k memory
unsigned char V[16];
unsigned short I;
unsigned short pc;
unsigned char gfx[64 * 32]; // screen 
unsigned char delay_timer;
unsigned char sound_timer;
unsigned short stack[16];
unsigned short sp; // stack pointer
unsigned char key[16]; // 0x0-0xF
int main(int argc, char **argv)
{
    std::cout << "Hello World!\n";
}

