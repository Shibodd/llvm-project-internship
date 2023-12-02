#include "kernel.hpp"
#include "device.hpp"
#include "global_handler.hpp"
#include "plugin.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

GenericPluginTy *Plugin::createPlugin() {
  return new RiscvPluginTy();
}
GenericDeviceTy *Plugin::createDevice(int32_t DeviceId, int32_t NumDevices) {
  return new RiscvDeviceTy(DeviceId, NumDevices);
}
GenericGlobalHandlerTy *Plugin::createGlobalHandler() {
  return new RiscvGlobalHandlerTy();
}

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm