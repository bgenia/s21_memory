#pragma once

#include <cstddef>

namespace s21::memory {

using raw_byte = unsigned char;
using raw_ptr = raw_byte*;

constexpr auto word_size = sizeof(std::size_t);

constexpr auto align_of(std::size_t n) {
  return n + (word_size - n % word_size) % word_size;
}

enum class block_type { free, char_t, int_t, double_t };

struct alignas(word_size) block_header {
  block_type type;
  std::size_t size;

  block_header* next = nullptr;

  block_header(block_type type, std::size_t size) : type(type), size(size) {}
};

constexpr auto block_size_of(std::size_t n) {
  return align_of(n) + sizeof(block_header);
}

auto header_of(raw_ptr data) -> block_header*;

auto data_of(block_header* header) -> raw_ptr;

}  // namespace s21::memory
