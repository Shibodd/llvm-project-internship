#ifndef ELF_LOADER_HPP
#define ELF_LOADER_HPP

#include <vector>
#include <iostream>
#include <cstdint>
#include <functional>
#include "elf_view.hpp"

struct Mapper {
  virtual Span<char> map(size_t size) = 0;
  virtual void unmap(Span<char> mapped_area) = 0;
};

// Provides a way to load an ELF in memory and execute symbols
class ElfLoader {
  using Mapper = std::function<llvm::Expected<Span<char>>(size_t)>;
  using Unmapper = std::function<llvm::Error(Span<char>)>;

  Span<char> mapped_memory;
  std::vector<char> elf_file;
  ElfView elf;
  Mapper mapper;
  Unmapper unmapper;

  size_t get_mmap_len();
  void copy_segments_to_mem();
  void fixup_relocations();
  void single_rela_fixup(const Elf64_Shdr& phdr);

  inline uintptr_t get_base_addr() { return (uintptr_t)mapped_memory.begin(); }
  
public:
  ElfLoader(Span<char> elf_file, Mapper mapper, Unmapper unmapper);

  llvm::Error load();
  llvm::Error unload();

  uintptr_t get_symbol_addr(const char* name) {
    Elf64_Sym* sym = elf.resolve_symbol(name);
    if (sym == nullptr)
      return 0;
      
    uintptr_t addr = get_base_addr() + sym->st_value;
    return addr;
  }
};

#endif // !ELF_LOADER_HPP