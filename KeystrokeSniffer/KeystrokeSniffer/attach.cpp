#include <main.h>

PDEVICE_OBJECT FilterDevice = nullptr;




void InitAttachedDevice(PDEVICE_OBJECT Device)
{
	for (int i = 0; i < ARRAYSIZE(Device->DriverObject->MajorFunction); i++)
		Device->DriverObject->MajorFunction[i] = FilterGenericDispatch;
	Device->DriverObject->MajorFunction[IRP_MJ_READ] = FilterReadDispatch;

}



NTSTATUS AttachDevice(PDRIVER_OBJECT FilterDriverObject)
{

	PDEVICE_OBJECT TargetDeviceObject;
	PDEVICE_OBJECT DeviceObject;
	PFILE_OBJECT FileObject;
	PDEVICE_OBJECT TopMostDevice;
	UNICODE_STRING TargetName;

	RtlInitUnicodeString(&TargetName, KEYBOARD_DEVICE_NAME);
	auto status = IoGetDeviceObjectPointer(&TargetName, FILE_READ_DATA, &FileObject, &TargetDeviceObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[*] failed getting target device object\n");
		return STATUS_ABANDONED;
	}
	FileRef File(FileObject);
	TopMostDevice = IoGetAttachedDeviceReference(TargetDeviceObject);
	if (!TopMostDevice)
	{
		DbgPrint("[*] failed get attached device reference\n");
		return STATUS_ABANDONED;
	}

	 status = IoCreateDevice(FilterDriverObject, sizeof(DeviceExtension), nullptr,
		TopMostDevice->DeviceType, 0, FALSE, &DeviceObject);
	 if (!NT_SUCCESS(status))
	 {
		 DbgPrint("[*] failed to create filter device\n");
		 return status;
	 }
	auto ext = (DeviceExtension*)DeviceObject->DeviceExtension;

	// make sure our filter attaches cleanly to the stack 
	DeviceObject->Flags |= (TopMostDevice->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO));

	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	DeviceObject->Flags |= DO_POWER_PAGABLE;

	DeviceObject->DeviceType = TopMostDevice->DeviceType;

	ExInitializeRundownProtection(&PendingIrps);


	InitAttachedDevice(DeviceObject);

	status = IoAttachDeviceToDeviceStackSafe(DeviceObject, TargetDeviceObject, &ext->LowerDeviceObject);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		DbgPrint("[*] failed to attach to keyboard stack\n");
		return status;
	}

	FilterDevice = DeviceObject;
	

	return status;
}




void DetachDevice(PDEVICE_OBJECT FilterDevice)
{
	auto ext = (DeviceExtension*)FilterDevice->DeviceExtension;
	IoDetachDevice(ext->LowerDeviceObject);
	IoDeleteDevice(FilterDevice);

}


