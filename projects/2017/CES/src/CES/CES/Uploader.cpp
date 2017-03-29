#include "stdafx.h"
#include "Uploader.h"
#include <Tomato.Core/NonCopyable.h>
#include <minizip\zip.h>
#include <minizip\ioapi_mem.h>
#include <filesystem>
using namespace WRL;
using namespace CES;
namespace fs = std::experimental::filesystem;

namespace
{
	class zipFile_raii : NonCopyable
	{
	public:
		zipFile_raii() noexcept
			: _file(nullptr)
		{

		}

		explicit zipFile_raii(zipFile file) noexcept
			: _file(file)
		{

		}

		~zipFile_raii()
		{
			if (_file)
				zipClose(_file, nullptr);
		}

		zipFile detach() noexcept
		{
			auto old = _file;
			_file = nullptr;
			return old;
		}

		zipFile get() const noexcept
		{
			return _file;
		}

		void reset(zipFile file = nullptr, const char* comment = nullptr)
		{
			if (_file)
				zipClose(_file, comment);
			_file = file;
		}

		bool valid() const noexcept
		{
			return _file != nullptr;
		}
	private:
		zipFile _file;
	};
}

Uploader::Uploader()
{
}

namespace
{
	uint32_t FileTimeToDosDateTime(const FILETIME& fileTime)
	{
		WORD date, time;
		ThrowWin32IfNot(::FileTimeToDosDateTime(&fileTime, &date, &time));
		return MAKELONG(time, date);
	}

	uint32_t TryGetFileDosTime(std::wstring_view fileName)
	{
		try
		{
			WRL::Wrappers::FileHandle file(CreateFile(fileName.data(), FILE_READ_ATTRIBUTES, 0, nullptr, OPEN_EXISTING, 0, nullptr));
			ThrowWin32IfNot(file.IsValid());
			FILETIME creationTime;
			ThrowWin32IfNot(GetFileTime(file.Get(), &creationTime, nullptr, nullptr));
			FileTimeToLocalFileTime(&creationTime, &creationTime);
			return FileTimeToDosDateTime(creationTime);
		}
		catch(...)
		{
			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);
			FILETIME creationTime{};
			SystemTimeToFileTime(&sysTime, &creationTime);
			FileTimeToLocalFileTime(&creationTime, &creationTime);
			return FileTimeToDosDateTime(creationTime);
		}
	}
}

void Uploader::Upload(IStream * imageStream, std::wstring && fileName)
{
	ULARGE_INTEGER size;
	ThrowIfFailed(imageStream->Seek({ 0 }, SEEK_CUR, &size));
	ThrowIfNot(!size.HighPart, L"Image file is too large.");

	zlib_filefunc_def filefunc32{};
	ourmemory_t zipmem{};
	zipmem.grow = 1;
	fill_memory_filefunc(&filefunc32, &zipmem);
	auto fin = make_finalizer([&] { free(zipmem.base); zipmem.base = nullptr; });
	zipFile_raii zipFile(zipOpen3("__notused__", APPEND_STATUS_CREATE, 0, 0, &filefunc32));
	ThrowIfNot(zipFile.valid(), L"Cannot create zip file.");

	auto imageFileName = fs::path(fileName).filename().string();
	auto zipFileName = fs::path(fileName).filename().replace_extension(L".zip").string();

	zip_fileinfo info{};
	info.dos_date = TryGetFileDosTime(fileName);
	ThrowIfNot(zipOpenNewFileInZip(zipFile.get(), imageFileName.data(), &info, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == Z_OK,
		L"Cannot open new file in zip.");
	HGLOBAL hMem;
	ThrowIfFailed(GetHGlobalFromStream(imageStream, &hMem));
	auto fin2 = make_finalizer([&] {GlobalUnlock(hMem); });
	const auto imageData = GlobalLock(hMem);
	ThrowIfNot(imageData, L"Cannot lock image memory.");
	ThrowIfNot(zipWriteInFileInZip(zipFile.get(), imageData, size.LowPart) == Z_OK, L"Cannot write image to zip file.");
	ThrowIfNot(zipCloseFileInZip(zipFile.get()) == Z_OK, L"Cannot close image file.");
	ThrowIfFailed(zipClose(zipFile.detach(), nullptr) == Z_OK, L"Cannot close zip file.");
}