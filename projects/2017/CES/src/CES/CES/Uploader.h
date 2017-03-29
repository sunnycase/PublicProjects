#pragma once
#include <Tomato.Core/Tomato.Core.h>

namespace CES
{
	class Uploader
	{
	public:
		Uploader();

		void Upload(IStream* imageStream, std::wstring&& fileName);
	};
}
