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

#ifndef TState
#define TState TState

#include <vector>

class TState;

class TStateListener
{
	public:
		virtual void stateChanged(TState*) = 0;
};

class TState
{
		typedef vector<TStateListener*> _TListenerBuffer;
		_TListenerBuffer _listener;
		int _reason;

	public:
		void Register(TStateListener *sl) {
			_listener.push_back(sl);
		}
		void UnRegister(TStateListener *) {
		}
		
		void Notify(int reason=0, TStateListener *who=NULL) {
			_reason = reason;
			_TListenerBuffer::iterator p,e;
			p = _listener.begin();
			e = _listener.end();
			while(p!=e) {
				if (who!=*p) {
					(*p)->stateChanged(this);
				}
				p++;
			}
		}
};

#endif
