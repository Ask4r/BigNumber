#include "big_number.h"
#include <iostream>
#include <chrono>

int main() {
    // 115 decimal fraction digits
    const uint64_t precision = 53 * 64;

    const auto start = std::chrono::high_resolution_clock::now();

    BigNumber::BigNumber _1(1, precision);
    BigNumber::BigNumber _20(20, precision);
    BigNumber::BigNumber _32(32, precision);
    BigNumber::BigNumber _48(48, precision);
    BigNumber::BigNumber _1_18 = _1 / 18;
    BigNumber::BigNumber _1_57 = _1 / 57;
    BigNumber::BigNumber _1_239 = _1 / 239;

    BigNumber::BigNumber pi = _48 * arctan(_1_18)
                              + _32 * arctan(_1_57)
                              - _20 * arctan(_1_239);

    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << pi << std::endl;
    std::cout << std::endl;
    std::cout << (end - start) / 1'000'000 << std::endl;

    return 0;
}
