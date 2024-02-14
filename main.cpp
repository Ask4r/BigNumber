#include "big_number.h"
#include <iostream>
#include <chrono>

int main() {

    const uint64_t precision = 53 * 64;

    /*const auto start = std::chrono::high_resolution_clock::now();

    BigNumber::BigNumber one(1, precision);
    BigNumber::BigNumber _6k_f(1, precision);
    BigNumber::BigNumber _3k_f(1, precision);
    BigNumber::BigNumber _k_f_cube_f(1, precision);
    BigNumber::BigNumber sum_k(13591409, precision);
    BigNumber::BigNumber k_mul(545140134, precision);
    BigNumber::BigNumber div_k_m = sqrt(pow(BigNumber::BigNumber(640320, precision), 3));
    BigNumber::BigNumber div_k = div_k_m;

    BigNumber::BigNumber _12_inv_pi = sum_k / div_k;

    for (size_t i = 1; i < 5; ++i) {
        for (size_t j = 0; j < 3; ++j)
            _3k_f *= _3k_f + one;
        for (size_t j = 0; j < 6; ++j)
            _6k_f *= _6k_f + one;
        _k_f_cube_f *= i * i * i;
        sum_k += k_mul;
        div_k *= div_k_m;

        if (i % 2 == 0) {
            _12_inv_pi += (_6k_f * sum_k) / (_3k_f * _k_f_cube_f * div_k);
        } else {
            _12_inv_pi -= (_6k_f * sum_k) / (_3k_f * _k_f_cube_f * div_k);
        }
    }

    BigNumber::BigNumber pi = BigNumber::BigNumber(1, precision) / (_12_inv_pi * BigNumber::BigNumber(12, precision));
    std::cout << pi << std::endl;

    const auto end = std::chrono::high_resolution_clock::now();*/


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
