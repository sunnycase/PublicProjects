#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <mfidl.h>
#include <atomic>
#include <queue>

namespace CES
{
	class SamplePool
	{
	public:
		SamplePool();

		bool TryGetSample(IMFSample** sample);
		void ReturnSample(IMFSample* sample);
		bool HasPendingSample() const noexcept;

		bool IsContainerOf(IMFSample* sample) const;
		void ClearDesiredSampleTime(IMFSample* sample) const;

		template<class TIt>
		void AssignSamples(TIt first, TIt last)
		{
			auto locker = _stateLock.Lock();
			_samples.swap(decltype(_samples)());
			auto version = ++_version;
			std::for_each(first, last, [&](auto&& it) { EmplaceSample(it.Get(), version); });
		}
	private:
		void EmplaceSample(IMFSample* sample, uint32_t version);
	private:
		WRL::Wrappers::CriticalSection _stateLock;
		std::atomic<size_t> _pendings = 0;
		std::queue<WRL::ComPtr<IMFSample>> _samples;
		std::atomic<uint32_t> _version = 0;
	};
}