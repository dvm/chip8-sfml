#pragma once

#include "Chip8.h"

namespace chip8
{
	class Keypad
	{
	public:
		Keypad() = default;

		void keyDown(const sf::Event::KeyEvent& event);
		void keyUp(const sf::Event::KeyEvent& event);

		bool isPressed(int key) const;
		int getPressedKey() const;

		static constexpr auto kNone = -1;

	private:
		void keyEvent(const sf::Event::KeyEvent& event, bool pressed);

		static constexpr auto kKeyPadSize = 16;
		static constexpr sf::Keyboard::Key keymap[kKeyPadSize] =
		{
			sf::Keyboard::X,    sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3,
			sf::Keyboard::Q,    sf::Keyboard::W,    sf::Keyboard::E,    sf::Keyboard::A,
			sf::Keyboard::S,    sf::Keyboard::D,    sf::Keyboard::Z,    sf::Keyboard::C,
			sf::Keyboard::Num4, sf::Keyboard::R,    sf::Keyboard::F,    sf::Keyboard::V,
		};

		std::array<int, kKeyPadSize> keys{ 0 };
	};
}
