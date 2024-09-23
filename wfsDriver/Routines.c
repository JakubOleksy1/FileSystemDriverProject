#include <ntifs.h>   
#include "WfsDriver.h"
#include "Routines.h"
#include "Messages.h"

NTSTATUS CreateRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    (void)DeviceObject; // suppress unused parameter warning
    (void)Irp;          // suppress unused parameter warning

    // implementation of the create routine

    return STATUS_SUCCESS;
}

NTSTATUS CloseRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    (void)DeviceObject; // suppress unused parameter warning
    (void)Irp;          // suppress unused parameter warning

    // implementation of the create routine

    return STATUS_SUCCESS;
}

NTSTATUS ReadRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
    PFILE_OBJECT fileObject = ioStackLocation->FileObject;
    PVOID readBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG readLength = ioStackLocation->Parameters.Read.Length;
    ULONG fileOffset = ioStackLocation->Parameters.Read.ByteOffset.LowPart;

    // Check if the buffer is valid
    if (readBuffer == NULL || readLength == 0) {
        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_INVALID_PARAMETER;
    }

    // Ensure we have enough data to read from our file system's internal structure
    NTSTATUS status = STATUS_SUCCESS;

    __try {
        // Simulate file content manipulation; copy file data to buffer.
        PCHAR fileData = fileObject->FsContext; // Points to the file's in-memory data structure

        // Ensure we don't read beyond the file size.
        if (fileOffset + readLength > MAX_FILE_SIZE) {
            readLength = MAX_FILE_SIZE - fileOffset;
        }

        RtlCopyMemory(readBuffer, fileData + fileOffset, readLength); // Read the data
        Irp->IoStatus.Information = readLength;  // Indicate how much was read

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    // Complete the IRP and set the status
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS WriteRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
    PFILE_OBJECT fileObject = ioStackLocation->FileObject;
    PVOID writeBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG writeLength = ioStackLocation->Parameters.Write.Length;

    // Check if the buffer is valid
    if (writeBuffer == NULL || writeLength == 0) {
        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_INVALID_PARAMETER;
    }

    // Ensure we have enough space in our file system's internal structure
    NTSTATUS status = STATUS_SUCCESS;

    __try {
        PCHAR fileData = fileObject->FsContext; // Points to the file's in-memory data structure
        ULONG fileOffset = ioStackLocation->Parameters.Write.ByteOffset.LowPart; // Start writing at this position

        // Ensure we don't exceed the buffer size.
        if (fileOffset + writeLength > MAX_FILE_SIZE) {
            status = STATUS_DISK_FULL;  // Handle a "disk full" or similar error.
        }
        else {
            RtlCopyMemory(fileData + fileOffset, writeBuffer, writeLength); // Write the data
            Irp->IoStatus.Information = writeLength;  // Indicate how much was written
        }

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS QueryInformationRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {

    UNREFERENCED_PARAMETER(DeviceObject); // Suppress the unused parameter warning

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    PFILE_OBJECT fileObject = irpSp->FileObject;
    FILE_INFORMATION_CLASS fileInformationClass = irpSp->Parameters.QueryFile.FileInformationClass;
    PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG length = irpSp->Parameters.QueryFile.Length;

    FILE_BASIC_INFORMATION basicInfo;
    FILE_STANDARD_INFORMATION standardInfo;
    IO_STATUS_BLOCK ioStatus;

    if (!fileObject || !buffer) {
        status = STATUS_INVALID_PARAMETER;
        DebugMessage("QueryInformationRoutine: Invalid file object or buffer");
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    HANDLE fileHandle;
    status = ObOpenObjectByPointer(fileObject, OBJ_KERNEL_HANDLE, NULL, FILE_READ_ATTRIBUTES, *IoFileObjectType, KernelMode, &fileHandle);

    if (!NT_SUCCESS(status)) {
        DebugMessage("QueryInformationRoutine: Failed to get handle for file object, status 0x%X", status);
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    // Handle different types of file information queries
    switch (fileInformationClass) {
    case FileBasicInformation: {
        if (length < sizeof(FILE_BASIC_INFORMATION)) {
            status = STATUS_BUFFER_TOO_SMALL;
        }
        else {
            status = ZwQueryInformationFile(fileHandle, &ioStatus, &basicInfo, sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
            if (NT_SUCCESS(status)) {
                RtlCopyMemory(buffer, &basicInfo, sizeof(FILE_BASIC_INFORMATION));
            }
        }
        break;
    }

    case FileStandardInformation: {
        if (length < sizeof(FILE_STANDARD_INFORMATION)) {
            status = STATUS_BUFFER_TOO_SMALL;
        }
        else {
            status = ZwQueryInformationFile(fileHandle, &ioStatus, &standardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
            if (NT_SUCCESS(status)) {
                RtlCopyMemory(buffer, &standardInfo, sizeof(FILE_STANDARD_INFORMATION));
            }
        }
        break;
    }

    default: {
        status = STATUS_INVALID_INFO_CLASS;
        DebugMessage("QueryInformationRoutine: Unsupported file information class");
        break;
    }
    }

    ZwClose(fileHandle);
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS DirectoryControlRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
    FILE_INFORMATION_CLASS fileInformationClass = ioStackLocation->Parameters.QueryDirectory.FileInformationClass;
    PUNICODE_STRING directoryName = ioStackLocation->Parameters.QueryDirectory.FileName;
    BOOLEAN restartScan = ioStackLocation->Flags & SL_RESTART_SCAN;
    BOOLEAN returnSingleEntry = ioStackLocation->Flags & SL_RETURN_SINGLE_ENTRY;
    PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG length = ioStackLocation->Parameters.QueryDirectory.Length;

    HANDLE directoryHandle;
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatusBlock;

    // Open the directory handle
    OBJECT_ATTRIBUTES objAttributes;
    InitializeObjectAttributes(&objAttributes, directoryName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwOpenFile(
        &directoryHandle,
        FILE_LIST_DIRECTORY | SYNCHRONIZE,
        &objAttributes,
        &ioStatusBlock,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
    );

    if (!NT_SUCCESS(status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    // Query the directory
    status = ZwQueryDirectoryFile(
        directoryHandle,
        NULL,
        NULL,
        NULL,
        &ioStatusBlock,
        buffer,
        length,
        fileInformationClass,
        returnSingleEntry,
        NULL,
        restartScan
    );

    ZwClose(directoryHandle);

    // Set the status of the IRP
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = ioStatusBlock.Information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS DeleteFileRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject); 

    PIO_STACK_LOCATION ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
    PFILE_OBJECT fileObject = ioStackLocation->FileObject;  // Get the file object from the IRP stack
    PUNICODE_STRING fileName = &fileObject->FileName;       // Get the file name from the file object

    OBJECT_ATTRIBUTES objAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    HANDLE fileHandle;
    NTSTATUS status;

    InitializeObjectAttributes(&objAttributes, fileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // Open the file with delete access
    status = ZwOpenFile(
        &fileHandle,
        DELETE,
        &objAttributes,
        &ioStatusBlock,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
    );

    if (!NT_SUCCESS(status)) {
        DebugMessage("Failed to open file for deletion, status: 0x%X", status);
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    // Mark the file for deletion
    FILE_DISPOSITION_INFORMATION fileInfo;
    fileInfo.DeleteFile = TRUE;

    status = ZwSetInformationFile(
        fileHandle,
        &ioStatusBlock,
        &fileInfo,
        sizeof(FILE_DISPOSITION_INFORMATION),
        FileDispositionInformation
    );

    ZwClose(fileHandle);

    // Complete the IRP
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}