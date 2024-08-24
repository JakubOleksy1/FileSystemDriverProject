#pragma warning(disable : 4100)

#include "WfsDriver.h"
#include "Messages.h"

PFILE_SYSTEM_DATA GlobalFileSystemData;

NTSTATUS YourCreateRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS YourCloseRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS YourReadRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS YourWriteRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS YourQueryInformationRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS YourDirectoryControlRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {
	NTSTATUS status;

	//pDriverObject->MajorFunction[IRP_MJ_CREATE] = YourCreateRoutine;
	//pDriverObject->MajorFunction[IRP_MJ_CLOSE] = YourCloseRoutine;
	//pDriverObject->MajorFunction[IRP_MJ_READ] = YourReadRoutine;
	//pDriverObject->MajorFunction[IRP_MJ_WRITE] = YourWriteRoutine;
	//pDriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = YourQueryInformationRoutine;
	//pDriverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] = YourDirectoryControlRoutine;

	pDriverObject->DriverUnload = UnloadDriver;

	status = InitializeDriverResources();
	if (!NT_SUCCESS(status)) {
		CleanupDriverResources();
		return status;
	}

	status = CreateDevice(pDriverObject);
	if (!NT_SUCCESS(status)) {
		CleanupDriverResources();
		return status;
	}

	DebugMessage("DriverEntry - full success!");

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {
	CleanupDriverResources();
	DeleteDevice(pDriverObject);

	DebugMessage("UnloadDriver unloaded driver!");
	return STATUS_SUCCESS;
}

NTSTATUS InitializeDriverResources() {
	GlobalFileSystemData = (PFILE_SYSTEM_DATA)ExAllocatePool2(
		POOL_FLAG_NON_PAGED,  // use POOL_FLAG_PAGED for paged pool if needed (whatever it is)
		sizeof(FILE_SYSTEM_DATA),
		FILE_SYSTEM_TAG
	);
	if (GlobalFileSystemData == NULL) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	InitializeListHead(&GlobalFileSystemData->FileList);
	KeInitializeEvent(&GlobalFileSystemData->Event, SynchronizationEvent, FALSE);

	return STATUS_SUCCESS;
}

NTSTATUS CleanupDriverResources() {
	if (GlobalFileSystemData != NULL) {
		ExFreePoolWithTag(GlobalFileSystemData, FILE_SYSTEM_TAG);
		GlobalFileSystemData = NULL;
	}

	return STATUS_SUCCESS;
}

NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject) {
	NTSTATUS status;
	PDEVICE_OBJECT deviceObject;
	UNICODE_STRING deviceName;
	UNICODE_STRING symbolicLinkName;

	RtlInitUnicodeString(&deviceName, DEVICE_NAME);
	status = IoCreateDevice(
		DriverObject,
		0,
		&deviceName,
		FILE_DEVICE_DISK,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&deviceObject
	);

	if (!NT_SUCCESS(status)) {
		return status;
	}

	RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);
	status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);

	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(deviceObject);
		return status;
	}

	return STATUS_SUCCESS;
}

NTSTATUS DeleteDevice(PDRIVER_OBJECT pDriverObject) {
	UNICODE_STRING symbolicLinkName;

	RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);
	IoDeleteSymbolicLink(&symbolicLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);

	return STATUS_SUCCESS;
}