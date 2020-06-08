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

#ifndef ftd2xx2libftdi_h
#define ftd2xx2libftdi_h

#include "ftdi/ftd2xx.h"

 /** FTDI chip type */
enum ftdi_chip_type
{
	TYPE_AM = FT_DEVICE_AM,
	TYPE_BM = FT_DEVICE_BM,
	TYPE_2232C = FT_DEVICE_2232C,
	TYPE_R = FT_DEVICE_232R,
	TYPE_2232H = FT_DEVICE_2232H,
	TYPE_4232H = FT_DEVICE_4232H,
	TYPE_232H = FT_DEVICE_232H,
	TYPE_230X = FT_DEVICE_X_SERIES,
};

/** MPSSE bitbang modes */
enum ftdi_mpsse_mode
{
	BITMODE_RESET = FT_BITMODE_RESET,    /**< switch off bitbang mode, back to regular serial/FIFO */
	BITMODE_BITBANG = FT_BITMODE_ASYNC_BITBANG,    /**< classical asynchronous bitbang mode, introduced with B-type chips */
	BITMODE_MPSSE = FT_BITMODE_MPSSE,    /**< MPSSE mode, available on 2232x chips */
	BITMODE_SYNCBB = FT_BITMODE_SYNC_BITBANG,    /**< synchronous bitbang mode, available on 2232x and R-type chips  */
	BITMODE_MCU = FT_BITMODE_MCU_HOST,    /**< MCU Host Bus Emulation mode, available on 2232x chips */
	/* CPU-style fifo mode gets set via EEPROM */
	BITMODE_OPTO = FT_BITMODE_FAST_SERIAL,    /**< Fast Opto-Isolated Serial Interface Mode, available on 2232x chips  */
	BITMODE_CBUS = FT_BITMODE_CBUS_BITBANG,    /**< Bitbang on CBUS pins of R-type chips, configure in EEPROM before */
	BITMODE_SYNCFF = FT_BITMODE_SYNC_FIFO,    /**< Single Channel Synchronous FIFO mode, available on 2232H chips */
#if 0
	BITMODE_FT1284 = 0x80,    /**< FT1284 mode, available on 232H chips */
#endif
};

/** Port interface for chips with multiple interfaces */
enum ftdi_interface
{
	INTERFACE_ANY = 0,
	INTERFACE_A = 1,
	INTERFACE_B = 2,
	INTERFACE_C = 3,
	INTERFACE_D = 4
};

/* Shifting commands IN MPSSE Mode*/
#define MPSSE_WRITE_NEG 0x01   /* Write TDI/DO on negative TCK/SK edge*/
#define MPSSE_BITMODE   0x02   /* Write bits, not bytes */
#define MPSSE_READ_NEG  0x04   /* Sample TDO/DI on negative TCK/SK edge */
#define MPSSE_LSB       0x08   /* LSB first */
#define MPSSE_DO_WRITE  0x10   /* Write TDI/DO */
#define MPSSE_DO_READ   0x20   /* Read TDO/DI */
#define MPSSE_WRITE_TMS 0x40   /* Write TMS/CS */

/* FTDI MPSSE commands */
#define SET_BITS_LOW   0x80
/*BYTE DATA*/
/*BYTE Direction*/
#define SET_BITS_HIGH  0x82
/*BYTE DATA*/
/*BYTE Direction*/
#define GET_BITS_LOW   0x81
#define GET_BITS_HIGH  0x83
#define TCK_DIVISOR    0x86

/* Commands in MPSSE and Host Emulation Mode */
#define SEND_IMMEDIATE 0x87

struct ftdi_context
{
	FT_HANDLE handle;

	/* FTDI specific */
	/** FTDI chip type */
	enum ftdi_chip_type type;

	/** FT2232C index number: 1 or 2 */
	int index;       /* 1 or 2 */

	/** maximum packet size. Needed for filtering modem status bytes every n packets. */
	unsigned int max_packet_size;

	void *usb_dev;


	const char *last_error;
};

struct ftdi_context *ftdi_new(void);
int ftdi_init(struct ftdi_context *ftdi);
int ftdi_set_interface(struct ftdi_context *ftdi, enum ftdi_interface intf);
void ftdi_deinit(struct ftdi_context *ftdi);
void ftdi_free(struct ftdi_context *ftdi);

int ftdi_usb_open_desc_index(struct ftdi_context *ftdi, int vendor, int product,
	const char* description, const char* serial, unsigned int index);
int ftdi_usb_purge_buffers(struct ftdi_context *ftdi);
int ftdi_usb_close(struct ftdi_context *ftdi);

int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size);
int ftdi_write_data(struct ftdi_context *ftdi, const unsigned char *buf, int size);

int ftdi_set_baudrate(struct ftdi_context *ftdi, int baudrate);
int ftdi_set_bitmode(struct ftdi_context *ftdi, unsigned char bitmask, unsigned char mode);
int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency);

const char *ftdi_get_error_string(struct ftdi_context *ftdi);

#endif
