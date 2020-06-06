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
#include "ftd2xx2libftdi.h"


struct ftdi_context *ftdi_new(void)
{
	struct ftdi_context *ftdi = calloc(1, sizeof(struct ftdi_context));
	return ftdi;
}

int ftdi_init(struct ftdi_context *ftdi)
{
	return 0;
}

int ftdi_set_interface(struct ftdi_context *ftdi, enum ftdi_interface intf)
{
	return 0;
}

void ftdi_deinit(struct ftdi_context *ftdi)
{
}

void ftdi_free(struct ftdi_context *ftdi)
{
	free(ftdi);
}

int ftdi_usb_open_desc_index(struct ftdi_context *ftdi, int vendor, int product,
	const char *description, const char *serial, unsigned int index)
{
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
	return 0;
}
