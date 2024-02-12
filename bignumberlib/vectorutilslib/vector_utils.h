#pragma once

#include "../big_number.h"

#include <vector>
#include <cstdint>
#include <string>

namespace BigNumber::VectorUtils {
    void extend(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    bool is_null(const std::vector<uint64_t>&);
    void shift_left(std::vector<uint64_t>&, uint64_t);
    void shift_right(std::vector<uint64_t>&, uint64_t);
    uint64_t normalise_mantissa(std::vector<uint64_t>&, uint64_t);
    void align_fraction_mantissa(std::vector<uint64_t>& self);
    std::strong_ordering compare_vectors(const std::vector<uint64_t>&, const std::vector<uint64_t>&);
    uint64_t add_vector(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    uint64_t add_number(std::vector<uint64_t>&, uint64_t);
    uint64_t subtract_vector(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    std::vector<uint64_t> multiply_vectors(const std::vector<uint64_t>&, const std::vector<uint64_t>&);
    uint64_t modulo_vector(std::vector<uint64_t>&, uint64_t);
    std::vector<uint64_t> modulo_vector(std::vector<uint64_t>&, std::vector<uint64_t>);
    std::vector<uint64_t> to_integer_vector(std::string);
    std::vector<uint64_t> to_fraction_vector(std::string, uint64_t);
}
