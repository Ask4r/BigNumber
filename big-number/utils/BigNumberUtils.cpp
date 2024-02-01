//
// Created by Askar on 1/30/2024.
//

#include "BigNumberUtils.h"


BigNumber& BigNumber::operator=(const BigNumber& number) {
    sign = number.sign;
    exponent = number.exponent;
    mantissa = number.mantissa;
    return *this;
}

BigNumber operator-(const BigNumber& number) {
    BigNumber result = number;
    result.sign = -result.sign;
    return result;
}

BigNumber abs(const BigNumber& number) {
    BigNumber result = number;
    result.sign = 0;
    return result;
}

uint64_t BigNumber::get_number_exponent(uint64_t number) {
    uint64_t exponent_value = 0;
    do {
        ++exponent_value;
    } while (number >>= 1);
    return exponent_value;
}

void BigNumber::negate_mantissa_vector(std::vector<uint64_t>& self) {
    for (uint64_t& item : self)
        item = ~item;

    uint64_t addition_buffer = 0;
    const uint64_t vector_chunk_max = std::numeric_limits<uint64_t>::max();

    int64_t i = self.size() - 1;
    do {
        // if chunks sum overflows
        if (self[i] > (vector_chunk_max - 1)
            || self[i] > (vector_chunk_max - 1 - addition_buffer)) {
            self[i] += addition_buffer;
            addition_buffer = 1;
        }
        self[i] += 1;
        --i;
    } while (addition_buffer != 0 || i >= 0);
}

uint64_t BigNumber::add_mantissa_vector(std::vector<uint64_t>& self,
                                        const std::vector<uint64_t>& other) {
    uint64_t addition_buffer = 0;
    const uint64_t mantissa_chunk_max = std::numeric_limits<uint64_t>::max();

    for (int64_t i = self.size() - 1; i >= 0; --i) {
        // if chunks sum overflows
        if (self[i] > (mantissa_chunk_max - other[i])
            || self[i] > (mantissa_chunk_max - other[i] - addition_buffer)) {
            self[i] += addition_buffer;
            addition_buffer = 1;
        }
        self[i] += other[i];
    }

    return addition_buffer;
}

void BigNumber::set_binary_precision(size_t precision) {
    const size_t mantissa_size = precision / MANTISSA_CHUNK_SIZE
                                 + (precision % MANTISSA_CHUNK_SIZE > 0);
    mantissa.resize(mantissa_size, 0);
}

void BigNumber::add_vector(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
    uint64_t addition_buffer = 0;
    const uint64_t vector_chunk_max = std::numeric_limits<uint64_t>::max();

    for (size_t i = 0; i < self.size(); ++i) {
        // if chunks sum overflows
        if (self[i] > (vector_chunk_max - other[i])
            || self[i] > (vector_chunk_max - other[i] - addition_buffer)) {
            self[i] += addition_buffer;
            addition_buffer = 1;
        }
        self[i] += other[i];
    }
}

void BigNumber::shift_vector_right(std::vector<uint64_t>& self, uint64_t shift_step) {
    if (shift_step >= 64) {
        std::shift_right(self.begin(), self.end(), shift_step / 64);
        shift_step %= 64;
    }

    uint64_t shift_buffer = 0;
    uint64_t next_shift_buffer;
    const uint64_t shift_mask = 0xFFFF'FFFF'FFFF'FFFF << (64 - shift_step);

    for (uint64_t& chunk : self) {
        next_shift_buffer = (chunk & shift_mask) >> (64 - shift_step);
        chunk <<= shift_step;
        chunk += shift_buffer;
        shift_buffer = next_shift_buffer;
    }
}

