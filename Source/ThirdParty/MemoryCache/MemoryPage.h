#pragma once

// Copyright (c) 2018 QB'k Games.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#if __ANDROID__
#include <cstddef>
#endif
#include <atomic>

#define MEMORY_PAGE_SIZE	(1024 * 16)

namespace EnginePlus
{
	// ------------------------------------------------------------------------
	// A page of memory allocated by the memory manager.
	// ------------------------------------------------------------------------
	class CMemoryPage
	{
		// FIELDS =============================================================
		size_t _uFreeSize;
		char* _pFreeData;

		char _data[MEMORY_PAGE_SIZE];

#ifdef _MULTITHREADED_
		std::atomic_flag _lockFlag = ATOMIC_FLAG_INIT;
#endif

	public:
		// INITIALISATION =====================================================

		// Default constructor.
		CMemoryPage()
			: _uFreeSize(MEMORY_PAGE_SIZE), _pFreeData(_data), pNext(nullptr)	{}

		// PROPERTIES =========================================================

		// Gets the size of available free memory.
		size_t FreeSize() const				{ return _uFreeSize; }

		// Get or set the next page pointer.
		CMemoryPage* pNext;

		// MUTATORS ===========================================================

		// Allocates a block of the specified size from the data.
		// Returns the block pointer or null if not enough free data is available.
		void* Allocate(size_t uBlockSize)
		{
#ifdef _MULTITHREADED_
			// Use a different algorithm for thread safe, which is a bit less efficient but correct
			void* pData = nullptr;

			while (_lockFlag.test_and_set()) {}
			if (uBlockSize <= _uFreeSize)
			{
				pData = _pFreeData;

				_pFreeData += uBlockSize;
				_uFreeSize -= uBlockSize;
			}
			_lockFlag.clear();

			return pData;
#else
			if (uBlockSize > _uFreeSize)
				return nullptr;

			void* pData = _pFreeData;

			_pFreeData += uBlockSize;
			_uFreeSize -= uBlockSize;

			return pData;
#endif
		}

		// ACCESSORS ==========================================================

		// CLEAN UP ===========================================================

		// Destructor.
		~CMemoryPage() = default;

	private:
		// IMPLEMENTATION =====================================================
	};
}
