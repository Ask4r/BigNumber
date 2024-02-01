#include <iostream>

#include "big-number/BigNumber.h"


int main() {
   /* std::string _a("114.5");
    std::string _b("6.70");

    BigNumber a(_a, 64);
    BigNumber b(_b, 64);*/

    // a *= b;
    // std::cout << a << std::endl;

   BigNumber x(122.0, 64);
   BigNumber y(11.0, 64);

   x /= y;

    std::cout << x << std::endl;


    return 0;
}
