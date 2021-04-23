#pragma once
#include "platorm/platform.h"

namespace octal {
  class LinuxPlatform : Platform {
    private:
    public:
      bool Init() override;

      bool Stop() override;

      bool Flush() override;

      void* Allocate(u64 size, bool aligned) override;

			void Free(void* block) override;

      void* MemZero(void* block, u64 size) override;

      void* MemSet(void* block, i32 value, u64 size) override;

      void* MemCopy(void* dest, const void* source, u64 size) override;

      void Write(const char* msg, u8 color) override;

      void WriteError(const char* msg, u8 color) override;

      f64 AbsoluteTime() override;

      void Sleep(u64 ms) override;
  };
}
