#pragma once
#include <ntifs.h>
#include <dontuse.h>
#include <ntddk.h>
#include <new.h>
#include <delete.h>
#include <attach.h>
#include <filter.h>
#include <objreference.h>
#include <ntddkbd.h>
#include <spinlock.h>
#include <autolock.h>
#include <log.h>
#include <writer.h>
#include <kbd.h>


#define TAG 'ksnf'
#define KEYBOARD_DEVICE_NAME L"\\Device\\KeyboardClass0"

struct DeviceExtension {
	PDEVICE_OBJECT LowerDeviceObject;
};


