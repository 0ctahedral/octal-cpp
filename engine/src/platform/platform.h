#pragma once
#include "octal/defines.h"

namespace octal {

	class Platform {
		public:
			/// Start the platform
			static bool Init();

			/// Stop the platform
			static bool Stop();

			/// Get all events from the system
			static bool Flush();

			/// Allocate on this platform
			/// @param size of the block we are allocating
			/// @param aligned if the block should be aligned
			static void* Allocate(u64 size, bool aligned);
		
			/// Free on this platform
			/// @param block to free
			static void Free(void* block);

			/// Set block of memory to zero
			/// @param block to operate on
			/// @param size to affect
			static void* MemZero(void* block, u64 size);

			/// Set block of memory to value
			/// @param block to operate on
			/// @param size to affect
			static void* MemSet(void* block, i32 value, u64 size);

			/// Copy block of memory
			/// @param dest block where the data is going
			/// @param source block where the data is from
			/// @param size of the block
			static void* MemCopy(void* dest, const void* source, u64 size);

			/// Write to the platform's console
			/// @param msg text to print
			/// @param color of the text
			static void Write(const char* msg, u8 color);

			/// Write to the platform's console as an error
			/// @param msg text to print
			/// @param color of the text
			static void WriteError(const char* msg, u8 color);

			/// What time is it?
			static f64 AbsoluteTime();

			// Sleep on the thread for the provided ms. This blocks the main thread.
			// Should only be used for giving time back to the OS for unused update power.
			// Therefore it is not exported.
			/// Sleep main thread
			/// @param ms amount of time to sleep in ms
			static void Sleep(u64 ms);
    private:
      /// State held by the platform
      static void* s_State;
	};

}
