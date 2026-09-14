#include <ntddk.h>

#define WAD_DEVICE_NAME L"\\Device\\WindowsAdminDefender"
#define WAD_DOS_NAME    L"\\DosDevices\\WindowsAdminDefender"

static VOID WadUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING dosName;
    RtlInitUnicodeString(&dosName, WAD_DOS_NAME);
    IoDeleteSymbolicLink(&dosName);

    if (DriverObject->DeviceObject != NULL) {
        IoDeleteDevice(DriverObject->DeviceObject);
    }
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject,
                     _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    UNICODE_STRING deviceName;
    UNICODE_STRING dosName;
    PDEVICE_OBJECT deviceObject = NULL;
    NTSTATUS status;

    RtlInitUnicodeString(&deviceName, WAD_DEVICE_NAME);
    RtlInitUnicodeString(&dosName, WAD_DOS_NAME);

    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = IoCreateSymbolicLink(&dosName, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    DriverObject->DriverUnload = WadUnload;
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}
