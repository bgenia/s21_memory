#include "s21_memory/block.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using namespace std::string_literals;
using namespace testing;

TEST(align_of, zero_should_remain_zero) {
  auto value = 0;

  auto result = s21::memory::align_of(value);

  EXPECT_EQ(result, 0);
}

TEST(align_of, aligned_values_should_remain_the_same) {
  auto value = s21::memory::word_size;

  auto result = s21::memory::align_of(value);

  EXPECT_EQ(result, value);
}

TEST(align_of, not_aligned_values_should_become_aligned) {
  auto aligned = s21::memory::word_size;
  auto value = aligned + 1;

  auto result = s21::memory::align_of(value);

  EXPECT_EQ(result, aligned + s21::memory::word_size);
}

TEST(block_size_of, should_append_block_header_size) {
  auto size = 1;

  auto result = s21::memory::block_size_of(size);

  EXPECT_EQ(result, size + sizeof(s21::memory::block_header));
}

TEST(header_of, should_subtract_header_size) {
  s21::memory::raw_ptr block = 0x0;

  auto data = block + s21::memory::block_size_of(0);

  auto result = s21::memory::header_of(data);

  EXPECT_EQ(reinterpret_cast<s21::memory::raw_ptr>(result),
            reinterpret_cast<s21::memory::raw_ptr>(block));
}

TEST(data_of, should_add_header_size) {
  s21::memory::raw_ptr block = 0x0;

  auto data = block + s21::memory::block_size_of(0);

  auto result =
      s21::memory::data_of(reinterpret_cast<s21::memory::block_header*>(block));

  EXPECT_EQ(reinterpret_cast<s21::memory::raw_ptr>(result),
            reinterpret_cast<s21::memory::raw_ptr>(data));
}
