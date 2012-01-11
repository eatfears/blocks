#pragma once
#include <windows.h>
#include <process.h>

class Mutex
{
	//friend class Lock;
public:
	Mutex () { InitializeCriticalSection(& _critSection); }
	~Mutex () { DeleteCriticalSection(& _critSection); }
//private:
	void Acquire () { EnterCriticalSection (& _critSection); }
	void Release () { LeaveCriticalSection (& _critSection); }
	CRITICAL_SECTION _critSection;
};

class Lock
{
public:
	// Acquire the state of the semaphore
	Lock ( Mutex & mutex )
		: _mutex(mutex)
	{
		_mutex.Acquire();
	}
	// Release the state of the semaphore
	~Lock ()
	{
		_mutex.Release();
	}
private:
	Mutex & _mutex;
};
