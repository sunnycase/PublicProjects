//
// Tomato Media
// 字符编码
// 
// (c) SunnyCase 
// 创建日期 2015-03-18
#pragma once
#include "Tomato.Core.h"
#include <string_view>
#include <memory>

DEFINE_NS_CORE

TOMATO_CORE_API std::wstring s2ws(std::string_view str, UINT codePage);
TOMATO_CORE_API std::wstring s2ws(std::string_view str);
TOMATO_CORE_API std::string ws2s(std::wstring_view str, UINT codePage);

END_NS_CORE