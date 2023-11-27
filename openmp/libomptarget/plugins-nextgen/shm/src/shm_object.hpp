#ifndef SHM_SUPPORT_HPP
#define SHM_SUPPORT_HPP

#include <string>
#include "span.hpp"
#include "llvm/Support/Error.h"

class ShmObject {
  std::string name;
  Span<char> mapped_memory;
  int file;
  bool opened;

public:
  ShmObject(const std::string& name) : name(name), mapped_memory((char*)nullptr, (size_t)0), file(-1), opened(false)  { }

  llvm::Error create(size_t size);
  void unlink();

  inline bool is_open() { return opened; }
  inline Span<char> get_span() { return mapped_memory; }
};

#endif // !SHM_OBJECT_HPP