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

  /// Log a fatal error
#ifndef FATAL
#define FATAL(msg, ...) Log(octal::LogLevel::Fatal, msg, ##__VA_ARGS__);
#endif

  /// Log an error
#ifndef ERROR
#define ERROR(msg, ...) Log(octal::LogLevel::Error, msg, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
  /// warn the user of something non-fatal
#define WARN(msg, ...) Log(octal::LogLevel::Warn, msg, ##__VA_ARGS__);
#else
#define WARN(msg, ...)
#endif

#if LOG_INFO_ENABLED == 1
  /// general info logging
#define INFO(msg, ...) Log(octal::LogLevel::Info, msg, ##__VA_ARGS__);
#else
#define INFO(msg, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
  /// logging for debug purposes
#define DEBUG(msg, ...) Log(octal::LogLevel::Debug, msg, ##__VA_ARGS__);
#else
#define DEBUG(msg, ...)
#endif

#if LOG_TRACE_ENABLED == 1
  /// logging for tracing stuff
#define TRACE(msg, ...) Log(octal::LogLevel::Trace, msg, ##__VA_ARGS__);
#else
#define TRACE(msg, ...)
#endif

  /// What level to log at
  /// Should not be used directly, that's what we have the macros for!
  enum LogLevel {
    Fatal = 0,
    Error,
    Warn,
    Info,
    Debug,
    Trace,
  };

  /// Setup logging for this application
  API bool InitLog();
  /// Shutdown logging for this application
  API void ShutdownLog();

  /// Helper to log an assertion failure
  API void report_assertion_failure(const char* expr, const char* msg,  const char* file, i32 line);

  /// Log something! use the macros instead though
  API void Log(LogLevel, const char* msg, ...);
}
