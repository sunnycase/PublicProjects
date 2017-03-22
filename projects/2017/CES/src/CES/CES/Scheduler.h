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

	interface DECLSPEC_UUID("20400FB4-9B98-40E4-BDD3-224E698FDA83") ISamplePresenter : public IUnknown
	{
		virtual void PresentSample(IMFSample* sample, MFTIME hnsTarget, MFTIME hnsTimeDelta, size_t remainingInQueue, MFTIME hnsFrameDurationDiv4) = 0;
	};

	class Scheduler : public NS_CORE::WeakReferenceBase<Scheduler, WRL::RuntimeClassFlags<WRL::ClassicCom>, IUnknown>
	{
	public:
		Scheduler();

		float GetClockRate() const noexcept { return _clockRate; }
		void SetClockRate(float rate);

		void SetFrameDropThrehold(int threhold) {}
		int GetFrameDropThrehold() const noexcept { return _frameDropThrehold; }

		void StartScheduler(IMFClock* clock);
		void StopScheduler();
		void ScheduleSample(IMFSample* sample, bool presentNow);
		void SetSamplePresenter(ISamplePresenter* presenter);

		MFTIME GetLastSampleTime() const noexcept { return _lastSampleTime; }
		MFTIME GetFrameDuration() const noexcept { return _perFrameInterval; }
	private:
		void ProcessSample(IMFSample* sample);
	private:
		float _clockRate;
		int _frameDropThrehold;
		WRL::Wrappers::CriticalSection _stateLock;
		std::shared_ptr<NS_CORE::OperationQueue<PreparedSample>> _samplesQueue;
		WRL::ComPtr<IMFClock> _clock;
		MFTIME _lastSampleTime;
		MFTIME _perFrameInterval;
		MFTIME _perFrame1_4th;
		WRL::ComPtr<ISamplePresenter> _samplePresenter;
	};
}