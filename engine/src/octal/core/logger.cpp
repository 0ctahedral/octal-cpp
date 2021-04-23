#include "octal/core/logger.h"
#include "octal/core/asserts.h"
#include "platform/platform.h"

// TODO: remove
#include <cstdio>
#include <cstring>
#include <cstdarg>

namespace octal {
  void report_assertion_failure(const char* expr, const char* msg,  const char* file, i32 line) {
    Log(LogLevel::Fatal, "%s:%d assertion: %s failed because: \"%s.\"",
         file, line, expr, msg);
  }

  bool InitLog() {
    // TODO: create log file
    return true;
  }

  void ShutdownLog() {
    // TODO: cleanup and shit
  }

  void Log(LogLevel lvl, const char* msg, ...) {
    const char* lvl_string[6] = {
      "[Fatal]: ", "[Error]: ", "[Warn]: ",
      "[Info]: ", "[Debug]: ", "[Trace]: "
    };

    // error is lvl is less than 2
    bool is_err = lvl < 2;

    // store the output in a buffer because faster
    // 32k limit on log entry
    char out_buff[32000];
    char out_msg[32000];
    // reset to 0
    std::memset(out_msg, 0, sizeof(out_msg));

    // put our arguments into the buffer
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, msg);
    vsnprintf(out_msg, 32000, msg, arg_ptr);
    va_end(arg_ptr);

    sprintf(out_buff, "%s%s\n", lvl_string[lvl], out_msg);

    // TODO: platform specific
    Platform::WriteError(out_buff, lvl);

  }

}
