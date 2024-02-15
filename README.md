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


For more detailed information on how to use the library and the available functions, please refer to
the [Library's README.md](https://github.com/Ask4r/BigNumber/blob/main/bignumberlib/README.md).

## Installation

To use the Big Number Library in your C++ project, follow these steps:

1. Download the latest release of the library from
   the [GitHub repository](https://github.com/Ask4r/BigNumber).
2. Extract the downloaded archive to a directory of your choice.
3. Include the necessary header files in your project:

   ```cpp
   #include "big_number.h"
   ```

4. Link against the library binary file (`.a` or `.lib`) during the compilation and linking process.

## Getting Started

To get started with the BigNumber library, follow the example code snippets below:

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

## Documentation

For detailed usage instructions and available functions, please refer to
the [Library's README.md](https://github.com/Ask4r/BigNumber/blob/main/bignumberlib/README.md).
 open an issue or submit a pull request on the [GitHub repository](https://github.com/Ask4r/BigNumber).

## License

This library is licensed under the [RICK License](https://www.youtube.com/watch?v=dQw4w9WgXcQ).
