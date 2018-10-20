#pragma once
#ifndef _MANAGEDOBJECT_H_
#define _MANAGEDOBJECT_H_

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

namespace EnginePlus
{
#define MANAGED_OBJECT(classType)	\
	public:	\
		static void* operator new(size_t size)	{ return CMemoryMgr::Instance().Allocate(size); }	\
		static void operator delete(void* pData)	{ CMemoryMgr::Instance().Free(pData, sizeof(classType)); }

#define RECYCLABLE_OBJECT(classType)	\
	public:	\
		static classType* Retrive()	{ return new(CMemoryMgr::Instance().Allocate(sizeof(classType))) classType(); }	\
		static void Recycle(classType* pObject)	{ pObject->~classType(); CMemoryMgr::Instance().Free(pObject, sizeof(classType)); }
		
#define CACHED_FACTORY	\
	template<typename T>	\
	static T* Create() { return new(CMemoryMgr::Instance().Allocate(sizeof(T))) T(); }
}

#endif