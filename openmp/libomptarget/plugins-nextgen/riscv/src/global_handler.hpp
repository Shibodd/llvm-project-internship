#ifndef RISCV_GLOBAL_HANDLER_HPP
#define RISCV_GLOBAL_HANDLER_HPP

#include "PluginInterface.h"
#include "debug_helpers.hpp"
#include "ELFSymbols.h"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

/// Helper class to do the heavy lifting when it comes to moving globals between
/// host and device. Through the GenericDeviceTy we access memcpy DtoH and HtoD,
/// which means the only things specialized by the subclass is the retrival of
/// global metadata (size, addr) from the device.
/// \see getGlobalMetadataFromDevice
struct RiscvGlobalHandlerTy : public GenericGlobalHandlerTy {
public:
  virtual ~RiscvGlobalHandlerTy() {}

  /// Get the address and size of a global from the device. Address is return in
  /// \p DeviceGlobal, the global name and expected size are passed in
  /// \p DeviceGlobal.
  virtual Error getGlobalMetadataFromDevice(GenericDeviceTy &GenericDevice,
                                            DeviceImageTy &Image,
                                            GlobalTy &DeviceGlobal) override {
    RISCV_TRACE_FN;
    RiscvDeviceTy& Device = reinterpret_cast<RiscvDeviceTy&>(GenericDevice);

    Expected<ELF64LEObjectFile> ElfOrErr = ELF64LEObjectFile::create(Image.getMemoryBuffer());
    if (!ElfOrErr)
      return ElfOrErr.takeError();

    Expected<const ELF64LE::Sym*> SymOrErr = getELFSymbol(*ElfOrErr, DeviceGlobal.getName());
    if (!SymOrErr)
      return SymOrErr.takeError();

    DeviceGlobal.setPtr((void*)((uint64_t)Device.getBaseAddress() + SymOrErr.get()->getValue()));
    return Plugin::success();
  }
};



} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !RISCV_GLOBAL_HANDLER_HPP