#include "s21_memory/heap.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "s21_memory/block.hpp"

using namespace testing;

TEST(heap_constructor, should_allocate_exact_size) {
  auto heap = s21::memory::heap(s21::memory::word_size);

  auto size = heap.size();

  EXPECT_GE(size, s21::memory::word_size);
}
