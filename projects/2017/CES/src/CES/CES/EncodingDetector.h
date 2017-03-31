//
// Tomato Media Core
// ����̽��
// ���ߣ�SunnyCase
// ����ʱ�䣺2015-12-31
//
#pragma once
#include "Tomato.Core/Tomato.Core.h"
#include <MLang.h>

DEFINE_NS_CORE

class EncodingDetector
{
public:
	EncodingDetector();
	~EncodingDetector();

	DWORD DetectCodePage(std::string_view text);
private:
	WRL::ComPtr<IMultiLanguage2> _multiLang;
};

END_NS_CORE