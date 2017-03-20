//
// Tomato Media
// ���ɸ��ƻ���
// 
// ���ߣ�SunnyCase
// �������� 2015-08-16
#pragma once

struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = default;
	NonCopyable& operator=(NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = default;
};