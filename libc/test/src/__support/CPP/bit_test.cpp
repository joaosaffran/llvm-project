//===-- Unittests for Bit -------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/stdint_proxy.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/big_int.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/types.h" // LIBC_TYPES_HAS_INT128
#include "test/UnitTest/Test.h"

namespace LIBC_NAMESPACE_DECL {
namespace cpp {

using UnsignedTypes = testing::TypeList<
#if defined(LIBC_TYPES_HAS_INT128)
    __uint128_t,
#endif // LIBC_TYPES_HAS_INT128
    unsigned char, unsigned short, unsigned int, unsigned long,
    unsigned long long, UInt<128>>;

#ifdef FAKE_MACRO_DISABLE
TYPED_TEST(LlvmLibcBitTest, HasSingleBit, UnsignedTypes) {
  constexpr auto ZERO = T(0);
  constexpr auto ALL_ONES = T(~ZERO);
  EXPECT_FALSE(has_single_bit<T>(ZERO));
  EXPECT_FALSE(has_single_bit<T>(ALL_ONES));

  for (T value = 1; value; value <<= 1)
    EXPECT_TRUE(has_single_bit<T>(value));

  // We test that if two bits are set has_single_bit returns false.
  // We do this by setting the highest or lowest bit depending or where the
  // current bit is. This is a bit convoluted but it helps catch a bug on BigInt
  // where we have to work on an element-by-element basis.
  constexpr auto MIDPOINT = T(ALL_ONES / 2);
  constexpr auto LSB = T(1);
  constexpr auto MSB = T(~(ALL_ONES >> 1));
  for (T value = 1; value; value <<= 1) {
    T two_bits_value =
        static_cast<T>(value | ((value <= MIDPOINT) ? MSB : LSB));
    EXPECT_FALSE(has_single_bit<T>(two_bits_value));
  }
}
#endif

TYPED_TEST(LlvmLibcBitTest, CountLZero, UnsignedTypes) {
  EXPECT_EQ(countl_zero<T>(T(0)), cpp::numeric_limits<T>::digits);
  int expected = 0;
  for (T value = T(~0); value; value >>= 1, ++expected)
    EXPECT_EQ(countl_zero<T>(value), expected);
}

TYPED_TEST(LlvmLibcBitTest, CountRZero, UnsignedTypes) {
  EXPECT_EQ(countr_zero<T>(T(0)), cpp::numeric_limits<T>::digits);
  int expected = 0;
  for (T value = T(~0); value; value <<= 1, ++expected)
    EXPECT_EQ(countr_zero<T>(value), expected);
}

TYPED_TEST(LlvmLibcBitTest, CountLOne, UnsignedTypes) {
  EXPECT_EQ(countl_one<T>(T(0)), 0);
  int expected = cpp::numeric_limits<T>::digits;
  for (T value = T(~0); value; value <<= 1, --expected)
    EXPECT_EQ(countl_one<T>(value), expected);
}

TYPED_TEST(LlvmLibcBitTest, CountROne, UnsignedTypes) {
  EXPECT_EQ(countr_one<T>(T(0)), 0);
  int expected = cpp::numeric_limits<T>::digits;
  for (T value = T(~0); value; value >>= 1, --expected)
    EXPECT_EQ(countr_one<T>(value), expected);
}

TYPED_TEST(LlvmLibcBitTest, BitWidth, UnsignedTypes) {
  EXPECT_EQ(bit_width<T>(T(0)), 0);
  int one_index = 0;
  for (T value = 1; value; value <<= 1, ++one_index)
    EXPECT_EQ(bit_width<T>(value), one_index + 1);
}

TEST(LlvmLibcBitTest, BitCeil) {
  EXPECT_EQ(uint8_t(1), bit_ceil(uint8_t(0)));
  EXPECT_EQ(uint16_t(1), bit_ceil(uint16_t(0)));
  EXPECT_EQ(uint32_t(1), bit_ceil(uint32_t(0)));
  EXPECT_EQ(uint64_t(1), bit_ceil(uint64_t(0)));

  EXPECT_EQ(uint8_t(1), bit_ceil(uint8_t(1)));
  EXPECT_EQ(uint16_t(1), bit_ceil(uint16_t(1)));
  EXPECT_EQ(uint32_t(1), bit_ceil(uint32_t(1)));
  EXPECT_EQ(uint64_t(1), bit_ceil(uint64_t(1)));

  EXPECT_EQ(uint8_t(2), bit_ceil(uint8_t(2)));
  EXPECT_EQ(uint16_t(2), bit_ceil(uint16_t(2)));
  EXPECT_EQ(uint32_t(2), bit_ceil(uint32_t(2)));
  EXPECT_EQ(uint64_t(2), bit_ceil(uint64_t(2)));

  EXPECT_EQ(uint8_t(4), bit_ceil(uint8_t(3)));
  EXPECT_EQ(uint16_t(4), bit_ceil(uint16_t(3)));
  EXPECT_EQ(uint32_t(4), bit_ceil(uint32_t(3)));
  EXPECT_EQ(uint64_t(4), bit_ceil(uint64_t(3)));

  EXPECT_EQ(uint8_t(4), bit_ceil(uint8_t(4)));
  EXPECT_EQ(uint16_t(4), bit_ceil(uint16_t(4)));
  EXPECT_EQ(uint32_t(4), bit_ceil(uint32_t(4)));
  EXPECT_EQ(uint64_t(4), bit_ceil(uint64_t(4)));

  // The result is the representable power of 2 for each type.
  EXPECT_EQ(uint8_t(0x80), bit_ceil(uint8_t(0x7f)));
  EXPECT_EQ(uint16_t(0x8000), bit_ceil(uint16_t(0x7fff)));
  EXPECT_EQ(uint32_t(0x80000000), bit_ceil(uint32_t(0x7fffffff)));
  EXPECT_EQ(uint64_t(0x8000000000000000),
            bit_ceil(uint64_t(0x7fffffffffffffff)));
}

TEST(LlvmLibcBitTest, BitFloor) {
  EXPECT_EQ(uint8_t(0), bit_floor(uint8_t(0)));
  EXPECT_EQ(uint16_t(0), bit_floor(uint16_t(0)));
  EXPECT_EQ(uint32_t(0), bit_floor(uint32_t(0)));
  EXPECT_EQ(uint64_t(0), bit_floor(uint64_t(0)));

  EXPECT_EQ(uint8_t(1), bit_floor(uint8_t(1)));
  EXPECT_EQ(uint16_t(1), bit_floor(uint16_t(1)));
  EXPECT_EQ(uint32_t(1), bit_floor(uint32_t(1)));
  EXPECT_EQ(uint64_t(1), bit_floor(uint64_t(1)));

  EXPECT_EQ(uint8_t(2), bit_floor(uint8_t(2)));
  EXPECT_EQ(uint16_t(2), bit_floor(uint16_t(2)));
  EXPECT_EQ(uint32_t(2), bit_floor(uint32_t(2)));
  EXPECT_EQ(uint64_t(2), bit_floor(uint64_t(2)));

  EXPECT_EQ(uint8_t(2), bit_floor(uint8_t(3)));
  EXPECT_EQ(uint16_t(2), bit_floor(uint16_t(3)));
  EXPECT_EQ(uint32_t(2), bit_floor(uint32_t(3)));
  EXPECT_EQ(uint64_t(2), bit_floor(uint64_t(3)));

  EXPECT_EQ(uint8_t(4), bit_floor(uint8_t(4)));
  EXPECT_EQ(uint16_t(4), bit_floor(uint16_t(4)));
  EXPECT_EQ(uint32_t(4), bit_floor(uint32_t(4)));
  EXPECT_EQ(uint64_t(4), bit_floor(uint64_t(4)));

  EXPECT_EQ(uint8_t(0x40), bit_floor(uint8_t(0x7f)));
  EXPECT_EQ(uint16_t(0x4000), bit_floor(uint16_t(0x7fff)));
  EXPECT_EQ(uint32_t(0x40000000), bit_floor(uint32_t(0x7fffffff)));
  EXPECT_EQ(uint64_t(0x4000000000000000),
            bit_floor(uint64_t(0x7fffffffffffffffull)));

  EXPECT_EQ(uint8_t(0x80), bit_floor(uint8_t(0x80)));
  EXPECT_EQ(uint16_t(0x8000), bit_floor(uint16_t(0x8000)));
  EXPECT_EQ(uint32_t(0x80000000), bit_floor(uint32_t(0x80000000)));
  EXPECT_EQ(uint64_t(0x8000000000000000),
            bit_floor(uint64_t(0x8000000000000000)));

  EXPECT_EQ(uint8_t(0x80), bit_floor(uint8_t(0xff)));
  EXPECT_EQ(uint16_t(0x8000), bit_floor(uint16_t(0xffff)));
  EXPECT_EQ(uint32_t(0x80000000), bit_floor(uint32_t(0xffffffff)));
  EXPECT_EQ(uint64_t(0x8000000000000000),
            bit_floor(uint64_t(0xffffffffffffffff)));
}

TYPED_TEST(LlvmLibcBitTest, RotateIsInvariantForZeroAndOne, UnsignedTypes) {
  constexpr T all_zeros = T(0);
  constexpr T all_ones = T(~0);
  for (int i = 0; i < cpp::numeric_limits<T>::digits; ++i) {
    EXPECT_EQ(rotl<T>(all_zeros, i), all_zeros);
    EXPECT_EQ(rotl<T>(all_ones, i), all_ones);
    EXPECT_EQ(rotr<T>(all_zeros, i), all_zeros);
    EXPECT_EQ(rotr<T>(all_ones, i), all_ones);
  }
}

TEST(LlvmLibcBitTest, Rotl) {
  EXPECT_EQ(uint8_t(0x53U), rotl<uint8_t>(0x53, 0));
  EXPECT_EQ(uint8_t(0x4dU), rotl<uint8_t>(0x53, 2));
  EXPECT_EQ(uint8_t(0xa6U), rotl<uint8_t>(0x53, 9));
  EXPECT_EQ(uint8_t(0x9aU), rotl<uint8_t>(0x53, -5));

  EXPECT_EQ(uint16_t(0xabcdU), rotl<uint16_t>(0xabcd, 0));
  EXPECT_EQ(uint16_t(0xf36aU), rotl<uint16_t>(0xabcd, 6));
  EXPECT_EQ(uint16_t(0xaf36U), rotl<uint16_t>(0xabcd, 18));
  EXPECT_EQ(uint16_t(0xf36aU), rotl<uint16_t>(0xabcd, -10));

  EXPECT_EQ(uint32_t(0xdeadbeefU), rotl<uint32_t>(0xdeadbeef, 0));
  EXPECT_EQ(uint32_t(0x7ddfbd5bU), rotl<uint32_t>(0xdeadbeef, 17));
  EXPECT_EQ(uint32_t(0x5b7ddfbdU), rotl<uint32_t>(0xdeadbeef, 41));
  EXPECT_EQ(uint32_t(0xb6fbbf7aU), rotl<uint32_t>(0xdeadbeef, -22));

  EXPECT_EQ(uint64_t(0x12345678deadbeefULL),
            rotl<uint64_t>(0x12345678deadbeefULL, 0));
  EXPECT_EQ(uint64_t(0xf56df77891a2b3c6ULL),
            rotl<uint64_t>(0x12345678deadbeefULL, 35));
  EXPECT_EQ(uint64_t(0x8d159e37ab6fbbc4ULL),
            rotl<uint64_t>(0x12345678deadbeefULL, 70));
  EXPECT_EQ(uint64_t(0xb7dde2468acf1bd5ULL),
            rotl<uint64_t>(0x12345678deadbeefULL, -19));
}

TEST(LlvmLibcBitTest, Rotr) {
  EXPECT_EQ(uint8_t(0x53U), rotr<uint8_t>(0x53, 0));
  EXPECT_EQ(uint8_t(0xd4U), rotr<uint8_t>(0x53, 2));
  EXPECT_EQ(uint8_t(0xa9U), rotr<uint8_t>(0x53, 9));
  EXPECT_EQ(uint8_t(0x6aU), rotr<uint8_t>(0x53, -5));

  EXPECT_EQ(uint16_t(0xabcdU), rotr<uint16_t>(0xabcd, 0));
  EXPECT_EQ(uint16_t(0x36afU), rotr<uint16_t>(0xabcd, 6));
  EXPECT_EQ(uint16_t(0x6af3U), rotr<uint16_t>(0xabcd, 18));
  EXPECT_EQ(uint16_t(0x36afU), rotr<uint16_t>(0xabcd, -10));

  EXPECT_EQ(uint32_t(0xdeadbeefU), rotr<uint32_t>(0xdeadbeef, 0));
  EXPECT_EQ(uint32_t(0xdf77ef56U), rotr<uint32_t>(0xdeadbeef, 17));
  EXPECT_EQ(uint32_t(0x77ef56dfU), rotr<uint32_t>(0xdeadbeef, 41));
  EXPECT_EQ(uint32_t(0xbbf7ab6fU), rotr<uint32_t>(0xdeadbeef, -22));

  EXPECT_EQ(uint64_t(0x12345678deadbeefULL),
            rotr<uint64_t>(0x12345678deadbeefULL, 0));
  EXPECT_EQ(uint64_t(0x1bd5b7dde2468acfULL),
            rotr<uint64_t>(0x12345678deadbeefULL, 35));
  EXPECT_EQ(uint64_t(0xbc48d159e37ab6fbULL),
            rotr<uint64_t>(0x12345678deadbeefULL, 70));
  EXPECT_EQ(uint64_t(0xb3c6f56df77891a2ULL),
            rotr<uint64_t>(0x12345678deadbeefULL, -19));
}

TYPED_TEST(LlvmLibcBitTest, CountOnes, UnsignedTypes) {
  EXPECT_EQ(popcount(T(0)), 0);
  for (int i = 0; i != cpp::numeric_limits<T>::digits; ++i)
    EXPECT_EQ(
        popcount<T>(cpp::numeric_limits<T>::max() >> static_cast<size_t>(i)),
        cpp::numeric_limits<T>::digits - i);
}

} // namespace cpp
} // namespace LIBC_NAMESPACE_DECL
