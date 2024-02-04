#include "BigNumber.h"

namespace BigNumber {

    // Constructors
    BigNumber::BigNumber(uint64_t precision) {
        size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
        mantissa.resize(mantissa_size, 0);
        sign = 0;
        exponent = 0;
    }

    BigNumber::BigNumber(const char *s, uint64_t precision) {
        size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
        mantissa.resize(mantissa_size, 0);

        std::istringstream iss(s);
        if (iss.eof() || iss.fail()) {
            sign = 0;
            exponent = 0;
            return;
        }

        sign = (s[0] == '-');
        exponent = 0;

        uint64_t period_index = 0;
        while (s[period_index] != '.')
            ++period_index;

        BigNumber ten(10.0, precision);
        BigNumber one_tenth(precision);
        one_tenth.exponent = -3;
        for (uint64_t& chunk : one_tenth.mantissa) {
            chunk = 0xCCCC'CCCC'CCCC'CCCC;
        }

        // integral part
        for (size_t i = period_index - 1; isdigit(s[i]); --i) {
            BigNumber _(static_cast<uint64_t>(s[i] - '0'), precision);
            for (size_t j = 0; j < period_index - i - 1; ++j)
                _ *= ten;
            *this += _;
        }

        // fraction part
        for (size_t i = period_index + 1; isdigit(s[i]); ++i) {
            BigNumber _(static_cast<uint64_t>(s[i] - '0'), precision);
            for (size_t j = 0; j < i - period_index; ++j)
                _ *= one_tenth;
            *this += _;
        }
    }

