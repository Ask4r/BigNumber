#include "big_number.h"
#include <iostream>
#include <chrono>

int main() {

    const uint64_t precision = 5 * 64; // 115 decimal digits

    BigNumber::BigNumber x("2", precision);

    x = sqrt(x);

    std::cout << x << std::endl;

    /*const auto start = std::chrono::high_resolution_clock::now();

    BigNumber::BigNumber _12_inv_pi(0, precision);

    BigNumber::BigNumber _6k_f(1, precision);
    BigNumber::BigNumber _3k_f(1, precision);
    BigNumber::BigNumber _k_f_cube_f(1, precision);
    BigNumber::BigNumber sum_k(13591409, precision);
    BigNumber::BigNumber k_mul(545140134, precision);
    BigNumber::BigNumber div_k = pow(BigNumber::BigNumber(640320, precision), 3);

    for (size_t i = 1; i < 10; ++i) {
        if (i % 2 == 0) {
            _12_inv_pi += (_6k_f * sum_k) / (_3k_f * _k_f_cube_f * sqrt(pow(div_k, i + 1)));
        } else {
            _12_inv_pi -= (_6k_f * sum_k) / (_3k_f * _k_f_cube_f * sqrt(pow(div_k, i + 1)));
        }
        if (i == 0)
            continue;
        _3k_f *= 3 * i;
        _6k_f *= 6 * i;
        _k_f_cube_f *= i * i * i;
        sum_k += k_mul;
    }


    const auto end = std::chrono::high_resolution_clock::now();*/



    /*const auto start = std::chrono::high_resolution_clock::now();

    BigNumber::BigNumber _1(1ull, precision);
    BigNumber::BigNumber _20(20ull, precision);
    BigNumber::BigNumber _32(32ull, precision);
    BigNumber::BigNumber _48(48ull, precision);
    BigNumber::BigNumber _1_18 = _1 / 18;
    BigNumber::BigNumber _1_57 = _1 / 57;
    BigNumber::BigNumber _1_239 = _1 / 239;

    BigNumber::BigNumber pi = _48 * arctan(_1_18)
                              + _32 * arctan(_1_57)
                              - _20 * arctan(_1_239);

    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << pi << std::endl;
    std::cout << std::endl;
    std::cout << (end - start) / 1'000'000 << std::endl;*/

    return 0;
}
