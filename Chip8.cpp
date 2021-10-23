#include "Chip8.h"
#include "Cpu.h"

using namespace chip8;

int main(int argc, char *argv[])
{
	//argv[argc++] = "test_opcodes.ch8";

	bool tracing{ false };

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " [-t] <program>" << std::endl;
		return 1;
	}

	if (std::string("-t") == argv[1]) {
		tracing = true;
		++argv;
		--argc;
	}

	std::string fileName = argv[1];

	sf::VideoMode mode{ Screen::kWidth * Screen::kScale, Screen::kHeight * Screen::kScale };
	std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(mode, "Chip8sfml!");
	window->setFramerateLimit(30);

	std::shared_ptr<Screen> screen =
		std::make_shared<Screen>(window, Screen::kWidth, Screen::kHeight, float(Screen::kScale));

	std::shared_ptr<Keypad> keypad = std::make_shared<Keypad>();

	Cpu cpu(screen, keypad);

	try {
		cpu.loadProgram(fileName);
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Failed to load \"" << fileName << "\": " << e.what() << std::endl;
		return 1;
	}

	std::srand(12345);

	sf::Clock clock;
	float tickTimer{ 0 };

	cpu.setTracing(tracing);
	cpu.start();

	while (window->isOpen())
	{
		sf::Time deltaTime = clock.restart();
		sf::Event event;

		// keyboard
		while (window->pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window->close();
				break;
			case sf::Event::KeyPressed:
				keypad->keyDown(event.key);
				break;
			case sf::Event::KeyReleased:
				keypad->keyUp(event.key);
				break;
			default:
				break;
			}
		}

		// timer
		tickTimer += deltaTime.asSeconds();
		if (tickTimer > Cpu::kTimerStep) {
			tickTimer = 0.0f;
			cpu.timerTick();
		}

		// execute
		if (cpu.isRunning())
			if (cpu.step())
				screen->update();
	}

	return 0;
}
