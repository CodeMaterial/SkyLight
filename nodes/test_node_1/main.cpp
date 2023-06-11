#include <iostream>
#include <lcm/lcm-cpp.hpp>

int main() {

    lcm::LCM lcm;

    if (!lcm.good())
        return 1;

    std::cout << "LCM is running in Test Node 1" << std::endl;

    std::string s;
    std::cin >> s;
    return 0;
}
