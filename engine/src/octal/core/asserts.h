#pragma once
namespace octal {
#include "octal/defines.h"

#define ASSERTIONS_ENABLED

#ifdef ASSERTIONS_ENABLED

#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_debugtrap()
#endif

/// Printout and debug break if there the condition inside is not met
#define ASSERT(expr, msg)                                     \
{                                                             \
  if (expr) {                                                 \
  } else {                                                    \
    octal::report_assertion_failure(#expr, msg, __FILE__, __LINE__); \
    debugBreak();                                             \
  }                                                           \
}

/// Printout and debug break if there the condition inside is not met (only in debug mode)
#ifdef _DEBUG
#define ASSERT_DEBUG_(expr, msg)                              \
{                                                             \
  if (expr) {                                                 \
  } else {                                                    \
    octal::report_assertion_failure(#expr, msg, __FILE__, __LINE__); \
    debugBreak();                                             \
  }                                                           \
}
#else
#define DEBUG_ASSERT(expr, msg)
#endif

#else

#define ASSERT(expr, msg)
#define DEBUG_ASSERT(expr, msg)

#endif

}
