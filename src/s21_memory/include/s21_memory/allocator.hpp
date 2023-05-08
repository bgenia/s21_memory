#pragma once

#include <cstddef>
#include <vector>

#include "s21_memory/block.hpp"
#include "s21_memory/heap.hpp"

namespace s21::memory {

class allocator {
 public:
  allocator(std::size_t heap_size);

  auto allocate_block(std::size_t size, block_type type = block_type::char_t)
      -> block_header*;

  auto reallocate_block(block_header* block, std::size_t size) -> block_header*;

  auto free_block(block_header* block) -> void;

  auto merge_free_blocks() -> void;

  auto size() const -> std::size_t;

  auto blocks() const -> std::vector<block_header*>;

 private:
  auto split_block(block_header* block, std::size_t size) -> block_header*;

  auto shrink_block(block_header* block, std::size_t size) -> block_header*;

  auto expand_block(block_header* block, std::size_t size) -> block_header*;

 private:
  heap heap_;

  block_header* root_;
};

}  // namespace s21::memory
