#include <iostream>
#include <memory>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <functional>
#include <cmath>

class ManoVM {
public:
	static unsigned short MEMORY_SIZE;

	ManoVM(unsigned short programCounter = 0) : m_programCounter{ programCounter } {
		m_memory = new unsigned short[MEMORY_SIZE];
		initMemory();

		m_programCounter = std::rand() % MEMORY_SIZE;
	}
	~ManoVM() {
		delete[] m_memory;
	}

	void startVm() {
		int i = 0;
		while (true) {
			singleStepVm();
			std::cout << "Frame " << i << std::endl;
			i++;
		}
	}
	void singleStepVm() {
		std::cout << "S0:\t";
		printFrame();
		loadAddressRegister(m_programCounter);
		std::cout << "T0:\t";
		printFrame();
		loadInstructionRegister(m_memory[m_addressRegister]);
		std::cout << "T1:\t";
		printFrame();
		auto instruction = decodeInstruction(m_instructionRegister);
		std::cout << "T2:\t";
		printFrame();
		instruction();
		std::cout << "T3:\t";
		printFrame();
	}

private:
	//Inits memory with randomly generated values.
	void initMemory() {
		for (int i = 0; i < MEMORY_SIZE; i++) {
			int randInstruction = std::rand() % 15;
			int contents = 0;
			if (randInstruction == 7) {
				contents = registerInstructionCodes[std::rand() % 12];
			} else {
				contents =  + std::rand() % MEMORY_SIZE;
			}
			auto val = (randInstruction * std::pow(16, 3)) + contents;
			m_memory[i] = val;
		}
	}
	//Loads an address into the address register.
	void loadAddressRegister(unsigned short address) {
		m_addressRegister = address;
	}
	//Loads an instruction into the instruction register.
	void loadInstructionRegister(unsigned short instruction) {
		m_programCounter++;
		m_instructionRegister = instruction;
	}
	//Decodes an instruction and returns the executable instruction.
	std::function<void()> decodeInstruction(unsigned short instruction) {
		auto bits = std::bitset<16>(instruction);
		bool indirectionBit = bits[15];
		int opcode = (bits[12]) + (bits[13] * 2) + (bits[14] * (2 << 1));

		bits[15] = 0;
		bits[14] = 0;
		bits[13] = 0;
		bits[12] = 0;

		loadAddressRegister(static_cast<unsigned short>(bits.to_ulong()));

		if (opcode == 7) {
			return registerInstructionMap[m_addressRegister];
		}
		else {
			// if indirectionBit = 0, direct, else, indirect
			if (indirectionBit) {
				m_addressRegister = m_memory[m_addressRegister];
			}

			//Logic to get the memory referencing instruction goes here
			return memoryReferencingInstructionMap[opcode];
		}
	}
	void printFrame() {
		printf("PC:%X\t\tAR:%X\t\tM[AR]:%X\t\t\tDR:%X\t\tIR:%X\t\tAC:%X\t\tE:%X\n", m_programCounter, m_addressRegister, m_memory[m_addressRegister], m_dataRegister, m_instructionRegister, m_accumulator, m_e);
	}

	//Memory
	unsigned short* m_memory{ nullptr };
	//Registers
	unsigned short m_programCounter{ 0 };
	unsigned short m_addressRegister{ 0 };
	unsigned short m_instructionRegister{ 0 };
	unsigned short m_dataRegister{ 0 };
	unsigned short m_tempRegister{ 0 };
	unsigned short m_outputRegister{ 0 };
	unsigned short m_inputRegister{ 0 };
	unsigned short m_accumulator{ 0 };
	unsigned short m_e{ 0 };

	std::vector<unsigned short> registerInstructionCodes = {
			0x800,
			0x400,
			0x200,
			0x100,
			0x080,
			0x040,
			0x020,
			0x010,
			0x008,
			0x004,
			0x002,
			0x001
	};

	// memory referencing instructions here
	std::unordered_map<unsigned short, std::function<void()>> memoryReferencingInstructionMap = {
		// AND
		{ 0, [&]() {
			m_dataRegister = m_memory[m_addressRegister];
			m_accumulator = m_accumulator & m_dataRegister;
		}},
		// ADD
		{ 1, [&]() {
			m_dataRegister = m_memory[m_addressRegister];
			m_accumulator += m_dataRegister;
		} },
		// LDA
		{ 2, [&]() {
			m_dataRegister = m_memory[m_addressRegister];
			m_accumulator = m_dataRegister;
		} },
			// STA
		{ 3, [&]() {
			m_memory[m_addressRegister] = m_accumulator;
		} },
			// BUN
		{ 4, [&]() {
			m_dataRegister = m_memory[m_addressRegister];
			m_programCounter = m_dataRegister;
		} },
			// BSA
		{ 5, [&]() {
			m_dataRegister = m_memory[m_addressRegister];
			m_memory[m_addressRegister] = m_programCounter;
			m_programCounter = m_dataRegister;
		} },
			// ISZ
		{ 6, [&]() {
			m_dataRegister = m_memory[m_addressRegister];
			m_dataRegister++;
			m_memory[m_addressRegister] = m_dataRegister;
			if (m_dataRegister == 0) {
				m_programCounter++;
			}
		} }
	};

	//This kinda takes the place of the ALU.
	std::unordered_map<unsigned short, std::function<void()>> registerInstructionMap = {
			//Clear AC
		{ 0x800, [&]() {
			m_accumulator = 0;
		} },
			//Clear E
		{ 0x400, [&]() {
			m_e = 0;
		} },
			//Compliment AC
		{ 0x200, [&]() {
			// flip bits
			m_accumulator = ~m_accumulator + 1;
		} },
			//Complement E
		{ 0x100, [&]() {
			m_e = ~m_e + 1;
		} },
			//Circulate right AC/E
		{ 0x080, [&]() {
			auto bits = std::bitset<16>(m_accumulator);
			m_e = bits[0];
			bits = bits >> 1;
			bits[15] = m_e;
			m_accumulator = bits.to_ulong();
		} },
			//Circulate left AC/E
		{ 0x040, [&]() {
			auto bits = std::bitset<16>(m_accumulator);
			m_e = bits[15];
			bits = bits << 1;
			bits[0] = m_e;
			m_accumulator = bits.to_ulong();
		} },
			//Increment AC
		{ 0x020, [&]() {
			m_accumulator++;
		} },
			//Skip next instruction if AC positive
		{ 0x010, [&]() {
			if (m_accumulator > 0) {
				m_programCounter++;
			}
		} },
			//Skip next instruction if AC negative
		{ 0x008, [&]() {
			if (m_accumulator < 0) {
				m_programCounter++;
			}
		} },
			//Skip next instruction if AC zero
		{ 0x004, [&]() {
			if (m_accumulator == 0) {
				m_programCounter++;
			}
		} },
			//Skip next instruction if E is 0
		{ 0x002, [&]() {
			if (m_e == 0) {
				m_programCounter++;
			}
		} },
			//Halt computer
		{ 0x001, [&]() {
			std::cout << "Halt computer..." << std::endl;
		} }
	};
};
unsigned short ManoVM::MEMORY_SIZE = 4096;