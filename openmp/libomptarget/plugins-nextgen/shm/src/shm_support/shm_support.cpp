#include "shm_support.hpp"
#include "shm_errs.hpp"
#include "shm_api_wrap.hpp"

#include <string>

Err ShmObject::open(size_t size_in) {
  if (opened)
    return make_err_msg("The shared memory object was already open!");

  Err err;

  if ((err = wrap_shm_open(name, file)))
  if ((err = wrap_ftruncate(file, size_in)))
  if ((err = wrap_shm_mmap(file, size_in, address))) {
    opened = true;
    size = size_in;
    return make_success();
  }

  if (file >= 0)
    wrap_close(file);

  return err;
}

Err ShmObject::resize(size_t new_size) {
  if (!opened)
    return make_err_msg("This shared memory object is not open!");
    
  Err err;
  if (!(err = wrap_ftruncate(file, new_size)))
    return err;

  size = new_size;
  return make_success();
}

void ShmObject::close() {
  wrap_munmap(address, size);
  wrap_close(file);

  file = -1;
  address = nullptr;
  opened = false;
}