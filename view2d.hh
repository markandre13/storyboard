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

#ifndef TView2D
#define TView2D TView2D

#include <toad/toad.hh>
#include "state.hh"
#include "model3d.hh"

class TView2D:
	public TWindow, public TStateListener
{
	public:
		enum ESide {SIDE_XY, SIDE_XZ, SIDE_ZY};
		typedef TWindow super;
		TView2D(TWindow*,const string&, TModel3D*, ESide);
	protected:
		void mouseLDown(int,int,unsigned);
		void mouseMove(int,int,unsigned);
		void paint();
		void stateChanged(TState*);
	public:
		TPoint3D Transform2DTo3D(int,int) const;
		TPoint Transform3DTo2D(const TPoint3D&) const;
		int FindPoint2D(int,int) const;
		void MovePoint2D(int idx, int x, int y);

		TModel3D *_model;
		TPoint3D _origin;
		ESide _side;
		double _scale;
};

#endif