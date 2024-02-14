#include "big_number.h"

namespace BigNumber {

    BigNumber::BigNumber(const char *s, uint64_t precision) {
        size_t mantissa_size = precision / 64 + (precision % 64 > 0);
        std::istringstream iss(s);
        if (iss.eof() || iss.fail()) {
            sign = 0;
            exponent = 0;
            return;
        }
        std::string number(s);
        if (number[0] == '-') {
            number = number.substr(1);
            sign = 1;
        } else {
            sign = 0;
        }
        std::string integer_slice;
        std::string fraction_slice;
        const uint64_t period = number.find('.');
        if (period == std::string::npos) {
            integer_slice = number;
            fraction_slice = "0";
        } else {
            integer_slice = number.substr(0, number.find('.'));
            fraction_slice = number.substr(number.find('.') + 1);
        }
        std::vector<uint64_t> integer = VectorUtils::to_integer_vector(integer_slice);
        std::vector<uint64_t> fraction = VectorUtils::to_fraction_vector(fraction_slice,
                                                                         mantissa_size - integer.size());
        exponent = -static_cast<int64_t>(mantissa_size - integer.size());
        VectorUtils::extend(mantissa, fraction);
        VectorUtils::extend(mantissa, integer);
        const uint64_t shift = VectorUtils::normalise_mantissa(mantissa, mantissa_size);
        exponent += static_cast<int64_t>(shift);
    }

    BigNumber::BigNumber(float number, uint64_t precision)
        : BigNumber(static_cast<double>(number), precision) {
    }

