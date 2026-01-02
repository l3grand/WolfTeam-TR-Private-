#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <ntdddisk.h>
#include <scsi.h>
#include <intrin.h>
#include <stdlib.h>
#include <Ntstrsafe.h>
#include <windef.h>

#pragma region please_move_me_to_header_file

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#define DRIVER_NAME "WolfVerifyX"

#define IO_INIT_REQUEST             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_REQUEST            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0667, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_REQUEST             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0668, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_PEB_REQUEST              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0669, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct init_t {
	DWORD32 gameId;
	NTSTATUS result;
} init, * p_init;

typedef struct read_t {
	DWORD_PTR address;
	void* response;
	SIZE_T size;
	NTSTATUS result;
} read, * p_read;

typedef struct write_t {
	DWORD_PTR address;
	void* value;
	SIZE_T size;
	NTSTATUS result;
} write, * p_write;

typedef struct peb_t {
	void* data;
	int module_type;
	NTSTATUS result;
} peb, * p_peb;

// Forward declarations
extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
	NTKERNELAPI PPEB NTAPI PsGetProcessPeb(IN PEPROCESS Process);
	NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process(_In_ PEPROCESS Process);
}

extern "C" extern POBJECT_TYPE* IoDriverObjectType;

// 32-bit structures
typedef struct _LIST_ENTRY32 {
	ULONG Flink;
	ULONG Blink;
} LIST_ENTRY32, * PLIST_ENTRY32;

typedef struct _UNICODE_STRING32 {
	USHORT Length;
	USHORT MaximumLength;
	ULONG Buffer;
} UNICODE_STRING32, * PUNICODE_STRING32;

typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	UCHAR Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderLinks;
	LIST_ENTRY32 InInitializationOrderLinks;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY32 HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG Ldr;
	ULONG ProcessParameters;
	ULONG SubSystemData;
	ULONG ProcessHeap;
	ULONG FastPebLock;
	ULONG AtlThunkSListPtr;
	ULONG IFEOKey;
	ULONG CrossProcessFlags;
	ULONG UserSharedInfoPtr;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	ULONG ApiSetMap;
} PEB32, * PPEB32;

#pragma endregion

namespace data {
	HANDLE gameId;
	PEPROCESS gameProcess;
	HANDLE cheatId;
	PEPROCESS cheatProcess;
}

void init_cheat_data(PIRP irp) {
	p_init pInitData = (p_init)irp->AssociatedIrp.SystemBuffer;
	if (pInitData) {
		data::gameId = (HANDLE)pInitData->gameId;
		pInitData->result = PsLookupProcessByProcessId(data::gameId, &data::gameProcess);
	}
}

PVOID get_module_entry(PEPROCESS pe, LPCWSTR ModuleName) {
	if (!pe) {
		return NULL;
	}

	__try {
		PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(pe);
		if (!pPeb32 || !pPeb32->Ldr) {
			return NULL;
		}

		PPEB_LDR_DATA32 pLdrData = (PPEB_LDR_DATA32)((ULONG_PTR)pPeb32->Ldr);

		for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((ULONG_PTR)pLdrData->InLoadOrderModuleList.Flink);
			pListEntry != &pLdrData->InLoadOrderModuleList;
			pListEntry = (PLIST_ENTRY32)((ULONG_PTR)pListEntry->Flink)) {

			PLDR_DATA_TABLE_ENTRY32 pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

			if (pEntry->BaseDllName.Buffer && pEntry->BaseDllName.Length > 0) {
				PWCHAR pModuleName = (PWCHAR)((ULONG_PTR)pEntry->BaseDllName.Buffer);
				if (pModuleName && _wcsicmp(pModuleName, ModuleName) == 0) {
					return (PVOID)((ULONG_PTR)pEntry->DllBase);
				}
			}
		}
		return NULL;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return NULL;
	}
}

NTSTATUS KeReadVirtualMemory(PEPROCESS process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes) {
	NTSTATUS status = STATUS_SUCCESS;
	__try {
		status = MmCopyVirtualMemory(data::gameProcess, SourceAddress, process, TargetAddress, Size, KernelMode, ReadedBytes);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_ACCESS_VIOLATION;
	}
	return status;
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size, PSIZE_T WrittenBytes) {
	NTSTATUS status = STATUS_SUCCESS;
	__try {
		status = MmCopyVirtualMemory(process, TargetAddress, data::gameProcess, SourceAddress, Size, KernelMode, WrittenBytes);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_ACCESS_VIOLATION;
	}
	return status;
}

