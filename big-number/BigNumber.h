//
// Created by Askar on 1/30/2024.
//

#ifndef _BIGNUMBER_H_
#define _BIGNUMBER_H_

#include <iostream>
#include <vector>
#include <cstdint>
#include <complex>

class BigNumber {
 private:
    /*
    * number = (-1)^sign * 2^(exponent - mantissa_size * 64) * mantissa
    *
    * or (-1)^sign * 2^exponent * 0.[mantissa]
    * 0.[mantissa] = mantissa / 2^(mantissa size in bits)
    * */
    uint64_t sign;
    int64_t exponent;
    std::vector<uint64_t> mantissa;

    typedef uint64_t MANTISSA_CHUNK_TYPE_T;
    const uint64_t MANTISSA_CHUNK_SIZE = 64;

    // Utils
    static uint64_t get_number_exponent(uint64_t number);
    static void negate_mantissa_vector(std::vector<uint64_t>& self);
    static uint64_t add_mantissa_vector(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    static void shift_mantissa_vector_left(std::vector<uint64_t>&, uint64_t shift_step);
    static void shift_mantissa_vector_right(std::vector<uint64_t>&, uint64_t shift_step);
    static void add_vector(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    static void shift_vector_right(std::vector<uint64_t>&, uint64_t shift_step);
    static void multiply_vector(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    static void inverse_of_vector(std::vector<uint64_t>&);

    void set_binary_precision(uint64_t precision);

    static void multiply_fft(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    static void divide_fft(std::vector<uint64_t>&, const std::vector<uint64_t>&);
    static std::vector<std::complex<double>> fft(const std::vector<std::complex<double>>&);
    static std::vector<std::complex<double>> inverse_fft(const std::vector<std::complex<double>>& polynomial_values);

    // Polymorphisms
    [[nodiscard]] std::string get_integer_part() const;
    [[nodiscard]] std::string get_fraction_part() const;

    // Addition and subtraction
    void add_positive(const BigNumber&);
    void subtract_positive(const BigNumber&);

    // Multiplication and division

    // Comparison
    friend int64_t compare(const BigNumber&, const BigNumber&);

 public:
    // Constructors
    explicit BigNumber(std::string&, uint64_t precision);
    BigNumber(double number, uint64_t precision);
    BigNumber(int64_t number, uint64_t precision);
    BigNumber(uint64_t number, uint64_t precision);
    BigNumber(const BigNumber&);

    // Utils
    BigNumber &operator=(const BigNumber &);
    friend BigNumber operator-(const BigNumber&);
    friend BigNumber abs(const BigNumber&);

    // Polymorphisms
    [[nodiscard]] std::string to_string() const;

    // Getters
    [[nodiscard]] bool is_positive() const;
    [[nodiscard]] bool is_negative() const;

    // Representation
    friend std::ostream& operator<<(std::ostream&, const BigNumber&);

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
    friend bool operator==(const BigNumber &, const BigNumber &);
    friend bool operator!=(const BigNumber &, const BigNumber &);

    friend bool operator>(const BigNumber &, const BigNumber &);
    friend bool operator>=(const BigNumber &, const BigNumber &);
    friend bool operator<(const BigNumber &, const BigNumber &);
    friend bool operator<=(const BigNumber &, const BigNumber &);
};

#endif //_BIGNUMBER_H_
