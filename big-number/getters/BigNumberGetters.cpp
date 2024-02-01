//
// Created by Askar on 1/30/2024.
//

#include "BigNumberGetters.h"

bool BigNumber::is_positive() const {
    return sign == 0;
}

bool BigNumber::is_negative() const {
    return sign != 0;
}