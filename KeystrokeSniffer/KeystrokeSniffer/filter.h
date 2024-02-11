#pragma once
#include <main.h>


extern EX_RUNDOWN_REF PendingIrps;

typedef struct _WORKER_CONTEXT
{
	PVOID64 Data;
	int Count;
}WORKER_CONTEXT, * PWORKER_CONTEXT;




NTSTATUS FilterGenericDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS FilterReadDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp);