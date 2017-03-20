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

void CES::Scheduler::ProcessSample(IMFSample* sample)
{
	if (auto clock = _clock)
	{
		OutputDebugString(L"Process sample.\r\n");
	}
}
