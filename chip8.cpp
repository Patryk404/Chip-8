#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


chip8::chip8() {
    ;
}
void chip8::initialize() {
	pc = 0x200; // Program counter starts at 0x200
	opcode = 0; // reset current opcode
	I = 0; // reset index register
	sp = 0;// reset stack pointer

  // Clear display
    for (int i = 0; i < 2048; ++i)
    {
        gfx[i] = 0;
    }
  // Clear stack
    for (int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
    }
  // Clear registers V0-VF
    for (int i = 0; i < 16; ++i)
    {
        V[i] = 0;
        key[i] = 0;
    }
  // Clear memory
    for (int i = 0; i < 4096; ++i)
    {
        memory[i] = 0;
    }

	//load fontset
	for (int i = 0; i < 80; i++)
	{
		memory[i] = chip8_fontset[i];
	}
	if (!buffer.loadFromFile("beep.wav")) {
		std::cout << "Cannot load a beep sound" << std::endl;
	}
	//Reset timers
	delay_timer = 0;
	sound_timer = 0;

	
	srand(time(NULL));

	BEEP.setBuffer(buffer);
}

void chip8::emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000)// Decode opcode
	{
	case 0x0000: {// V[opcode & 0x0F00 >> 8] -> Vx  V[opcode & 0x00F0 >> 4] -> Vy
		switch (opcode & 0x000F) {
		case 0x0000: { // opcode 0x00E0 clears the screen
			for (int i = 0; i < 2048; i++) {
				gfx[i] = 0x0;
			}
			pc += 2;
			break;
		}
		case 0x000E: { // opcode 00EE 
			--sp;
			pc = stack[sp];
			pc += 2;
			break;
		}
		default: {
			std::cout << "Opcode don't exist" << std::endl;
		}
		}
		break;
	}
	case 0x1000: { // 1NNN
		pc = opcode & 0x0FFF;
		break;
	}
	case 0x2000: {// 2NNN
		stack[sp] = pc;			// Store current address in stack
		++sp;					// Increment stack pointer
		pc = opcode & 0x0FFF;	// Set the program counter to the address at NNN
		break;
	}
	case 0x3000: { // 3XNN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		{
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	case 0x4000: { //4XNN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		{
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	case 0x5000: { //5XY0
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	case 0x6000: { // 6XNN
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2;
		break;
	}
	case 0x7000: { // 7XNN
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;
		break;
	}
	case 0x8000: {
		switch (opcode & 0x000F)
		{
		case 0x0000: { //8XY0
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]; 
			pc += 2;
			break;
		}
		case 0x0001: { //8XY1
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}
		case 0x0002: { //8XY2
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}
		case 0x0003: { //8XY3
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}
		case 0x0004: { //8XY4
			if (V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8]) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}
		case 0x0005: { //8XY5
			if (V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8]) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}
		case 0x0006: { // 8XY6
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
			break;
		}
		case 0x0007: { // 8XY7
			if (V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8]) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}
		case 0x000E: { // 8XYE
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			pc += 2;
			break;
		}
		default: {
			printf("Wrong opcode [0x8000]: 0x%X\n",opcode);
			break; 
		}

		}
		break;
	}
	case 0x9000: { // 9XY0
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	case 0xA000: { // ANNN
		I = opcode & 0x0FFF;
		pc += 2;
		break;
	}
	case 0xB000: { // BNNN
		pc = (opcode & 0x0FFF) + V[0];
		break;
	}
	case 0xC000: { // CXNN
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF); 
		pc += 2;
		break;
	}
	case 0xD000: { // DXYN
		unsigned short vx = V[(opcode & 0x0F00) >> 8];
		unsigned short vy = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int y = 0; y < height; y++) {
			pixel = memory[I + y];
			for (int x = 0; x < 8; x++) {
				if ((pixel & (0x80 >> x)) != 0) {
					if (gfx[x + vx + (y + vy) * 64]) {
						V[0xF] = 1;
					}
					gfx[x + vx + (y + vy) * 64] ^= 1;
				}
			}
		}
		pc += 2;
		break;
	}
	case 0xE000: { 
		switch (opcode & 0x00FF) {
			case 0x009E:{ // EX9E
				if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
					pc += 4;
				}
				else {
					pc += 2;
				}
				break;
			}
			case 0x00A1: { // EXA1
				if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
					pc += 4;
				}
				else {
					pc += 2;
				}
				break;
			}
			default: {
				printf("Wrong opcode: 0x%X\n", opcode);
				break;
			}
		}
		break;
	}
	case 0xF000: {
		switch (opcode & 0x00FF) 
		{
		case 0x0007: { // FX07
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;
		}
		case 0x000A: { // FX0A
			bool keyPress = false;

			for (int i = 0; i < 16; ++i)
			{
				if (key[i] != 0)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					keyPress = true;
				}
			}

			// If we didn't received a keypress, skip this cycle and try again.
			if (!keyPress)
				return;

			pc += 2;
			break;
		}
		case 0x0015: { // FX15
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}
		case 0x0018: { // FX18
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}
		case 0x001E: { // FX1E
			if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}	
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}
		case 0x0029: { // FX29
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;
		}
		case 0x0033: {
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
			break;
		}
		case 0x0055: { // FX55
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
				memory[I + i] = V[i];
			}
			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;
		}
		case 0x0065: { // FX65
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
				V[i] = memory[I + i];
			}
			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;
		}
		default: {
			printf("Wrong opcode: 0x%X\n", opcode);
			break;
		}
		}
		break;
	}
}
	update_timers();
}

