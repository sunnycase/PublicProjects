//
// Tomato Media
// �ַ�����
// 
// (c) SunnyCase 
// �������� 2015-03-18
#include "stdafx.h"
#include "Tomato.Core\Encoding.h"
#include "EncodingDetector.h"

using namespace NS_CORE;

std::wstring NS_CORE::s2ws(std::string_view str, UINT codePage)
{
	auto len = MultiByteToWideChar(codePage, 0, str.data(), str.size(), nullptr, 0);
	std::wstring ws(len, 0);
	MultiByteToWideChar(codePage, 0, str.data(), str.size(), &ws[0], len);

	return ws;
}

std::string NS_CORE::ws2s(std::wstring_view str, UINT codePage)
{
	auto len = WideCharToMultiByte(codePage, 0, str.data(), str.size(), nullptr, 0, nullptr, nullptr);
	std::string s(len, 0);
	WideCharToMultiByte(codePage, 0, str.data(), str.size(), &s[0], len, nullptr, nullptr);

	return s;
}

std::wstring NS_CORE::s2ws(std::string_view str)
{
	static EncodingDetector encDet;
	if (str.empty())
		return{};
	else
		return s2ws(str, encDet.DetectCodePage(str));
}