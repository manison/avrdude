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

#define _d(call)		__try { return call; } \
				__except (IsDelayLoadException(GetExceptionCode())) \
				{ ftdi->last_error = "Unable to find " FTD2XX_NAME; return FT_MOD_NOT_FOUND; }

static FT_STATUS FTd_Close(struct ftdi_context *ftdi, FT_HANDLE handle)
{
	_d(FT_Close(handle))
}

static FT_STATUS FTd_GetLibaryVersion(struct ftdi_context *ftdi, LPDWORD lpdwVersion)
{
	_d(FT_GetLibraryVersion(lpdwVersion))
}

static FT_STATUS FTd_CreateDeviceInfoList(struct ftdi_context *ftdi, LPDWORD lpdwNumDevs)
{
	_d(FT_CreateDeviceInfoList(lpdwNumDevs))
}

static FT_STATUS FTd_GetDeviceInfoList(struct ftdi_context *ftdi, FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs)
{
	_d(FT_GetDeviceInfoList(pDest, lpdwNumDevs))
}

static FT_STATUS FTd_GetDeviceInfoDetail(
	struct ftdi_context *ftdi,
	DWORD dwIndex,
	LPDWORD lpdwFlags,
	LPDWORD lpdwType,
	LPDWORD lpdwID,
	LPDWORD lpdwLocId,
	LPVOID lpSerialNumber,
	LPVOID lpDescription,
	FT_HANDLE *pftHandle
)
{
	_d(FT_GetDeviceInfoDetail(dwIndex, lpdwFlags, lpdwType, lpdwID, lpdwLocId, lpSerialNumber, lpDescription, pftHandle))
}

struct ftdi_context *ftdi_new(void)
{
	struct ftdi_context * ftdi = (struct ftdi_context *)malloc(sizeof(struct ftdi_context));

	if (ftdi == NULL) {
		return NULL;
	}

	memset(ftdi, 0, sizeof(*ftdi));

	return ftdi;
}

int ftdi_init(struct ftdi_context *ftdi)
{
	_ASSERT(ftdi->handle == NULL);

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
		ftdi->last_error = "Interface can not be changed on an already open device";
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
		FT_STATUS status = FTd_Close(ftdi, ftdi->handle);
		_ASSERTE(FT_SUCCESS(status));
		ftdi->handle = NULL;
	}
}

void ftdi_free(struct ftdi_context *ftdi)
{
	ftdi_deinit(ftdi);
	free(ftdi);
}

int ftdi_usb_open_desc_index(struct ftdi_context *ftdi, int vendor, int product,
	const char *description, const char *serial, unsigned int index)
{
	_ASSERTE(ftdi != NULL);
	_ASSERTE(ftdi->handle == NULL);

	// These are not used in avrftdi.c, don't bother.
	_ASSERTE(description == NULL);
	_ASSERTE(index == 0);

	// Call to any ftd2xx.dll function will try to delay load it, possibly raising an exception.
	DWORD count;
	FT_STATUS status = FTd_CreateDeviceInfoList(ftdi, &count);
	if (!FT_SUCCESS(status)) {
		// last_error is already set
		return -3;
	}

	if (count == 0) {
		ftdi->last_error = "No device found";
		return -3;
	}

	FT_DEVICE_LIST_INFO_NODE *devinfo_list = malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * count);
	status = FTd_GetDeviceInfoList(ftdi, devinfo_list, &count);
	if (!FT_SUCCESS(status)) {
		free(devinfo_list);
		return -3;
	}
	for (DWORD i = 0; i < count; i++) {

	}
	free(devinfo_list);

	ftdi->usb_dev = ftdi->handle;
	return 0;
}

int ftdi_usb_purge_buffers(struct ftdi_context *ftdi)
{
	return 0;
}

int ftdi_usb_close(struct ftdi_context *ftdi)
{
	return 0;
}

int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size)
{
	return 0;
}

int ftdi_write_data(struct ftdi_context *ftdi, const unsigned char *buf, int size)
{
	return 0;
}

int ftdi_set_bitmode(struct ftdi_context *ftdi, unsigned char bitmask, unsigned char mode)
{
	return 0;
}

int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency)
{
	return 0;
}

const char *ftdi_get_error_string(struct ftdi_context *ftdi)
{
	if (ftdi == NULL)
		return NULL;

	return ftdi->last_error;
}
