#pragma once

#include "Chip8.h"

namespace chip8
{
	class Screen
	{
	public:
		static constexpr auto kWidth = 64;
		static constexpr auto kHeight = 32;
		static constexpr auto kScreenSize = kWidth * kHeight;
		static constexpr auto kScale = 20;

		static constexpr sf::Uint32 kForeColor{ 0xff0fc4f1 }; // ABGR
		static constexpr sf::Uint32 kBackColor{ 0xff503e2c }; // ABGR

		Screen(std::shared_ptr<sf::RenderWindow> renderWindow,
			unsigned int screenWidth, unsigned int screenHeight, float spriteScale);
		~Screen() = default;

		void clear();
		void update();
		Byte setPixel(int x, int y, int color = 1);
		int getPixel(int x, int y) const;

	private:
		void init();

		unsigned int width{ kWidth };
		unsigned int height{ kHeight };
		float scale{ kScale };
		std::vector<Byte> screen;
		std::vector<sf::Uint32> pixels;

		std::shared_ptr<sf::RenderWindow> window;
		sf::Texture texture;
		sf::Sprite sprite;
	};
}
