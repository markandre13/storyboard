#ifndef TModel3D
#define TModel3D TModel3D

#include <vector>
#include <string>
#include "point3d.hh"
#include "state.hh"

class TModel3D:
	public TState
{
	public:
		typedef vector<TPoint3D> _TPointBuffer;
		_TPointBuffer _pointBuffer;
		struct _TTriangle {
			_TTriangle() {
				color.r=255; color.g=191; color.b=128;
				idx[0]=idx[1]=idx[2]=0;
			}
			_TTriangle(const int *i) {
				color.r=255; color.g=191; color.b=128;
				idx[0] = i[0];
				idx[1] = i[1];
				idx[2] = i[2];
			}
			int idx[3];
			TRGB color;
		};
		typedef vector<_TTriangle> _TTriangleBuffer;
		_TTriangleBuffer _triangleBuffer;
		void AddPoint(const TPoint3D &p) {
			_pointBuffer.push_back(p);
			Notify();
		}
		void MovePoint(int n, const TPoint3D &p) {
			_pointBuffer[n] = p;
			Notify();
		}
		void AddTriangle(const int *idx) {
			_triangleBuffer.push_back(idx);
			Notify();
		}
		void Save(const string&) const;
		void Load(const string&);
};

#endif
