#ifndef SHM_SUPPORT_HPP
#define SHM_SUPPORT_HPP

#include <string>
#include "llvm/Support/Error.h"

class ShmObject {
  std::string name;
  size_t size;
  void* address;
  int file;
  bool opened;

public:
  ShmObject(const std::string& name) : name(name), size(0), address(nullptr), file(-1)  { }
  ShmObject(const std::string& name, size_t size, void* address, int file)
    : name(name), size(size), address(address), file(file) { }

  llvm::Error open();
  llvm::Error create(size_t size);
  void close();

  inline bool is_open() { return opened; }
};

#endif // !SHM_SUPPORT_HPP