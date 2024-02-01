//
// Created by Askar on 1/30/2024.
//

#include "BigNumberPolymorphisms.h"


std::string BigNumber::to_string() const {

    std::string result;

    if (is_negative())
        result.append("-");

    if (exponent > 0)
        result.append(get_integer_part());
    else
        result.append("0");


    result.append(".");

    result.append(get_fraction_part());

    return result;
}

std::string BigNumber::get_integer_part() const {

    std::string integer_part;

    std::vector<MANTISSA_CHUNK_TYPE_T> division_mantissa;
    uint64_t division_mantissa_size = exponent / MANTISSA_CHUNK_SIZE + (exponent % MANTISSA_CHUNK_SIZE > 0);
    division_mantissa.resize(division_mantissa_size, 0);
    const uint64_t number_mantissa_shift = MANTISSA_CHUNK_SIZE - exponent % MANTISSA_CHUNK_SIZE;
    const uint64_t more_significant_bit_mask = (1 << number_mantissa_shift) - 1;

    // iter for mantissa's items
    for (uint64_t i = 0; i < division_mantissa_size; ++i) {
        division_mantissa[i] = mantissa[division_mantissa_size - 1 - i] >> number_mantissa_shift;

        if (i < division_mantissa_size - 1) {
            division_mantissa[i] += (mantissa[division_mantissa_size - i - 2]
                                     & more_significant_bit_mask) << number_mantissa_shift;
        }
    }

    // divide integer mantissa part by 10 for decimal form
    std::vector<MANTISSA_CHUNK_TYPE_T> next_division_mantissa = { 0 };
    uint64_t division_buffer = 0;
    uint64_t shift_buffer = 0;
    uint64_t current_division_bit_index;
    uint64_t mantissa_iterations = (division_mantissa.size() - 1) * MANTISSA_CHUNK_SIZE
                                   + BigNumber::get_number_exponent(division_mantissa[0]);
    bool is_division_complete;

    while (true) {
        // divide and find current digit
        for (uint64_t i = 0; i < mantissa_iterations; ++i) {
            // shift buffer and append next mantissa's bit
            division_buffer <<= 1;
            current_division_bit_index = (mantissa_iterations - i) % MANTISSA_CHUNK_SIZE - 1;
            division_buffer += (division_mantissa[i / MANTISSA_CHUNK_SIZE]
                                & (1 << current_division_bit_index)) >> current_division_bit_index;

            // shift next_division_mantissa
            for (MANTISSA_CHUNK_TYPE_T& mantissa_item : next_division_mantissa) {
                // if shift doesn't overflow
                if (mantissa_item < (1 << (MANTISSA_CHUNK_SIZE - 1))) {
                    mantissa_item <<= 1;
                    mantissa_item += shift_buffer;
                    shift_buffer = 0;
                    continue;
                }
                mantissa_item <<= 1;
                mantissa_item += shift_buffer;
                shift_buffer = 1;
            }
            if (shift_buffer != 0) {
                next_division_mantissa.push_back(1);
                shift_buffer = 0;
            }

            // if division occurs
            if (division_buffer >= 10) {
                division_buffer -= 10;
                ++next_division_mantissa[0];
            }
        }

        integer_part.append(std::to_string(division_buffer));

        // check if division is complete
        is_division_complete = true;
        if (next_division_mantissa[0] >= 10)
            is_division_complete = false;

        for (size_t i = 1; i < next_division_mantissa.size(); ++i) {
            if (next_division_mantissa[i] != 0) {
                is_division_complete = false;
                break;
            }
        }

        if (is_division_complete) {
            if (next_division_mantissa[0] != 0)
                integer_part.append(std::to_string(next_division_mantissa[0]));
            break;
        }

        // next division iteration
        division_mantissa = next_division_mantissa;
        mantissa_iterations = (division_mantissa.size() - 1) * MANTISSA_CHUNK_SIZE
                              + BigNumber::get_number_exponent(division_mantissa[0]);;
        next_division_mantissa.resize(1);
        next_division_mantissa[0] = 0;
        division_buffer = 0;
    }

    // reverse result
    std::reverse(integer_part.begin(), integer_part.end());

    return integer_part;
}

std::string BigNumber::get_fraction_part() const {

    std::string result;

    std::vector<MANTISSA_CHUNK_TYPE_T> mantissa_buffer = mantissa;

    // remove integer part of mantissa
    if (exponent > 0) {
        shift_mantissa_vector_left(mantissa_buffer, exponent);
    } else if (exponent < 0) {
        shift_mantissa_vector_right(mantissa_buffer, -exponent);
    }

    std::vector<MANTISSA_CHUNK_TYPE_T> mantissa_summand = mantissa_buffer;
    if ((mantissa_summand.back() & 0x3) > 0) {
        mantissa_buffer.push_back(0);
        mantissa_summand.push_back(0);
    }

    const uint64_t digit_bit_mask = 0xE000'0000'0000'0000;
    const uint64_t bit_removal_mask = 0x1FFF'FFFF'FFFF'FFFF;
    shift_mantissa_vector_right(mantissa_summand, 2);
    // remove 2 bits filled by shift
    mantissa_summand[0] &= 0x3FFF'FFFF'FFFF'FFFF;

    uint64_t addition_buffer;

    for (uint64_t i = 0; i < mantissa_buffer.size() * MANTISSA_CHUNK_SIZE; ++i) {
        addition_buffer = add_mantissa_vector(mantissa_buffer, mantissa_summand);

        result.append(std::to_string(
            ((mantissa_buffer[0] & digit_bit_mask) >> (MANTISSA_CHUNK_SIZE - 3))
            + (addition_buffer << 3)
        ));
        mantissa_buffer[0] &= bit_removal_mask;
        mantissa_summand = mantissa_buffer;
        shift_mantissa_vector_left(mantissa_buffer, 3);
        shift_mantissa_vector_left(mantissa_summand, 1);

    }

    return result;
}