#ifndef DEBUG_HELPERS_HPP
#define DEBUG_HELPERS_HPP


#include "Debug.h" // Libomptarget Debug (for DP)

#define RISCV_DP(s, ...) DP("[%s] " s "\n", __func__ __VA_OPT__(,) __VA_ARGS__)
#define RISCV_NOT_IMPLEMENTED RISCV_DP("Not implemented!")
#define RISCV_TRACE_FN RISCV_DP("Entering")

#endif // !DEBUG_HELPERS_HPP