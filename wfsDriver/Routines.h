#pragma once

#include <ntddk.h>

NTSTATUS CreateRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS CloseRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ReadRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS WriteRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS QueryInformationRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DirectoryControlRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);

