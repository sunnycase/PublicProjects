#include "stdafx.h"
#include "Scheduler.h"
#include <Tomato.Media/MFWorkerQueueProvider.h>

using namespace WRL;
using namespace CES;

#define LOCK_STATE() auto locker = _stateLock.Lock()

Scheduler::Scheduler()
	:_samplesQueue(std::make_shared<NS_CORE::OperationQueue<PreparedSample>>([weak = AsWeak()](PreparedSample sample)
{
	if (auto me = weak.Resolve())
		me->ProcessSample(sample.Sample.Get());
}))
{
	_samplesQueue->SetWorkerQueue(NS_MEDIA::MFWorkerQueueProvider::GetAudio());
}

void Scheduler::SetClockRate(float rate)
{
	LOCK_STATE();

	_clockRate = rate;
}

void CES::Scheduler::StartScheduler(IMFClock * clock)
{
	_clock = clock;
	_samplesQueue->SetEnabled(true);
}

void CES::Scheduler::StopScheduler()
{
	_samplesQueue->SetEnabled(false);
}

void CES::Scheduler::ScheduleSample(IMFSample * sample, bool presentNow)
{
	if (presentNow || !_clock)
		;
	else
		_samplesQueue->Enqueue(PreparedSample{ sample });
}

void CES::Scheduler::SetSamplePresenter(ISamplePresenter * presenter)
{
	_samplePresenter = presenter;
}

void CES::Scheduler::ProcessSample(IMFSample* sample)
{
	if (auto clock = _clock)
	{
		OutputDebugString(L"Process sample.\r\n");

		MFTIME hnsPresentationTime = 0;
		MFTIME hnsTimeNow = 0;
		MFTIME hnsSystemTime = 0;
		bool presentNow = false;

		if (SUCCEEDED(sample->GetSampleTime(&hnsPresentationTime)))
			clock->GetCorrelatedTime(0, &hnsTimeNow, &hnsSystemTime);
		auto hnsDelta = hnsPresentationTime - hnsTimeNow;
		auto rate = GetClockRate();
		if (rate < 0)
			hnsDelta = -hnsDelta;
		if(std::abs(rate) > 2)
			if (std::abs(hnsDelta) > _perFrameInterval * GetFrameDropThrehold())
			{
				OutputDebugString(L"Drop frame.\r\n");
				return;
			}
		if (hnsDelta < _perFrame1_4th)
			presentNow = true;
		else if (hnsDelta > 3 * _perFrame1_4th)
		{
			presentNow = false;
		}
		if (presentNow)
		{
			if (auto sp = _samplePresenter)
				sp->PresentSample(sample, hnsPresentationTime, hnsDelta, _samplesQueue->GetRemainingOperationsCount(), _perFrame1_4th);
		}
		else
			_samplesQueue->Enqueue(PreparedSample{ sample });
	}
}
