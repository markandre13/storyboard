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

#ifndef TView3D
#define TView3D TView3D

#include "glwindow.hh"
#include "model3d.hh"
#include "state.hh"

class TView3D:
	public TGLWindow, public TStateListener
{
	public:
		typedef TGLWindow super;
		TView3D(TWindow*,const string&, TModel3D*);
	protected:
		void glPaint();
		void stateChanged(TState*);
		void keyDown(TKey, char*, unsigned);
		void mouseEnter(int,int,unsigned);
		void mouseLDown(int,int,unsigned);
	public:
		TModel3D *_model;
		TPoint3D rotate;
};

#endif
