#include "s21_memory/heap.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "s21_memory/block.hpp"

using namespace std::string_literals;
using namespace testing;

TEST(heap_constructor,
     should_initialize_heap_large_enough_for_at_lest_one_block_header) {
  auto heap = s21::memory::heap(0);

  auto size = heap.size();

  EXPECT_GE(size, s21::memory::block_size_of(0));
}
