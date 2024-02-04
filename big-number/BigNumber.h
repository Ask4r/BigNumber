#pragma once

#include "vector-utils//VectorUtils.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <complex>
#include <algorithm>

#include <format>
#include <bitset>

namespace BigNumber {

    class BigNumber {
        /*
         * number = (-1)^sign * (2^64)^exponent * mantissa
         *
         *
         *
         *
        * number = (-1)^sign * 2^(exponent - mantissa_size * 64) * mantissa
        *
        * or (-1)^sign * 2^exponent * 0.[mantissa]
        * 0.[mantissa] = mantissa / 2^(mantissa size in bits)
        * */
     private:
        uint64_t sign;
        int64_t exponent;
        std::vector<uint64_t> mantissa;

        typedef uint64_t MANTISSA_CHUNK_TYPE_T;
        const uint64_t MANTISSA_CHUNK_SIZE = 64;

        // Addition and subtraction
        void add_positive(const BigNumber&);
        void subtract_positive(const BigNumber&);

        // Adapters
        [[nodiscard]] std::string get_integer_part() const;
        [[nodiscard]] std::string get_fraction_part() const;

        // Other
        static uint64_t get_number_exponent(uint64_t);

     public:

        // Constructors
        explicit BigNumber(uint64_t);
        explicit BigNumber(const char *, uint64_t);
        explicit BigNumber(float, uint64_t);
        explicit BigNumber(double, uint64_t);
        explicit BigNumber(int64_t, uint64_t);
        explicit BigNumber(uint64_t, uint64_t);
        BigNumber(const BigNumber&);

        // Getters
        [[nodiscard]] bool is_positive() const;
        [[nodiscard]] bool is_negative() const;

        // Direct assignment
        BigNumber& operator=(const BigNumber&);
        // Unary minus
        friend BigNumber operator-(const BigNumber&);
        // Math utils
        friend BigNumber abs(const BigNumber&);
        friend BigNumber inverse(const BigNumber&);

        // Addition and subtraction
        friend BigNumber& operator+=(BigNumber&, const BigNumber&);
        friend BigNumber& operator-=(BigNumber&, const BigNumber&);
        friend BigNumber operator+(const BigNumber&, const BigNumber&);
        friend BigNumber operator-(const BigNumber&, const BigNumber&);

        // Multiplication and division
        friend BigNumber& operator*=(BigNumber&, const BigNumber&);
        friend BigNumber& operator/=(BigNumber&, const BigNumber&);
        friend BigNumber operator*(const BigNumber&, const BigNumber&);
        friend BigNumber operator/(const BigNumber&, const BigNumber&);

        // Comparison
        friend std::strong_ordering operator<=>(const BigNumber&, const BigNumber&);
        friend bool operator==(const BigNumber&, const BigNumber&);

        // Stream representation
        friend std::ostream& operator<<(std::ostream&, const BigNumber&);

        // Adapters
        [[nodiscard]] std::string to_string() const;
    };

}