    BigNumber::BigNumber(double number, uint64_t precision) {
        const size_t mantissa_size = precision / 64 + (precision % 64 > 0);
        mantissa.resize(mantissa_size);
        sign = (number < 0);
        const int64_t num_exp = (((*(int64_t *)&number) & 0x7FF0'0000'0000'0000) >> 52) - 1023 + 1;
        const uint64_t num_mantissa = (((*(uint64_t *)&number) & 0x000F'FFFF'FFFF'FFFF) + 0x0010'0000'0000'0000);
        exponent = -1;
        mantissa[0] = num_mantissa << (11 + num_exp);
        mantissa[1] = num_mantissa >> (53 - num_exp);
    }

    BigNumber::BigNumber(int8_t number, uint64_t precision)
        : BigNumber(static_cast<int64_t>(number), precision) {
    }

    BigNumber::BigNumber(int16_t number, uint64_t precision)
        : BigNumber(static_cast<int64_t>(number), precision) {
    }

    BigNumber::BigNumber(int32_t number, uint64_t precision)
        : BigNumber(static_cast<int64_t>(number), precision) {
    }

    BigNumber::BigNumber(int64_t number, size_t precision) {
        const size_t mantissa_size = precision / 64 + (precision % 64 > 0);
        mantissa.resize(mantissa_size);
        sign = (number < 0);
        exponent = 0;
        mantissa[0] = std::abs(number);
    }

    BigNumber::BigNumber(uint8_t number, uint64_t precision)
        : BigNumber(static_cast<uint64_t>(number), precision) {
    }

    BigNumber::BigNumber(uint16_t number, uint64_t precision)
        : BigNumber(static_cast<uint64_t>(number), precision) {
    }

    BigNumber::BigNumber(uint32_t number, uint64_t precision)
        : BigNumber(static_cast<uint64_t>(number), precision) {
    }

    BigNumber::BigNumber(uint64_t number, size_t precision) {
        const size_t mantissa_size = precision / 64 + (precision % 64 > 0);
        mantissa.resize(mantissa_size);
        sign = 0;
        exponent = 0;
        mantissa[0] = number;
    }

    BigNumber::BigNumber(const BigNumber& number) {
        sign = number.sign;
        exponent = number.exponent;
        mantissa = number.mantissa;
    }

    BigNumber::BigNumber(BigNumber&& number) noexcept {
        sign = number.sign;
        exponent = number.exponent;
        mantissa = std::move(number.mantissa);
    }


    // Getters
    bool BigNumber::is_positive() const {
        return sign == 0;
    }

    bool BigNumber::is_negative() const {
        return sign != 0;
    }

    bool BigNumber::is_null() const {
        return VectorUtils::is_null(mantissa);
    }


    // Direct assignment
    BigNumber& BigNumber::operator=(const BigNumber& number) = default;

    BigNumber& BigNumber::operator=(BigNumber&& other) noexcept {
        sign = other.sign;
        exponent = other.exponent;
        mantissa = std::move(other.mantissa);
        return *this;
    }

    // Unary minus
    BigNumber operator-(const BigNumber& number) {
        BigNumber result = number;
        result.sign = 1 - result.sign;
        return result;
    }

    // Math utils
    BigNumber abs(const BigNumber& number) {
        BigNumber result = number;
        result.sign = 0;
        return result;
    }

    BigNumber floor(const BigNumber& number) {
        if (number.exponent >= 0)
            return number;
        BigNumber result = number;
        VectorUtils::shift_left(result.mantissa, -result.exponent);
        const uint64_t shift = VectorUtils::normalise_mantissa(result.mantissa, number.mantissa.size());
        result.exponent = static_cast<int64_t>(shift);
        return result;
    }

    BigNumber ceil(const BigNumber& number) {
        if (number.exponent >= 0)
            return number;
        bool rounded = false;
        for (size_t i = 0; i < -number.exponent; ++i) {
            if (number.mantissa[i] != 0) {
                rounded = true;
                break;
            }
        }
        BigNumber result = number;
        VectorUtils::shift_left(result.mantissa, -result.exponent);
        const uint64_t shift = VectorUtils::normalise_mantissa(result.mantissa, number.mantissa.size());
        result.exponent = static_cast<int64_t>(shift);
        if (rounded)
            result += 1;
        return result;
    }

    /*BigNumber sqrt(const BigNumber& number) {
        const int64_t initial_exponent = number.exponent;
        BigNumber one(1, number.mantissa.size() * 3 * 64);
        BigNumber half(0.5, number.mantissa.size() * 3 * 64);
        BigNumber start(1, number.mantissa.size() * 3 * 64);
        BigNumber end = number;
        end.exponent = static_cast<int64_t>(number.mantissa.size() * 2);
        end.mantissa.resize(number.mantissa.size() * 3, 0);
        BigNumber target = end;
        BigNumber mid(0, number.mantissa.size() * 3 * 64);
        BigNumber square(0, number.mantissa.size() * 3 * 64);
        BigNumber ans(0, number.mantissa.size() * 3 * 64);
        while (start <= end) {
            mid = floor((start + end) * half);
            square = mid * mid;
            if (square == target) {
                ans = mid;
                break;
            }
            if (square < target) {
                start = mid + one;
                ans = mid;
            } else {
                end = mid - one;
            }
        }
        if (initial_exponent % 2 != 0)
            VectorUtils::half_shift_right(ans.mantissa);
        const uint64_t shift = VectorUtils::normalise_mantissa(ans.mantissa, number.mantissa.size());
        ans.exponent = initial_exponent / 2
                       - static_cast<int64_t>(number.mantissa.size())
                       + static_cast<int64_t>(shift);
        return ans;
    }*/

    BigNumber pow(const BigNumber& number, uint64_t pow) {
        BigNumber result(1, number.mantissa.size() * 64);
        if (pow == 0)
            return result;
        BigNumber multiplier = number;
        while (pow > 0) {
            if (pow & 1)
                result *= multiplier;
            multiplier *= multiplier;
            pow >>= 1;
        }
        return result;
    }

    BigNumber arctan(const BigNumber& number) {
        BigNumber result(0.0, number.mantissa.size() * 64);
        BigNumber next_result = number;
        BigNumber summand = number;
        uint64_t n = 2;
        size_t identity_count = 0;
        do {
            result = next_result;
            summand *= (number * number * (n + n - 3)) / (n + n - 1);
            if (n % 2 == 1)
                next_result += summand;
            else
                next_result -= summand;
            ++n;
            if (result.mantissa.size() < (result.exponent - summand.exponent)) {
                ++identity_count;
            } else {
                identity_count = 0;
            }
        } while (identity_count < 10);
        return result;
    }

    BigNumber factorial(const BigNumber& number) {
        if (number.exponent < 0)
            return BigNumber(0, number.mantissa.size() * 64);
        if (number.is_null())
            return BigNumber(1, number.mantissa.size() * 64);
        BigNumber result(1, number.mantissa.size() * 64);
        BigNumber i(1, number.mantissa.size() * 64);
        while (i <= number) {
            result *= i;
            i += 1;
        }
        return result;
    }


    // Addition and subtraction
    void BigNumber::add_positive(const BigNumber& number) {
        const uint64_t initial_size = mantissa.size();
        BigNumber summand = number;
        uint64_t calc_size;
        if (exponent < summand.exponent) {
            calc_size = std::max(mantissa.size() + summand.exponent - exponent,
                                 summand.mantissa.size());
            summand.mantissa.resize(calc_size, 0);
            mantissa.resize(calc_size, 0);
            VectorUtils::shift_right(summand.mantissa, summand.exponent - exponent);
        } else if (exponent > summand.exponent) {
            calc_size = std::max(summand.mantissa.size() + exponent - summand.exponent,
                                 mantissa.size());
            summand.mantissa.resize(calc_size, 0);
            mantissa.resize(calc_size, 0);
            VectorUtils::shift_right(mantissa, exponent - summand.exponent);
            exponent = summand.exponent;
        } else {
            calc_size = std::max(summand.mantissa.size(), mantissa.size());
            summand.mantissa.resize(calc_size, 0);
            mantissa.resize(calc_size, 0);
        }
        const uint64_t carry = VectorUtils::add_vector(mantissa, summand.mantissa);
        if (carry != 0)
            mantissa.push_back(1);
        const uint64_t shift = VectorUtils::normalise_mantissa(mantissa, initial_size);
        exponent += static_cast<int64_t>(shift);
    }

    void BigNumber::subtract_positive(const BigNumber& number) {
        const uint64_t initial_size = mantissa.size();
        BigNumber subtrahend = number;
        if (exponent < subtrahend.exponent) {
            const uint64_t calc_size = std::max(mantissa.size() + subtrahend.exponent - exponent,
                                                subtrahend.mantissa.size());
            subtrahend.mantissa.resize(calc_size, 0);
            mantissa.resize(calc_size, 0);
            VectorUtils::shift_right(subtrahend.mantissa, subtrahend.exponent - exponent);
        } else if (exponent > subtrahend.exponent) {
            const uint64_t calc_size = std::max(subtrahend.mantissa.size() + exponent - subtrahend.exponent,
                                                mantissa.size());
            subtrahend.mantissa.resize(calc_size, 0);
            mantissa.resize(calc_size, 0);
            VectorUtils::shift_right(mantissa, exponent - subtrahend.exponent);
            exponent = subtrahend.exponent;
        } else {
            const uint64_t calc_size = std::max(subtrahend.mantissa.size(), mantissa.size());
            subtrahend.mantissa.resize(calc_size, 0);
            mantissa.resize(calc_size, 0);
        }
        VectorUtils::subtract_vector(mantissa, subtrahend.mantissa);
        const uint64_t shift = VectorUtils::normalise_mantissa(mantissa, initial_size);
        exponent += static_cast<int64_t>(shift);
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

    BigNumber& operator+=(BigNumber& self, uint64_t number) {
        const uint64_t carry = VectorUtils::add_number(self.mantissa, number);
        if (carry != 0) {
            VectorUtils::shift_left(self.mantissa, 1);
            self.mantissa.back() = 1;
            self.exponent -= 1;
        }
        return self;
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


    // Multiplication and division
    BigNumber& operator*=(BigNumber& self, const BigNumber& other) {
        const uint64_t initial_size = self.mantissa.size();
        self.mantissa = VectorUtils::multiply_vectors(self.mantissa, other.mantissa);
        const uint64_t shift = VectorUtils::normalise_mantissa(self.mantissa, initial_size);
        self.sign = (self.sign != other.sign);
        self.exponent += static_cast<int64_t>(other.exponent + shift);
        return self;
    }

    BigNumber& operator*=(BigNumber& self, uint64_t number) {
        const uint64_t initial_size = self.mantissa.size();
        self.mantissa = VectorUtils::multiply_vectors(self.mantissa,
                                                      std::vector<uint64_t>(1, number));
        const uint64_t shift = VectorUtils::normalise_mantissa(self.mantissa, initial_size);
        self.exponent += static_cast<int64_t>(shift);
        return self;
    }

    BigNumber& operator/=(BigNumber& self, const BigNumber& other) {
        if (other.is_null())
            throw std::runtime_error("Division by zero");
        const uint64_t initial_size = self.mantissa.size();
        std::vector<uint64_t> dividend = self.mantissa;
        dividend.resize(initial_size << 1, 0);
        VectorUtils::shift_right(dividend, initial_size);
        while (dividend.back() == 0)
            dividend.pop_back();
        std::vector<uint64_t> divisor = other.mantissa;
        while (divisor.back() == 0)
            divisor.pop_back();
        VectorUtils::modulo_vector(dividend, divisor);
        self.mantissa = dividend;
        const uint64_t shift = VectorUtils::normalise_mantissa(self.mantissa, initial_size);
        self.sign = (self.sign != other.sign);
        self.exponent += static_cast<int64_t>(shift - other.exponent - initial_size);
        return self;
    }

    BigNumber operator*(const BigNumber& lhs, const BigNumber& rhs) {
        BigNumber result = lhs;
        result *= rhs;
        return result;
    }

    BigNumber operator*(const BigNumber& self, uint64_t number) {
        const uint64_t initial_size = self.mantissa.size();
        BigNumber result(0, initial_size);
        result.mantissa = VectorUtils::multiply_vectors(self.mantissa,
                                                        std::vector<uint64_t>(1, number));
        const uint64_t shift = VectorUtils::normalise_mantissa(result.mantissa, initial_size);
        result.sign = self.sign;
        result.exponent = self.exponent + static_cast<int64_t>(shift);
        return result;
    }

    BigNumber operator/(const BigNumber& lhs, const BigNumber& rhs) {
        BigNumber result = lhs;
        result /= rhs;
        return result;
    }

    BigNumber operator/(const BigNumber& self, uint64_t number) {
        return self / BigNumber(number, self.mantissa.size() * 64);
    }


    // Comparison
    std::strong_ordering operator<=>(const BigNumber& lhs, const BigNumber& rhs) {
        if (lhs.is_null() && rhs.is_null())
            return std::strong_ordering::equal;
        if (lhs.sign != 0 && rhs.sign == 0)
            return std::strong_ordering::less;
        if (lhs.sign == 0 && rhs.sign != 0)
            return std::strong_ordering::greater;
        BigNumber lhs_copy = lhs;
        BigNumber rhs_copy = rhs;
        lhs_copy.normalise();
        rhs_copy.normalise();
        const uint64_t comp_diff = std::abs(lhs.exponent - rhs.exponent);
        const uint64_t comp_size = std::max(lhs.mantissa.size(), rhs.mantissa.size()) + comp_diff;
        lhs_copy.mantissa.resize(comp_size, 0);
        rhs_copy.mantissa.resize(comp_size, 0);
        if (lhs.exponent < rhs.exponent) {
            VectorUtils::shift_right(rhs_copy.mantissa, rhs.exponent - lhs.exponent);
        } else if (lhs.exponent > rhs.exponent) {
            VectorUtils::shift_right(lhs_copy.mantissa, lhs.exponent - rhs.exponent);
        }
        const std::strong_ordering result = VectorUtils::compare_vectors(lhs_copy.mantissa, rhs_copy.mantissa);
        return result;
    }

    bool operator==(const BigNumber& lhs, const BigNumber& rhs) {
        return (lhs <=> rhs) == std::strong_ordering::equal;
    }


    // Stream representation
    std::ostream& operator<<(std::ostream& outs, const BigNumber& number) {
        outs << number.to_string();
        return outs;
    }


    // Adapters
    std::string BigNumber::to_string() const {
        if (is_null())
            return "0";
        std::string result;
        std::vector<uint64_t> integer;
        if (exponent <= 0) {
            if (-exponent > mantissa.size() - 1)
                integer = { 0 };
            else
                integer = std::vector(mantissa.begin() - exponent, mantissa.end());
        } else {
            integer = mantissa;
            VectorUtils::extend(integer, std::vector<uint64_t>(exponent, 0));
            VectorUtils::shift_right(integer, exponent);
            while (integer.back() == 0)
                integer.pop_back();
        }
        std::vector<uint64_t> ten = { 10 };
        while (!VectorUtils::is_null(integer))
            result.append(std::to_string(VectorUtils::modulo_vector(integer, ten)[0]));
        if (result.empty())
            result.append("0");
        if (sign != 0)
            result.append("-");
        std::reverse(result.begin(), result.end());
        if (exponent >= 0)
            return result;
        std::vector<uint64_t> fraction(mantissa.begin(), mantissa.begin() - exponent);
        if (fraction.empty() || VectorUtils::is_null(fraction))
            return result;
        result.append(".");
        fraction.push_back(0);
        while (!VectorUtils::is_null(fraction)) {
            fraction = VectorUtils::multiply_vectors(fraction, ten);
            fraction.pop_back();
            result.append(std::to_string(fraction.back()));
            fraction.back() = 0;
        }
        return result;
    }

    // Other
    void BigNumber::normalise() {
        if (is_null())
            return;
        const uint64_t shift = VectorUtils::normalise_mantissa(mantissa, mantissa.size());
        exponent += static_cast<int64_t>(shift);
    }
}


// UD Literals
BigNumber::BigNumber operator ""_b(const char *s) {
    return BigNumber::BigNumber(s);
}

BigNumber::BigNumber operator ""_b(long double number) {
    return BigNumber::BigNumber(static_cast<double>(number));
}

BigNumber::BigNumber operator ""_b(unsigned long long int number) {
    return BigNumber::BigNumber(number);
}