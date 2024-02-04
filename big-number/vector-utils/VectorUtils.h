#pragma once

#include "../BigNumber.h"

#include <vector>
#include <cstdint>

namespace BigNumber::VectorUtils {
    uint64_t add_mantissa(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    uint64_t normalise_mantissa(std::vector<uint64_t>&);

    void shift_mantissa_right(std::vector<uint64_t>&, uint64_t);
    void shift_mantissa_left(std::vector<uint64_t>&, uint64_t);
   std::strong_ordering compare_vectors(const std::vector<uint64_t>&, const std::vector<uint64_t>&);
    void add_vector(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    std::vector<uint64_t> subtract_vectors(const std::vector<uint64_t>&, const std::vector<uint64_t>&);
    std::vector<uint64_t> multiply_vectors(const std::vector<uint64_t>&, const std::vector<uint64_t>&);
    std::vector<uint64_t> modulo_vectors(std::vector<uint64_t>&, const std::vector<uint64_t>&);
}
