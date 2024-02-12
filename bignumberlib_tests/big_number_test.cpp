#include "gtest/gtest.h"
#include "big_number.h"

#include <iostream>
#include <stdexcept>

const uint64_t precision = 100 * 64;

TEST(BigNumberTest, ToString) {
    BigNumber::BigNumber a("1234567890123456789012345678901234567.8901234567890", precision);
    EXPECT_EQ("1234567890123456789012345678901234567.8901234567890", a.to_string());
}

TEST(BigNumberTest, Add) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber c = a + b;
    EXPECT_EQ("24691357802469135780246913578024691357802469135780", c.to_string());
}

TEST(BigNumberTest, Sub) {
    BigNumber::BigNumber a("123456789012345678901234567890.12345678901234567890", precision);
    BigNumber::BigNumber b("123456789012345678901234567890.12345678901234567890", precision);
    BigNumber::BigNumber c = a - b;
    EXPECT_EQ("0", c.to_string());
}

TEST(BigNumberTest, Mul) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber c = a * b;
    EXPECT_EQ( "15241578769632126357653440751670000000000000000000", c.to_string());
}

TEST(BigNumberTest, Div) {
    BigNumber::BigNumber a("123456789012345678901234567890123456.78901234567890", precision);
    BigNumber::BigNumber b("123456789012345678901234567890123456.78901234567890", precision);
    BigNumber::BigNumber c = a / b;
    EXPECT_EQ("1", c.to_string());
    // EXPECT_TRUE(a == b);
}

/*TEST(BigNumberTest, DivByZero) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b("0", precision);
    EXPECT_THROW(std::runtime_error, a / b);
}*/

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

TEST(BiNumberTest, Abs) {
    BigNumber::BigNumber a("-12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b = abs(a);
    EXPECT_EQ("12345678901234567890123456789012345678901234567890", b.to_string());
}

TEST(BigNumberTest, Pow) {
    BigNumber::BigNumber a("12345678901234567890123456789012345678901234567890", precision);
    BigNumber::BigNumber b = pow(a, 2);
    EXPECT_EQ("15241578769632126357653440751670000000000000000000", b.to_string());
}

TEST(BigNumberTest, Arctan) {
    BigNumber::BigNumber a("1", precision);
    BigNumber::BigNumber b = arctan(a);
    EXPECT_EQ("0.7853981633974483", b.to_string());
}
