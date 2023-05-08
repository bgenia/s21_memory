#include "s21_memory/block.hpp"

namespace s21::memory {

auto header_of(void* data) -> block_header* {
  return reinterpret_cast<block_header*>(data) - 1;
}

auto data_of(block_header* header) -> raw_ptr {
  return reinterpret_cast<raw_ptr>(header + 1);
}

}  // namespace s21::memory
