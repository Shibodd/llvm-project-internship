#ifndef SHM_SUPPORT_HPP
#define SHM_SUPPORT_HPP

#include <string>

struct Err {
  bool ok;
  std::string err_msg;

  Err();
  Err(std::string err_msg);

  operator bool() { return ok; }
};

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

  Err open(size_t size);
  Err resize(size_t new_size);
  void close();
};

#endif // !SHM_SUPPORT_HPP