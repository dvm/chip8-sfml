#pragma once

#include "Chip8.h"

#include "Screen.h"
#include "Keypad.h"

namespace chip8
{
	class Cpu
	{
	public:
		static constexpr auto kTimerStep = 1.0f / 60.0f;

		enum class State
		{
			Initializing,
			Running,
			Stopped
		};

		explicit Cpu(std::shared_ptr<Screen> screen, std::shared_ptr<Keypad> keypad);

		State getState() const noexcept;
		bool isRunning() const noexcept;

		void setTracing(bool value) noexcept;

		void loadProgram(const std::string& filename);
		void reset();
		void start();

		/// <returns>true when screen update is requested</returns>
		bool step();

		void timerTick();

	private:
		static constexpr auto kRegisters = 16;
		static constexpr auto kCarry = 15;
		static constexpr auto kStackSize = 16;
		static constexpr auto kMemorySize = 4096;
		static constexpr auto kProgramStart = 512;

		std::array<Byte, kMemorySize> memory; // memory (4096 bbytes)
		std::array<Word, kStackSize> stack;   // stack (32 bytes)
		std::array<Byte, kRegisters> regs;    // registers V0-VE, VF is carry bit
		Word pc{ kProgramStart };             // program counter
		Word sp{ kStackSize };                // stack pointer
		Word ireg{ 0 };                       // I(ndex) register
		Byte delayTimer{ 0 };
		Byte soundTimer{ 0 };

		std::shared_ptr<Screen> screen;
		std::shared_ptr<Keypad> keypad;

		State state{ State::Initializing };
		bool tracing{ false };

		union Decoder {
			Word opcode;
			// [mxyz]
			struct nib1 {
				Byte z : 4;
				Byte y : 4;
				Byte x : 4;
				Byte m : 4;
			} n4;
			// [mxkk]
			struct nib2 {
				Byte k : 8;
				Byte x : 4;
				Byte m : 4;
			} n3;
			// [mnnn]
			struct nib3 {
				Word n : 12;
				Word m : 4;
			} n2;
		};

		static_assert(sizeof(Decoder) == sizeof(Word));

	private:
		static constexpr auto kFontStart = 80;
		static constexpr auto kFontWidth = 5;

		void initFont();

		inline void push(Word value);
		inline Word pop();

		inline void setCarry(bool flag = true);
		inline void clearCarry();
		inline bool carry() const;

		inline void advance();
		inline void reverse();
	};
}
