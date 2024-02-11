#pragma once

#include <main.h>

class SpinLock {
public:
	void Init();

	void Lock();
	void Unlock();

private:
	KSPIN_LOCK _spinlock;
	KIRQL _irql;
};