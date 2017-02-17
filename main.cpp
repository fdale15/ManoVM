#include "ManoMV.h"

class ManoVM;

int main() {
    //Creates the virtual machine with PC set a 1.
    auto vm = ManoVM(1);
    //Executes one instruction.
    vm.singleStepVm();

    return 0;
}