    BigNumber::BigNumber(float number, uint64_t precision) {
        const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
        mantissa.resize(mantissa_size, 0);
        sign = (number < 0);
        exponent = static_cast<int64_t>(((*(int32_t *)&number) & 0x7F80'0000) >> 23) - 127 + 1;
        mantissa[0] = static_cast<uint64_t>(((*(uint32_t *)&number) & 0x007F'FFFF) + 0x0080'0000) << 40;
    }

    BigNumber::BigNumber(double number, uint64_t precision) {
        const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
        mantissa.resize(mantissa_size, 0);
        sign = (number < 0);
        exponent = (((*(int64_t *)&number) & 0x7FF0'0000'0000'0000) >> 52) - 1023 + 1;
        mantissa[0] = (((*(uint64_t *)&number) & 0x000F'FFFF'FFFF'FFFF) + 0x0010'0000'0000'0000) << 11;
    }

    BigNumber::BigNumber(int64_t number, size_t precision) {
        const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
        mantissa.resize(mantissa_size, 0);
        sign = (number < 0);
        number = std::abs(number);
        exponent = static_cast<int64_t>(get_number_exponent(static_cast<uint64_t>(number)));
        mantissa[0] = number << (MANTISSA_CHUNK_SIZE - exponent);
    }

    BigNumber::BigNumber(uint64_t number, size_t precision) {
        const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE + (precision % MANTISSA_CHUNK_SIZE > 0);
        mantissa.resize(mantissa_size, 0);
        sign = 0;
        exponent = static_cast<int64_t>(get_number_exponent(number));
        mantissa[0] = number << (MANTISSA_CHUNK_SIZE - exponent);
    }

    BigNumber::BigNumber(const BigNumber& number) {
        sign = number.sign;
        exponent = number.exponent;
        mantissa = number.mantissa;
    }


    // Getters
    bool BigNumber::is_positive() const {
        return sign == 0;
    }

    bool BigNumber::is_negative() const {
        return sign != 0;
    }


    // Direct assignment
    BigNumber& BigNumber::operator=(const BigNumber& number) {
        sign = number.sign;
        exponent = number.exponent;
        mantissa = number.mantissa;
        return *this;
    }

    // Unary minus
    BigNumber operator-(const BigNumber& number) {
        BigNumber result = number;
        result.sign = -result.sign;
        return result;
    }

    // Math utils
    BigNumber abs(const BigNumber& number) {
        BigNumber result = number;
        result.sign = 0;
        return result;
    }

    BigNumber inverse(const BigNumber& number) {
        BigNumber inverse_prev(number.mantissa.size() << 6);
        BigNumber inverse(number.mantissa.size() << 6);
        inverse.exponent = 1 - number.exponent;
        inverse.mantissa[0] = 0x8000'0000'0000'0000;
        BigNumber two(2.0, number.mantissa.size() << 6);
        // size_t identical_counter = 0;
        while (inverse != inverse_prev) {
            inverse_prev = inverse;
            inverse = inverse_prev * (two - number * inverse_prev);
            // if (inverse == inverse_prev)
            //     ++identical_counter;

            std::bitset<64> m1(inverse.mantissa[0]);
            std::bitset<64> m2(inverse.mantissa[1]);
            std::cout << "2**" << inverse.exponent << " " << m1 << " " << m2 << std::endl;
        }
        inverse.sign = number.sign;
        return inverse;
    }


    // Addition and subtraction

    void BigNumber::add_positive(const BigNumber& number) {
        BigNumber summand = number;
        summand.mantissa.resize(mantissa.size(), 0);
        // take number with smaller exponent and
        // shift its mantissa to match result's exponent
        if (exponent < summand.exponent) {
            VectorUtils::shift_mantissa_right(mantissa, summand.exponent - exponent);
            exponent = summand.exponent;
        } else if (exponent > summand.exponent) {
            VectorUtils::shift_mantissa_right(summand.mantissa, exponent - summand.exponent);
        }
        // std::reverse(mantissa.begin(), mantissa.end());
        // std::reverse(summand.mantissa.begin(), summand.mantissa.end());
        // adding mantissa
        const uint64_t carry = VectorUtils::add_mantissa(mantissa, summand.mantissa);
        // std::reverse(mantissa.begin(), mantissa.end());
        // normalize mantissa if buffer is not empty
        if (carry != 0) {
            ++exponent;
            VectorUtils::shift_mantissa_right(mantissa, 1);
            mantissa[0] |= 0x8000'0000'0000'0000;
        }
    }

    void BigNumber::subtract_positive(const BigNumber& number) {
        BigNumber subtrahend = number;
        subtrahend.mantissa.resize(mantissa.size());
        // take number with smaller exponent and
        // shift its mantissa to match result's exponent
        if (exponent < subtrahend.exponent) {
            VectorUtils::shift_mantissa_right(mantissa, subtrahend.exponent - exponent);
            exponent = subtrahend.exponent;
        } else if (exponent > subtrahend.exponent) {
            VectorUtils::shift_mantissa_right(subtrahend.mantissa, exponent - subtrahend.exponent);
        }
        std::reverse(subtrahend.mantissa.begin(), subtrahend.mantissa.end());
        std::reverse(mantissa.begin(), mantissa.end());
        // subtracting mantissas
        mantissa = VectorUtils::subtract_vectors(mantissa, subtrahend.mantissa);
        std::reverse(mantissa.begin(), mantissa.end());
        uint64_t bit_shift = VectorUtils::normalise_mantissa(mantissa);
        exponent -= static_cast<int64_t>(bit_shift);
    }

    BigNumber operator+(const BigNumber& lhs, const BigNumber& rhs) {
        BigNumber result = lhs;
        result += rhs;
        return result;
    }

    BigNumber operator-(const BigNumber& lhs, const BigNumber& rhs) {
        BigNumber result = lhs;
        result -= rhs;
        return result;
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
            self.sign = 1 - self.sign;
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
            self.sign = 1 - self.sign;
            return self;
        }
    }


    // Multiplication and division

    BigNumber& operator*=(BigNumber& self, const BigNumber& other) {
        std::vector<uint64_t> multiplier = other.mantissa;
        std::reverse(self.mantissa.begin(), self.mantissa.end());
        std::reverse(multiplier.begin(), multiplier.end());
        // multiply
        self.mantissa = VectorUtils::multiply_vectors(self.mantissa, multiplier);
        std::reverse(self.mantissa.begin(), self.mantissa.end());
        // normalise
        uint64_t bit_shift = VectorUtils::normalise_mantissa(self.mantissa);
        self.sign = (self.sign != other.sign);
        self.exponent = self.exponent + other.exponent - bit_shift;
        self.mantissa.resize(self.mantissa.size());
        return self;
    }

    BigNumber& operator/=(BigNumber& self, const BigNumber& other) {
        self *= inverse(other);
        // std::vector<uint64_t> divisor = other.mantissa;
        // if (self.mantissa.size() < divisor.size())
        //     self.mantissa.resize(divisor.size());
        // else if (divisor.size() < self.mantissa.size())
        //     divisor.resize(self.mantissa.size());
        // self.mantissa.resize(self.mantissa.size() * 2);
        // std::reverse(self.mantissa.begin(), self.mantissa.end());
        // std::reverse(divisor.begin(), divisor.end());
        // divisor.resize(divisor.size() * 2);
        // // divide
        // VectorUtils::modulo_vectors(self.mantissa, divisor);
        // std::reverse(self.mantissa.begin(), self.mantissa.end());
        // // normalize
        // uint64_t bit_shift = VectorUtils::normalise_mantissa(self.mantissa);
        // self.sign = (self.sign != other.sign);
        // self.exponent = self.exponent - other.exponent - bit_shift + self.mantissa.size() * 32;
        // self.mantissa.resize(self.mantissa.size() / 2);
        return self;
    }

    BigNumber operator*(const BigNumber& lhs, const BigNumber& rhs) {
        BigNumber result = lhs;
        result *= rhs;
        return result;
    }

    BigNumber operator/(const BigNumber& lhs, const BigNumber& rhs) {
        BigNumber result = lhs;
        result /= rhs;
        return result;
    }


    // Comparison

    std::strong_ordering operator<=>(const BigNumber& lhs, const BigNumber& rhs) {
        // lhs - negative, rhs - positive
        if (lhs.sign > 0 && rhs.sign == 0)
            return std::strong_ordering::less;
        // lhs - positive, rhs - negative
        if (lhs.sign == 0 && rhs.sign > 0)
            return std::strong_ordering::greater;

        const std::strong_ordering comparison_order = (lhs.sign == 0 && rhs.sign == 0)
                                                      ? std::strong_ordering::greater
                                                      : std::strong_ordering::less;
        const std::strong_ordering inverse_comparison_order = (lhs.sign == 0 && rhs.sign == 0)
                                                              ? std::strong_ordering::less
                                                              : std::strong_ordering::greater;
        if (lhs.exponent > rhs.exponent)
            return comparison_order;
        if (lhs.exponent < rhs.exponent)
            return inverse_comparison_order;
        for (size_t i = 0; i < std::min(lhs.mantissa.size(), rhs.mantissa.size()); ++i) {
            if (lhs.mantissa[i] > rhs.mantissa[i])
                return comparison_order;
            if (lhs.mantissa[i] < rhs.mantissa[i])
                return inverse_comparison_order;
        }
        if (lhs.mantissa.size() > rhs.mantissa.size() && lhs.mantissa.back() != 0)
            return comparison_order;
        if (lhs.mantissa.size() < rhs.mantissa.size() && rhs.mantissa.back() != 0)
            return inverse_comparison_order;

        return std::strong_ordering::equal;
    }

    bool operator==(const BigNumber& lhs, const BigNumber& rhs) {
        return lhs.sign == rhs.sign && lhs.exponent == rhs.exponent && lhs.mantissa == rhs.mantissa;
    }


    // Stream representation

    std::ostream& operator<<(std::ostream& outs, const BigNumber& number) {
        outs << number.to_string();
        return outs;
    }


    // Adapters

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
        if (exponent > 0)
            VectorUtils::shift_mantissa_left(mantissa_buffer, exponent);
        else if (exponent < 0)
            VectorUtils::shift_mantissa_right(mantissa_buffer, -exponent);

        std::vector<MANTISSA_CHUNK_TYPE_T> mantissa_summand = mantissa_buffer;
        if ((mantissa_summand.back() & 0x3) > 0) {
            mantissa_buffer.push_back(0);
            mantissa_summand.push_back(0);
        }

        // multiply by 10 and add to the buffer
        const uint64_t digit_bit_mask = 0xE000'0000'0000'0000;
        const uint64_t bit_removal_mask = 0x1FFF'FFFF'FFFF'FFFF;
        VectorUtils::shift_mantissa_right(mantissa_summand, 2);
        // remove 2 bits filled by shift
        mantissa_summand[0] &= 0x3FFF'FFFF'FFFF'FFFF;

        uint64_t addition_buffer;

        for (uint64_t i = 0; i < mantissa_buffer.size() * MANTISSA_CHUNK_SIZE; ++i) {
            addition_buffer = VectorUtils::add_mantissa(mantissa_buffer, mantissa_summand);

            result.append(std::to_string(
                ((mantissa_buffer[0] & digit_bit_mask) >> (MANTISSA_CHUNK_SIZE - 3))
                + (addition_buffer << 3)
            ));
            mantissa_buffer[0] &= bit_removal_mask;
            mantissa_summand = mantissa_buffer;
            VectorUtils::shift_mantissa_left(mantissa_buffer, 3);
            VectorUtils::shift_mantissa_left(mantissa_summand, 1);

        }

        return result;
    }

    std::string BigNumber::to_string() const {
        std::string result;
        if (is_negative())
            result.append("-");
        result.append(exponent > 0 ? get_integer_part() : "0");
        result.append(".");
        result.append(get_fraction_part());
        return result;
    }


    // Other

    uint64_t BigNumber::get_number_exponent(uint64_t number) {
        uint64_t exponent_value = 0;
        do {
            ++exponent_value;
        } while (number >>= 1);
        return exponent_value;
    }

}