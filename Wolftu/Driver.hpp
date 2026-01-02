#pragma once
#include <Windows.h>
#include <cstddef>
#include <string>
#include "lazy_importer.h"
#include "skCrypter.h"

#define DRIVER_NAME skCrypt(L"\\\\.\\WolfVerifyX")

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define FILE_DEVICE_UNKNOWN             0x00000022
#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)

#define IO_INIT_REQUEST             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // Init cheat
#define IO_WRITE_REQUEST            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0667, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // Write Proccess Memory
#define IO_READ_REQUEST             CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0668, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // Read Proccess Memory
#define IO_PEB_REQUEST              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0669, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) // Get PEB

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;

typedef struct init_t {
	DWORD32 gameId;
	NTSTATUS result;
} init, * p_init;

typedef struct read_t {
	DWORD_PTR address;
	void* response;
	SIZE_T size;
	NTSTATUS result;
} m_read, * p_read;

typedef struct write_t {
	DWORD_PTR address;
	void* value;
	SIZE_T size;
	NTSTATUS result;
} m_write, * p_write;

typedef struct peb_t {
	void* data;
	int module_type;
	NTSTATUS result;
} peb, * p_peb;

namespace memory {
	inline HANDLE driverHandle;
	__forceinline auto isvalidptr(uint64_t ptr) {
		if (ptr < 0x500 || ptr > 0x7FFFFFFFFFFF) return false;
		return true;
	}

	HANDLE DriverHandle;
	__forceinline auto getPid(const char* processName) -> DWORD
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = LI_FN(CreateToolhelp32Snapshot).get()(TH32CS_SNAPPROCESS, NULL);

		if (LI_FN(Process32First).get()(snapshot, &entry) == TRUE)
		{
			while (LI_FN(Process32Next).get()(snapshot, &entry) == TRUE)
			{
				if (_stricmp(entry.szExeFile, processName) == 0)
				{
					LI_FN(CloseHandle).get()(snapshot);

					return entry.th32ProcessID;
				}
			}
		}

		LI_FN(CloseHandle).get()(snapshot);

		return 0;
	}

	bool checkingDriver()
	{
		DriverHandle = LI_FN(CreateFileW).get()(DRIVER_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if (DriverHandle == INVALID_HANDLE_VALUE)
		{
			CloseHandle(DriverHandle);
			return false;
		}

		return true;
	};

	__forceinline auto initialize(DWORD pId) -> bool {

		driverHandle = LI_FN(CreateFileW).get()(DRIVER_NAME, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (driverHandle == INVALID_HANDLE_VALUE) [[unlikely]] {
			return false;
		}

		init_t ioData;
		ioData.gameId = pId;
		ioData.result = -1;

		LI_FN(DeviceIoControl).cached()(driverHandle, IO_INIT_REQUEST, &ioData, sizeof(ioData), &ioData, sizeof(ioData), nullptr, nullptr);
		return ioData.result == 0;
	}

	template <typename Type>
	__forceinline auto read(uintptr_t address) -> Type
	{
		if (address < 0x0 || address > 0x7FFFFFFFFFFF) return Type();
		read_t ioData;
		ioData.address = address;

		Type returnValue;
		ioData.response = &returnValue;
		ioData.size = sizeof(Type);
		ioData.result = -1;

		LI_FN(DeviceIoControl).cached()(driverHandle, IO_READ_REQUEST, &ioData, sizeof(ioData), &ioData, sizeof(ioData), nullptr, nullptr);
		if (ioData.result != 0) return Type();
		return *(Type*)&returnValue;
	}

	template <typename Type>
	__forceinline auto write(uint64_t address, Type value) -> NTSTATUS
	{
		if (address < 0x0 || address > 0x7FFFFFFFFFFF) return -1;
		write_t ioData;
		ioData.size = sizeof(Type);
		ioData.address = address;
		ioData.value = &value;
		ioData.result = -1;

		LI_FN(DeviceIoControl).cached()(driverHandle, IO_WRITE_REQUEST, &ioData, sizeof(ioData), &ioData, sizeof(ioData), nullptr, nullptr);
		return ioData.result;
	}

	__forceinline auto getModuleAddress(int p_module_type) -> uint64_t
	{
		peb_t ioDataBase;
		ioDataBase.data = 0;
		ioDataBase.result = -1;
		ioDataBase.module_type = p_module_type;
		LI_FN(DeviceIoControl).cached()(driverHandle, IO_PEB_REQUEST, &ioDataBase, sizeof(ioDataBase), &ioDataBase, sizeof(ioDataBase), nullptr, nullptr);
		uint64_t result = (unsigned long long int)ioDataBase.data;
		if (ioDataBase.result == 0) return result;
		else return 0x0;
	}
}