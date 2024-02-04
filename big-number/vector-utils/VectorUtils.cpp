#include "VectorUtils.h"

namespace BigNumber::VectorUtils {

    // Mantissa utils

    uint64_t add_mantissa(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
        uint64_t carry = 0;
        const uint64_t chunk_max = std::numeric_limits<uint64_t>::max();
        for (int64_t i = self.size() - 1; i >= 0; --i) {
            if (self[i] > (chunk_max - other[i]) || self[i] + other[i] > (chunk_max - carry)) {
                self[i] += other[i];
                self[i] += carry;
                carry = 1;
            } else {
                self[i] += other[i] + carry;
                carry = 0;
            }
        }
        return carry;
    }

    uint64_t normalise_mantissa(std::vector<uint64_t>& self) {
        uint64_t bit_shift = 0;
        bool is_normalised = false;
        for (uint64_t chunk : self) {
            if (is_normalised)
                break;
            if (chunk == 0) {
                bit_shift += 64;
                continue;
            }
            for (size_t i = 0; i < 64; ++i) {
                if ((chunk & (1ull << (63 - i))) != 0) {
                    VectorUtils::shift_mantissa_left(self, bit_shift);
                    is_normalised = true;
                    break;
                }
                ++bit_shift;
            }
        }
        return bit_shift;
    }

    void shift_mantissa_right(std::vector<uint64_t>& self, uint64_t shift_step) {
        if (shift_step >= 64) {
            std::shift_right(self.begin(), self.end(), shift_step / 64);
            shift_step %= 64;
        }
        uint64_t carry;
        const uint64_t shift_mask = (1 << (shift_step + 1)) - 1;
        for (size_t i = 0; i < self.size(); ++i) {
            carry = (i != self.size() - 1) ? (self[i + 1] & shift_mask) : 0;
            self[i] >>= shift_step;
            // remove bits filled with 1
            if ((self[i] & 0x8000'0000'0000'0000) > 0)
                self[i] += (1 << (64 - shift_step));
            self[i] += carry << (64 - shift_step);
        }
    }

    void shift_mantissa_left(std::vector<uint64_t>& self, uint64_t shift_step) {
        if (shift_step >= 64) {
            std::shift_left(self.begin(), self.end(), shift_step / 64);
            for (size_t i = self.size() - shift_step / 64; i < self.size(); ++i)
                self[i] = 0;
            shift_step %= 64;
        }
        uint64_t carry = 0;
        uint64_t next_shift_carry;
        const uint64_t shift_mask = 0xFFFF'FFFF'FFFF'FFFF << (64 - shift_step);
        for (int64_t i = self.size() - 1; i >= 0; --i) {
            next_shift_carry = (self[i] & shift_mask) >> (64 - shift_step);
            self[i] <<= shift_step;
            self[i] += carry;
            carry = next_shift_carry;
        }
    }


    // Vector utils

    std::strong_ordering compare_vectors(const std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
        for (int64_t i = self.size() - 1; i >= 0; --i) {
            if (self[i] > other[i])
                return std::strong_ordering::greater;
            if (self[i] < other[i])
                return std::strong_ordering::less;
        }
        return std::strong_ordering::equal;
    }

    void shift_vector_left(std::vector<uint64_t>& self, uint64_t shift_step) {
        if (shift_step >= 64) {
            std::shift_right(self.begin(), self.end(), shift_step / 64);
            shift_step %= 64;
        }
        uint64_t carry = 0;
        uint64_t next_shift_carry;
        const uint64_t shift_mask = 0xFFFF'FFFF'FFFF'FFFF << (64 - shift_step);
        for (uint64_t& chunk : self) {
            next_shift_carry = (chunk & shift_mask) >> (64 - shift_step);
            chunk <<= shift_step;
            chunk += carry;
            carry = next_shift_carry;
        }
    }

    void add_vector(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
        uint64_t carry = 0;
        const uint64_t chunk_max = std::numeric_limits<uint64_t>::max();
        for (size_t i = 0; i < self.size(); ++i) {
            if (self[i] > (chunk_max - other[i]) || self[i] + other[i] > (chunk_max - carry)) {
                self[i] += other[i];
                self[i] += carry;
                carry = 1;
            } else {
                self[i] += other[i] + carry;
                carry = 0;
            }
        }
    }

    std::vector<uint64_t> subtract_vectors(const std::vector<uint64_t>& lhs,
                                           const std::vector<uint64_t>& rhs) {
        uint64_t borrow = 0;
        std::vector<uint64_t> result(lhs.size(), 0);
        __uint128_t minuend;
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (lhs[i] < rhs[i] || lhs[i] < rhs[i] + borrow) {
                minuend = 1;
                minuend <<= 64;
                minuend += static_cast<__uint128_t>(lhs[i]);
                minuend -= static_cast<__uint128_t>(rhs[i]);
                minuend -= borrow;
                result[i] = minuend;
                borrow = 1;
            } else {
                result[i] = lhs[i] - rhs[i] - borrow;
                borrow = 0;
            }
        }
        return result;
    }

    std::vector<uint64_t> multiply_vectors(const std::vector<uint64_t>& lhs,
                                           const std::vector<uint64_t>& rhs) {
        std::vector<uint64_t> result(lhs.size() + rhs.size(), 0);
        std::vector<uint64_t> trunc_product(lhs.size() + rhs.size());
        std::vector<uint64_t> carry(lhs.size() + rhs.size());
        __uint128_t mul;
        uint64_t overflow;
        for (size_t i = 0; i < lhs.size(); ++i) {
            std::fill(trunc_product.begin(), trunc_product.end(), 0);
            std::fill(carry.begin(), carry.end(), 0);
            for (size_t j = 0; j < rhs.size(); ++j) {
                mul = static_cast<__uint128_t>(lhs[i]) * static_cast<__uint128_t>(rhs[j]);
                overflow = static_cast<uint64_t>(mul >> 64);
                if (overflow > 0)
                    carry[i + j + 1] = overflow;
                trunc_product[i + j] = static_cast<uint64_t>(mul);
            }
            add_vector(result, trunc_product);
            add_vector(result, carry);
        }
        return result;
    }

    std::vector<uint64_t> modulo_vectors(std::vector<uint64_t>& dividend, const std::vector<uint64_t>& divisor) {
        std::vector<uint64_t> quotient(dividend.size(), 0);
        std::vector<uint64_t> remainder(dividend.size(), 0);
        for (int64_t i = dividend.size() - 1; i >= 0; --i) {
            for (int64_t j = 63; j >= 0; --j) {
                shift_vector_left(remainder, 1);
                shift_vector_left(quotient, 1);
                remainder[0] += (dividend[i] & (1ull << j)) >> j;
                // if (remainder[3] >= divisor[3])
                //     printf("Yay!");
                if (compare_vectors(remainder, divisor) != std::strong_ordering::less) {
                    remainder = subtract_vectors(remainder, divisor);
                    ++quotient[0];
                }
            }
        }
        dividend = quotient;
        return remainder;
    }

}
