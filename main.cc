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

#include <toad/toad.hh>
#include <toad/form.hh>
#include <toad/fatradiobutton.hh>
#include <toad/menubar.hh>
#include <toad/io/binstream.hh>
#include <vector>
#include "glwindow.hh"
#include <cmath>
#include <fstream>

#include "MainWindow.hh"
#include "model3d.hh"
#include "state.hh"
#include "view2d.hh"
#include "glwindow.hh"
#include "point3d.hh"
#include "tool.hh"
#include "view3d.hh"

static const char *program_title = "Storyboard 0.1";

TModel3D global_model;

void TModel3D::Save(const string &filename) const
{
	fstream io(filename.c_str(), ios::out);
	TBinaryFile file(&io);
	{
		file.WriteDWord(_pointBuffer.size());
		_TPointBuffer::const_iterator p,e;
		p = _pointBuffer.begin();
		e = _pointBuffer.end();
		while(p!=e) {
			file.Write((byte*)&(*p), sizeof(*p));
			p++;
		}
	}
	{
		file.WriteDWord(_triangleBuffer.size());
		_TTriangleBuffer::const_iterator p,e;
		p = _triangleBuffer.begin();
		e = _triangleBuffer.end();
		while(p!=e) {
			file.Write((byte*)&(*p), sizeof(_TTriangle));
			p++;
		}
	}
}

void TModel3D::Load(const string &filename)
{
	fstream io(filename.c_str(), ios::in);
	TBinaryFile file(&io);
	{
		int n = file.ReadDWord();
		for(int i=0; i<n; i++) {
			TPoint3D d;
			file.Read((byte*)&d, sizeof(d));
			_pointBuffer.push_back(d);
		}
	}
	{
		int n = file.ReadDWord();
		for(int i=0; i<n; i++) {
			_TTriangle d;
			file.Read((byte*)&d, sizeof(d));
			_triangleBuffer.push_back(d);
		}
	}
	Notify();
}

void TToolPoint::mouseLDown(int x,int y, unsigned, TView2D *v)
{
	int idx = v->FindPoint2D(x,y);
	if (idx>=0) {
		return;
	}
	v->_model->AddPoint(v->Transform2DTo3D(x,y));
	v->Invalidate();
}

void TToolMove::reset()
{
	idx = -1;
}

void TToolMove::paint(TPen &pen, TView2D *v)
{
	if (idx>=0) {
		pen.SetColor(0,0,255);
		TPoint pnt = v->Transform3DTo2D(v->_model->_pointBuffer[idx]);
		pen.DrawRectangle(pnt.x-2, pnt.y-2,5,5);
	}
}

void TToolMove::mouseLDown(int x,int y, unsigned, TView2D *v)
{
	int i = v->FindPoint2D(x,y);
	if (i!=idx) {
		idx = i;
		v->_model->Notify();
	}
}

void TToolMove::mouseMove(int x,int y, unsigned, TView2D *v)
{
	if (idx>=0)
		v->MovePoint2D(idx, x,y);  
}

void TToolTriangle::reset()
{
	n = 0;
}

void TToolTriangle::mouseLDown(int x,int y, unsigned, TView2D *v)
{
	idx[n] = v->FindPoint2D(x,y);
	if (idx[n]<0) {
		return;
	}
	n++;
	if (n==3) {
		v->_model->AddTriangle(idx);
		reset();
	}
	v->_model->Notify();
}

void TToolTriangle::paint(TPen &pen, TView2D *v)
{
	pen.SetColor(0,255,0);
	for(int i=0; i<n; i++) {
		TPoint pnt = v->Transform3DTo2D(v->_model->_pointBuffer[idx[i]]);
		pen.DrawRectangle(pnt.x-2, pnt.y-2,5,5);
	}
}

int ToadMain()
{
	return TMainWindow(NULL, program_title).Run();
}

TTool *_current_tool = NULL;

TTool* TTool::Current()
{
	return _current_tool;
}

void TTool::SetCurrent(TTool *t)
{
	_current_tool = t;
	_current_tool->reset();
}

TMainWindow::TMainWindow(TWindow *p,const string &t):
	super(p,t)
{
	TMenuBar *mb = new TMenuBar(this, "menubar");
	TMenuItem *mi;
	mb->BgnPulldown("File");
		mi = mb->AddItem("Open");
		CONNECT(mi->sigActivate, this, menuOpen);
		mi = mb->AddItem("Save");
		CONNECT(mi->sigActivate, this, menuSave);
		mi = mb->AddItem("Quit");
		CONNECT(mi->sigActivate, this, closeRequest);
	mb->EndPulldown();

	TFatRadioButton *rb1, *rb2, *rb3, *rb4;
	
	TRadioState *state = new TRadioState();
	rb1 = new TFatRadioButton(this, "Point", state);
		CONNECT(rb1->sigActivate, this, selectTool, 0);
		rb1->LoadBitmap("tool_point.bmp");
		rb1->SetSize(30,30);
		rb1->SetDown(true);
	rb2 = new TFatRadioButton(this, "Triangle",  state);
		CONNECT(rb2->sigActivate, this, selectTool, 1);
		rb2->LoadBitmap("tool_triangle.bmp");
		rb2->SetSize(30,30);
	rb3 = new TFatRadioButton(this, "Move",  state);
		CONNECT(rb3->sigActivate, this, selectTool, 2);
		rb3->LoadBitmap("tool_move.bmp");
		rb3->SetSize(30,30);
	rb4 = new TFatRadioButton(this, "Flip", state);
		CONNECT(rb4->sigActivate, this, selectTool, 3);
		rb4->LoadBitmap("surface_flip.bmp");
		rb4->SetSize(30,30);

	TView2D *v1,*v2,*v3;
	TView3D *v4;
	v1 = new TView2D(this, "Front", &global_model, TView2D::SIDE_XY);
	v2 = new TView2D(this, "Top", &global_model, TView2D::SIDE_XZ);
	v3 = new TView2D(this, "Side", &global_model, TView2D::SIDE_ZY);
	v4 = new TView3D(this, "3D", &global_model);

	Attach(mb, TOP | LEFT | RIGHT);

	Attach(rb1, TOP, mb);
	Distance(rb1,3, TOP|BOTTOM|LEFT);
	Attach(rb1, LEFT);

	Attach(rb2, TOP, mb);
	Distance(rb2,3, TOP|BOTTOM);
	Attach(rb2, LEFT, rb1);

	Attach(rb3, TOP, mb);
	Distance(rb3,3,TOP|BOTTOM);
	Attach(rb3, LEFT, rb2);

	Attach(rb4, TOP, mb);
	Distance(rb4,3,TOP|BOTTOM|LEFT);
	Attach(rb4, LEFT, rb3);

	Attach(v1, TOP, rb1);
	Attach(v1, LEFT);
	Attach(v2, TOP, v1);
	Attach(v2, LEFT);
	Attach(v3, TOP, rb1);
	Attach(v3, LEFT, v1);
	Attach(v4, TOP,  v1);
	Attach(v4, LEFT, v1);
	
	SetSize(640,400);
	SetBackground(TColor::DIALOG);
}

