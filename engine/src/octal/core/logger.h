#pragma once
#include "octal/defines.h"

namespace octal {
#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

#ifndef FATAL
#define FATAL(msg, ...) Log(octal::LogLevel::Fatal, msg, ##__VA_ARGS__);
#endif

#ifndef ERROR
#define ERROR(msg, ...) Log(octal::LogLevel::Error, msg, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define WARN(msg, ...) Log(octal::LogLevel::Warn, msg, ##__VA_ARGS__);
#else
#define WARN(msg, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define INFO(msg, ...) Log(octal::LogLevel::Info, msg, ##__VA_ARGS__);
#else
#define INFO(msg, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define DEBUG(msg, ...) Log(octal::LogLevel::Debug, msg, ##__VA_ARGS__);
#else
#define DEBUG(msg, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define TRACE(msg, ...) Log(octal::LogLevel::Trace, msg, ##__VA_ARGS__);
#else
#define TRACE(msg, ...)
#endif

  enum LogLevel {
    Fatal = 0,
    Error,
    Warn,
    Info,
    Debug,
    Trace,
  };

  API bool InitLog();
  API void ShutdownLog();

  API void report_assertion_failure(const char* expr, const char* msg,  const char* file, i32 line);

  /// Log something! use the macros instead tho
  API void Log(LogLevel, const char* msg, ...);
}
