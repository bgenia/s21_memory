#include "s21_memory/allocator.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <new>

#include "s21_memory/block.hpp"

using namespace std::string_literals;
using namespace testing;

TEST(allocator_constructor,
     should_allocate_heap_large_enough_for_at_lest_one_block_header) {
  auto allocator = s21::memory::allocator(0);

  EXPECT_GE(allocator.size(), s21::memory::block_size_of(0));
}

TEST(allocator_allocate_block, should_set_block_type_to_char_by_default) {
  auto allocator = s21::memory::allocator(0);

  auto block = allocator.allocate_block(0);

  EXPECT_EQ(block->type, s21::memory::block_type::char_t);
}

TEST(allocator_allocate_block, should_align_block_size) {
  for (auto i = 0ul; i < s21::memory::word_size; i++) {
    auto allocator = s21::memory::allocator(s21::memory::word_size * 3);

    auto size = s21::memory::word_size + i;

    auto block = allocator.allocate_block(size);

    EXPECT_EQ(block->size % s21::memory::word_size, 0);
  }
}

TEST(allocator_allocate_block, should_throw_bad_alloc_if_out_of_memory) {
  auto allocator = s21::memory::allocator(0);

  EXPECT_THROW(allocator.allocate_block(1), std::bad_alloc);
}

TEST(allocator_allocate_block, should_set_block_type) {
  auto block_types = {
      s21::memory::block_type::free, s21::memory::block_type::char_t,
      s21::memory::block_type::int_t, s21::memory::block_type::double_t};

  for (auto type : block_types) {
    auto allocator = s21::memory::allocator(0);

    auto block = allocator.allocate_block(0, type);

    EXPECT_EQ(block->type, type);
  }
}

TEST(allocator_free_block, should_set_block_type_to_free) {
  auto allocator = s21::memory::allocator(0);

  auto block = allocator.allocate_block(0);

  allocator.free_block(block);

  EXPECT_EQ(block->type, s21::memory::block_type::free);
}

TEST(allocator_free_block, should_not_throw_on_nullptr) {
  auto allocator = s21::memory::allocator(0);

  auto block = nullptr;

  EXPECT_NO_THROW(allocator.free_block(block));
}

TEST(allocator_free_block, should_not_fail_on_nullptr) {
  auto allocator = s21::memory::allocator(0);

  auto block = nullptr;

  EXPECT_NO_FATAL_FAILURE(allocator.free_block(block));
}

TEST(allocator_merge_free_blocks, should_merge_free_blocks) {
  auto allocator = s21::memory::allocator(128);

  auto block1 = allocator.allocate_block(0);
  auto block2 = allocator.allocate_block(0);

  auto size1 = block1->size;
  auto size2 = block2->size;

  allocator.free_block(block1);
  allocator.free_block(block2);

  allocator.merge_free_blocks();

  EXPECT_GE(block1->size, size1 + s21::memory::block_size_of(size2));
}

TEST(allocator_reallocate_block, should_ignore_nullptr) {
  auto allocator = s21::memory::allocator(0);

  auto result = allocator.reallocate_block(nullptr, 0);

  EXPECT_EQ(result, nullptr);
}

TEST(allocator_reallocate_block, should_free_block_when_resized_to_zero) {
  auto allocator = s21::memory::allocator(0);

  auto block = allocator.allocate_block(0);

  allocator.reallocate_block(block, 0);

  EXPECT_EQ(block->type, s21::memory::block_type::free);
}

TEST(allocator_reallocate_block,
     should_do_nothing_when_aligned_size_is_the_same_but_not_zero) {
  auto allocator = s21::memory::allocator(s21::memory::word_size);

  auto block = allocator.allocate_block(s21::memory::word_size);
  auto block_copy = *block;

  for (auto i = 0ul; i < s21::memory::word_size; i++) {
    auto reallocated_block = allocator.reallocate_block(block, block->size - i);

    EXPECT_EQ(block, reallocated_block);
    EXPECT_EQ(reallocated_block->size, block_copy.size);
    EXPECT_EQ(reallocated_block->type, block_copy.type);
    EXPECT_EQ(reallocated_block->next, block_copy.next);
  }
}

TEST(
    allocator_reallocate_block,
    should_do_nothing_when_aligned_size_is_lower_but_there_is_not_enough_space_for_another_block) {
  auto allocator = s21::memory::allocator(256);

  auto block = allocator.allocate_block(100);
  auto block_copy = *block;

  auto new_size = block->size - s21::memory::block_size_of(0) + 1;

  allocator.allocate_block(0);

  auto reallocated_block = allocator.reallocate_block(block, new_size);

  EXPECT_EQ(block, reallocated_block);
  EXPECT_EQ(reallocated_block->size, block_copy.size);
  EXPECT_EQ(reallocated_block->type, block_copy.type);
  EXPECT_EQ(reallocated_block->next, block_copy.next);
}

TEST(
    allocator_reallocate_block,
    should_shrink_block_when_aligned_size_is_lower_by_at_least_one_block_size_and_not_zero) {
  auto allocator = s21::memory::allocator(256);

  auto block = allocator.allocate_block(100);
  auto block_copy = *block;

  auto new_size = block->size - s21::memory::block_size_of(0);

  allocator.allocate_block(0);

  auto reallocated_block = allocator.reallocate_block(block, new_size);

  EXPECT_EQ(reallocated_block->size, new_size);
  EXPECT_EQ(reallocated_block->type, block_copy.type);
  EXPECT_NE(reallocated_block->next, block_copy.next);
}

TEST(
    allocator_reallocate_block,
    should_merge_with_next_block_if_size_is_greater_and_enough_space_is_available) {
  auto allocator = s21::memory::allocator(256);

  auto old_size = 10;

  auto block = allocator.allocate_block(old_size);
  auto block_copy = *block;

  auto new_size = block->size + 10;

  auto reallocated_block = allocator.reallocate_block(block, new_size);

  EXPECT_EQ(block, reallocated_block);
  EXPECT_EQ(reallocated_block->size, s21::memory::align_of(new_size));
  EXPECT_EQ(block_copy.type, reallocated_block->type);
  EXPECT_NE(block_copy.next, reallocated_block->next);
}

TEST(
    allocator_reallocate_block,
    should_allocate_bigger_block_if_size_is_greater_and_merging_is_not_available) {
  auto allocator = s21::memory::allocator(256);

  auto old_size = 10;

  auto block = allocator.allocate_block(old_size);
  auto block_copy = *block;

  allocator.allocate_block(0);

  auto new_size = block->size + 10;

  auto reallocated_block = allocator.reallocate_block(block, new_size);

  EXPECT_NE(block, reallocated_block);
  EXPECT_EQ(reallocated_block->size, s21::memory::align_of(new_size));
  EXPECT_EQ(block_copy.type, reallocated_block->type);
  EXPECT_NE(block_copy.next, reallocated_block->next);
  EXPECT_EQ(block->type, s21::memory::block_type::free);
}

TEST(allocator_blocks, should_return_block_vector) {
  auto allocator = s21::memory::allocator(256);

  auto blocks = std::vector{
      allocator.allocate_block(0), allocator.allocate_block(0),
      allocator.allocate_block(0), allocator.allocate_block(0),
      allocator.allocate_block(0), allocator.allocate_block(0),
  };

  blocks.push_back(blocks.back()->next);

  auto result = allocator.blocks();

  EXPECT_EQ(blocks.size(), result.size());

  for (auto i = 0ul; i < blocks.size(); i++) {
    EXPECT_EQ(blocks[i], result[i]);
  }
}
