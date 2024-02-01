//
// Created by Askar on 1/30/2024.
//

#include "BigNumberMultiplicationAndDivision.h"

BigNumber& operator*=(BigNumber& self, const BigNumber& other) {

    std::vector<uint64_t> self_copy = self.mantissa;
    std::vector<uint64_t> other_copy = other.mantissa;

    if (self_copy.size() > other_copy.size()) {
        other_copy.resize(self_copy.size(), 0);
    } else if (self_copy.size() < other_copy.size()) {
        self_copy.resize(other_copy.size(), 0);
    }

    const int64_t self_exponent = self.exponent - (self_copy.size() << 6);
    const int64_t other_exponent = other.exponent - (other_copy.size() << 6);

    std::reverse(self_copy.begin(), self_copy.end());
    std::reverse(other_copy.begin(), other_copy.end());

    // Fast Fourier Transform multiplication
    BigNumber::multiply_fft(self_copy, other_copy);

    // TODO: fuck bitwise multiplication
    // BigNumber::multiply_vector(self_copy, other_copy);

    std::reverse(self_copy.begin(), self_copy.end());

    // normalise mantissa
    uint64_t normalisation_shift = 0;
    bool is_normalised = false;

    for (uint64_t chunk : self_copy) {
        if (is_normalised)
            break;
        for (size_t i = 0; i < 64; ++i) {
            if ((chunk & (1ull << (63 - i))) != 0) {
                BigNumber::shift_mantissa_vector_left(self_copy, normalisation_shift);
                is_normalised = true;
                break;
            }
            ++normalisation_shift;
        }
    }

    self.sign = (self.sign != other.sign);
    self.exponent = self_exponent + other_exponent + (self_copy.size() << 6) - normalisation_shift;
    self.mantissa = self_copy;

    return self;
};

BigNumber& operator/=(BigNumber& self, const BigNumber& other) {

    BigNumber others_inverse = other;
    BigNumber::inverse_of_vector(others_inverse.mantissa);
    others_inverse.exponent = -others_inverse.exponent + 1;

    self *= others_inverse;

    return self;
};

BigNumber operator*(const BigNumber& a, const BigNumber& b) {
    BigNumber result = a;
    result *= b;
    return result;
};

BigNumber operator/(const BigNumber& a, const BigNumber& b) {
    BigNumber result = a;
    result /= b;
    return result;
};