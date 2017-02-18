#include "ManoMV.h"

class ManoVM;

int main() {
    //Creates the virtual machine with PC set a 1.
    auto vm = ManoVM(1);
    //Executes one instruction.
    vm.singleStepVm();

	std::cout << "Press any key to continue..." << std::endl;
	std::string i;
	std::getline(std::cin, i);

    return 0;
}