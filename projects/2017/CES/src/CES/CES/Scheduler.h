#pragma once
#include <Tomato.Core/Tomato.Core.h>
#include <Tomato.Core/WeakReferenceBase.h>
#include <Tomato.Core/WorkerThread.h>
#include <mfidl.h>
#include <Tomato.Core/OperationQueue.h>

namespace CES
{
	struct PreparedSample
	{
		WRL::ComPtr<IMFSample> Sample;
	};

	class Scheduler : public NS_CORE::WeakReferenceBase<Scheduler, WRL::RuntimeClassFlags<WRL::ClassicCom>, IUnknown>
	{
	public:
		Scheduler();

		void SetClockRate(float rate);

		void StartScheduler(IMFClock* clock);
		void StopScheduler();
		void ScheduleSample(IMFSample* sample, bool presentNow);
	private:
		void ProcessSample(IMFSample* sample);
	private:
		float _clockRate;
		WRL::Wrappers::CriticalSection _stateLock;
		std::shared_ptr<NS_CORE::OperationQueue<PreparedSample>> _samplesQueue;
		WRL::ComPtr<IMFClock> _clock;
	};
}