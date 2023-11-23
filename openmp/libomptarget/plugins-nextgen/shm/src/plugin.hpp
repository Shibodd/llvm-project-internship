#ifndef SHM_PLUGIN_HPP
#define SHM_PLUGIN_HPP

#include "PluginInterface.h"
#include "shm_debug.hpp"

namespace llvm {
namespace omp {
namespace target {
namespace plugin {

/// Class implementing common functionalities of offload plugins. Each plugin
/// should define the specific plugin class, derive from this generic one, and
/// implement the necessary virtual function members.
struct ShmPluginTy : GenericPluginTy {
  /// Create a CUDA plugin.
  ShmPluginTy() : GenericPluginTy(getTripleArch()) {}

  /// This class should not be copied.
  ShmPluginTy(const ShmPluginTy &) = delete;
  ShmPluginTy(ShmPluginTy &&) = delete;
  virtual ~ShmPluginTy() {}

  /// Initialize the plugin and return the number of available devices.
  virtual Expected<int32_t> initImpl() override { SHM_TRACE_FN; return 1; }

  /// Deinitialize the plugin and release the resources.
  virtual Error deinitImpl() override { SHM_TRACE_FN; return Plugin::success(); }

  /// Get the ELF code to recognize the binary image of this plugin.
  virtual uint16_t getMagicElfBits() const override { SHM_TRACE_FN; return ELF::EM_X86_64; }

  /// Get the target triple of this plugin.
  virtual Triple::ArchType getTripleArch() const override { SHM_TRACE_FN; return Triple::x86_64; }

  /// Indicate if an image is compatible with the plugin devices. Notice that
  /// this function may be called before actually initializing the devices. So
  /// we could not move this function into GenericDeviceTy.
  virtual Expected<bool> isImageCompatible(__tgt_image_info *Info) const override {
    SHM_DP("TODO: Check arch %s\n", Info->Arch);
    return true;
  }
};

GenericPluginTy *Plugin::createPlugin() { return new ShmPluginTy(); }

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_PLUGIN_HPP