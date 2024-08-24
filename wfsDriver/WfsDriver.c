#pragma warning(disable : 4100)

#include "WfsDriver.h"
#include "Messages.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {
	pDriverObject->DriverUnload = UnloadDriver;
	DebugMessage("Hello World!");

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {
	DebugMessage("Msg from UnloadDriver!");

	return STATUS_SUCCESS;
}