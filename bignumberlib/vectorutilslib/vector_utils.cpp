#include "vector_utils.h"

namespace BigNumber::VectorUtils {

    void extend(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
        self.reserve(self.size() + other.size());
        std::copy(other.begin(), other.end(), std::back_inserter(self));
    }

    bool is_null(const std::vector<uint64_t>& self) {
        return std::all_of(self.begin(), self.end(), [](uint64_t elem) { return elem == 0; });
    }

    void shift_left(std::vector<uint64_t>& self, uint64_t shift) {
        std::move(self.begin() + shift, self.end(), self.begin());
        std::fill(self.end() - shift, self.end(), 0);
    }

    void shift_right(std::vector<uint64_t>& self, uint64_t shift) {
        std::move(self.begin(), self.end() - shift, self.begin() + shift);
        std::fill(self.begin(), self.begin() + shift, 0);
    }

    void half_shift_right(std::vector<uint64_t>& self) {
        uint64_t carry = 0;
        uint64_t next_carry;
        for (uint64_t chunk : self) {
            next_carry = (chunk & 0xFFFF'FFFF'0000'0000) >> 32;
            chunk <<= 32;
            chunk += carry;
            carry = next_carry;
        }
    }

    uint64_t normalise_mantissa(std::vector<uint64_t>& self, uint64_t desired) {
        if (is_null(self))
            return 0;
        uint64_t most_significant = self.size();
        for (int64_t i = self.size() - 1; i >= 0; --i) {
            if (self[i] != 0)
                break;
            --most_significant;
        }
        uint64_t least_non_significant = 0;
        for (uint64_t chunk : self) {
            if (chunk != 0)
                break;
            ++least_non_significant;
        }
        uint64_t shift = 0;
        if (most_significant > desired)
            shift += most_significant - desired;
        if (least_non_significant > shift)
            shift += least_non_significant - shift;
        VectorUtils::shift_left(self, shift);
        self.resize(desired);
        return shift;
    }

    void align_fraction_mantissa(std::vector<uint64_t>& self) {
        uint64_t shift = 0;
        while ((1ull << (63 - shift)) > self.back())
            ++shift;
        uint64_t carry = 0;
        uint64_t next_carry;
        const uint64_t carry_mask = 0xFFFF'FFFF'FFFF'FFFF << (64 - shift);
        for (uint64_t& chunk : self) {
            next_carry = (chunk & carry_mask) >> (64 - shift);
            chunk <<= shift;
            chunk += carry;
            carry = next_carry;
        }
    }

    std::strong_ordering compare_vectors(const std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
        for (int64_t i = self.size() - 1; i >= 0; --i) {
            if (self[i] > other[i])
                return std::strong_ordering::greater;
            if (self[i] < other[i])
                return std::strong_ordering::less;
        }
        return std::strong_ordering::equal;
    }

    uint64_t add_vector(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
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
        return carry;
    }

    uint64_t add_number(std::vector<uint64_t>& self, uint64_t number) {
        const uint64_t chunk_max = std::numeric_limits<uint64_t>::max();
        uint64_t carry = (self[0] > (chunk_max - number));
        self[0] += number;
        if (carry == 0)
            return 0;
        for (size_t i = 1; i < self.size(); ++i) {
            if (self[i] <= (chunk_max - carry)) {
                ++self[i];
                break;
            }
            ++self[i];
            carry = 1;
        }
        return carry;
    }

    uint64_t subtract_vector(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
        uint64_t borrow = 0;
        const uint64_t chunk_max = std::numeric_limits<uint64_t>::max();
        for (size_t i = 0; i < self.size(); ++i) {
            if (self[i] < other[i] || self[i] < other[i] + borrow) {
                self[i] = (chunk_max - other[i]) + self[i] + 1 - borrow;
                borrow = 1;
            } else {
                self[i] -= other[i] + borrow;
                borrow = 0;
            }
        }
        return borrow;
    }

    std::vector<uint64_t> multiply_vectors(const std::vector<uint64_t>& lhs, const std::vector<uint64_t>& rhs) {
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

    std::vector<uint64_t> modulo_vector(std::vector<uint64_t>& dividend, std::vector<uint64_t> divisor) {
        // IMPORTANT!
        // dividend size is not less than divisor size
        // and divisor size is greater than 1
        uint64_t dividend_size = dividend.size();
        uint64_t divisor_size = divisor.size();
        bool is_size_unsatisfied = false;
        if (divisor_size <= 1) {
            is_size_unsatisfied = true;
            dividend.insert(dividend.begin(), 0);
            divisor.insert(divisor.begin(), 0);
            ++dividend_size;
            ++divisor_size;
        }
        const uint64_t quotient_size = dividend_size - divisor_size;
        std::vector<uint64_t> quotient(quotient_size + 1, 0);
        __uint128_t base = 1;
        base <<= 64;
        const __uint128_t d = base / (static_cast<__uint128_t>(divisor.back()) + 1);
        std::vector<uint64_t> d_vec = { static_cast<uint64_t>(d) };
        dividend = multiply_vectors(dividend, d_vec);
        divisor = multiply_vectors(divisor, d_vec);
        divisor.resize(divisor_size);
        __uint128_t q;
        __uint128_t r;
        std::vector<uint64_t> q_vec;
        std::vector<uint64_t> division_complement(divisor_size + 1);
        division_complement.back() = 1;
        std::vector<uint64_t> division_minuend;
        std::vector<uint64_t> division_subtrahend;
        uint64_t borrow;
        std::vector<uint64_t> dividend_summand;
        std::vector<uint64_t> divisor_summand;

        for (int64_t i = quotient_size; i >= 0; --i) {
            q = ((static_cast<__uint128_t>(dividend[i + divisor_size]) << 64)
                 + static_cast<__uint128_t>(dividend[i + divisor_size - 1]))
                / divisor.back();
            r = ((static_cast<__uint128_t>(dividend[i + divisor_size]) << 64)
                 + static_cast<__uint128_t>(dividend[i + divisor_size - 1]))
                % divisor.back();
            do {
                if (q == base
                    || q * static_cast<__uint128_t>(divisor[divisor_size - 2])
                       > base * r + static_cast<__uint128_t>(dividend[i + divisor_size - 2])) {
                    --q;
                    r += divisor.back();
                } else {
                    break;
                }
            } while (r < base);
            division_minuend = std::vector<uint64_t>(dividend.begin() + i, dividend.begin() + i + divisor_size + 1);
            // q_vec = { static_cast<uint64_t>(q), static_cast<uint64_t>(q >> 64) };
            q_vec = { static_cast<uint64_t>(q) };
            division_subtrahend = multiply_vectors(divisor, q_vec);
            // division_subtrahend.resize(divisor_size, 0);
            if (compare_vectors(division_minuend, division_subtrahend) != std::strong_ordering::less) {
                subtract_vector(division_minuend, division_subtrahend);
                std::copy(division_minuend.begin(), division_minuend.end(), dividend.begin() + i);
                borrow = 0;
            } else {
                division_minuend.push_back(0);
                division_subtrahend.push_back(0);
                add_vector(division_minuend, division_complement);
                subtract_vector(division_minuend, division_subtrahend);
                division_minuend.pop_back();
                std::copy(division_minuend.begin(), division_minuend.end(), dividend.begin() + i);
                borrow = 1;
            }
            quotient[i] = static_cast<uint64_t>(q);
            if (borrow != 0) {
                --quotient[i];
                dividend_summand = std::vector(dividend.begin() + i, dividend.begin() + i + divisor_size + 1);
                divisor_summand = std::vector(divisor.begin(), divisor.end());
                divisor_summand.push_back(0);
                add_vector(dividend_summand, divisor_summand);
                std::copy(dividend_summand.begin(), dividend_summand.end(), dividend.begin() + i);
            }
        }

        std::vector<uint64_t> remainder = std::vector<uint64_t>(dividend.begin(), dividend.begin() + divisor_size);

        for (size_t i = 0; i < divisor_size; ++i)
            remainder[i] /= static_cast<uint64_t>(d);

        if (is_size_unsatisfied)
            remainder.erase(remainder.begin());

        dividend = quotient;
        return remainder;
    }

    uint64_t modulo_vector(std::vector<uint64_t>& self, uint64_t divisor) {
        // TODO: remainder is correct, quotient is not
        uint64_t remainder = 0;
        const uint64_t chunk_max = std::numeric_limits<uint64_t>::max();
        const uint64_t mod_mul = (chunk_max - divisor + 1) % divisor; // = 2^64 mod divisor
        uint64_t greater, less;
        for (int64_t i = self.size() - 1; i >= 0; --i) {
            if (self[i] > remainder * mod_mul) {
                greater = self[i];
                less = remainder * mod_mul;
            } else {
                greater = remainder * mod_mul;
                less = self[i];
            }
            self[i] = (greater / divisor + (greater % divisor + less) / divisor);
            remainder = (greater - divisor + less) % divisor;
        }
        return remainder;
    }

    std::vector<uint64_t> to_integer_vector(std::string s) {
        std::vector<uint64_t> result = { 0 };
        std::vector<uint64_t> ten = { 10 };
        uint64_t carry;
        for (size_t i = 0; s[i] != '\0'; ++i) {
            result = multiply_vectors(result, ten);
            carry = add_number(result, s[i] - '0');
            if (carry != 0)
                result.push_back(1);
            while (result.back() == 0)
                result.pop_back();
        }
        return result;
    }

    std::vector<uint64_t> to_fraction_vector(std::string s, uint64_t size) {
        std::vector<uint64_t> result(size, 0);
        std::vector<uint64_t> one_tenth(size, 0x9999'9999'9999'9999);
        one_tenth.back() &= 0x1FFF'FFFF'FFFF'FFFF;
        std::vector<uint64_t> order = one_tenth;
        std::vector<uint64_t> digit(1);
        std::vector<uint64_t> product;
        for (size_t i = 0; s[i] != '\0'; ++i) {
            digit[0] = s[i] - '0';
            product = multiply_vectors(order, digit);
            product.pop_back();
            add_vector(result, product);
            order = multiply_vectors(order, one_tenth);
            order.erase(order.begin(), order.end() - size);
        }
        return result;
    }

}
