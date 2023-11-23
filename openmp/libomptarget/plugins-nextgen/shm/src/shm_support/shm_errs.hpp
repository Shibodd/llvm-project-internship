#ifndef SHM_ERRS_HPP
#define SHM_ERRS_HPP

#include <string>
#include <string.h>
#include <errno.h>
#include "llvm/Support/Error.h"
#include "PluginInterface.h"

static inline llvm::Error make_success() { return llvm::omp::target::plugin::Plugin::success(); }
static inline llvm::Error make_err_msg(std::string err_msg) { return llvm::omp::target::plugin::Plugin::error(err_msg.c_str()); }
static inline llvm::Error make_err_withno(std::string err_msg) { return make_err_msg(err_msg + " (" + strerror(errno) + ")"); }

#endif // !SHM_ERRS_HPP