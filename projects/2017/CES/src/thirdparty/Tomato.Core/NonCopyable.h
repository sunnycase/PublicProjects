//
// Tomato Media
// ���ɸ��ƻ���
// 
// ���ߣ�SunnyCase
// �������� 2015-08-16
#pragma once
#include "Tomato.Core.h"

DEFINE_NS_CORE

struct NonCopyable
{
	NonCopyable(){}
	NonCopyable(NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&) = delete;
};

END_NS_CORE