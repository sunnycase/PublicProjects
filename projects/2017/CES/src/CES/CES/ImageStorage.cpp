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

void CES::ImageStorage::SetSessionId(std::wstring_view sessionId)
{
	_sessionId = sessionId;
	Refresh();
}

void CES::ImageStorage::Refresh()
{
	EnsureRootDirectoryExists();

	rapidjson::MemoryPoolAllocator<> allocator;
	_storageJson.RemoveAllMembers();
	for (auto&& entry : fs::directory_iterator(_rootDir))
	{
		if (entry.status().type() == fs::file_type::directory)
		{
			rapidjson::GenericValue<rapidjson::UTF16<>> sessionObj;
			
			_storageJson.AddMember(rapidjson::StringRef(entry.path().filename().wstring()), sessionObj, allocator);
		}
	}
}

void CES::ImageStorage::EnsureRootDirectoryExists()
{
	fs::create_directories(_rootDir);
}
