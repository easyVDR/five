/*
 * Pixel.h
 *
 * Copyright (C) 2013 - Christian Völlinger
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

#ifndef __PIXEL_H
#define __PIXEL_H

//***************************************************************************
// Pixel - format as provided by softhddevice 
//***************************************************************************

struct Pixel
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;

	inline int isBlack(int threshold) {
		return (r < threshold || g < threshold || b < threshold);
	}
};

#endif