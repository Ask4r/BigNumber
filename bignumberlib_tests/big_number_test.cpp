#include "gtest/gtest.h"
#include "big_number.h"

#include <iostream>
#include <stdexcept>

const uint64_t precision = 10 * 64;

// Constructors
TEST(BigNumberTest, StringConstructor) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    EXPECT_EQ("12345678901234567890123456789012345678901234567890", a.to_string());
}

TEST(BigNumberTest, FractionStringConstructor) {
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890.12345678901234567890123456789012345678901234567890", precision);
    EXPECT_EQ("12345678901234567890123456789012345678901234567890.12345678901234567890123456789012345678901234567890", b
        .to_string());
}

TEST(BigNumberTest, NegativeStringConstructor) {
    BigNumber::BigNumber c("-12345678901234567890123456789.12345678901234567890123456789", precision);
    EXPECT_EQ("-12345678901234567890123456789.12345678901234567890123456789", c.to_string());
}

TEST(BigNumberTest, NegativeDoubleConstructor) {
    BigNumber::BigNumber d(-123.613037109375, precision);
    EXPECT_EQ("-123.613037109375", d.to_string());
}

TEST(BigNumberTest, FloatConstructor) {
    BigNumber::BigNumber e(123.613037109375f, precision);
    EXPECT_EQ("123.613037109375", e.to_string());
}

TEST(BigNumberTest, DoubleConstructor) {
    BigNumber::BigNumber e(123.613037109375, precision);
    EXPECT_EQ("123.613037109375", e.to_string());
}

TEST(BigNumberTest, ZeroConstructor) {
    BigNumber::BigNumber f(0, precision);
    EXPECT_EQ("0", f.to_string());
}

TEST(BigNumberTest, StringZeroConstructor) {
    BigNumber::BigNumber g("0", precision);
    EXPECT_EQ("0", g.to_string());
}

TEST(BigNumberTest, NegativeZeroConstructor) {
    BigNumber::BigNumber h("-0", precision);
    EXPECT_EQ("0", h.to_string());
}

TEST(BigNumberTest, Int8Constructor) {
    BigNumber::BigNumber j(static_cast<int8_t>(-123), precision);
    EXPECT_EQ("-123", j.to_string());
}

TEST(BigNumberTest, Int16Constructor) {
    BigNumber::BigNumber j(static_cast<int16_t>(-123), precision);
    EXPECT_EQ("-123", j.to_string());
}

TEST(BigNumberTest, Int32Constructor) {
    BigNumber::BigNumber j(static_cast<int32_t>(-123456789), precision);
    EXPECT_EQ("-123456789", j.to_string());
}

TEST(BigNumberTest, Int64Constructor) {
    BigNumber::BigNumber j(-1234567890123456, precision);
    EXPECT_EQ("-1234567890123456", j.to_string());
}

TEST(BigNumberTest, Uint8Constructor) {
    BigNumber::BigNumber j(static_cast<uint8_t>(123), precision);
    EXPECT_EQ("123", j.to_string());
}

TEST(BigNumberTest, Uint16Constructor) {
    BigNumber::BigNumber j(static_cast<uint16_t>(123), precision);
    EXPECT_EQ("123", j.to_string());
}

TEST(BigNumberTest, Uint32Constructor) {
    BigNumber::BigNumber j(static_cast<uint32_t>(123456789), precision);
    EXPECT_EQ("123456789", j.to_string());
}

TEST(BigNumberTest, Uint64Constructor) {
    BigNumber::BigNumber j(1234567890123456, precision);
    EXPECT_EQ("1234567890123456", j.to_string());
}

TEST(BigNumberTest, ClassConstructor) {
    BigNumber::BigNumber j(-1234567890123456, precision);
    BigNumber::BigNumber k(j);
    EXPECT_EQ("-1234567890123456", k.to_string());
}

TEST(BigNumberTest, RvalueConstructor) {
    BigNumber::BigNumber l(BigNumber::BigNumber(-1234567890123456, precision));
    EXPECT_EQ("-1234567890123456", l.to_string());
}

// Getters
TEST(BigNumberTest, SignGetter) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("-12345678901234567890123456789012345678901234567890", precision);
    EXPECT_TRUE(a.is_positive());
    EXPECT_FALSE(a.is_negative());
    EXPECT_TRUE(b.is_negative());
    EXPECT_FALSE(b.is_positive());
}

TEST(BigNumberTest, IsNull) {
    BigNumber::BigNumber a("0", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890", precision);
    EXPECT_TRUE(a.is_null());
    EXPECT_FALSE(b.is_null());
}

// Direct assignment
TEST(BigNumberTest, DirectAssignment) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b = a;
    EXPECT_EQ("12345678901234567890123456789012345678901234567890", b.to_string());

    BigNumber::BigNumber c = BigNumber::BigNumber("-12345678901234567890123456789012345678901234567890", precision);
    EXPECT_EQ("-12345678901234567890123456789012345678901234567890", c.to_string());
}

// Unary minus
TEST(BigNumberTest, Minus) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b = -a;
    EXPECT_EQ("-12345678901234567890123456789012345678901234567890", b.to_string());
}

