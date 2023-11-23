#ifndef SHM_PLUGIN_HPP
#define SHM_PLUGIN_HPP

#include "PluginInterface.h"

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
  virtual Expected<int32_t> initImpl() override { }

  /// Deinitialize the plugin and release the resources.
  virtual Error deinitImpl() override { }

  /// Get the ELF code to recognize the binary image of this plugin.
  virtual uint16_t getMagicElfBits() const override { }

  /// Get the target triple of this plugin.
  virtual Triple::ArchType getTripleArch() const override { }

  /// Indicate whether data can be exchanged directly between two devices under
  /// this same plugin. If this function returns true, it's safe to call the
  /// GenericDeviceTy::exchangeData() function on the source device.
  virtual bool isDataExchangable(int32_t SrcDeviceId, int32_t DstDeviceId) override {
    return isValidDeviceId(SrcDeviceId) && isValidDeviceId(DstDeviceId);
  }

  /// Indicate if an image is compatible with the plugin devices. Notice that
  /// this function may be called before actually initializing the devices. So
  /// we could not move this function into GenericDeviceTy.
  virtual Expected<bool> isImageCompatible(__tgt_image_info *Info) const override { }

  /// Indicate whether the plugin supports empty images.
  virtual bool supportsEmptyImages() const override { return false; }
};

GenericPluginTy *Plugin::createPlugin() { return new ShmPluginTy(); }

} // namespace plugin
} // namespace target
} // namespace omp
} // namespace llvm

#endif // !SHM_PLUGIN_HPP