void TMainWindow::menuOpen()
{
	global_model.Load("scene.sbf");
}

void TMainWindow::menuSave()
{
	global_model.Save("scene.sbf");
}

void TMainWindow::selectTool(unsigned n)
{
// cout << "TOOL " << rb->Value() << endl;
	static TTool *tool[4] = {
		new TToolPoint(),
		new TToolTriangle(),
		new TToolMove(),
		new TToolMove()
	};
	TTool::SetCurrent(tool[n]);
}


TView2D::TView2D(TWindow *p,const string &t, TModel3D *model, ESide side):
	super(p, t)
{
	_model = model;
	_side  = side;
	_model->Register(this);
	SetMouseMoveMessages(TMMM_LBUTTON);
	_origin.x = _origin.y = _origin.z = 100.0;
	_scale = 0.01;
}

void TView2D::mouseLDown(int x, int y, unsigned m)
{
	TTool::Current()->mouseLDown(x,y,m, this);
}

void TView2D::mouseMove(int x, int y, unsigned m)
{
	TTool::Current()->mouseMove(x,y,m, this);
}

void TView2D::paint()
{
	TPen pen(this);
	{
		TModel3D::_TPointBuffer::iterator p,e;
		p = _model->_pointBuffer.begin();
		e = _model->_pointBuffer.end();
		while(p!=e) {
			TPoint pnt = Transform3DTo2D(*p);
			pen.DrawRectangle(pnt.x-2, pnt.y-2,5,5);
			p++;
		}
	}
	{
		TModel3D::_TTriangleBuffer::iterator p,e;
		p = _model->_triangleBuffer.begin();
		e = _model->_triangleBuffer.end();
		while(p!=e) {
			int *idx = (*p).idx;
			TPoint pnt[3];
			for(int i=0; i<3; i++) {
				pnt[i] = Transform3DTo2D(_model->_pointBuffer[idx[i]]);
			}
			pen.DrawPolygon(pnt, 3);
			p++;
		}
	}
	TTool::Current()->paint(pen, this);
}

void TView2D::stateChanged(TState*)
{
	Invalidate();
}

TPoint3D TView2D::Transform2DTo3D(int x, int y) const
{
	double dx = (double)x;
	double dy = (double)y;
	dx = (dx-_origin.x) * _scale;
	dy = -(dy-_origin.y) * _scale;
	switch(_side) {
		case SIDE_XY:
			return TPoint3D(dx, dy, 0.0);
		case SIDE_XZ:
			return TPoint3D(dx, 0.0, dy);
		case SIDE_ZY:
			return TPoint3D(0.0, dy, dx);
	}
	return TPoint3D(0,0,0);
}

TPoint TView2D::Transform3DTo2D(const TPoint3D &p) const
{
	switch(_side) {
		case SIDE_XY:
			return TPoint(p.x/_scale+_origin.x, -p.y/_scale+_origin.y);
		case SIDE_XZ:
			return TPoint(p.x/_scale+_origin.x, -p.z/_scale+_origin.z);
		case SIDE_ZY:
			return TPoint(p.z/_scale+_origin.z, -p.y/_scale+_origin.y);
	}
	return TPoint(0, 0);
}

int TView2D::FindPoint2D(int x, int y) const
{
	TModel3D::_TPointBuffer::iterator p,e;
	p = _model->_pointBuffer.begin();
	e = _model->_pointBuffer.end();
	int i = 0;
	while(p!=e) {
		TPoint pnt = Transform3DTo2D(*p);
		if (pnt.x-3<=x && x<=pnt.x+3 && pnt.y-3<=y && y<=pnt.y+3) {
			return i;
		}
		p++;
		i++;
	}
	return -1;
}

void TView2D::MovePoint2D(int idx, int x, int y)
{
	TPoint3D &po = _model->_pointBuffer[idx];
	TPoint3D p3 = Transform2DTo3D(x,y);
	switch(_side) {
		case SIDE_XY:
			p3.z = po.z;
			break;
		case SIDE_XZ:
			p3.y = po.y;
			break;
		case SIDE_ZY:
			p3.x = po.x;
			break;
	}
	_model->MovePoint(idx, p3);
	Invalidate();
}
