struct ShmKernelTy;
struct ShmDeviceTy;
struct ShmPluginTy;
struct ShmGlobalHandlerTy;
struct ShmDeviceImageTy;
#include "device_image.hpp"
// Cleanup elf.h's macros...
#undef EM_X86_64

#include "kernel.hpp"
#include "device.hpp"
#include "global_handler.hpp"
#include "plugin.hpp"