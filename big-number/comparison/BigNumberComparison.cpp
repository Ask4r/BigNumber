//
// Created by Askar on 1/31/2024.
//

#include "BigNumberComparison.h"


int64_t compare(const BigNumber& a, const BigNumber& b) {
    bool are_positive;

    // a - negative, b - positive
    if (a.sign > 0 && b.sign == 0)
        return -1;
    // a - positive, b - negative
    if (a.sign == 0 && b.sign > 0)
        return 1;

    if (a.sign == 0 && b.sign == 0)
        are_positive = true;
    else
        are_positive = false;

    if (a.exponent > b.exponent)
        return are_positive ? 1 : -1;
    for (size_t i = 0; i < std::min(a.mantissa.size(), b.mantissa.size()); ++i) {
        if (a.mantissa[i] > b.mantissa[i])
            return are_positive ? 1 : -1;
    }
    if ((a.mantissa.size() > b.mantissa.size() && a.mantissa.back() != 0)
        || (a.mantissa.size() < b.mantissa.size() && b.mantissa.back() != 0))
        return are_positive ? 1 : -1;

    return 0;
}

bool operator<(const BigNumber& a, const BigNumber& b) {
    return compare(a, b) < 0;
}

bool operator>(const BigNumber& a, const BigNumber& b) {
    return compare(a, b) > 0;
}

bool operator==(const BigNumber& a, const BigNumber& b) {
    return compare(a, b) == 0;
}

bool operator!=(const BigNumber& a, const BigNumber& b) {
    return compare(a, b) != 0;
}

bool operator>=(const BigNumber& a, const BigNumber& b) {
    return compare(a, b) >= 0;
}

bool operator<=(const BigNumber& a, const BigNumber& b) {
    return compare(a, b) <= 0;
}