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

		void Initialize(std::wstring_view sessionId, std::wstring_view categories);
		void Refresh();
	private:
		void EnsureRootDirectoryExists();
		void LoadStorageRecursive(const std::experimental::filesystem::directory_entry& directory, const rapidjson::GenericValue<rapidjson::UTF16<>>& cntLevelCategories, rapidjson::GenericValue<rapidjson::UTF16<>>& destValue, rapidjson::MemoryPoolAllocator<>& allocator);
	private:
		std::experimental::filesystem::path _rootDir;
		rapidjson::GenericDocument<rapidjson::UTF16<>> _categoriesJson;
		rapidjson::GenericDocument<rapidjson::UTF16<>> _storageJson;
		std::wstring _sessionId;
	};
}