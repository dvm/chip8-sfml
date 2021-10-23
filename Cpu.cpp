#include "Cpu.h"

namespace chip8
{
	Cpu::Cpu(std::shared_ptr<Screen> renderScreen, std::shared_ptr<Keypad> inputKeypad)
		: screen(renderScreen)
		, keypad(inputKeypad)
	{
		reset();
	}

	Cpu::State Cpu::getState() const noexcept
	{
		return state;
	}

	bool Cpu::isRunning() const noexcept
	{
		return state == State::Running;
	}

	void Cpu::setTracing(bool value) noexcept
	{
		tracing = value;
	}

	void Cpu::reset()
	{
		state = State::Initializing;

		memory.fill(0);
		regs.fill(0);
		stack.fill(0);
		pc = kProgramStart;
		sp = kStackSize;
		ireg = 0;
		delayTimer = 0;
		soundTimer = 0;

		initFont();
		screen->clear();
	}

	void Cpu::initFont()
	{
		static constexpr std::array<Byte, 80> fontset =
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
		std::copy(fontset.begin(), fontset.end(), memory.begin() + kFontStart);
	}

	void Cpu::loadProgram(const std::string& filename)
	{
		if (std::ifstream is{ filename, std::ios::binary | std::ios::ate }) {
			auto size = is.tellg();

			if (size > (kMemorySize - kProgramStart))
				throw std::runtime_error("not enough memory, max size of program "
					+ std::to_string(kMemorySize - kProgramStart));

			is.seekg(std::ios::beg);

			if (is.read(reinterpret_cast<char*>(&memory[kProgramStart]), size))
				return;

			throw std::runtime_error("read error");
		}

		throw std::runtime_error("open error");
	}

	void Cpu::start()
	{
		state = State::Running;
	}

	void Cpu::timerTick()
	{
		if (delayTimer > 0)
			--delayTimer;

		if (soundTimer > 0) {
			--soundTimer;
			// TODO: play sound
		}
		else {
			// TODO: stop playing
		}
	}

	inline void Cpu::push(Word value)
	{
		sp = (sp - 1) & (kStackSize - 1);
		stack[sp] = value;
	}

	inline Word Cpu::pop() {
		Word val = stack[sp];
		sp = (sp + 1) & (kStackSize - 1);
		return val;
	}

	inline void Cpu::setCarry(bool flag)
	{
		regs[kCarry] = flag ? 1 : 0;
	}

	inline void Cpu::clearCarry()
	{
		regs[kCarry] = 0;
	}

	inline bool Cpu::carry() const
	{
		return regs[kCarry] != 0;
	}

	inline void Cpu::advance()
	{
		pc += sizeof(Decoder);
		pc &= kMemorySize - 1;
	}

	inline void Cpu::reverse()
	{
		pc -= sizeof(Decoder);
		pc &= kMemorySize - 1;
	}

