#include <iostream>

#include "s21_memory/allocator.hpp"

auto print_memory_layout(s21::memory::allocator& allocator) {
  std::cout << "Allocator [" << std::dec << allocator.size() << "]:\n";

  for (auto& block : allocator.blocks()) {
    std::cout << "[" << std::hex << block << "]:\n"
              << "\ttype: " << (int)block->type << "\n"
              << "\tsize: " << std::dec << block->size << "\n"
              << "\tnext: " << std::hex << block->next << "\n";
  }

  std::cout << std::dec << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  auto allocator = s21::memory::allocator(1024);

  allocator.allocate_block(100);
  auto p = allocator.allocate_block(100);
  allocator.allocate_block(100);

  print_memory_layout(allocator);

  allocator.reallocate_block(p, 50);

  print_memory_layout(allocator);

  allocator.allocate_block(800);
}
