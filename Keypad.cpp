#include "Keypad.h"

namespace chip8
{
	void Keypad::keyDown(const sf::Event::KeyEvent& event)
	{
		keyEvent(event, true);
	}

	void Keypad::keyUp(const sf::Event::KeyEvent& event)
	{
		keyEvent(event, false);
	}

	bool Keypad::isPressed(int key) const
	{
		return keys[key];
	}

	int Keypad::getPressedKey() const
	{
		for (int k = 0; k < kKeyPadSize; ++k)
			if (keys[k])
				return k;
		return kNone;
	}

	void Keypad::keyEvent(const sf::Event::KeyEvent& event, bool pressed)
	{
		for (int k = 0; k < kKeyPadSize; ++k)
			if (event.code == keymap[k])
				keys[k] = pressed;
	}
}
