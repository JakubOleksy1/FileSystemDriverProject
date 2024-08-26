#pragma once

#include <ntifs.h>

#define FILE_SYSTEM_TAG 'FSYS'
#define FILE_CONTROL_BLOCK_TAG 'FCB '
#define IO_PACKET_TAG 'IOPB'
#define FILE_OPERATION_TAG 'FOPT'
#define METADATA_TAG 'META'
#define DRIVER_INTERNAL_TAG 'DRIV'
#define SYNCHRONIZATION_TAG 'SYNC'

#define DEVICE_NAME L"\\Device\\WFSDevice"
#define SYMBOLIC_LINK_NAME L"\\DosDevices\\W:"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT DriverObject);

typedef struct _FILE_SYSTEM_DATA {
    LIST_ENTRY FileList;
    KEVENT Event;

} FILE_SYSTEM_DATA, * PFILE_SYSTEM_DATA;

extern PFILE_SYSTEM_DATA GlobalFileSystemData;

NTSTATUS InitializeDriverResources();
NTSTATUS CleanupDriverResources();
//NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject);
//NTSTATUS DeleteDevice(PDRIVER_OBJECT DriverObject);