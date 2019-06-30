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

#include "BlockCache.h"
#include "MemoryPage.h"
#ifdef _MULTITHREADED_
#include <mutex>
#endif

#define MEM_CACHE_COUNT			24
#define MIN_BLOCK_SIZE_STEP		8
#define MAX_BLOCK_SIZE			1024
#define MEM_ALLOCATOR_INDEXES	(MAX_BLOCK_SIZE / MIN_BLOCK_SIZE_STEP)

namespace EnginePlus
{
	// ------------------------------------------------------------------------
	// Handles efficient allocation and recycling of small data blocks
	// ------------------------------------------------------------------------
	class CMemoryMgr
	{
		// FIELDS =============================================================

		CBlockCache _blockCaches[MEM_CACHE_COUNT];
		unsigned _cacheIndexes[MEM_ALLOCATOR_INDEXES];

		CMemoryPage* _pPageA;
		CMemoryPage* _pPageB;
		CMemoryPage* _pFilledPages;

#ifdef _MULTITHREADED_
		std::mutex _hndMutex;
#endif

#ifdef _STATISTICS_
		unsigned _uLargeBlockCount;
		unsigned _uLargeBlocksSize;

		unsigned _uLargeBlocksMin;
		unsigned _uLargeBlocksMax;
#endif
		// INITIALISATION =====================================================

		// Default constructor private to enforce the singleton pattern.
		CMemoryMgr();

	public:
		CMemoryMgr(const CMemoryMgr&) = delete;
		CMemoryMgr& operator=(const CMemoryMgr&) = delete;

		// PROPERTIES =========================================================

		// Gets the singleton instance of the memory manger.
		static CMemoryMgr& Instance()
		{
			static CMemoryMgr instance;

			return instance;
		}

		// MUTATORS ===========================================================

		// Allocates a block of memory of the specified size.
		// 
		// First it trys to allocate from a cache of the appropriate size,
		// otherwise it allocates from a page, creating new pages as needed.
		void* Allocate(size_t uBlockSize);

		// Deallocates the given block of memory of the specified size.
		void Free(void* pMem, size_t uBlockSize);

		// ACCESSORS ==========================================================

#ifdef _STATISTICS_
		// Logs usage statistics of all allocators.
		void LogStatistics() const;
#endif
		// CLEAN UP ===========================================================

		// TODO: Releases all unused pages.
		// void Compact();

		// Destructor.
		~CMemoryMgr();

	private:
		// IMPLEMENTATION =====================================================

		// Initialises the cache data and indexes.
		void InitialiseCache();

		// Calculates the cache index from the given block size.
		unsigned CalcCacheIndex(size_t uBlockSize) const
		{
			unsigned uBlockSizeFactor = uBlockSize / MIN_BLOCK_SIZE_STEP;
			if (!(uBlockSize % MIN_BLOCK_SIZE_STEP))
				uBlockSizeFactor--;

			return _cacheIndexes[uBlockSizeFactor];
		}

		// Allocates the required block from a memory page,
		// allocating new pages as needed.
		void* AllocateFromPage(size_t uBlockSize);
	};
}
