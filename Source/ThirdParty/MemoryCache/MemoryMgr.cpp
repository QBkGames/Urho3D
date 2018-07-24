//
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
//

#include "MemoryMgr.h"
#include <malloc.h>
#ifdef _STATISTICS_
#include "Platform\Common\Logger.h"
#endif

namespace EnginePlus
{
	// ----------------------------------------------------------------------------
	// Instantiates with default values.
	// ----------------------------------------------------------------------------
	CMemoryMgr::CMemoryMgr()
		: _pFilledPages(nullptr)
	{
		InitialiseCache();
		_pPageA = ::new CMemoryPage();
		_pPageB = ::new CMemoryPage();

#ifdef _STATISTICS_
		_uLargeBlockCount = 0;
		_uLargeBlocksSize = 0;

		_uLargeBlocksMin = (unsigned)-1;
		_uLargeBlocksMax = 0;
#endif
	}

	// ----------------------------------------------------------------------------
	// Allocates a block of memory of the specified size.
	// 
	// First it trys to allocate from a cache of the appropriate size,
	// otherwise it allocates from a page, creating new pages as needed.
	// ----------------------------------------------------------------------------
	void* CMemoryMgr::Allocate(size_t uBlockSize)
	{
		if (uBlockSize > MAX_BLOCK_SIZE)
		{
#ifdef _STATISTICS_
			_uLargeBlockCount++;
			_uLargeBlocksSize += uBlockSize;

			if (_uLargeBlocksMin > uBlockSize)
				_uLargeBlocksMin = uBlockSize;
			if (_uLargeBlocksMax < uBlockSize)
				_uLargeBlocksMax = uBlockSize;
#endif
			return malloc(uBlockSize);
		}

		unsigned uCacheIndex = CalcCacheIndex(uBlockSize);
#ifdef _MULTITHREADED_
		std::unique_lock<std::mutex> hndLock(_hndMutex);
#endif
		SMemBlock* pBlock = _blockCaches[uCacheIndex].pCacheBlock;

		if (!pBlock)
			return AllocateFromPage(_blockCaches[uCacheIndex].uBlockSize);

		_blockCaches[uCacheIndex].pCacheBlock = pBlock->pNext;
		return pBlock;
	}

	// ----------------------------------------------------------------------------
	// Deallocates the given block of memory of the specified size.
	// ----------------------------------------------------------------------------
	void CMemoryMgr::Free(void* pMem, size_t uBlockSize)
	{
		if (uBlockSize > MAX_BLOCK_SIZE)
		{
			free(pMem);
			return;
		}

		unsigned uCacheIndex = CalcCacheIndex(uBlockSize);
		SMemBlock* pBlock = (SMemBlock*)pMem;

#ifdef _MULTITHREADED_
		std::unique_lock<std::mutex> hndLock(_hndMutex);
#endif
		pBlock->pNext = _blockCaches[uCacheIndex].pCacheBlock;
		_blockCaches[uCacheIndex].pCacheBlock = pBlock;
	}

#ifdef _STATISTICS_
	// ----------------------------------------------------------------------------
	// Logs usage statistics of all allocators.
	// ----------------------------------------------------------------------------
	void CMemoryMgr::LogStatistics() const
	{
		Logger << "Memory Manager\n";

		unsigned uAllocatedPages = 3;
		unsigned uUnusedMemory = 0;

		uUnusedMemory += _pPageA->FreeSize();
		uUnusedMemory += _pPageB->FreeSize();

		CMemoryPage* pPage = _pFilledPages;

		while (pPage)
		{
			uAllocatedPages++;
			uUnusedMemory += pPage->FreeSize();

			pPage = pPage->pNext;
		}

		unsigned uAllocatedSize = uAllocatedPages * MEMORY_PAGE_SIZE;
		unsigned uUnusedPercent = (uUnusedMemory * 100) / uAllocatedSize;

		Logger << "Allocated: " << uAllocatedPages << " x 16 KB\t" << uAllocatedSize << NL;
		Logger << "Unused: " << uUnusedMemory << " (" << uUnusedPercent << "%)\n";
		Logger << "Large Blocks: " << _uLargeBlockCount << "\tAllocated: " << (_uLargeBlocksSize / 1024) << "kB\n";
		if (_uLargeBlockCount)
			Logger << "Large Blocks range: " << _uLargeBlocksMin << " - " << _uLargeBlocksMax << NL;
	}
#endif

