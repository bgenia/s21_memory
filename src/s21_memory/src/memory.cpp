#include <cstring>
#include <new>
#include <optional>

#include "s21_memory.h"
#include "s21_memory.hpp"
#include "s21_memory/allocator.hpp"
#include "s21_memory/block.hpp"

#ifndef S21_MEMORY_DEFAULT_HEAP_SIZE
#define S21_MEMORY_DEFAULT_HEAP_SIZE 4096
#endif

namespace s21 {

namespace memory::internal {

std::optional<memory::allocator> default_allocator = std::nullopt;

}

auto set_heap(std::size_t size) -> void {
  memory::internal::default_allocator = memory::allocator(size);
}

auto malloc(std::size_t size) -> void* {
  if (!memory::internal::default_allocator) {
    set_heap(S21_MEMORY_DEFAULT_HEAP_SIZE);
  }

  try {
    auto block = memory::internal::default_allocator->allocate_block(size);

    return memory::data_of(block);
  } catch (std::bad_alloc&) {
    return nullptr;
  }
}

auto calloc(std::size_t n, std::size_t size) -> void* {
  if (!memory::internal::default_allocator) {
    set_heap(S21_MEMORY_DEFAULT_HEAP_SIZE);
  }

  if (n == 0 || size == 0) {
    return nullptr;
  }

  std::size_t size_;

  if (!__builtin_mul_overflow(n, size, &size_)) {
    return nullptr;
  }

  auto result = malloc(size_);

  if (!result) {
    return result;
  }

  std::memset(result, 0, size_);

  return result;
}

auto realloc(void* block, std::size_t size) -> void* {
  if (!memory::internal::default_allocator) {
    set_heap(S21_MEMORY_DEFAULT_HEAP_SIZE);
  }

  try {
    auto result = memory::internal::default_allocator->reallocate_block(
        memory::header_of(block), size);

    return memory::data_of(result);
  } catch (std::bad_alloc&) {
    return nullptr;
  }
}

auto free(void* block) -> void {
  if (!memory::internal::default_allocator) {
    set_heap(S21_MEMORY_DEFAULT_HEAP_SIZE);
  }

  memory::internal::default_allocator->free_block(memory::header_of(block));
}

}  // namespace s21

auto s21_malloc(size_t size) -> void* { return s21::malloc(size); }

auto s21_calloc(size_t n, size_t size) -> void* { return s21::calloc(n, size); }

auto s21_realloc(void* block, size_t size) -> void* {
  return s21::realloc(block, size);
}

auto s21_free(void* block) -> void { return s21::free(block); }
