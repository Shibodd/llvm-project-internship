#include <string>

#include "shm_object.hpp"
#include "shm_api_wrap.hpp"
#include "shm_err.hpp"


llvm::Error ShmObject::create(size_t size_in) {
  ASSERT_OR_ERR(!opened, "The shared memory object was already open!");

  auto file_or_err = wrap_shm_open(name.c_str(), true);
  if (!file_or_err)
    return file_or_err.takeError();
  file = *file_or_err;

  if (auto err = wrap_ftruncate(file, size_in)) {
    static_cast<void>(wrap_close(file));
    return err;
  }

  auto addr_or_err = wrap_shm_mmap(file, size_in);
  if (!addr_or_err) {
    static_cast<void>(wrap_close(file));
    return addr_or_err.takeError();
  }

  mapped_memory = { (char*)*addr_or_err, size_in };
  opened = true;
  return make_success();
}

void ShmObject::unlink() {
  static_cast<void>(wrap_munmap(mapped_memory.begin(), mapped_memory.len()));
  static_cast<void>(wrap_close(file));

  file = -1;
  mapped_memory = Span<char>::empty();
  opened = false;
  static_cast<void>(wrap_shm_unlink(name.c_str()));
}