/*
 * avrdude - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2020 manison <manison@manison.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <crtdbg.h>
#include "ftd2xx2libftdi.h"

#define VcppException(sev, err)	((sev) | (FACILITY_VISUALCPP<<16) | (err))
#define VcppModNotFound		VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND)
#define VcppProcNotFound	VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND)

static int IsDelayLoadException(unsigned int code)
{
	if (code == VcppModNotFound || code == VcppProcNotFound)
		return EXCEPTION_EXECUTE_HANDLER;

	return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef _M_X64
#	define FTD2XX_NAME	"ftd2xx64.dll"
#else
#	define FTD2XX_NAME	"ftd2xx.dll"
#endif

#define FT_MOD_NOT_FOUND	((FT_STATUS)ERROR_MOD_NOT_FOUND)

#define _d(call)		__try { \
					FT_STATUS __status = call; \
					if (!FT_SUCCESS(__status)) \
						ftdi->last_error = ft_strerr(__status); \
					return __status; \
				} \
				__except (IsDelayLoadException(GetExceptionCode())) { \
					ftdi->last_error = "Unable to find " FTD2XX_NAME; \
					return FT_MOD_NOT_FOUND; \
				}

static inline const char * ft_strerr(FT_STATUS status)
{
	static const char *msg[] = {
		"ok",
		"invalid handle",
		"device not found",
		"device not opened",
		"io error",
		"insufficient resources",
		"invalid parameter",
		"invalid baud rate",
		"device not opened for erase",
		"device not opened for write",
		"failed to write device",
		"eeprom read failed",
		"eeprom write failed",
		"eeprom erase failed",
		"eeprom not present",
		"eeprom not programmed",
		"invalid args",
		"not supported",
		"other error",
		"device list not ready",
	};
	
	if (status < sizeof(msg) / sizeof(msg[0]))
		return msg[status];
	return NULL;
}

static FT_STATUS FTd_OpenEx(struct ftdi_context *ftdi, PVOID pArg1, DWORD Flags)
{
	_d(FT_OpenEx(pArg1, Flags, &ftdi->handle))
}

static FT_STATUS FTd_Close(struct ftdi_context *ftdi)
{
	_d(FT_Close(ftdi->handle))
}

static FT_STATUS FTd_CreateDeviceInfoList(struct ftdi_context *ftdi, LPDWORD lpdwNumDevs)
{
	_d(FT_CreateDeviceInfoList(lpdwNumDevs))
}

static FT_STATUS FTd_GetDeviceInfoList(struct ftdi_context *ftdi, FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs)
{
	_d(FT_GetDeviceInfoList(pDest, lpdwNumDevs))
}

static FT_STATUS FTd_GetDeviceInfo(struct ftdi_context *ftdi, FT_DEVICE *lpftDevice, LPDWORD lpdwID, PCHAR SerialNumber, PCHAR Description, LPVOID Dummy)
{
	_d(FT_GetDeviceInfo(ftdi->handle, lpftDevice, lpdwID, SerialNumber, Description, Dummy));
}

static FT_STATUS FTd_SetBitMode(struct ftdi_context *ftdi, UCHAR ucMask, UCHAR ucEnable)
{
	_d(FT_SetBitMode(ftdi->handle, ucMask, ucEnable))
}

static FT_STATUS FTd_SetBaudRate(struct ftdi_context *ftdi, ULONG BaudRate)
{
	_d(FT_SetBaudRate(ftdi->handle, BaudRate))
}

static FT_STATUS FTd_Read(struct ftdi_context *ftdi, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpBytesReturned)
{
	_d(FT_Read(ftdi->handle, lpBuffer, dwBytesToRead, lpBytesReturned))
}

static FT_STATUS FTd_Write(struct ftdi_context *ftdi, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpBytesWritten)
{
	_d(FT_Write(ftdi->handle, lpBuffer, dwBytesToWrite, lpBytesWritten))
}

static FT_STATUS FTd_SetTimeouts(struct ftdi_context *ftdi, ULONG ReadTimeout, ULONG WriteTimeout)
{
	_d(FT_SetTimeouts(ftdi->handle, ReadTimeout, WriteTimeout))
}

static FT_STATUS FTd_SetLatencyTimer(struct ftdi_context *ftdi, UCHAR ucLatency)
{
	_d(FT_SetLatencyTimer(ftdi->handle, ucLatency))
}

static FT_STATUS FTd_Purge(struct ftdi_context *ftdi, ULONG Mask)
{
	_d(FT_Purge(ftdi->handle, Mask))
}

struct ftdi_context *ftdi_new(void)
{
	struct ftdi_context * ftdi = (struct ftdi_context *)malloc(sizeof(struct ftdi_context));
	if (ftdi == NULL)
		return NULL;

	memset(ftdi, 0, sizeof(*ftdi));

	return ftdi;
}

int ftdi_init(struct ftdi_context *ftdi)
{
	memset(ftdi, 0, sizeof(*ftdi));
	ftdi_set_interface(ftdi, INTERFACE_ANY);
	return 0;
}

int ftdi_set_interface(struct ftdi_context *ftdi, enum ftdi_interface intf)
{
	if (ftdi == NULL)
		return -2;
	
	int index = (intf == INTERFACE_ANY) ? INTERFACE_A : intf;
	if (ftdi->handle != NULL && index != ftdi->index) {
		ftdi->last_error = "interface can not be changed on an already open device";
		return -3;
	}

	ftdi->index = index;

	return 0;
}

void ftdi_deinit(struct ftdi_context *ftdi)
{
	if (ftdi == NULL)
		return;

	if (ftdi->handle != NULL) {
		FT_STATUS status = FTd_Close(ftdi);
		_ASSERTE(FT_SUCCESS(status));
		ftdi->handle = NULL;
	}
}

void ftdi_free(struct ftdi_context *ftdi)
{
	ftdi_deinit(ftdi);
	free(ftdi);
}

static bool matches_criteria(const FT_DEVICE_LIST_INFO_NODE *devinfo, unsigned current, int vendor, int product,
	const char *description, const char *serial, unsigned int index)
{
	if (description != NULL && strcmp(devinfo->Description, description) != 0)
		return false;

	if (serial != NULL && strcmp(devinfo->SerialNumber, serial) != 0)
		return false;

	if (index > 0 && current != index)
		return false;

	return true;
}

int ftdi_usb_open_desc_index(struct ftdi_context *ftdi, int vendor, int product,
	const char *description, const char *serial, unsigned int index)
{
	_ASSERTE(ftdi != NULL);
	_ASSERTE(ftdi->handle == NULL);

	// Call to any ftd2xx.dll function will try to delay load it, possibly raising an exception.
	DWORD count;
	FT_STATUS status = FTd_CreateDeviceInfoList(ftdi, &count);
	if (!FT_SUCCESS(status)) {
		// last_error is already set
		return -3;
	}

	if (count == 0) {
		ftdi->last_error = "no device found";
		return -3;
	}

	FT_DEVICE_LIST_INFO_NODE *devinfo_list = malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * count);
	status = FTd_GetDeviceInfoList(ftdi, devinfo_list, &count);
	if (!FT_SUCCESS(status)) {
		free(devinfo_list);
		// last_error is already set
		return -3;
	}

	FT_DEVICE_LIST_INFO_NODE *devinfo = NULL;

	for (DWORD i = 0; i < count; i++) {
		if (matches_criteria(&devinfo_list[i], i, vendor, product, description, serial, index)) {
			devinfo = &devinfo_list[i];
			break;
		}
	}

	if (devinfo == NULL) {
		ftdi->last_error = "device not found";
		free(devinfo_list);
		return -3;
	}

	FT_DEVICE type = devinfo->Type;

	status = FTd_OpenEx(ftdi, devinfo->SerialNumber, FT_OPEN_BY_SERIAL_NUMBER);
	free(devinfo_list);
	if (!FT_SUCCESS(status)) {
		// last_error is already set
		return -3;
	}

	// libftdi ftdi_read_data does not block and returns whatever it currently
	// has. Set the read timeout so the FT_Read returns (almost) immediately.
	status = FTd_SetTimeouts(ftdi, 1, 1000);
	_ASSERTE(FT_SUCCESS(status));

	ftdi->usb_dev = ftdi->handle;
	ftdi->type = type;
	return 0;
}

int ftdi_usb_purge_buffers(struct ftdi_context *ftdi)
{
	FT_STATUS status = FTd_Purge(ftdi,  FT_PURGE_RX | FT_PURGE_TX);
	if (!FT_SUCCESS(status))
		return -1;
	return 0;
}

int ftdi_usb_close(struct ftdi_context *ftdi)
{
	_ASSERTE(ftdi->handle != NULL);

	FT_STATUS status = FTd_Close(ftdi);
	if (!FT_SUCCESS(status))
		return -1;

	ftdi->handle = ftdi->usb_dev = NULL;

	return 0;
}

int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size)
{
	DWORD transferred;
	FT_STATUS status = FTd_Read(ftdi, buf, size, &transferred);
	if (!FT_SUCCESS(status))
		return -1;
	return (int)transferred;
}

int ftdi_write_data(struct ftdi_context *ftdi, const unsigned char *buf, int size)
{
	DWORD transferred;
	FT_STATUS status = FTd_Write(ftdi, (void *)buf, size, &transferred);
	if (!FT_SUCCESS(status))
		return -1;
	return (int)transferred;
}

int ftdi_set_bitmode(struct ftdi_context *ftdi, unsigned char bitmask, unsigned char mode)
{
	FT_STATUS status = FTd_SetBitMode(ftdi, bitmask, mode);
	if (!FT_SUCCESS(status))
		return -1;
	return 0;
}

int ftdi_set_baudrate(struct ftdi_context *ftdi, int baudrate)
{
	FT_STATUS status = FTd_SetBaudRate(ftdi, baudrate);
	if (!FT_SUCCESS(status))
		return -1;
	return 0;
}

int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency)
{
	FT_STATUS status = FTd_SetLatencyTimer(ftdi, latency);
	if (!FT_SUCCESS(status))
		return -1;
	return 0;
}

const char *ftdi_get_error_string(struct ftdi_context *ftdi)
{
	if (ftdi == NULL)
		return NULL;

	return ftdi->last_error;
}
