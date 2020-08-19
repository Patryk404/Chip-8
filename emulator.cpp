#define emulator_chip8 
#include "chip8.h"
#include <iostream>
#include <conio.h>
#include <SFML/Graphics.hpp>

chip8 CHIP;
int main(int argc, char **argv)
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Emulator chip-8");
    window.setFramerateLimit(60);
	if (argc < 2)
	{
		printf("Usage: myChip8.exe chip8application\n\n");
		return 1;
	}

    CHIP.loadGame(argv[1]);
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        CHIP.emulateCycle();
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Clear screen
        window.clear();

        CHIP.render(window);

        window.display();
    }
}