// Math
TEST(BigNumberTest, Abs) {
    BigNumber::BigNumber a("-12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b = abs(a);
    EXPECT_EQ("12345678901234567890123456789012345678901234567890", b.to_string());
}

TEST(BigNumberTest, Floor) {
    BigNumber::BigNumber a("1234567890123456789.0123456789012345678901234567890", precision);
    BigNumber::BigNumber b = floor(a);
    EXPECT_EQ("1234567890123456789", b.to_string());

    BigNumber::BigNumber c("-123456789012345678901234567890.12345678901234567890", precision);
    BigNumber::BigNumber d = floor(c);
    EXPECT_EQ("-123456789012345678901234567890", d.to_string());
}

TEST(BigNumberTest, Ceil) {
    BigNumber::BigNumber a("1234567890123456789.0123456789012345678901234567890", precision);
    BigNumber::BigNumber b = ceil(a);
    EXPECT_EQ("1234567890123456790", b.to_string());

    BigNumber::BigNumber c("-123456789012345678901234567890.12345678901234567890", precision);
    BigNumber::BigNumber d = ceil(c);
    EXPECT_EQ("-123456789012345678901234567891", d.to_string());

    BigNumber::BigNumber e("12345678901234567890123456789.0", precision);
    BigNumber::BigNumber f = ceil(e);
    EXPECT_EQ("12345678901234567890123456789", f.to_string());

    BigNumber::BigNumber g("-12345678901234567890123456789.0", precision);
    BigNumber::BigNumber h = ceil(g);
    EXPECT_EQ("-12345678901234567890123456789", h.to_string());
}

// TEST(BigNumberTest, Sqrt) {
//     BigNumber::BigNumber a("1522756", precision);
//     BigNumber::BigNumber b = sqrt(a);
//     EXPECT_EQ("1234", b.to_string());
// }

TEST(BigNumberTest, Pow) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b = pow(a, 2);
    EXPECT_EQ("152415787532388367504953515625666819450083828733757049236500533455762536198787501905199875019052100", b
        .to_string());
}

TEST(BigNumberTest, Arctan) {
    BigNumber::BigNumber a("0.5", precision);
    BigNumber::BigNumber b = arctan(a);
    EXPECT_EQ("0.463647609000806", b.to_string());
}

TEST(BigNumberTest, Factorial) {
    BigNumber::BigNumber a("5", precision);
    BigNumber::BigNumber b = factorial(a);
    EXPECT_EQ("120", b.to_string());
}

// Addition and subtraction
TEST(BigNumberTest, Add) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber c = a + b;
    EXPECT_EQ("24691357802469135780246913578024691357802469135780", c.to_string());
}

TEST(BigNumberTest, AddNegative) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("-12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber c = a + b;
    EXPECT_EQ("0", c.to_string());
}

TEST(BigNumberTest, Sub) {
    BigNumber::BigNumber a("123456789012345678901234567890.12345678901234567890", precision);
    BigNumber::BigNumber b("123456789012345678901234567890.12345678901234567890", precision);
    BigNumber::BigNumber c = a - b;
    EXPECT_EQ("0", c.to_string());
}

// Multiplication and division
TEST(BigNumberTest, Mul) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber c = a * b;
    EXPECT_EQ("152415787532388367504953515625666819450083828733757049236500533455762536198787501905199875019052100", c
        .to_string());
}

TEST(BigNumberTest, AssignMul) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890", precision);
    a *= b;
    EXPECT_EQ("152415787532388367504953515625666819450083828733757049236500533455762536198787501905199875019052100", a
        .to_string());
}

TEST(BigNumberTest, Div) {
    BigNumber::BigNumber a("123456789012345678901234567890123456.78901234567890", precision);
    BigNumber::BigNumber b("123456789012345678901234567890123456.78901234567890", precision);
    BigNumber::BigNumber c = a / b;
    EXPECT_EQ("1", c.to_string());
}

// TEST(BigNumberTest, DivByZero) {
//     BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
//     BigNumber::BigNumber b("0", precision);
//     EXPECT_THROW(std::runtime_error, a / b);
// }

// Comparison
TEST(BigNumberTest, Equal) {
    BigNumber::BigNumber a("1234567890123456789012345678901234.5678901234567890", precision);
    BigNumber::BigNumber b("1234567890123456789012345678901234.5678901234567890", precision);
    EXPECT_TRUE(a == b);
}

TEST(BigNumberTest, Greater) {
    BigNumber::BigNumber a("123456789012345.67890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("123456789012345.67890123456789012345678901234567890", precision);
    EXPECT_FALSE(a > b);
}

TEST(BigNumberTest, Less) {
    BigNumber::BigNumber a("12345678901234567890123456789012.345678901234567890", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012.345678901234567890", precision);
    EXPECT_FALSE(a < b);
}

// Adapters
TEST(BigNumberTest, ToString) {
    BigNumber::BigNumber a("1234567890123456789012345678901234567.8901234567890", precision);
    EXPECT_EQ("1234567890123456789012345678901234567.8901234567890", a.to_string());
}

// UD literals
TEST(BigNumberTest, Uint16Literal) {
    BigNumber::BigNumber a = 123_b;
    EXPECT_EQ("123", a.to_string());
}

TEST(BigNumberTest, DoubleLiteral) {
    BigNumber::BigNumber a = 1234.5_b;
    EXPECT_EQ("1234.5", a.to_string());
}


