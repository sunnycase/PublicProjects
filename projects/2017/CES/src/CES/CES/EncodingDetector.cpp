//
// Tomato Media Core
// ����̽��
// ���ߣ�SunnyCase
// ����ʱ�䣺2015-12-31
//
#include "stdafx.h"
#include "EncodingDetector.h"

using namespace NS_CORE;

EncodingDetector::EncodingDetector()
{
	ThrowIfFailed(CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&_multiLang)));
}

EncodingDetector::~EncodingDetector()
{
}

DWORD EncodingDetector::DetectCodePage(std::string_view text)
{
	INT len = text.length(), score = 2;
	auto infos = std::make_unique<DetectEncodingInfo[]>(score);
	ThrowIfFailed(_multiLang->DetectInputCodepage(MLDETECTCP_NONE, 0, const_cast<CHAR*>(text.data()), 
		&len, infos.get(), &score));
	if (score == 0) return CP_ACP;
	return infos[0].nCodePage;
}
