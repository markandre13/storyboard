/*
 * Storyboard - A Simple 3D Editor
 * Copyright (C) 1998 by Mark-André Hopf
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
 * along with this program; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * Boston, MA 02111-1307, USA.
 */

#ifndef TPoint3D
#define TPoint3D TPoint3D

class TPoint3D
{
	public:
		TPoint3D() {
			x=y=z=0.0;
		}
		TPoint3D(double ix, double iy, double iz) {
			x=ix; y=iy; z=iz;
		}
		double x,y,z;
};

inline TPoint3D operator+(const TPoint3D &p1, const TPoint3D &p2)
{
	return TPoint3D(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}

inline TPoint3D operator-(const TPoint3D &p1, const TPoint3D &p2)
{
	return TPoint3D(p1.x-p2.x, p1.y-p2.y, p1.z-p2.z);
}


#endif
