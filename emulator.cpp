#define emulator_chip8 
#include "chip8.h"
#include <iostream>
#include <conio.h>
#include <SFML/Graphics.hpp>

chip8 CHIP;
char fromKtoS(const sf::Keyboard::Key& k);
int main(int argc, char **argv)
{
	sf::RenderWindow window(sf::VideoMode(650, 320), "Emulator chip-8");
    window.setFramerateLimit(1000);
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
            if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;
               unsigned char key2 = fromKtoS(key);
               if (key2=='0') {
                   //std::cout << "Wrong key " << std::endl;
                   break;
               }
               CHIP.keyEvents(key2, true);
               std::cout << key2 << std::endl;
            }
            else if (event.type == sf::Event::KeyReleased) {
                sf::Keyboard::Key key = event.key.code;
                unsigned char key2 = fromKtoS(key);
                CHIP.keyEvents(key2, false);
                std::cout << key2 << std::endl;
            }
        }
        // Clear screen
        window.clear();

        CHIP.render(window);

        window.display();
    }
}

char fromKtoS(const sf::Keyboard::Key& k) {
    char ret = '0';
    switch (k)
    {
    case sf::Keyboard::Num1: {
        ret = '1';
        break;
    }
    case sf::Keyboard::Num2: {
        ret = '2';
        break;
    }
    case sf::Keyboard::Num3: {
        ret = '3';
        break;
    }
    case sf::Keyboard::Num4: {
        ret = '4';
        break;
    }
    case sf::Keyboard::Q: {
        ret = 'Q';
        break;
    }
    case sf::Keyboard::W: {
        ret = 'W';
        break;
    }
    case sf::Keyboard::E: {
        ret = 'E';
        break;
    }
    case sf::Keyboard::R: {
        ret = 'R';
        break;
    }
    case sf::Keyboard::A: {
        ret = 'A';
        break;
    }
    case sf::Keyboard::S: {
        ret = 'S';
        break;
    }
    case sf::Keyboard::D: {
        ret = 'D';
        break;
    }
    case sf::Keyboard::F: {
        ret = 'F';
        break;
    }
    case sf::Keyboard::Z: {
        ret = 'Z';
        break;
    }
    case sf::Keyboard::X: {
        ret = 'X';
        break;
    }
    case sf::Keyboard::C: {
        ret = 'C';
        break;
    }
    case sf::Keyboard::V: {
        ret = 'V';
        break;
    }
    default: {
        break;
    }
    }
    return ret;
}

