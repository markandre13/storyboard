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

#ifndef TTool
#define TTool TTool

class TPen;
class TView2D;

class TTool
{
	public:
		static TTool* Current();
		static void SetCurrent(TTool*);
		virtual void reset() {};
		virtual void paint(TPen&,TView2D*) {};
		virtual void mouseLDown(int,int,unsigned,TView2D*) {};
		virtual void mouseMove(int,int,unsigned,TView2D*) {};
};

class TToolPoint:
	public TTool
{
		int n;
	public:
		void mouseLDown(int,int,unsigned, TView2D*);
};

class TToolTriangle:
	public TTool
{
		int n;
		int idx[3];
	public:
		void reset();
		void mouseLDown(int,int,unsigned, TView2D*);
		void paint(TPen&, TView2D*);
};

class TToolMove:
	public TTool
{
		int idx;
	public:
		void reset();
		void paint(TPen&, TView2D*);
		void mouseLDown(int,int,unsigned, TView2D*);
		void mouseMove(int,int,unsigned,TView2D*);
};

#endif