void chip8::debugRender() {
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (gfx[(y * 64) + x] == 0)
                printf("O");
            else
                printf(" ");
        }
		printf("\n");
    }
	system("cls");
}

void chip8::render(sf::RenderWindow& window) {
	std::vector<sf::RectangleShape> block;
	int position_x=0;
	int position_y=0;
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 64; ++x)
		{
			position_x += 10;
			if (gfx[(y * 64) + x] != 0)
			{
				sf::RectangleShape temp(sf::Vector2f(10, 10));
				temp.setPosition(sf::Vector2f(position_x, position_y));
				block.push_back(temp);
			}
			if (position_x == 640)
			{
				position_x = 0;
			}
		}
		position_y += 10;
	}
	for (int i = 0; i < block.size(); i++)
	{
		window.draw(block[i]);
	}
}

void chip8::keyEvents(unsigned char key,bool pressed) {
		switch (key)
		{
		case '1': {
			this->key[0x0] = pressed ? 1 : 0;
			break;
		}
		case '2': {
			this->key[0x1] = pressed ? 1 : 0;
			break;
		}
		case '3': {
			this->key[0x2] = pressed ? 1 : 0;
			break;
		}
		case '4': {
			this->key[0x3] = pressed ? 1 : 0;
			break;
		}
		case 'Q': {
			this->key[0x4] = pressed ? 1 : 0;
			break;
		}
		case 'W': {
			this->key[0x5] = pressed ? 1 : 0;
			break;
		}
		case 'E': {
			this->key[0x6] = pressed ? 1 : 0;
			break;
		}
		case 'R': {
			this->key[0x7] = pressed ? 1 : 0;
			break;
		}
		case 'A': {
			this->key[0x8] = pressed ? 1 : 0;
			break;
		}
		case 'S': {
			this->key[0x9] = pressed ? 1 : 0;
			break;
		}
		case 'D': {
			this->key[0xA] = pressed ? 1 : 0;
			break;
		}
		case 'F': {
			this->key[0xB] = pressed ? 1 : 0;
			break;
		}
		case 'Z': {
			this->key[0xC] = pressed ? 1 : 0;
			break;
		}
		case 'X': {
			this->key[0xD] = pressed ? 1 : 0;
			break;
		}
		case 'C': {
			this->key[0xE] = pressed ? 1 : 0;
			break;
		}
		case 'V': {
			this->key[0xF] = pressed ? 1 : 0;
			break;
		}
		default:
			break;
		}

}

void chip8::update_timers() {
	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1) {
			BEEP.play();
		}
		--sound_timer;
	}
}

bool chip8::loadGame(const char * name_game) {
	initialize();
    FILE* file = fopen(name_game, "rb");
    fseek(file, 0, SEEK_END);
    long lSize = ftell(file);
    rewind(file);
    printf("Filesize: %d\n", (int)lSize);
    char* buffer = (char*)malloc(sizeof(char) * lSize);
    size_t result = fread(buffer, 1, lSize, file);
    if (result != lSize)
    {
        fputs("Reading error", stderr);
        return false;
    }
    if ((4096 - 512) > lSize)
    {
        for (int i = 0; i < lSize; ++i)
        {
            memory[i + 512] = buffer[i];//512 bytes is occupied by chip-8
        }   
    }
    else {
        printf("Error: ROM too big for memory");
    }
    fclose(file);
    free(buffer);
    return true;
}
//fetch Opcode -> During this step, the system will fetch one opcode from the memory at the location specified by the program counter (pc).