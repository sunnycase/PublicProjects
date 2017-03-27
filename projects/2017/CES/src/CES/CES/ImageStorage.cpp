#include "stdafx.h"
#include "ImageStorage.h"

using namespace WRL;
using namespace CES;
namespace fs = std::experimental::filesystem;

namespace
{
	fs::path GetKnownFolderPath(const KNOWNFOLDERID& id)
	{
		unique_cotaskmem_arr<wchar_t> path;
		ThrowIfFailed(SHGetKnownFolderPath(id, KF_FLAG_CREATE, nullptr, &path._Myptr()));
		return path.get();
	}
}

ImageStorage::ImageStorage()
	:_rootDir(GetKnownFolderPath(FOLDERID_LocalAppDataLow).append(L"CES")),
	_storageJson(rapidjson::kObjectType)
{

}

void CES::ImageStorage::Initialize(std::wstring_view sessionId, std::wstring_view categories)
{
	_sessionId = sessionId;
	_categoriesJson.Parse(categories.data());
	Refresh();
}

namespace
{
	template<class TIt, class TAllocator>
	decltype(auto) AddMember(TIt it, rapidjson::GenericValue<rapidjson::UTF16<>>& sessionObj, TAllocator& allocator)
	{
		rapidjson::GenericValue<rapidjson::UTF16<>> nodeObj(rapidjson::kObjectType);
		return sessionObj.AddMember(rapidjson::StringRef(it->GetString(), it->GetStringLength()), nodeObj, allocator);
	}

	template<class TAllocator>
	void InitializeSessionObjRecursive(const rapidjson::GenericValue<rapidjson::UTF16<>>& categories, rapidjson::GenericValue<rapidjson::UTF16<>>& sessionObj, TAllocator& allocator)
	{
		assert(categories.IsArray() || categories.IsObject());
		if (categories.IsArray())
		{
			for (auto it = categories.Begin(); it != categories.End(); ++it)
				AddMember(it, sessionObj, allocator);
		}
		else
		{
			for (auto it = categories.MemberBegin(); it != categories.MemberEnd(); ++it)
				InitializeSessionObjRecursive(it->value, AddMember(&it->name, sessionObj, allocator), allocator);
		}
	}
}

void CES::ImageStorage::Refresh()
{
	EnsureRootDirectoryExists();

	rapidjson::MemoryPoolAllocator<> allocator;
	for (auto&& entry : fs::directory_iterator(_rootDir))
	{
		if (entry.status().type() == fs::file_type::directory)
		{
			rapidjson::GenericValue<rapidjson::UTF16<>> sessionObj(rapidjson::kObjectType);

			InitializeSessionObjRecursive(_categoriesJson, sessionObj, allocator);
			LoadStorageRecursive(entry, _categoriesJson, _storageJson.AddMember(rapidjson::StringRef(entry.path().filename().wstring()), sessionObj, allocator), allocator);
		}
	}
}

void CES::ImageStorage::EnsureRootDirectoryExists()
{
	fs::create_directories(_rootDir);
}

void CES::ImageStorage::LoadStorageRecursive(const std::experimental::filesystem::directory_entry & directory, const rapidjson::GenericValue<rapidjson::UTF16<>>& cntLevelCategories, rapidjson::GenericValue<rapidjson::UTF16<>>& destValue, rapidjson::MemoryPoolAllocator<>& allocator)
{
	assert(directory.status().type() == fs::file_type::directory);

	// Ò¶½Úµã
	if (cntLevelCategories.IsString())
	{
		for (auto&& entry : fs::directory_iterator(directory))
		{
			if (entry.status().type() == fs::file_type::regular)
			{
				rapidjson::GenericValue<rapidjson::UTF16<>> fileObj(rapidjson::kObjectType);
				fileObj.AddMember(L"type", L"file", allocator);
				fileObj.AddMember(L"fileName", rapidjson::StringRef(entry.path().wstring()), allocator);
				destValue.PushBack(fileObj, allocator);
			}
		}
	}
	else
	{
		for (auto&& entry : fs::directory_iterator(directory))
		{
			if (entry.status().type() == fs::file_type::directory)
			{
				const auto name = entry.path().filename().wstring();
				auto node = destValue.FindMember(name);
				if (node != destValue.MemberEnd())
				{
					auto& category = cntLevelCategories.IsArray() ? *std::find_if(cntLevelCategories.Begin(), cntLevelCategories.End(),
						[&](auto& it) { return it.GetString() == name; }) : cntLevelCategories.FindMember(name)->value;
					LoadStorageRecursive(entry, category, node->value, allocator);
				}
			}
		}
	}
}
