#ifndef ELF_LOADER_HPP
#define ELF_LOADER_HPP

#include "llvm/Support/Error.h"
#include "llvm/Object/ELFObjectFile.h"

#include <cassert>
#include <functional>

extern "C" {
  #include <unistd.h>
}


size_t get_mem_size(const llvm::object::ELFFile<llvm::object::ELF64LE>& Elf) {
  uint64_t vaddr_min = std::numeric_limits<uint64_t>::max();
  uint64_t vaddr_max = std::numeric_limits<uint64_t>::min();

  for (auto &phdr : Elf.program_headers().get()) {
    if (phdr.p_type == llvm::ELF::PT_LOAD && phdr.p_memsz > 0) {
      vaddr_min = std::min(vaddr_min, (uint64_t)phdr.p_vaddr);
      vaddr_max = std::max(vaddr_max, (uint64_t)phdr.p_vaddr + (uint64_t)phdr.p_memsz);
    }
  }

  int page_size = getpagesize();
  uintptr_t base =
      vaddr_min - (vaddr_min % page_size); // floor to the nearest page boundary
  size_t len = vaddr_max - base;
  len += page_size - (len % page_size);

  // Check the result
  ptrdiff_t ori_size = vaddr_max - vaddr_min;
  assert((len < ori_size + (2 * page_size)) && "Exagerated size.");
  assert(len > ori_size && "Length is smaller than the original vaddr range.");
  assert((base % page_size) == 0 && "Base is not on a page boundary.");
  assert((len % page_size) == 0 && "Length is not a multiple of page size.");

  return len;
}

#endif