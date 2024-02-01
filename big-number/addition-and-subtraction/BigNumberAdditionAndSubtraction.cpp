//
// Created by Askar on 1/30/2024.
//

#include "BigNumberAdditionAndSubtraction.h"


void BigNumber::add_positive(const BigNumber& number) {

    BigNumber summand = number;

    summand.mantissa.resize(mantissa.size(), 0);

    // take number with smaller exponent and
    // shift its mantissa to match result's exponent
    if (exponent < summand.exponent) {
        shift_mantissa_vector_right(mantissa, summand.exponent - exponent);
        exponent = summand.exponent;
    } else if (exponent > summand.exponent) {
        shift_mantissa_vector_right(summand.mantissa, exponent - summand.exponent);
    }

    // adding mantissa
    const uint64_t addition_buffer = add_mantissa_vector(mantissa, summand.mantissa);

    // normalize mantissa if buffer is not empty
    if (addition_buffer != 0) {
        ++exponent;
        shift_mantissa_vector_right(mantissa, 1);
        mantissa[0] |= 0x8000'0000'0000'0000;
    }
}

void BigNumber::subtract_positive(const BigNumber& number) {

    BigNumber subtrahend = number;

    subtrahend.mantissa.resize(mantissa.size());

    // take number with smaller exponent and
    // shift its mantissa to match result's exponent
    if (exponent < subtrahend.exponent) {
        shift_mantissa_vector_right(mantissa, subtrahend.exponent - exponent);
        exponent = subtrahend.exponent;
    } else if (exponent > subtrahend.exponent){
        shift_mantissa_vector_right(subtrahend.mantissa, exponent - subtrahend.exponent);
    }

    // subtracting mantissas
    negate_mantissa_vector(subtrahend.mantissa);

    add_mantissa_vector(mantissa, subtrahend.mantissa);
}

BigNumber operator+(const BigNumber& a, const BigNumber& b) {
    if (a.sign == b.sign) {
        BigNumber result = a;
        result.add_positive(b);
        return result;
    } else if (abs(a) > abs(b)) {
        BigNumber result = a;
        result.subtract_positive(b);
        return result;
    } else {
        BigNumber result = b;
        result.subtract_positive(a);
        return result;
    }
}

BigNumber operator-(const BigNumber& a, const BigNumber& b) {
    if (a.sign != b.sign) {
        BigNumber result = a;
        result.add_positive(b);
        return result;
    } else if (abs(a) > abs(b)) {
        BigNumber result = a;
        result.subtract_positive(b);
        return result;
    } else {
        BigNumber result = b;
        result.subtract_positive(a);
        return result;
    }
}

BigNumber& operator+=(BigNumber& self, const BigNumber& other) {
    if (self.sign == other.sign) {
        self.add_positive(other);
        return self;
    } else if (abs(self) > abs(other)) {
        self.subtract_positive(other);
        return self;
    } else {
        BigNumber result = other;
        result.subtract_positive(self);
        self = result;
        return self;
    }
}

BigNumber& operator-=(BigNumber& self, const BigNumber& other) {
    if (self.sign != other.sign) {
        self.add_positive(other);
        return self;
    } else if (abs(self) > abs(other)) {
        self.subtract_positive(other);
        return self;
    } else {
        BigNumber result = other;
        result.subtract_positive(self);
        self = result;
        return self;
    }
}
