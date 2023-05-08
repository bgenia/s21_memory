#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "s21_memory.h"
#include "s21_memory.hpp"
#include "s21_memory/block.hpp"

std::unordered_map<s21::memory::block_type, std::size_t> type_sizes = {
    {s21::memory::block_type::char_t, sizeof(char)},
    {s21::memory::block_type::int_t, sizeof(int)},
    {s21::memory::block_type::double_t, sizeof(double)},
};

std::unordered_map<std::string, s21::memory::block_type> type_names = {
    {"char", s21::memory::block_type::char_t},
    {"int", s21::memory::block_type::int_t},
    {"double", s21::memory::block_type::double_t},
};

auto print_block_info(s21::memory::block_header* block) {
  void* data_address = s21::memory::data_of(block);

  std::cout << "[ " << std::hex << data_address << " ]:\n"
            << "\ttype: " << static_cast<int>(block->type) << "\n"
            << "\tsize: " << std::dec << block->size << "\n";

  if (block->type == s21::memory::block_type::free) {
    return;
  }

  auto element_size = type_sizes[block->type];

  auto start = s21::memory::data_of(block);
  auto end = start + block->size;

  auto length = (end - start) / element_size;

  std::cout << "\tcontent: [" << std::dec << length << "] { ";

  for (auto i = start; i < end; i += element_size) {
    switch (block->type) {
      case s21::memory::block_type::char_t: {
        auto value = reinterpret_cast<char*>(i);

        std::cout << "'" << *value << "' ";

        break;
      }
      case s21::memory::block_type::int_t: {
        auto value = reinterpret_cast<int*>(i);

        std::cout << *value << " ";

        break;
      }
      case s21::memory::block_type::double_t: {
        auto value = reinterpret_cast<double*>(i);

        std::cout << *value << " ";

        break;
      }
      default:
        break;
    }
  }

  std::cout << "}\n";
}

auto print_memory_layout(s21::memory::allocator& allocator) {
  std::cout << "heap layout [" << std::dec << allocator.size() << "]:\n";

  for (auto& block : allocator.blocks()) {
    print_block_info(block);
  }

  std::cout << std::dec << std::endl;
}

auto handle_help(std::istringstream&) {
  std::cout
      << "available commands:\n"
         "\tset_heap <size> - allocates a new heap with the specified size\n"
         "\theap - displays current heap layout\n"
         "\tblock <address> - displays info about the specified memory block\n"
         "\tmalloc <size> - calls s21_malloc for current heap\n"
         "\tcalloc <n> <size> - calls s21_calloc for current heap\n"
         "\trealloc <address> <size> - calls s21_realloc for current heap\n"
         "\tfree <address> - calls s21_free for current heap\n"
         "\tset <address> <type> = <value> - assigns a single value by the "
         "specified address\n"
         "\tset <address> <type> [] <length> [values...] - assigns an array of "
         "values by the specified address\n"
         "\thelp - displays this message\n"
         "\texit - exits the repl\n"
      << std::endl;
}

auto handle_set_heap(std::istringstream& argv) {
  std::size_t size;

  argv >> size;

  s21::set_heap(size);

  std::cout << "ok " << size << std::endl;
}

auto handle_heap(std::istringstream&) {
  if (!s21::memory::internal::default_allocator) {
    std::cout << "(none)" << std::endl;

    return;
  }

  print_memory_layout(*s21::memory::internal::default_allocator);
}

auto handle_block(std::istringstream& argv) {
  void* address;

  argv >> address;

  print_block_info(s21::memory::header_of(address));
}

auto handle_malloc(std::istringstream& argv) {
  std::size_t size;

  argv >> size;

  auto result = s21_malloc(size);

  std::cout << "ok " << std::hex << result << std::endl;
}

auto handle_calloc(std::istringstream& argv) {
  std::size_t n;
  std::size_t size;

  argv >> n >> size;

  auto result = s21_calloc(n, size);

  std::cout << "ok " << std::hex << result << std::endl;
}

auto handle_realloc(std::istringstream& argv) {
  void* address;
  std::size_t size;

  argv >> address >> size;

  auto result = s21_realloc(address, size);

  std::cout << "ok " << std::hex << result << std::endl;
}

auto handle_free(std::istringstream& argv) {
  void* address;

  argv >> address;

  s21_free(address);

  std::cout << "ok " << std::hex << address << std::endl;
}

auto set_value(void* address, std::string_view type, std::istringstream& argv) {
  if (type == "char") {
    char value;

    argv >> value;

    *reinterpret_cast<char*>(address) = value;

    return;
  }

  if (type == "int") {
    int value;

    argv >> value;

    *reinterpret_cast<int*>(address) = value;

    return;
  }

  if (type == "double") {
    double value;

    argv >> value;

    *reinterpret_cast<double*>(address) = value;

    return;
  }
}

auto handle_set(std::istringstream& argv) {
  void* address;

  argv >> address;

  std::string type;
  std::string mode;

  argv >> type >> mode;

  if (type_names.count(type) == 0) {
    std::cout << "invalid type '" << type << "'" << std::endl;
    return;
  }

  auto header = s21::memory::header_of(address);

  if (mode == "=") {
    set_value(address, type, argv);

    header->type = type_names[type];

    std::cout << "ok" << std::endl;

    return;
  }

  if (mode == "[]") {
    std::size_t length;
    auto item_size = type_sizes[type_names[type]];

    argv >> length;

    for (auto i = 0ul; i < length; i++) {
      auto pointer = reinterpret_cast<char*>(address);

      set_value(pointer + i * item_size, type, argv);
    }

    header->type = type_names[type];

    std::cout << "ok" << std::endl;

    return;
  }

  std::cout << "invalid operation mode, use = or []" << std::endl;
}

auto cli() -> void {
  std::cout << "s21_memory repl :: use 'help' for more info\n" << std::endl;

  while (true) {
    std::cout << "$ ";

    std::string line;

    std::getline(std::cin, line);

    std::istringstream argv(line);

    std::string command;

    argv >> command;

    if (command == "exit") {
      std::cout << "goodbye" << std::endl;
      return;
    }

    if (command == "help") {
      handle_help(argv);
    } else if (command == "set_heap") {
      handle_set_heap(argv);
    } else if (command == "heap") {
      handle_heap(argv);
    } else if (command == "block") {
      handle_block(argv);
    } else if (command == "malloc") {
      handle_malloc(argv);
    } else if (command == "calloc") {
      handle_calloc(argv);
    } else if (command == "realloc") {
      handle_realloc(argv);
    } else if (command == "free") {
      handle_free(argv);
    } else if (command == "set") {
      handle_set(argv);
    } else {
      std::cout << "unknown command: " << command << std::endl;
    }
  }
}

auto main() -> int {
  try {
    cli();
  } catch (std::exception& e) {
    std::cout << "Fatal error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
