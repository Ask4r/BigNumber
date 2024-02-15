# BigNumber

The **BigNumber** is a C++ library that provides arithmetic operations on integer and floating point numbers of
defined length. It allows performing basic arithmetic (addition, subtraction, multiplication, division) and
mathematical (abs, floor, ceil, power, factorial) operations on numbers that exceed the range of built-in number types.

## Features

- Supports addition, subtraction, multiplication, and division operations on numbers of user-defined length
- Handles both positive and negative numbers
- Supports comparison operations (greater than, less than, equal to)
- Allows conversion to and from strings
- Provides various utility functions for performing common mathematical operations on big numbers

## Installation

To use the BigNumber Library in your C++ project, follow these steps:

1. Download the latest release of the library from
   the [GitHub repository](https://github.com/Ask4r/BigNumber).
2. Extract the downloaded archive to a directory of your choice.
3. Include the necessary header files in your project:

   ```cpp
   #include "big_number.h"
   ```

4. Link against the library binary file (`.a` or `.lib`) during the compilation and linking process.

## Getting Started

To get started with the BigNumber, follow the example code snippets below:

### Initialization and Assignment

Precision defines amount of significant binary digits (bits). Rounds up to multiple of 64 on initialisation.

```cpp
// Include the necessary header file
#include "big_number.h"

int main() {
   // Precision defines significant binary digits (bits)
   // Rounds to multiple of 64 on initialisation
   const uint64_t precision = 6 * 64;
   
   // Create big number objects
   BigNumber::BigNumber a(0.0, precision);
   BigNumber::BigNumber b("12345678901234567890", precision);
   
   // Assign a value to a big number
   a = BigNumber::BigNumber(100, precision);
   
   return 0;
}
```

BigNumber stores integer and fraction parts in separate chunks of 8 bytes. Stored number is truncated to fit the
allocated memory and save as much precision as possible. If precision is not specified 8 bytes for integer and 8 bytes
for fraction parts will be allocated. Literal definition is possible, but it doesn't allow you specifying any parameters
like precision.

```cpp
// BigNumber will allocate 8 * 6 = 48 bytes of memory to store the mantissa
const uint64_t precision = 6 * 64;

// First 8 bytes will store the integer part 123.0
// Other 40 bytes will store 0.1 binary periodic number
BigNumber::BigNumber a(123.1, precision);

// By default, 16 bytes is allocated
BigNumber::BigNumber b("3.1415926");

// You cannot define number precision this way
BigNumber::BigNumber c = 123456.7890_b;
```

### Arithmetic Operations

Here is specified basic usage of arithmetic operations.

```cpp
 // Create big number objects
BigNumber::BigNumber a("1234567890", precision);
BigNumber::BigNumber b("9876543210", precision);

// Perform arithmetic operations
BigNumber::BigNumber sum = a + b;
BigNumber::BigNumber difference = b - a;
BigNumber::BigNumber product = a * b;
BigNumber::BigNumber quotient = b / a;
```

### Comparison Operations

```cpp
BigNumber::BigNumber a("1234567890", precision);
BigNumber::BigNumber b("9876543210", precision);

// Perform comparison operations
bool greaterThan = b > a;
bool lessThan = b < a;
bool equalTo = a == b;
```

### Conversion to and from Strings

```cpp
BigNumber::BigNumber a("1234567890", precision);

// Convert to string
std::string str = a.to_string();

// Convert from string
BigNumber::BigNumber b = BigNumber::BigNumber("9876543210", precision);
```

### Math functions usage

```cpp
BigNumber::BigNumber a(0, precision);
BigNumber::BigNumber b("-1234567890.123456789", precision);

// Absolute value 
a = abs(b);

// Round functions
a = floor(b);
a = ceil(b);

// Positive integer power
a = pow(b, 3);

// Arctan
BigNumber::BigNumber c(0.5, precision);
a = arctan(c);

// Integer factorial
BigNumber::BigNumber d(100, precision);
a = factorial(d);
```

## License

This library is licensed under the [RICK License](https://www.youtube.com/watch?v=dQw4w9WgXcQ).
