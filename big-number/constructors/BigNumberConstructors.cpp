//
// Created by Askar on 1/30/2024.
//
#include "BigNumberConstructors.h"

BigNumber::BigNumber(std::string& s, size_t precision) {
    size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
    mantissa.resize(mantissa_size, 0);

    std::istringstream iss(s);
    if(iss.eof() || iss.fail()) {
        sign = 0;
        exponent = 0;
        return;
    }

    sign = (s[0] == '-');
    exponent = 0;

    const uint64_t period_index = s.find('.');

    BigNumber ten(10.0, precision);
    BigNumber one_tenth(0.0, precision * 2);
    one_tenth.exponent = 0;
    for (size_t i = 0; i < one_tenth.mantissa.size(); ++i) {
        one_tenth.mantissa[i] = 0xCCCC'CCCC'CCCC'CCCC;
    }

    // integral part
    for (size_t i = period_index - 1; isdigit(s[i]) ; --i) {
        BigNumber _(static_cast<uint64_t>(s[i] - '0'), precision);
        for (size_t j = 0; j < period_index - i - 1; ++j)
            _ *= ten;
        *this += _;
    }

    // fraction part
    for (size_t i = period_index + 1; isdigit(s[i]) ; ++i) {
        BigNumber _(static_cast<uint64_t>(s[i] - '0'), precision);
        for (size_t j = 0; j < i - period_index; ++j)
            _ *= one_tenth;
        *this += _;
    }
}

BigNumber::BigNumber(double number, uint64_t precision) {
    const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
    mantissa.resize(mantissa_size, 0);

    sign = (number < 0);
    // fucking double
    exponent = (((*(int64_t *)&number) & 0x7FF0'0000'0000'0000) >> 52) - 1023 + 1;
    mantissa[0] = (((*(uint64_t *)&number) & 0x000F'FFFF'FFFF'FFFF) + 0x0010'0000'0000'0000) << 11;

    // 32-bit implementation
    // mantissa[0] = ((uint32_t)(((*(uint64_t *)&number) & 0x000F'FFFF'FFE0'0000) >> 21))
    //               + (1 << (MANTISSA_CHUNK_SIZE - 1));
    // mantissa[1] = ((uint32_t)((*(uint64_t *)&number) & 0x0000'0000'001F'FFFF) << 11);
}

BigNumber::BigNumber(int64_t number, size_t precision) {
    const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
    mantissa.resize(mantissa_size, 0);

    sign = (number < 0);
    exponent = 0;
    mantissa[0] = number;

    // 32-bit implementation
    // mantissa[0] = number & 0xFFFFFFFF;
    // mantissa[1] = (number & (0xFFFFFFFFull << MANTISSA_CHUNK_SIZE)) >> MANTISSA_CHUNK_SIZE;

    // TODO
}

BigNumber::BigNumber(uint64_t number, size_t precision) {
    const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
    mantissa.resize(mantissa_size, 0);

    sign = 0;
    exponent = get_number_exponent(number);
    mantissa[0] = number << (MANTISSA_CHUNK_SIZE - exponent);
}

BigNumber::BigNumber(const BigNumber&number) {
    sign = -number.sign;
    exponent = number.exponent;
    mantissa = number.mantissa;
}