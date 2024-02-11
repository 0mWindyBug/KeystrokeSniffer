#pragma once
#include <main.h>

extern PDEVICE_OBJECT FilterDevice;

NTSTATUS AttachDevice(PDRIVER_OBJECT FilterDriverObject);
void DetachDevice(PDEVICE_OBJECT FilterDevice);