NTSTATUS ctl_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	ULONG informationSize = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	const ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	p_read pReadRequest;
	p_write pWriteRequest;
	p_peb pPebRequest;

	switch (controlCode) {
	case IO_INIT_REQUEST:
		init_cheat_data(irp);
		informationSize = sizeof(init);
		break;

	case IO_WRITE_REQUEST:
		pWriteRequest = (p_write)irp->AssociatedIrp.SystemBuffer;
		if (pWriteRequest) {
			if (pWriteRequest->address < 0x7FFFFFFFFFFF && data::gameProcess) {
				SIZE_T bytes = 0;
				pWriteRequest->result = KeWriteVirtualMemory(PsGetCurrentProcess(),
					(PVOID)pWriteRequest->address, pWriteRequest->value, pWriteRequest->size, &bytes);
			}
			else {
				pWriteRequest->result = STATUS_ACCESS_VIOLATION;
			}
		}
		informationSize = sizeof(write);
		break;

	case IO_READ_REQUEST:
		pReadRequest = (p_read)irp->AssociatedIrp.SystemBuffer;
		if (pReadRequest) {
			if (pReadRequest->address < 0x7FFFFFFFFFFF && data::gameProcess) {
				SIZE_T bytes = 0;
				pReadRequest->result = KeReadVirtualMemory(PsGetCurrentProcess(),
					(PVOID)pReadRequest->address, pReadRequest->response, pReadRequest->size, &bytes);
			}
			else {
				pReadRequest->response = NULL;
				pReadRequest->result = STATUS_ACCESS_VIOLATION;
			}
		}
		informationSize = sizeof(read);
		break;

	case IO_PEB_REQUEST:
		pPebRequest = (p_peb)irp->AssociatedIrp.SystemBuffer;
		if (pPebRequest && data::gameProcess) {
			LPCWSTR moduleNames[] = {
				L"Wolfteam.bin",
				L"cshell.dll",
				L"object.lto",
				L"ole32.dll",
				L"MSVCR100.dll"
			};

			if (pPebRequest->module_type >= 0 && pPebRequest->module_type < 5) {
				KAPC_STATE apc;
				KeStackAttachProcess(data::gameProcess, &apc);
				PVOID base_address = get_module_entry(data::gameProcess, moduleNames[pPebRequest->module_type]);
				KeUnstackDetachProcess(&apc);

				pPebRequest->data = base_address;
				pPebRequest->result = pPebRequest->data ? STATUS_SUCCESS : STATUS_NOT_FOUND;
			}
			else {
				pPebRequest->data = NULL;
				pPebRequest->result = STATUS_INVALID_PARAMETER;
			}
		}
		informationSize = sizeof(peb);
		break;

	default:
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = informationSize;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS unsupported_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_NOT_SUPPORTED;
}

NTSTATUS create_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS close_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS InitializeDriver(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path) {
	UNREFERENCED_PARAMETER(registery_path);

	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING sym_link, dev_name;
	PDEVICE_OBJECT dev_obj;

	RtlInitUnicodeString(&dev_name, L"\\Device\\" DRIVER_NAME);
	status = IoCreateDevice(driver_obj, 0, &dev_name, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);

	if (!NT_SUCCESS(status)) {
		return status;
	}

	RtlInitUnicodeString(&sym_link, L"\\DosDevices\\" DRIVER_NAME);
	status = IoCreateSymbolicLink(&sym_link, &dev_name);

	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(dev_obj);
		return status;
	}

	dev_obj->Flags |= DO_BUFFERED_IO;

	for (int t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
		driver_obj->MajorFunction[t] = unsupported_io;

	driver_obj->MajorFunction[IRP_MJ_CREATE] = create_io;
	driver_obj->MajorFunction[IRP_MJ_CLOSE] = close_io;
	driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ctl_io;
	driver_obj->DriverUnload = NULL;

	dev_obj->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path) {
	UNREFERENCED_PARAMETER(registery_path);

	UNICODE_STRING drv_name;
	RtlInitUnicodeString(&drv_name, L"\\Driver\\" DRIVER_NAME);

	NTSTATUS status = IoCreateDriver(&drv_name, &InitializeDriver);
	return status;
}