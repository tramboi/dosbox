/*
 *  Copyright (C) 2002-2004  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "dosbox.h"
#include "inout.h"
#include "pic.h"
#include "vga.h"

static Bit8u flip=0;

void write_p3d4_vga(Bitu port,Bitu val,Bitu iolen);
Bitu read_p3d4_vga(Bitu port,Bitu iolen);
void write_p3d5_vga(Bitu port,Bitu val,Bitu iolen);
Bitu read_p3d5_vga(Bitu port,Bitu iolen);

static Bitu read_p3da(Bitu port,Bitu iolen) {
	vga.internal.attrindex=false;
	if (vga.config.retrace) {
		switch (machine) {
		case MCH_HERC:
			 return 0x81;
		default:
			return 9;
		}
	}
	flip++;
	if (flip>20) flip=0;
	if (flip>10) return 1;
	return 0;
	/*
		0	Either Vertical or Horizontal Retrace active if set
		3	Vertical Retrace in progress if set
	*/
}


static void write_p3c2(Bitu port,Bitu val,Bitu iolen) {
	vga.misc_output=val;
	if (val & 0x1) {
		IO_RegisterWriteHandler(0x3d4,write_p3d4_vga,IO_MB);
		IO_RegisterReadHandler(0x3d4,read_p3d4_vga,IO_MB);
		IO_RegisterWriteHandler(0x3d5,write_p3d5_vga,IO_MB);
		IO_RegisterReadHandler(0x3d5,read_p3d5_vga,IO_MB);
		IO_RegisterReadHandler(0x3da,read_p3da,IO_MB);
		
		IO_FreeWriteHandler(0x3b4,IO_MB);
		IO_FreeReadHandler(0x3b4,IO_MB);
		IO_FreeWriteHandler(0x3b5,IO_MB);
		IO_FreeReadHandler(0x3b5,IO_MB);
		IO_FreeReadHandler(0x3ba,IO_MB);
	} else {
		IO_RegisterWriteHandler(0x3b4,write_p3d4_vga,IO_MB);
		IO_RegisterReadHandler(0x3b4,read_p3d4_vga,IO_MB);
		IO_RegisterWriteHandler(0x3b5,write_p3d5_vga,IO_MB);
		IO_RegisterReadHandler(0x3b5,read_p3d5_vga,IO_MB);
		IO_RegisterReadHandler(0x3ba,read_p3da,IO_MB);

		IO_FreeWriteHandler(0x3d4,IO_MB);
		IO_FreeReadHandler(0x3d4,IO_MB);
		IO_FreeWriteHandler(0x3d5,IO_MB);
		IO_FreeReadHandler(0x3d5,IO_MB);
		IO_FreeReadHandler(0x3da,IO_MB);
	}
	/*
		0	If set Color Emulation. Base Address=3Dxh else Mono Emulation. Base Address=3Bxh.
		2-3	Clock Select. 0: 25MHz, 1: 28MHz
		5	When in Odd/Even modes Select High 64k bank if set
		6	Horizontal Sync Polarity. Negative if set
		7	Vertical Sync Polarity. Negative if set
			Bit 6-7 indicates the number of lines on the display:
			1:  400, 2: 350, 3: 480
			Note: Set to all zero on a hardware reset.
			Note: This register can be read from port 3CCh.
	*/
}


static Bitu read_p3cc(Bitu port,Bitu iolen) {
	return vga.misc_output;
}



void VGA_SetupMisc(void) {
	if (machine==MCH_VGA) {
		IO_RegisterWriteHandler(0x3c2,write_p3c2,IO_MB);
		IO_RegisterReadHandler(0x3cc,read_p3cc,IO_MB);
	} else if (machine==MCH_CGA || machine==MCH_TANDY) {
		IO_RegisterReadHandler(0x3da,read_p3da,IO_MB);
	} else if (machine==MCH_HERC) {
		IO_RegisterReadHandler(0x3ba,read_p3da,IO_MB);
	}

}


