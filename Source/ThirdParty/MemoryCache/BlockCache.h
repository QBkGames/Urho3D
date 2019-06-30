#pragma once

// Copyright (c) 2019 QB'k Games.
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

#ifdef _MULTITHREADED_
#include <atomic>
#endif

namespace EnginePlus
{
	// A block of data which, when free, is part of a single linked list.
	struct SMemBlock
	{
		SMemBlock* pNext;
	};

	// ------------------------------------------------------------------------
	// Handles efficient allocation and recycling of data blocks of a single size
	// ------------------------------------------------------------------------
	class CBlockCache
	{
		// FIELDS =============================================================

		SMemBlock* _pCacheBlock = nullptr;
		size_t _uBlockSize;

#ifdef _MULTITHREADED_
		std::atomic_flag _lockFlag = ATOMIC_FLAG_INIT;
#endif
	public:
		// INITIALISATION =====================================================
		
		// Default constructor
		CBlockCache() = default;

		// Initialise the block data size.
		void Initialise(size_t uDataSize) { _uBlockSize = uDataSize; }

		// PROPERTIES =========================================================

		// Gets the block size of this cache.
		size_t BlockSize() const { return _uBlockSize; }

		// MUTATORS ===========================================================

		// Returns a block from the cache or null if the cache is empty.
		SMemBlock* Allocate()
		{
			SMemBlock* pBlock = nullptr;
#ifdef _MULTITHREADED_
			while (_lockFlag.test_and_set()) {}
#endif
			if (_pCacheBlock)
			{
				pBlock = _pCacheBlock;
				_pCacheBlock = _pCacheBlock->pNext;
			}

#ifdef _MULTITHREADED_
			_lockFlag.clear();
#endif
			return pBlock;
		}

		// Returns a data block to the cache for recycling.
		void Free(SMemBlock* pBlock)
		{
#ifdef _MULTITHREADED_
			while (_lockFlag.test_and_set()) {}
#endif
			pBlock->pNext = _pCacheBlock;
			_pCacheBlock = pBlock;
#ifdef _MULTITHREADED_
			_lockFlag.clear();
#endif
		}

		// ACCESSORS ==========================================================

		// CLEAN UP ===========================================================

	private:
		// IMPLEMENTATION =====================================================

	};
}
