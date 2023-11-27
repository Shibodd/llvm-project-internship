#ifndef SHM_ERR_HPP
#define SHM_ERR_HPP

#include "llvm/Support/Error.h"
#include "PluginInterface.h"

#define ASSERT_OR_ERR(cond, s, ...) do { if (!(cond)) { return llvm::omp::target::plugin::Plugin::error(s "\n" __VA_OPT__(,) __VA_ARGS__); } } while(0)
#define ASSERT_OR_PERR(cond, s, ...) ASSERT_OR_ERR(cond, s " (%s)" __VA_OPT__(,)  __VA_ARGS__, strerror(errno))
static inline llvm::Error make_success() { return llvm::omp::target::plugin::Plugin::success(); }

#endif // !SHM_ERR_HPP