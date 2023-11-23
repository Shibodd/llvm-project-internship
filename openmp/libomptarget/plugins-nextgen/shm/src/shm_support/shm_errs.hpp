#ifndef SHM_ERRS_HPP
#define SHM_ERRS_HPP

#include <string>
#include <string.h>
#include <errno.h>

Err::Err() : ok(true), err_msg("") { }
Err::Err(std::string err_msg) : ok(false), err_msg(err_msg) { }

static inline Err make_success() { return Err(); }
static inline Err make_err_withno(std::string err_msg) { return Err(err_msg + " (" + strerror(errno) + ")"); }
static inline Err make_err_msg(std::string err_msg) { return Err(err_msg); }
static inline Err make_not_opened_msg() { return Err("The shared memory object has not been opened."); }

#endif // !SHM_ERRS_HPP