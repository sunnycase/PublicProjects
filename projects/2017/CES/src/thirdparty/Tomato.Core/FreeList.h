//
// Tomato
// Free List
// 
// (c) SunnyCase 
// �������� 2016-08-28
#pragma once
#include "Tomato.Core.h"
#include <list>

DEFINE_NS_CORE

class FreeList
{
	struct FreeEntry
	{
		size_t offset;
		size_t length;
	};
public:
	FreeList(size_t initialSize = 0);

	bool TryAllocate(size_t count, size_t& offset);
	void Enlarge(size_t amount);
	void Retire(size_t offset, size_t length);
private:
	void CombineFreeNode(typename std::list<FreeEntry>::iterator it);
private:
	size_t _capacity;
	std::list<FreeEntry> _freeEntries;
};

END_NS_CORE