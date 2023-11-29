#ifndef SHM_GLOBAL_HANDLER_HPP
#define SHM_GLOBAL_HANDLER_HPP

#include "PluginInterface.h"
#include "shm_debug.hpp"

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
    SHM_NOT_IMPLEMENTED;

    SHM_DP("Looking for %s", DeviceGlobal.getName().c_str());

    GlobalTy ImageGlobal(DeviceGlobal);
    if (auto err = getGlobalMetadataFromImage(Device, Image, ImageGlobal)) {
      SHM_DP("Global not found\n");
      return err;
    }
    else {
      SHM_DP("Global found at %p", ImageGlobal.getPtr());
      SHM_DP("Correct resolution not implemented!");
      DeviceGlobal.setPtr(ImageGlobal.getPtr());
      return Plugin::success();
    }
  }
};

GenericGlobalHandlerTy *Plugin::createGlobalHandler() {
  return new ShmGlobalHandlerTy();
}

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_GLOBAL_HANDLER_HPP