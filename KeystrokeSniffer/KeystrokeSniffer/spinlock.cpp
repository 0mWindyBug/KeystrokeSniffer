#include <main.h>


void SpinLock::Init() {
	KeInitializeSpinLock(&_spinlock);
}

void SpinLock::Lock() {
	KeAcquireSpinLock(&_spinlock, &_irql);
}

void SpinLock::Unlock() {
	KeReleaseSpinLock(&_spinlock, _irql);
}