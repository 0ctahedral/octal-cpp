#pragma once
#include "defines.h"

class Platform {

  public:
		/// Start the platform
    virtual bool Init();

		/// Stop the platform
    virtual bool Stop();

		/// Get all events from the system
		bool Flush();

		/// Allocate on this platform
		/// @param size of the block we are allocating
		/// @param aligned if the block should be aligned
		void* Allocate(u64 size, bool aligned);

		/// Set block of memory to zero
		/// @param block to operate on
		/// @param size to affect
		void* MemZero(void* block, u64 size);

		/// Set block of memory to value
		/// @param block to operate on
		/// @param size to affect
		void* MemSet(void* block, i32 value, u64 size);

		/// Copy block of memory
		/// @param dest block where the data is going
		/// @param source block where the data is from
		/// @param size of the block
		void* MemCopy(void* dest, const void* source, u64 size);


		/// Write to the platform's console
		/// @param msg text to print
		/// @param color of the text
		void Write(const char* msg, u8 color);

		/// Write to the platform's console as an error
		/// @param msg text to print
		/// @param color of the text
		void WriteError(const char* msg, u8 color);

		/// What time is it?
		f64 AbsoluteTime();

		// Sleep on the thread for the provided ms. This blocks the main thread.
		// Should only be used for giving time back to the OS for unused update power.
		// Therefore it is not exported.
		/// Sleep main thread
		/// @param ms amount of time to sleep in ms
		void Sleep(u64 ms);
};
