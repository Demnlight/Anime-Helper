#include "User.h"
#include "../Globals.h"
#include "../../Security/base64.h"

std::string CUser::GetHWID()
{
	HANDLE hFile = CreateFileA(Xorstr("\\\\.\\PhysicalDrive0"), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return { };

	std::unique_ptr< std::remove_pointer <HANDLE >::type, void(*)(HANDLE) > hDevice
	{
		hFile, [](HANDLE handle)
		{
			CloseHandle(handle);
		}
	};

	STORAGE_PROPERTY_QUERY PropertyQuery;
	PropertyQuery.PropertyId = StorageDeviceProperty;
	PropertyQuery.QueryType = PropertyStandardQuery;

	STORAGE_DESCRIPTOR_HEADER DescHeader;
	DWORD dwBytesReturned = 0;
	if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		&DescHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, NULL))
		return { };

	const DWORD dwOutBufferSize = DescHeader.Size;
	std::unique_ptr< BYTE[] > pOutBuffer{ new BYTE[dwOutBufferSize] { } };

	if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		pOutBuffer.get(), dwOutBufferSize, &dwBytesReturned, NULL))
		return { };

	STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(pOutBuffer.get());
	if (!pDeviceDescriptor)
		return { };

	const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
	if (!dwSerialNumberOffset)
		return { };

	std::string sResult = reinterpret_cast<const char*>(pOutBuffer.get() + dwSerialNumberOffset);
	sResult.erase(std::remove_if(sResult.begin(), sResult.end(), std::isspace), sResult.end());
	return sResult;
}