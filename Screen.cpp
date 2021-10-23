#include "Screen.h"

namespace chip8
{
	Screen::Screen(std::shared_ptr<sf::RenderWindow> renderWindow,
		unsigned int screenWidth, unsigned int screenHeight, float spriteScale)
		: width(screenWidth)
		, height(screenHeight)
		, scale(spriteScale)
		, window(renderWindow)
	{
		init();
	}

	void Screen::init()
	{
		pixels.resize(std::size_t(width) * height);
		screen.resize(std::size_t(width) * height);

		texture.create(width, height);
		sprite.setTexture(texture);
		sprite.setScale({ scale,scale });
	}

	void Screen::clear()
	{
		std::fill(screen.begin(), screen.end(), 0);
	}

	Byte Screen::setPixel(int x, int y, int color)
	{
		Word addr = (y * width + x) % (width * height);
		screen[addr] ^= color;
		return screen[addr];
	}

	int Screen::getPixel(int x, int y) const
	{
		Word addr = (y * width + x) % (width * height);
		return screen[addr];
	}

	void Screen::update()
	{
		for (std::size_t i = 0; i < pixels.size(); ++i)
			pixels[i] = (screen[i] == 1) ? kForeColor : kBackColor;

		texture.update(reinterpret_cast<sf::Uint8*>(pixels.data()));

		window->clear();
		window->draw(sprite);
		window->display();
	}
}
