//
// Created by Askar on 1/30/2024.
//

#include "BigNumberRepresentation.h"


std::ostream& operator<<(std::ostream& outs, const BigNumber& number) {

    outs << number.to_string();

    return outs;
}