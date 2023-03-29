/* SPDX-License-Identifier: GPL-2.0 */
/**
 *
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2  of
 * the License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _USB_USBPHY_CAL_SNPS_USBDP_RAM_CODE_H_
#define _USB_USBPHY_CAL_SNPS_USBDP_RAM_CODE_H_

struct ram_code {
	unsigned short addr;
	unsigned short data;
};

const struct ram_code phy_fw_code[] = {
	{0xC005, 0x6044}, {0xC007, 0x04B3}, {0xC01B, 0x7000}, {0xC01C, 0x131A}, {0xC023, 0x7000}, {0xC024, 0x1324},
	{0xD31A, 0x034B}, {0xD31B, 0x0001}, {0xD31C, 0x080F}, {0xD31D, 0x0020}, {0xD31E, 0x034D}, {0xD31F, 0xFFFF},
	{0xD320, 0x080F}, {0xD321, 0xFFFF}, {0xD322, 0x7000}, {0xD323, 0x001D}, {0xD324, 0x080F}, {0xD325, 0x0020},
	{0xD326, 0x034D}, {0xD327, 0xFFFF}, {0xD328, 0x080F}, {0xD329, 0xFFFF}, {0xD32A, 0x1B42}, {0xD32B, 0xA003},
	{0xD32C, 0x7000}, {0xD32D, 0x002E}, {0xD32E, 0x7000}, {0xD32F, 0x0025}, {0xD330, 0x7FFF}, {0xD331, 0x7FFF},
};

#endif // _USB_USBPHY_CAL_SNPS_USBDP_RAM_CODE_H_
