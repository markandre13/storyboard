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

#include "view3d.hh"
#include <GL/glu.h>
#include <fstream>

TView3D::TView3D(TWindow *p,const string &t, TModel3D *m):
	super(p,t)
{
	_model = m;
	_model->Register(this);
}

double _dummy = 0.0;



void TView3D::glPaint()
{
	glClearColor( 0.0, 0.0, 0.5, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	double w = 0.001L * Width();
	double h = 0.001L * Height();
	glFrustum( -w,    // left
						  w,    // right
						 -h,    // bottom
						  h,    // top
						  1.0,  // near
    				100.0 );// far
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
//  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);

	glTranslatef(0.0, 0.0, -5.0);

	glRotated(rotate.x, 1.0, 0.0, 0.0);
	glRotated(rotate.y, 0.0, 1.0, 0.0);
	glRotated(rotate.z, 0.0, 0.0, 1.0);

	glTranslated(_dummy, 0.0, 0.0);
cout << _dummy << endl;


#if 0
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
		glVertex3d(0.0,-1.0,0.0);
		glVertex3d(0.0,1.0,0.0);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	{
		glBegin( GL_POINTS );
		TModel3D::_TPointBuffer::iterator p,e;
		p = _model->_pointBuffer.begin();
		e = _model->_pointBuffer.end();
		while(p!=e) {
			glVertex3d( (*p).x, (*p).y, (*p).z );
			p++;
		}
		glEnd();
	}
#endif  
  {
  	glBegin( GL_TRIANGLES );
		TModel3D::_TTriangleBuffer::iterator p,e;
		p = _model->_triangleBuffer.begin();
		e = _model->_triangleBuffer.end();
		while(p!=e) {
			TRGB &rgb = (*p).color;
			glColor3ub(rgb.r, rgb.g, rgb.b);
			int *idx = (*p).idx;
			{
				TPoint3D &p0 = _model->_pointBuffer[idx[0]];
				TPoint3D &p1 = _model->_pointBuffer[idx[1]];
				TPoint3D &p2 = _model->_pointBuffer[idx[2]];
				TPoint3D v = p1-p0;
				TPoint3D w = p2-p0;
				double kx,ky,kz;
				kx = v.y * w.z - v.z * w.y;
				ky = v.z * w.x - v.x * w.z;
				kz = v.x * w.y - v.y * w.x;
				double l = sqrt( kx*kx + ky*ky + kz*kz );
				glNormal3d( kx/l, ky/l, kz/l );
			}
			for(int i=0; i<3; i++) {
				TPoint3D &p = _model->_pointBuffer[idx[i]];
				glVertex3d( p.x, p.y, p.z );
			}
			p++;
		}
		glEnd();
	}

}

void TView3D::stateChanged(TState*)
{
	Invalidate();
}

void TView3D::mouseLDown(int x, int y, unsigned)
{
	const ulong BUFSIZE = 100;
	GLuint selectBuf[BUFSIZE];
	glRenderMode(GL_SELECT);
	glSelectBuffer(BUFSIZE, selectBuf);

	glInitNames();
	glPushName(0);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	gluPickMatrix( (GLdouble)x,(GLdouble)(viewport[3]-y), 0.1,0.1, viewport );
	double w = 0.001L * Width();
	double h = 0.001L * Height();
	glFrustum( -w,    // left
						  w,    // right
						 -h,    // bottom
						  h,    // top
						  1.0,  // near
    				100.0 );// far
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

	glTranslated(0.0, 0.0, -5.0);
	glRotated(rotate.x, 1.0, 0.0, 0.0);
	glRotated(rotate.y, 0.0, 1.0, 0.0);
	glRotated(rotate.z, 0.0, 0.0, 1.0);

	TModel3D::_TTriangleBuffer::iterator p,e;
	p = _model->_triangleBuffer.begin();
	e = _model->_triangleBuffer.end();
	GLuint iname = 0;
	while(p!=e) {
		int *idx = (*p).idx;
		glLoadName(iname);
  	glBegin( GL_TRIANGLES );
		for(int i=0; i<3; i++) {
			TPoint3D &p = _model->_pointBuffer[idx[i]];
			glVertex3d( p.x, p.y, p.z );
		}
		glEnd();
		iname++; 
		p++;
	}

	GLint hits = glRenderMode(GL_RENDER);
	GLuint *ptr = selectBuf, *best=NULL;
	float min;

	for (GLint i = 0; i < hits; i++) {
  	GLuint names = *ptr;
    ptr++;
    ptr++;
    float z2 = (float) *ptr/0x7fffffff;
    if (best==NULL || z2<min) {
    	best=ptr;
    	min = z2;
    }
    ptr++;
    for (GLuint j = 0; j < names; j++) {
			ptr++;
		}
	}
	if (best!=NULL) {
		ptr=best-2;
		GLuint names = *ptr;
		ptr+=3;
		for (GLuint j = 0; j < names; j++) {
			TModel3D::_TTriangle *t = &_model->_triangleBuffer[*ptr];
#if 0
			switch(_mode) {
				case SET_COLOR:
					t->color.Set(255,0,0);
					break;
				case FLIP_TRIANGLE:
					int a = t->idx[0]; t->idx[0] = t->idx[1]; t->idx[1] = a;
					break;
			}
#else
					int a = t->idx[0]; t->idx[0] = t->idx[1]; t->idx[1] = a;
#endif
			ptr++;
		}
		Invalidate();
	}
}

void TView3D::mouseEnter(int,int,unsigned)
{
	SetFocus();
}

void TView3D::keyDown(TKey key, char*, unsigned)
{
	static double s = 360.0 / 30.0;
	switch(key) {
		case TK_LEFT:
			rotate.y-=s;
			break;
		case TK_RIGHT:
			rotate.y+=s;
			break;
		case TK_UP:
			rotate.x-=s;
			break;
		case TK_DOWN:
			rotate.x+=s;
			break;
		case TK_PAGEUP:
			_dummy+=0.5;
			break;
		case TK_PAGEDOWN:
			_dummy-=0.5;
			break;
	}
	Invalidate();
}