	bool Cpu::step()
	{
		bool redraw{ false };

		//fetch
		Decoder decoder{ Word((memory[std::size_t(pc)] << 8) | memory[std::size_t(pc) + 1]) };

		//decode
		Byte m{ decoder.n4.m };    // m???
		Byte x{ decoder.n4.x };    // ?x??
		Byte y{ decoder.n4.y };    // ??y?
		Byte z{ decoder.n4.z };    // ???z
		Byte k{ decoder.n3.k };    // ??kk
		Word addr{ decoder.n2.n }; // ?nnn

		if (tracing) {
			std::cout << std::setfill('0') << std::hex
				<< "PC=" << std::setw(4) << (int)pc << " OP=" << std::setw(4) << decoder.opcode
				<< "  M=" << std::setw(1) << (int)m << "  X=" << std::setw(1) << (int)x
				<< "  Y=" << std::setw(1) << (int)y << "  Z=" << std::setw(1) << (int)z
				<< "  K=" << std::setw(2) << (int)k << "  A=" << std::setw(3) << (int)addr
				<< "\n" << "        "
				<< "SP=" << std::setw(2) << (int)sp << " "
				<< "I="  << std::setw(4) << (int)ireg << " "
				<< "DT=" << std::setw(2) << (int)delayTimer << " "
				<< "ST=" << std::setw(2) << (int)soundTimer << " ";
			for (int i = 0; i < kRegisters; ++i) {
				std::cout << std::hex << std::setw(1)
					<< "V" << (int)i << "=" << std::setw(2) << (int)regs[i] << " ";
			}
			std::cout << std::dec << "\n";
		}

		advance();

		//execute
		switch (m) {
		case 0:
			// CLS
			if (k == 0xe0) {
				screen->clear();
				redraw = true;
			}
			// RET
			else if (k == 0xee)
				pc = pop();
			else
				state = State::Stopped;
			break;
		case 1:
			// JP addr
			pc = addr;
			break;
		case 2:
			// CALL addr
			push(pc);
			pc = addr;
			break;
		case 3:
			// SE Vx, k
			if (regs[x] == k)
				advance();
			break;
		case 4:
			// SNE Vx, k
			if (regs[x] != k)
				advance();
			break;
		case 5:
			// SE Vx, Vy
			if (z == 0) {
				if (regs[x] == regs[y])
					advance();
			}
			else
				state = State::Stopped;
			break;
		case 6:
			// LD Vx, k
			regs[x] = k;
			break;
		case 7:
			// ADD Vx, k
			regs[x] += k;
			break;
		case 8:
			switch (z) {
			case 0:
				// LD Vx, Vy
				regs[x] = regs[y];
				break;
			case 1:
				// OR Vx, Vy
				regs[x] |= regs[y];
				break;
			case 2:
				// AND Vx, Vy
				regs[x] &= regs[y];
				break;
			case 3:
				// XOR Vx, Vy
				regs[x] ^= regs[y];
				break;
			case 4:
				// ADD Vx, Vy
				setCarry(std::numeric_limits<Byte>::max() - regs[y] < regs[x]);
				regs[x] += regs[y];
				break;
			case 5:
				// SUB Vx, Vy
				setCarry(regs[x] > regs[y]);
				regs[x] -= regs[y];
				break;
			case 6:
				// SHR Vx, Vy
				setCarry((regs[x] & 0b0000'0001) == 0b0000'0001);
				regs[x] >>= 1;
				break;
			case 7:
				// SUBN Vx, Vy
				setCarry(regs[y] > regs[x]);
				regs[x] = regs[y] - regs[x];
				break;
			case 0xe:
				// SHL Vx, Vy
				setCarry((regs[x] & 0b1000'0000) == 0b1000'0000);
				regs[x] <<= 1;
				break;
			default:
				state = State::Stopped;
			}
			break;
		case 9:
			// SNE Vx, Vy
			if (z == 0) {
				if (regs[x] != regs[y])
					advance();
			}
			else
				state = State::Stopped;
			break;
		case 0xa:
			// LD I, addr
			ireg = addr;
			break;
		case 0xb:
			// JP V0, addr
			pc = (addr + regs[0]) & (kMemorySize - 1);
			break;
		case 0xc:
			// RND Vx, k
			regs[x] = std::rand() & k;
			break;
		case 0xd:
			// DRW Vx, Vy, z
			clearCarry();
			for (int row = 0; row < z; ++row) {
				Byte* p = &memory[(ireg + row) & (kMemorySize - 1)];
				for (int col = 0; col < 8; ++col) {
					if ((*p & (0b1000'0000 >> col)) != 0) {
						if (screen->setPixel(regs[x] + col, regs[y] + row) == 0)
							setCarry();
					}
				}
			}
			redraw = true;
			break;
		case 0xe:
			// SKP Vx
			if (k == 0x9e) {
				if (keypad->isPressed(regs[x]))
					advance();
			}
			// SKNP Vx
			else if (k == 0xa1) {
				if (!keypad->isPressed(regs[x]))
					advance();
			}
			else
				state = State::Stopped;
			break;
		case 0xf:
			switch (k) {
			case 0x07:
				// LD Vx, DT
				regs[x] = delayTimer;
				break;
			case 0x0a: {
				// LD VX, KEY
				if (int key = keypad->getPressedKey(); key != Keypad::kNone)
					regs[x] = key;
				else
					reverse();
			}
				break;
			case 0x15:
				// LD DT, Vx
				delayTimer = regs[x];
				break;
			case 0x18:
				// LD ST, Vx
				soundTimer = regs[x];
				break;
			case 0x1e:
				// ADD I, Vx
				ireg = (ireg + regs[x]) & (kMemorySize - 1);
				break;
			case 0x29:
				// LD I, Fx
				ireg = (kFontStart + regs[x] * kFontWidth) & (kMemorySize - 1);
				break;
			case 0x33: {
				// LD B, Vx
				Byte t = regs[x];
				memory[std::size_t(ireg) + 2] = t % 10; t /= 10;
				memory[std::size_t(ireg) + 1] = t % 10; t /= 10;
				memory[std::size_t(ireg) + 0] = t % 10;
			}
				break;
			case 0x55:
				// LD [I], Vx
				for (int i = 0; i <= x; ++i)
					memory[ireg++] = regs[i];
				ireg &= kMemorySize - 1;
				break;
			case 0x65:
				// LD Vx, [I]
				for (int i = 0; i <= x; ++i)
					regs[i] = memory[ireg++];
				ireg &= kMemorySize - 1;
				break;
			}
		}

		return redraw;
	}
}
