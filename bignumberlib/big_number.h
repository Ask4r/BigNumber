#pragma once

#include "vectorutilslib/vector_utils.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <complex>
#include <algorithm>
#include <functional>

#include <format>
#include <bitset>

namespace BigNumber {

    class BigNumber {
        // number = (-1)^sign * (2^64)^exponent * mantissa
     private:
        uint64_t sign;
        int64_t exponent;
        std::vector<uint64_t> mantissa;

        // Addition and subtraction
        void add_positive(const BigNumber&);
        void subtract_positive(const BigNumber&);

     public:

        // Constructors
        explicit BigNumber(uint64_t);
        explicit BigNumber(const char *, uint64_t);
        explicit BigNumber(float, uint64_t);
        explicit BigNumber(double, uint64_t);
        explicit BigNumber(int8_t, uint64_t);
        explicit BigNumber(int16_t, uint64_t);
        explicit BigNumber(int32_t , uint64_t);
        explicit BigNumber(int64_t, uint64_t);
        explicit BigNumber(uint8_t, uint64_t);
        explicit BigNumber(uint16_t, uint64_t);
        explicit BigNumber(uint32_t , uint64_t);
        explicit BigNumber(uint64_t, uint64_t);
        BigNumber(const BigNumber&);
        BigNumber(BigNumber&&) noexcept;

        // Getters
        [[nodiscard]] bool is_positive() const;
        [[nodiscard]] bool is_negative() const;
        [[nodiscard]] bool is_null() const;

        // Direct assignment
        BigNumber& operator=(const BigNumber&);
        BigNumber& operator=(BigNumber&&) noexcept;
        // Unary minus
        friend BigNumber operator-(const BigNumber&);
        // Math utils
        friend BigNumber abs(const BigNumber&);
        friend BigNumber floor(const BigNumber&);
        friend BigNumber ceil(const BigNumber&);
        friend BigNumber sqrt(const BigNumber&);
        friend BigNumber pow(const BigNumber&, uint64_t);
        friend BigNumber arctan(const BigNumber&);
        friend BigNumber factorial(const BigNumber&);

        // Addition and subtraction
        friend BigNumber& operator+=(BigNumber&, const BigNumber&);
        friend BigNumber& operator+=(BigNumber&, uint64_t);
        friend BigNumber& operator-=(BigNumber&, const BigNumber&);
        friend BigNumber operator+(const BigNumber&, const BigNumber&);
        friend BigNumber operator-(const BigNumber&, const BigNumber&);

        // Multiplication and division
        friend BigNumber& operator*=(BigNumber&, const BigNumber&);
        friend BigNumber& operator*=(BigNumber&, uint64_t);
        friend BigNumber& operator/=(BigNumber&, const BigNumber&);
        friend BigNumber operator*(const BigNumber&, uint64_t);
        friend BigNumber operator*(const BigNumber&, const BigNumber&);
        friend BigNumber operator/(const BigNumber&, const BigNumber&);
        friend BigNumber operator/(const BigNumber&, uint64_t);

        // Comparison
        friend std::strong_ordering operator<=>(const BigNumber&, const BigNumber&);
        friend bool operator==(const BigNumber&, const BigNumber&);

        // Stream representation
        friend std::ostream& operator<<(std::ostream&, const BigNumber&);

        // Adapters
        [[nodiscard]] std::string to_string() const;
    };

}
