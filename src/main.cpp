#include "ManoMV.h"

class ManoVM;

int main() {
    std::srand(time(NULL));
    auto vm = ManoVM();
    //Executes five instructions.
    vm.singleStepVm();
    std::cout << "------------------------------------------------------" << std::endl;
    vm.singleStepVm();
    std::cout << "------------------------------------------------------" << std::endl;
    vm.singleStepVm();
    std::cout << "------------------------------------------------------" << std::endl;
    vm.singleStepVm();
    std::cout << "------------------------------------------------------" << std::endl;
    vm.singleStepVm();
    std::cout << "------------------------------------------------------" << std::endl;

    //vm.startVm();
    return 0;
}