#ifndef SHM_GLOBAL_HANDLER_HPP
#define SHM_GLOBAL_HANDLER_HPP

#include "PluginInterface.h"
#include "shm_helpers/shm_debug.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

/// Helper class to do the heavy lifting when it comes to moving globals between
/// host and device. Through the GenericDeviceTy we access memcpy DtoH and HtoD,
/// which means the only things specialized by the subclass is the retrival of
/// global metadata (size, addr) from the device.
/// \see getGlobalMetadataFromDevice
struct ShmGlobalHandlerTy : public GenericGlobalHandlerTy {
public:
  virtual ~ShmGlobalHandlerTy() {}

  /// Get the address and size of a global from the device. Address is return in
  /// \p DeviceGlobal, the global name and expected size are passed in
  /// \p DeviceGlobal.
  virtual Error getGlobalMetadataFromDevice(GenericDeviceTy &Device,
                                            DeviceImageTy &Image,
                                            GlobalTy &DeviceGlobal) override {
    SHM_TRACE_FN;
    ShmDeviceImageTy& shmImage = static_cast<ShmDeviceImageTy&>(Image);

    DP("Looking for %s\n", DeviceGlobal.getName().c_str());
    DeviceGlobal.setPtr((void*)shmImage.get_symbol_addr(DeviceGlobal.getName().c_str()));

    return Plugin::success();
  }
};

GenericGlobalHandlerTy *Plugin::createGlobalHandler() {
  SHM_TRACE_FN;
  return new ShmGlobalHandlerTy();
}

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_GLOBAL_HANDLER_HPP