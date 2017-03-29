#include "stdafx.h"
#include "ImageStorage.h"
#include <rapidjson/include/rapidjson/writer.h>
#include <rapidjson/include/rapidjson/stringbuffer.h>
#include <sstream>
#include <iomanip>

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
	template<class TAllocator>
	decltype(auto) AddMember(const std::wstring& name, rapidjson::GenericValue<rapidjson::UTF16<>>& sessionObj, TAllocator& allocator)
	{
		rapidjson::GenericValue<rapidjson::UTF16<>> nodeObj(rapidjson::kObjectType);
		sessionObj.AddMember({ name, allocator }, std::move(nodeObj), allocator);
		return std::ref(sessionObj.FindMember(name)->value);
	}

	template<class TIt, class TAllocator>
	decltype(auto) AddMember(TIt it, rapidjson::GenericValue<rapidjson::UTF16<>>& sessionObj, TAllocator& allocator)
	{
		rapidjson::GenericValue<rapidjson::UTF16<>> nodeObj(rapidjson::kObjectType);
		sessionObj.AddMember(rapidjson::StringRef(it->GetString(), it->GetStringLength()), std::move(nodeObj), allocator);
		return std::ref(sessionObj.FindMember(*it)->value);
	}

	template<class TAllocator>
	void InitializeSessionObjRecursive(const rapidjson::GenericValue<rapidjson::UTF16<>>& categories, rapidjson::GenericValue<rapidjson::UTF16<>>& sessionObj, TAllocator& allocator)
	{
		assert(categories.IsArray() || categories.IsObject());
		if (categories.IsArray())
		{
			for (auto it = categories.Begin(); it != categories.End(); ++it)
			{
				if (it->IsObject())
					InitializeSessionObjRecursive(*it, sessionObj, allocator);
				else
				{
					assert(it->IsString());
					rapidjson::GenericValue<rapidjson::UTF16<>> nodeObj(rapidjson::kArrayType);
					sessionObj.AddMember(rapidjson::StringRef(it->GetString(), it->GetStringLength()), std::move(nodeObj), allocator);
				}
			}
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

	for (auto&& entry : fs::directory_iterator(_rootDir))
	{
		if (entry.status().type() == fs::file_type::directory)
		{
			rapidjson::GenericValue<rapidjson::UTF16<>> name(entry.path().filename().wstring(), _allocator);

			auto& sessionObj = AddMember(entry.path().filename().wstring(), _storageJson, _allocator);
			InitializeSessionObjRecursive(_categoriesJson, sessionObj, _allocator);
			LoadStorageRecursive(entry, _categoriesJson, sessionObj, _allocator);
		}
	}
}

std::wstring CES::ImageStorage::GetStorageJson() const
{
	rapidjson::GenericStringBuffer<rapidjson::UTF16<>> buffer;
	rapidjson::Writer<decltype(buffer), rapidjson::UTF16<>, rapidjson::UTF16<>> writer(buffer);
	_storageJson.Accept(writer);
	return { buffer.GetString(), buffer.GetLength() };
}

void CES::ImageStorage::SetSelectedPath(std::wstring&& path)
{
	if (path.empty())
	{
		_isPathSelected = false;
		_selectedPath.clear();
	}
	else
	{
		_selectedPath = _rootDir;
		_selectedPath.append(path);
		fs::create_directories(_selectedPath);
		_isPathSelected = true;
	}
}

std::wstring CES::ImageStorage::GetNextAvailableFileName() const
{
	ThrowIfNot(_isPathSelected, L"Path is not selected.");

	SYSTEMTIME time;
	GetLocalTime(&time);
	std::wstringstream ss;
	ss << time.wYear << std::setfill(L'0') << std::setw(2)
		<< time.wMonth << time.wDay;
	auto prefix = ss.str();
	fs::path path;
	size_t i = 0;
	do
	{
		path = _selectedPath;
		std::wstringstream ss;
		ss << L'_' << i++ << L".jpg";
		if (!fs::exists(path.append(prefix + ss.str())))
			break;
	} while (true);
	return path;
}

std::wstring CES::ImageStorage::GetFullPath(std::wstring && relativePath) const
{
	return fs::path(_rootDir).append(relativePath);
}

void CES::ImageStorage::EnsureRootDirectoryExists()
{
	fs::create_directories(fs::path(_rootDir).append(_sessionId));
}

namespace
{
	const rapidjson::GenericValue<rapidjson::UTF16<>>* FindCategory(const rapidjson::GenericValue<rapidjson::UTF16<>>& categories, const std::wstring& name)
	{
		if (categories.IsArray())
		{
			for (auto it = categories.Begin(); it != categories.End(); ++it)
			{
				if (auto value = FindCategory(*it, name))
					return value;
			}
		}
		else if (categories.IsObject())
		{
			auto member = categories.FindMember(name);
			if (member != categories.MemberEnd())
				return &member->value;
		}
		else
		{
			assert(categories.IsString());
			if (categories.GetString() == name)
				return &categories;
		}
		return nullptr;
	}
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
				fileObj.AddMember(L"fileName", entry.path().filename().wstring(), allocator);
				destValue.PushBack(std::move(fileObj), allocator);
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
					auto category = FindCategory(cntLevelCategories, name);
					assert(category);
					LoadStorageRecursive(entry, *category, node->value, allocator);
				}
			}
		}
	}
}
