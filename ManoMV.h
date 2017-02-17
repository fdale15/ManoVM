#include <iostream>
#include <memory>
#include <vector>
#include <bitset>
#include <unordered_map>

class ManoVM {
public:
    static short MEMORY_SIZE;

    ManoVM(short programCounter = 0) : m_programCounter{programCounter} {
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
            //Logic to get the
            //Memory referencing instruction goes here.
        }
    }

    void printFrame() {
        printf("PC:%d\tAR:%d\tIR:%d\tAC:%d\n", m_programCounter, m_addressRegister, m_instructionRegister, m_accumulator);
    }

    //This kinda takes the place of the ALU.
    //TODO: Maybe actually implement an ALU.
    //Tentative maybe. Probably won't need it but it would be a more OO approach.
    std::unordered_map<short, std::function<void()>> registerInstructionMap = {
            {2048, [&](){
                m_accumulator = 0;
            }},//Clear AC
            //Clear E
            //Compliment AC
            //Complement E
            //Circulate right AC/E
            //Circulate left AC/E
            //Increment AC
            //Skip next instruction if AC positive
            //Skip next instruction if AC negative
            //Skip next instruction if AC zero
            //Skip next instruction if E is 0
            //Halt computer
    };

    short* m_memory{nullptr};

    short m_programCounter{0};
    short m_addressRegister{0};
    short m_instructionRegister{0};
    short m_dataRegister{0};
    short m_tempRegister{0};
    short m_outputRegister{0};
    short m_inputRegister{0};

    short m_accumulator{0};
};
short ManoVM::MEMORY_SIZE = 4096;