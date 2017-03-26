#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <rapidjson/include/rapidjson/rapidjson.h>
#include <rapidjson/include/rapidjson/document.h>
#include <filesystem>
#include <string_view>

namespace CES
{
	class ImageStorage
	{
	public:
		ImageStorage();

		void SetSessionId(std::wstring_view sessionId);
		void Refresh();
	private:
		void EnsureRootDirectoryExists();
	private:
		std::experimental::filesystem::path _rootDir;
		rapidjson::GenericDocument<rapidjson::UTF16<>> _storageJson;
		std::wstring _sessionId;
	};
}