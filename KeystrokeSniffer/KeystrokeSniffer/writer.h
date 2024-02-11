#pragma once
#include <main.h>


extern EX_RUNDOWN_REF WriterRef;
extern bool Unload; 

void Writer(PVOID StartContext);
NTSTATUS WriteKeys(PUCHAR data, ULONG dataSize);