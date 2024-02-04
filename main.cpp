#include "big-number/BigNumber.h"

#include <iostream>


int main() {

    uint64_t precision = 64;
    // uint64_t precision = 384; // 115 decimal digits

    /*BigNumber::BigNumber x(1.0, precision);
    BigNumber::BigNumber y(3.0, precision);

    x /= y;

    std::cout << x << std::endl;*/

    BigNumber::BigNumber pi(precision);
    BigNumber::BigNumber upper_limit(5.0, precision);
    BigNumber::BigNumber inverse_16(0.0625, precision);
    BigNumber::BigNumber _8(8.0, precision);
    BigNumber::BigNumber _6(6.0, precision);
    BigNumber::BigNumber _5(5.0, precision);
    BigNumber::BigNumber _4(4.0, precision);
    BigNumber::BigNumber _2(2.0, precision);
    BigNumber::BigNumber _1(1.0, precision);

    pi = _4 - _2 / _4 - _1 / _5 - _1 / _6;

    BigNumber::BigNumber k(precision);
    for (k = BigNumber::BigNumber(1.0, precision); k < upper_limit; k += _1) {
        pi += inverse_16 * (_4 / (_8 * k + _1)
                            - _2 / (_8 * k + _4)
                            - _1 / (_8 * k + _5)
                            - _1 / (_8 * k + _6));
        std::cout << (_4 / (_8 * k + _1)
                      - _2 / (_8 * k + _4)
                      - _1 / (_8 * k + _5)
                      - _1 / (_8 * k + _6)) << std::endl;
        std::cout << (k < upper_limit) << std::endl;
        std::cout << std::endl;
        inverse_16 *= inverse_16;
    }

    std::cout << pi << std::endl;

    return 0;
}
