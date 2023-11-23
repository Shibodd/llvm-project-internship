#ifndef SHM_DEVICE_IMAGE_HPP
#define SHM_DEVICE_IMAGE_HPP

#include "PluginInterface.h"
#include "shm_support/shm_debug.hpp"
#include "shm_support/shm_support.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

struct ShmDeviceImageTy : public DeviceImageTy {
  Error open_shm_object(std::string name, size_t size) {
    SHM_TRACE_FN;
    assert(shm_object.is_open() && "The SHM object is already opened.");
    return shm_object.open(size);
  }

  Error close_shm_object() {
    SHM_TRACE_FN;
    assert(!shm_object.is_open() && "The SHM object is not opened.");
  }
private:
  ShmObject shm_object;
};

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_DEVICE_IMAGE_HPP