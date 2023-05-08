#include "s21_memory/allocator.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

#include "s21_memory/block.hpp"

namespace s21::memory {

allocator::allocator(std::size_t heap_size)
    : heap_(sizeof(block_header) + heap_size),
      root_(new (heap_.data()) block_header(block_type::free, heap_size)) {}

auto allocator::merge_free_blocks() -> void {
  auto current = root_;
  auto next = root_->next;

  while (next) {
    if (current->type != block_type::free || next->type != block_type::free) {
      current = next;
      next = current->next;

      continue;
    }

    current->size += next->size + sizeof(block_header);
    current->next = next->next;

    next = current->next;
  }
}

auto allocator::split_block(block_header* block, std::size_t size)
    -> block_header* {
  auto next_block = new (data_of(block) + size)
      block_header(block_type::free, block->size - size - sizeof(block_header));

  next_block->next = block->next;

  block->size = size;
  block->next = next_block;

  return next_block;
}

auto allocator::allocate_block(std::size_t size, block_type type)
    -> block_header* {
  merge_free_blocks();

  auto aligned_size = align_of(size);

  auto block = root_;

  for (; block; block = block->next) {
    if (block->type != block_type::free || block->size < aligned_size) {
      continue;
    }

    if (block->size > aligned_size + sizeof(block_header)) {
      split_block(block, aligned_size);
    }

    block->type = type;

    return block;
  }

  throw std::bad_alloc();
}

auto allocator::shrink_block(block_header* block, std::size_t size)
    -> block_header* {
  auto next_block = split_block(block, size);

  free_block(next_block);

  return block;
}

auto allocator::expand_block(block_header* block, std::size_t size)
    -> block_header* {
  merge_free_blocks();

  auto next_block = block->next;

  if (next_block && next_block->type == block_type::free) {
    auto next_size = block->size + next_block->size + sizeof(block_header);

    if (next_size >= size) {
      block->next = next_block->next;
      block->size = next_size;

      if (block->size > size + sizeof(block_header)) {
        split_block(block, size);
      }

      return block;
    }
  }

  next_block = allocate_block(size, block->type);

  std::memcpy(data_of(next_block), data_of(block), block->size);

  free_block(block);

  return next_block;
}

auto allocator::reallocate_block(block_header* block, std::size_t size)
    -> block_header* {
  if (!block) {
    return nullptr;
  }

  if (size == 0) {
    free_block(block);
    return nullptr;
  }

  auto aligned_size = align_of(size);

  if (aligned_size < block->size) {
    return shrink_block(block, aligned_size);
  }

  if (aligned_size > block->size) {
    return expand_block(block, aligned_size);
  }

  return block;
}

auto allocator::free_block(block_header* block) -> void {
  if (!block) {
    return;
  }

  block->type = block_type::free;
}

auto allocator::size() const -> std::size_t { return heap_.size(); }

auto allocator::blocks() const -> std::vector<block_header*> {
  auto result = std::vector<block_header*>();

  for (auto block = root_; block; block = block->next) {
    result.push_back(block);
  }

  return result;
}

}  // namespace s21::memory
