#pragma once

#include <optional>

#include "s21_memory/allocator.hpp"

namespace s21 {

namespace memory::internal {

extern std::optional<memory::allocator> default_allocator;

}  // namespace memory::internal

/**
 * @brief Initializes new default heap for *alloc functions
 * @warning Invalidates all existing heap data
 * @param size Heap size
 */
auto set_heap(std::size_t size) -> void;

auto malloc(std::size_t size) -> void*;
auto calloc(std::size_t n, std::size_t size) -> void*;
auto realloc(void* block, std::size_t size) -> void*;
auto free(void* block) -> void;

}  // namespace s21
