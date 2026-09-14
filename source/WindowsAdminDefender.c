/*
 * fltKernel.h is the file-system minifilter header. It transitively includes
 * ntifs.h and provides the kernel definitions this driver needs. Do NOT also
 * include ntddk.h: including both pulls conflicting definitions of PEPROCESS/
 * PETHREAD (ntifs.h vs. ntddk.h) and fails to compile.
 */
#include <fltKernel.h>
#include "WindowsAdminDefender.h"

/*
 * Windows Admin Defender directory-locking minifilter.
 *
 * Policy: protected directories may still be traversed by path so ordinary
 * file opens and reads continue to work, but attempts to obtain a directory
 * handle are denied. That prevents directory-handle based enumeration,
 * directory copying, deletion, and rename operations while preserving normal
 * file I/O.
 *
 * This is a defensive baseline. NTFS ACLs, EFS, BitLocker, Defender, and
 * standard Windows administrator controls remain independent security layers.
 */

static PFLT_FILTER gFilter = NULL;

static const WCHAR* const WAD_PROTECTED_SUFFIXES[] = {
    L"\\Windows\\System32",
    L"\\Windows\\SysWOW64",
    L"\\Windows\\WinSxS",
    L"\\Program Files",
    L"\\Program Files (x86)",
    L"\\ProgramData"
};

static BOOLEAN WadContainsProtectedSuffix(
    _In_ PCUNICODE_STRING Name,
    _In_ PCUNICODE_STRING Suffix)
{
    USHORT i;
    USHORT maxStart;

    if (Name == NULL || Suffix == NULL || Name->Buffer == NULL ||
        Suffix->Buffer == NULL || Suffix->Length == 0 ||
        Name->Length < Suffix->Length) {
        return FALSE;
    }

    maxStart = (USHORT)(Name->Length - Suffix->Length);
    for (i = 0; i <= maxStart / sizeof(WCHAR); ++i) {
        UNICODE_STRING candidate;
        candidate.Buffer = Name->Buffer + i;
        candidate.Length = Suffix->Length;
        candidate.MaximumLength = Suffix->Length;

        if (RtlEqualUnicodeString(&candidate, Suffix, TRUE)) {
            return TRUE;
        }
    }

    return FALSE;
}

static BOOLEAN WadPathIsProtected(_In_ PFLT_CALLBACK_DATA Data)
{
    PFLT_FILE_NAME_INFORMATION nameInfo = NULL;
    NTSTATUS status;
    BOOLEAN protectedPath = FALSE;
    ULONG i;

    status = FltGetFileNameInformation(
        Data,
        FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
        &nameInfo);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    status = FltParseFileNameInformation(nameInfo);
    if (NT_SUCCESS(status)) {
        for (i = 0; i < RTL_NUMBER_OF(WAD_PROTECTED_SUFFIXES); ++i) {
            UNICODE_STRING suffix;
            RtlInitUnicodeString(&suffix, WAD_PROTECTED_SUFFIXES[i]);

            if (WadContainsProtectedSuffix(&nameInfo->Name, &suffix)) {
                protectedPath = TRUE;
                break;
            }
        }
    }

    FltReleaseFileNameInformation(nameInfo);
    return protectedPath;
}

static FLT_PREOP_CALLBACK_STATUS WadPreCreate(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    /* Only deny requests that explicitly ask for a directory object. */
    if (FlagOn(Data->Iopb->Parameters.Create.Options, FILE_DIRECTORY_FILE) &&
        WadPathIsProtected(Data)) {
        Data->IoStatus.Status = STATUS_ACCESS_DENIED;
        Data->IoStatus.Information = 0;
        return FLT_PREOP_COMPLETE;
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

static CONST FLT_OPERATION_REGISTRATION gCallbacks[] = {
    { IRP_MJ_CREATE, 0, WadPreCreate, NULL },
    { IRP_MJ_OPERATION_END }
};

static CONST FLT_REGISTRATION gRegistration = {
    sizeof(FLT_REGISTRATION),           /* Size */
    FLT_REGISTRATION_VERSION,           /* Version */
    0,                                  /* Flags */
    NULL,                               /* ContextRegistration */
    gCallbacks,                         /* OperationRegistration */
    NULL,                               /* FilterUnloadCallback */
    NULL,                               /* InstanceSetupCallback */
    NULL,                               /* InstanceQueryTeardownCallback */
    NULL,                               /* InstanceTeardownStartCallback */
    NULL,                               /* InstanceTeardownCompleteCallback */
    NULL,                               /* GenerateFileNameCallback */
    NULL,                               /* NormalizeNameComponentCallback */
    NULL,                               /* NormalizeContextCleanupCallback */
    NULL,                               /* TransactionNotificationCallback */
    NULL,                               /* NormalizeNameComponentExCallback */
    NULL                                /* SectionNotificationCallback */
};

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject,
                     _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;
    UNREFERENCED_PARAMETER(RegistryPath);

    status = FltRegisterFilter(DriverObject, &gRegistration, &gFilter);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = FltStartFiltering(gFilter);
    if (!NT_SUCCESS(status)) {
        FltUnregisterFilter(gFilter);
        gFilter = NULL;
        return status;
    }

    return STATUS_SUCCESS;
}
