#include "octal/platform/platform.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace octal {
  bool Platform::Init() {

    return true;
  }

  bool Platform::Stop() {
    return true;
  }

  bool Platform::Flush() {
    bool quit;

    return !quit;
  }

  void* Platform::Allocate(u64 size, bool aligned) {
    return malloc(size);
  }

	void Platform::Free(void* block) {
	  free(block);
	}

  void* Platform::MemZero(void* block, u64 size) {
    return memset(block, 0, size);
  }

  void* Platform::MemSet(void* block, i32 value, u64 size) {
    return memset(block, value, size);
  }

  void* Platform::MemCopy(void* dest, const void* src, u64 size) {
    return memcpy(dest, src, size);
  }


  void Platform::Write(const char* msg, u8 color) {
  }

  void Platform::WriteError(const char* msg, u8 color) {
  }

  f64 Platform::AbsoluteTime() {
    return 0.0;
  }

  void Platform::Sleep(u64 ms) {

  }
}
