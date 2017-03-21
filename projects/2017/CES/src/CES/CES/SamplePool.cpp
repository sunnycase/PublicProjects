#include "stdafx.h"
#include "SamplePool.h"
#include <evr.h>

using namespace WRL;
using namespace CES;

#define LOCK_STATE() auto locker = _stateLock.Lock()

namespace
{
#define INITGUID
#include <guiddef.h>

	// {ADE43674-0887-4F9B-B3CB-78AE8EB9807D}   MFSampleExtension_PoolVersion          {UINT32}
	DEFINE_GUID(MFSampleExtension_PoolVersion,
		0xade43674, 0x887, 0x4f9b, 0xb3, 0xcb, 0x78, 0xae, 0x8e, 0xb9, 0x80, 0x7d);
}

CES::SamplePool::SamplePool()
{
}

bool CES::SamplePool::TryGetSample(IMFSample ** sample)
{
	LOCK_STATE();

	if (_samples.empty())
		return false;
	else
	{
		auto tmp = std::move(_samples.front());
		_samples.pop();
		*sample = tmp.Detach();
		_pendings++;
		return true;
	}
}

void CES::SamplePool::ReturnSample(IMFSample * sample)
{
	LOCK_STATE();

	if (IsContainerOf(sample))
	{
		_samples.emplace(sample);
		_pendings--;
	}
}

bool CES::SamplePool::HasPendingSample() const noexcept
{
	return _pendings.load(std::memory_order_acquire) != 0;
}

bool CES::SamplePool::IsContainerOf(IMFSample * sample) const
{
	auto version = _version.load(std::memory_order_acquire);
	UINT32 sampleVersion;
	if (SUCCEEDED(sample->GetUINT32(MFSampleExtension_PoolVersion, &sampleVersion)))
		return version == sampleVersion;
	return false;
}

void CES::SamplePool::ClearDesiredSampleTime(IMFSample * sample) const
{
	ComPtr<IMFDesiredSample> desiredSample;
	if (SUCCEEDED(sample->QueryInterface(IID_PPV_ARGS(&desiredSample))))
	{
		UINT32 version;
		if (SUCCEEDED(sample->GetUINT32(MFSampleExtension_PoolVersion, &version)))
		{
			desiredSample->Clear();
			ThrowIfFailed(sample->SetUINT32(MFSampleExtension_PoolVersion, version));
		}
	}
}

void CES::SamplePool::EmplaceSample(IMFSample * sample, uint32_t version)
{
	ThrowIfFailed(sample->SetUINT32(MFSampleExtension_PoolVersion, version));
	_samples.emplace(sample);
}