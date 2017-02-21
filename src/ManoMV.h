#include <iostream>
#include <memory>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <functional>

class ManoVM {
public:
	static short MEMORY_SIZE;

	ManoVM(short programCounter = 0) : m_programCounter{ programCounter } {
		m_memory = new short[MEMORY_SIZE];
		m_accumulator = 1002;
		m_memory[1] = static_cast<short>(30720);
	}
	~ManoVM() {
		delete[] m_memory;
	}

	void startVm() {
		while (true) {
			singleStepVm();
		}
	}
	void singleStepVm() {
		printFrame();
		loadAddressRegister(m_programCounter);
		printFrame();
		loadInstructionRegister(m_memory[m_addressRegister]);
		printFrame();
		auto instruction = decodeInstruction(m_instructionRegister);
		printFrame();
		instruction();
		printFrame();
	}

private:
	//Loads an address into the address register.
	void loadAddressRegister(short address) {
		m_addressRegister = address;
	}
	//Loads an instruction into the instruction register.
	void loadInstructionRegister(short instruction) {
		m_programCounter++;
		m_instructionRegister = instruction;
	}
	//Decodes an instruction and returns the executable instruction.
	std::function<void()> decodeInstruction(short instruction) {
		auto bits = std::bitset<16>(instruction);
		bool indirectionBit = bits[15];
		int opcode = (bits[12]) + (bits[13] * 2) + (bits[14] * (2 << 1));

		bits[15] = 0;
		bits[14] = 0;
		bits[13] = 0;
		bits[12] = 0;

		loadAddressRegister(static_cast<short>(bits.to_ulong()));

		if (opcode == 7) {
			return registerInstructionMap[m_addressRegister];
		}
		else {
			// if indirectionBit = 0, direct, else, indirect
			if (indirectionBit)
				m_addressRegister = m_memory[m_addressRegister];

			//Logic to get the memory referencing instruction goes here
			return memoryReferencingInstructionMap[opcode];
		}
	}

	void printFrame() {
		printf("PC:%X\tAR:%X\tIR:%X\tAC:%X\n", m_programCounter, m_addressRegister, m_instructionRegister, m_accumulator);
	}

	//Memory
	short* m_memory{ nullptr };
	//Registers
	short m_programCounter{ 0 };
	short m_addressRegister{ 0 };
	short m_instructionRegister{ 0 };
	short m_dataRegister{ 0 };
	short m_tempRegister{ 0 };
	short m_outputRegister{ 0 };
	short m_inputRegister{ 0 };
	short m_accumulator{ 0 };
	short m_e{ 0 };

	// incriment bitset<16>
	std::bitset<16> incrementBitset(std::bitset<16> bits) {
		std::bitset<16> temp = bits;

		for (int i = 0; i < 16; i++) {
			if (temp[i] == 0) {
				temp[i] = 1;
				i = 16;
			}
			else
				temp[i] = 0;
		}

		return temp;
	}

	// memory referencing instructions here
	std::unordered_map<short, std::function<void()>> memoryReferencingInstructionMap = {
		// AND
		{ 0 | 8, [&]() {
		m_dataRegister = m_memory[m_addressRegister];
		m_accumulator = m_accumulator & m_dataRegister;
	} },
		// ADD
	{ 1 | 9, [&]() {
		m_dataRegister = m_memory[m_addressRegister];
		auto accBits = std::bitset<16>(m_accumulator);
		auto dataBits = std::bitset<16>(m_dataRegister);
		std::bitset<16> sum;
		short carry = 0;
		for (int i = 0; i < 16; i++) {
			auto temp = accBits[i] + dataBits[i] + carry;
			if (temp == 0)
				sum[i] = 0;
			else if (temp == 1) {
				sum[i] == 1;
				carry = 0;
			}
			else {
				if (temp == 2)
					sum[i] == 0;
				else
					sum[i] == 1;
				carry = 1;
			}
		}
		m_accumulator = sum.to_ulong();
	} },
		// LDA
	{ 2 | 0xA, [&]() {
		m_dataRegister = m_memory[m_addressRegister];
		m_accumulator = m_dataRegister;
	} },
		// STA
	{ 3 | 0xB, [&]() {
		m_memory[m_addressRegister] = m_accumulator;
	} },
		// BUN
	{ 4 | 0xC, [&]() {
		m_programCounter = m_addressRegister;
	} },
		// BSA
	{ 5 | 0xD, [&]() {
		m_memory[m_addressRegister] = m_programCounter;
		m_addressRegister++;
	} },
		// ISZ
	{ 6 | 0xE, [&]() {
		m_dataRegister = m_memory[m_addressRegister];
		m_dataRegister++;
		m_memory[m_addressRegister] = m_dataRegister;
		if (m_dataRegister == 0)
			m_programCounter++;
	} }
	};

	//This kinda takes the place of the ALU.
	//TODO: Maybe actually implement an ALU.
	//Tentative maybe. Probably won't need it but it would be a more OO approach.
	std::unordered_map<short, std::function<void()>> registerInstructionMap = {
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
		if (std::bitset<16>(m_accumulator)[15] == 0)
			m_programCounter++;
	} },
		//Skip next instruction if AC negative
	{ 0x008, [&]() {
		if (std::bitset<16>(m_accumulator)[15] == 1)
			m_programCounter++;
	} },
		//Skip next instruction if AC zero
	{ 0x004, [&]() {
		if (m_accumulator == 0)
			m_programCounter++;
	} },
		//Skip next instruction if E is 0
	{ 0x002, [&]() {
		if (m_e == 0)
			m_programCounter++;
	} },
		//Halt computer
	{ 0x001, [&]() {
		std::cout << "Halt computer..." << std::endl;
	} }
	};
};
short ManoVM::MEMORY_SIZE = 4096;