	// ------------------------------------------------------------------------
	// Destructor.
	// ------------------------------------------------------------------------
	CMemoryMgr::~CMemoryMgr()
	{
	}

	// ----------------------------------------------------------------------------
	// Initialises the cache data and indexes.
	// ----------------------------------------------------------------------------
	void CMemoryMgr::InitialiseCache()
	{
		unsigned uBlockSize = 8;
		unsigned uCacheIndex = 0;
		unsigned uIndex = 0;
		unsigned uLoopRound;

		for (uLoopRound = 0; uLoopRound < 8; uLoopRound++)
		{
			_blockCaches[uCacheIndex].pCacheBlock = nullptr;
			_blockCaches[uCacheIndex].uBlockSize = uBlockSize;
			uBlockSize += 8;

			_cacheIndexes[uIndex++] = uCacheIndex;
			uCacheIndex++;
		}
		uBlockSize += 8;

		for (uLoopRound = 0; uLoopRound < 4; uLoopRound++)
		{
			_blockCaches[uCacheIndex].pCacheBlock = nullptr;
			_blockCaches[uCacheIndex].uBlockSize = uBlockSize;
			uBlockSize += 16;

			for (unsigned uSizeIndex = 0; uSizeIndex < 2; uSizeIndex++)
				_cacheIndexes[uIndex++] = uCacheIndex;
			uCacheIndex++;
		}
		uBlockSize += 16;

		for (uLoopRound = 0; uLoopRound < 4; uLoopRound++)
		{
			_blockCaches[uCacheIndex].pCacheBlock = nullptr;
			_blockCaches[uCacheIndex].uBlockSize = uBlockSize;
			uBlockSize += 32;

			for (unsigned uSizeIndex = 0; uSizeIndex < 4; uSizeIndex++)
				_cacheIndexes[uIndex++] = uCacheIndex;
			uCacheIndex++;
		}
		uBlockSize += 32;

		for (uLoopRound = 0; uLoopRound < 4; uLoopRound++)
		{
			_blockCaches[uCacheIndex].pCacheBlock = nullptr;
			_blockCaches[uCacheIndex].uBlockSize = uBlockSize;
			uBlockSize += 64;

			for (unsigned uSizeIndex = 0; uSizeIndex < 8; uSizeIndex++)
				_cacheIndexes[uIndex++] = uCacheIndex;
			uCacheIndex++;
		}
		uBlockSize += 64;

		for (uLoopRound = 0; uLoopRound < 4; uLoopRound++)
		{
			_blockCaches[uCacheIndex].pCacheBlock = nullptr;
			_blockCaches[uCacheIndex].uBlockSize = uBlockSize;
			uBlockSize += 128;

			for (unsigned uSizeIndex = 0; uSizeIndex < 16; uSizeIndex++)
				_cacheIndexes[uIndex++] = uCacheIndex;
			uCacheIndex++;
		}
	}

	// ----------------------------------------------------------------------------
	// Allocates the required block from a memory page,
	// allocating new pages as needed.
	// ----------------------------------------------------------------------------
	void* CMemoryMgr::AllocateFromPage(size_t uBlockSize)
	{
		// Try finding an empty slot in an existing active page
		void* pMem;
		if (pMem = _pPageA->Allocate(uBlockSize))
			return pMem;
		if (pMem = _pPageB->Allocate(uBlockSize))
			return pMem;

		// Archive the page with lowest free space
		CMemoryPage*& pPageToArchive =
			(_pPageA->FreeSize() >= _pPageB->FreeSize()) ? _pPageB : _pPageA;

		pPageToArchive->pNext = _pFilledPages;
		_pFilledPages = pPageToArchive;

		// Create a new page and allocate from it
		pPageToArchive = ::new CMemoryPage();
		return pPageToArchive->Allocate(uBlockSize);
	}
}