void BigNumber::multiply_vector(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {

    self.resize(self.size() + other.size() + 1, 0);
    std::vector<uint64_t> shifted_buffer = self;

    std::fill(self.begin(), self.end(), 0);

    uint64_t shift_delta = 0;

    for (uint64_t other_chunk : other) {
        for (size_t j = 0; j < 64; ++j) {
            if ((other_chunk & (1ull << j)) != 0) {
                shift_vector_right(shifted_buffer, shift_delta);
                add_vector(self, shifted_buffer);
                shift_delta = 0;
                continue;
            }

            ++shift_delta;
        }
    }
}

void BigNumber::inverse_of_vector(std::vector<uint64_t>& self) {

    std::vector<uint64_t> dividend(self.size(), 0);
    std::vector<uint64_t> dividend_buffer(self.size(), 0);

    std::vector<uint64_t> result(self.size(), 0);

    // subtrahend
    std::vector<uint64_t> subtrahend = self;
    negate_mantissa_vector(subtrahend);

    add_mantissa_vector(dividend_buffer, subtrahend);
    result[0] += 1ull << 63;

    // subtract if possible
    for (size_t i = 0; i < dividend.size(); ++i) {
        for (size_t j = 0; j < 64; ++j) {
            if (i == 0 && j == 0)
                continue;
            // subtract from buffer for check
            add_mantissa_vector(dividend_buffer, subtrahend);
            if ((dividend_buffer[0] & 0x8000'0000'0000'0000) != 0) {
                dividend_buffer = dividend;
                shift_mantissa_vector_right(subtrahend, 1);
            } else {
                dividend = dividend_buffer;
                shift_mantissa_vector_right(subtrahend, 1);
                result[i] += 1ull << (63 - j);
            }
        }
    }

    // compensate unassigned 1 out of dividend
    // shift_mantissa_vector_right(subtrahend, 1);
    // subtrahend[0] |= 0x8000'0000'0000'0000;

    self = result;
}

void BigNumber::shift_mantissa_vector_right(std::vector<uint64_t>& self, uint64_t shift_step) {
    if (shift_step >= 64) {
        std::shift_right(self.begin(), self.end(), shift_step / 64);
        shift_step %= 64;
    }

    uint64_t shift_buffer;
    const uint64_t shift_mask = (1 << (shift_step + 1)) - 1;

    for (size_t i = 0; i < self.size(); ++i) {
        if (i != self.size() - 1)
            shift_buffer = self[i + 1] & shift_mask;
        else
            shift_buffer = 0;

        self[i] >>= shift_step;
        // remove bits filled with 1
        if ((self[i] & 0x8000'0000'0000'0000) > 0)
            self[i] += (1 << (64 - shift_step));

        self[i] += shift_buffer << (64 - shift_step);
    }
}

void BigNumber::shift_mantissa_vector_left(std::vector<uint64_t>& self, uint64_t shift_step) {
    if (shift_step >= 64) {
        std::shift_left(self.begin(), self.end(), shift_step / 64);
        shift_step %= 64;
    }

    uint64_t shift_buffer = 0;
    uint64_t next_shift_buffer;
    const uint64_t shift_mask = 0xFFFF'FFFF'FFFF'FFFF << (64 - shift_step);

    for (int64_t i = self.size() - 1; i >= 0; --i) {
        next_shift_buffer = (self[i] & shift_mask) >> (64 - shift_step);
        self[i] <<= shift_step;
        self[i] += shift_buffer;
        shift_buffer = next_shift_buffer;
    }
}

// FFT
void BigNumber::multiply_fft(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {

    std::vector<std::complex<double>> self_complex(self.size() << 1, { 0, 0 });
    std::vector<std::complex<double>> other_complex(other.size() << 1, { 0, 0 });

    for (size_t i = 0; i < self.size(); ++i) {
        self_complex[i] = { static_cast<double>(self[i]), 0 };
        other_complex[i] = { static_cast<double>(other[i]), 0 };
    }

    std::vector<std::complex<double>> self_values = fft(self_complex);
    std::vector<std::complex<double>> other_values = fft(other_complex);

    for (size_t i = 0; i < self_values.size(); ++i)
        self_values[i] *= other_values[i];

    self_complex = inverse_fft(self_values);

    for (size_t i = 0; i < self_complex.size(); ++i)
        self_complex[i] /= self_complex.size();

    std::vector<uint64_t> result(self_complex.size());
    double overflow;
    const auto max_chunk_value = static_cast<double>(std::numeric_limits<uint64_t>::max());

    for (size_t i = 0; i < self_complex.size(); ++i) {
        overflow = std::max(self_complex[i].real() / max_chunk_value - 1, 0.0);
        result[i] = static_cast<uint64_t>(self_complex[i].real() - overflow + 0.5);

        if (i != self_complex.size() - 1) {
            self_complex[i + 1] += overflow;
        }
    }

    self = result;
}

void BigNumber::divide_fft(std::vector<uint64_t>& self, const std::vector<uint64_t>& other) {
    std::vector<std::complex<double>> self_complex(self.size() << 1, { 0, 0 });
    std::vector<std::complex<double>> other_complex(other.size() << 1, { 0, 0 });

    for (size_t i = 0; i < self.size(); ++i) {
        self_complex[i] = { static_cast<double>(self[i]), 0 };
        other_complex[i] = { static_cast<double>(other[i]), 0 };
    }

    std::vector<std::complex<double>> self_values = fft(self_complex);
    std::vector<std::complex<double>> other_values = fft(other_complex);

    for (size_t i = 0; i < self_values.size(); ++i)
        self_values[i] /= other_values[i];

    self_complex = inverse_fft(self_values);

    for (size_t i = 0; i < self_complex.size(); ++i)
        self_complex[i] /= self_complex.size();

    std::vector<uint64_t> result(self_complex.size());
    double overflow;
    const auto max_chunk_value = static_cast<double>(std::numeric_limits<uint64_t>::max());

    for (size_t i = 0; i < self_complex.size(); ++i) {
        overflow = std::max(self_complex[i].real() / max_chunk_value - 1, 0.0);
        result[i] = static_cast<uint64_t>(self_complex[i].real() - overflow + 0.5);

        if (i != self_complex.size() - 1) {
            self_complex[i + 1] += overflow;
        }
    }

    self = result;
}

std::vector<std::complex<double>> BigNumber::fft(const std::vector<std::complex<double>>& polynomial) {
    const uint64_t n = polynomial.size();
    // size in bits
    uint64_t polynomial_binary_order = 0;
    while ((1 << polynomial_binary_order) < n)
        polynomial_binary_order++;

    std::vector<int64_t> reversed_remainders(n);
    reversed_remainders[0] = 0;
    int64_t high1 = -1;

    for (size_t i = 1; i < n; ++i) {
        // if i is power of 2
        if ((i & (i - 1)) == 0)
            high1++;
        // reverse remainder and add higher bit
        reversed_remainders[i] = reversed_remainders[i ^ (1 << high1)];
        reversed_remainders[i] |= (1 << (polynomial_binary_order - high1 - 1));
    }

    std::vector<std::complex<double>> roots(n);
    for (size_t i = 0; i < n; ++i) {
        double alpha = 2 * M_PI * i / n;
        roots[i] = std::complex<double>(cos(alpha), sin(alpha));
    }

    std::vector<std::complex<double>> cur(n);
    for (size_t i = 0; i < n; ++i)
        cur[i] = polynomial[reversed_remainders[i]];

    for (size_t len = 1; len < n; len <<= 1) {
        std::vector<std::complex<double>> ncur(n);
        int64_t rstep = roots.size() / (len * 2);
        for (size_t point_destination = 0; point_destination < n;) {
            uint64_t p1 = point_destination;
            for (size_t i = 0; i < len; ++i) {
                std::complex<double> val = roots[i * rstep] * cur[p1 + len];
                ncur[point_destination] = cur[p1] + val;
                ncur[point_destination + len] = cur[p1] - val;
                ++point_destination;
                ++p1;
            }
            point_destination += len;
        }
        cur.swap(ncur);
    }
    return cur;
}

std::vector<std::complex<double>> BigNumber::inverse_fft(
    const std::vector<std::complex<double>>& polynomial_values) {

    std::vector<std::complex<double>> result = polynomial_values;

    const uint64_t n = result.size();
    // size in bits
    uint64_t polynomial_binary_order = 0;
    while ((1 << polynomial_binary_order) < n)
        polynomial_binary_order++;

    // Bit reversal
    for (size_t i = 0; i < n; ++i) {
        size_t j = 0;
        for (size_t k = 0; k < polynomial_binary_order; ++k) {
            j <<= 1;
            j |= (i >> k) & 1;
        }

        if (j > i) {
            std::swap(result[i], result[j]);
        }
    }

    // Butterfly operations
    for (size_t s = 1; s <= polynomial_binary_order; s++) {
        int64_t m = pow(2, s);
        std::complex<double> Wm = std::polar(1.0, -2 * M_PI / m);

        for (size_t k = 0; k < n; k += m) {
            std::complex<double> W = 1;

            for (size_t j = 0; j < m / 2; j++) {
                std::complex<double> t = W * result[k + j + m / 2];
                std::complex<double> u = result[k + j];
                result[k + j] = u + t;
                result[k + j + m / 2] = u - t;
                W *= Wm;
            }
        }
    }
    return result;
}