#pragma once
namespace octal {
#include "defines.h"

#define ASSERTIONS_ENABLED

#ifdef ASSERTIONS_ENABLED

#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_debugtrap()
#endif

#define ASSERT(expr, msg)                                     \
{                                                             \
  if (expr) {                                                 \
  } else {                                                    \
    octal::report_assertion_failure(#expr, msg, __FILE__, __LINE__); \
    debugBreak();                                             \
  }                                                           \
}

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
