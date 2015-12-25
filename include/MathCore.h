//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// author:		Alexey Egorov (FadeToBlack aka EvilSpirit)
// mailto:		anandamide@mail.ru
// 				anandamide.script@gmail.com
//
// AnandamideAPI is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// AnandamideAPI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with AnandamideAPI. If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------

#ifndef MathCoreH
#define MathCoreH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include <math.h>

//------------------------------------------------------------------------------

#define PI			3.14159265358979323846f

#define DEGTORAD	0.01745329251994329576f
#define RADTODEG	57.2957795130823209071f

#define EPSILON		1.0e-6f

//------------------------------------------------------------------------------

namespace Anandamide {
	class vec3;
	class BBox;
	class Str;
}

//------------------------------------------------------------------------------

namespace math {

	//--------------------------------------------------------------------------

	ANANDAMIDE_API float	abs(float a);
	ANANDAMIDE_API float	sqrt(float a);
	ANANDAMIDE_API float	cos(float a);
	ANANDAMIDE_API float	sin(float a);
	ANANDAMIDE_API void		sincos(float alpha, float &sn, float &cs);
	ANANDAMIDE_API int random(int __num);

	template <class T> static T min_(const T &a, const T &b) { return (a < b) ? a : b; }
	template <class T> static T max_(const T &a, const T &b) { return (a > b) ? a : b; }
	template <class T> static void swap(T &a, T &b) { T temp = a; a = b; b = temp; }
	template <class T> static T lerp(const T &a, const T &b, float k) { return a + (b - a) * k; }
	
	//--------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//------------------------------------------------------------------------------
	
	class ANANDAMIDE_API vec3 {
	
	public:
	
		//--------------------------------------------------------------------------
	
		union {
			struct { float x, y, z; };
			struct { float r, g, b; };
			struct { float v[3]; };
		};
	
		//--------------------------------------------------------------------------
	
		inline vec3() : x(0.0f), y(0.0f), z(0.0f) {}
		inline vec3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}
		inline vec3(const vec3 &v) : x(v.x), y(v.y), z(v.z) {}
	
		//--------------------------------------------------------------------------
	
		inline vec3 &operator= (const vec3 &v) { x = v.x; y = v.y;	z = v.z; return *this; }
		inline vec3 operator- () const { return vec3(-x, -y, -z); }
		inline vec3 operator+ () const { return vec3(x, y, z); }
	
		//--------------------------------------------------------------------------
	
		inline vec3 operator+ (const vec3 &v) const { return vec3(x + v.x, y + v.y, z + v.z); }
		inline vec3 operator- (const vec3 &v) const { return vec3(x - v.x, y - v.y, z - v.z); }
		inline vec3 operator* (const vec3 &v) const { return vec3(x * v.x, y * v.y, z * v.z); }
		inline vec3 operator/ (const vec3 &v) const { return vec3(x / v.x, y / v.y, z / v.z); }
		inline vec3 operator* (float a) const { return vec3(x * a, y * a, z * a); }
		inline vec3 operator/ (float a) const { return vec3(x / a, y / a, z / a); }
	
		//--------------------------------------------------------------------------
	
		inline vec3 &operator+= (const vec3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
		inline vec3 &operator-= (const vec3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		inline vec3 &operator/= (const vec3 &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		inline vec3 &operator*= (const vec3 &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		inline vec3 &operator*= (float a) { x *= a; y *= a; z *= a; return *this; }
		inline vec3 &operator/= (float a) { x /= a; y /= a; z /= a; return *this; }
	
		//--------------------------------------------------------------------------
	
		inline float &operator[] (int i) { return v[i]; }
		inline float operator[] (int i) const { return v[i]; }
	
		//--------------------------------------------------------------------------
	
		inline bool operator== (const vec3 &v) const { return ::math::abs(x - v.x) <= EPSILON && ::math::abs(y - v.y) <= EPSILON && ::math::abs(z - v.z) <= EPSILON; }
		inline bool operator!= (const vec3 &v) const { return ::math::abs(x - v.x) > EPSILON || ::math::abs(y - v.y) > EPSILON || ::math::abs(z - v.z) > EPSILON; }
	
		//--------------------------------------------------------------------------
	
		inline bool operator> (const vec3 &v) const { return (x - v.x) > EPSILON && (y - v.y) > EPSILON && (z - v.z) > EPSILON; }
		inline bool operator< (const vec3 &v) const { return (x - v.x) < -EPSILON && (y - v.y) < -EPSILON && (z - v.z) < -EPSILON; }
		inline bool operator>= (const vec3 &v) const { return (x - v.x) >= -EPSILON && (y - v.y) >= -EPSILON && (z - v.z) >= -EPSILON; }
		inline bool operator<= (const vec3 &v) const { return (x - v.x) <= EPSILON && (y - v.y) <= EPSILON && (z - v.z) <= EPSILON; }
	
		inline bool isZero() const { return ::math::abs(x) < EPSILON && ::math::abs(y) < EPSILON && ::math::abs(z) < EPSILON; }
		inline bool isOne() const { return ::math::abs(x - 1.0f) < EPSILON && ::math::abs(y - 1.0f) < EPSILON && ::math::abs(z - 1.0f) < EPSILON; }
	
		//--------------------------------------------------------------------------
	
		inline float length() const { return ::math::sqrt(x * x + y * y + z * z); }
		inline float lengthSquared() const { return x * x + y * y + z * z; }
	
		inline bool equals(const vec3 &v, float eps = EPSILON) const { return ::math::abs(x - v.x) <= eps && ::math::abs(y - v.y) <= eps && ::math::abs(z - v.z) <= eps; }
	
		//--------------------------------------------------------------------------
	
		inline void normalize();
	
		//--------------------------------------------------------------------------
		
		void operator<<(const Str &str);
		void operator>>(Str &str) const;
	
	};
	
	inline vec3 normalize(const vec3 &v) {
	
		float len = v.length();
		if (::math::abs(len) > EPSILON) {
			float inv = 1.0f / len;
			return vec3(v.x * inv, v.y * inv, v.z * inv);
		} else {
			return v;
		}
	}
	
	//--------------------------------------------------------------------------
	
	inline float dot(const vec3 &a, const vec3 &b) {
		return
			a.x * b.x +
			a.y * b.y +
			a.z * b.z;
	}
	
	//--------------------------------------------------------------------------
	
	inline vec3 cross(const vec3 &a, const vec3 &b) {
	
		return vec3(
			a.y * b.z - b.y * a.z,
			a.z * b.x - b.z * a.x,
			a.x * b.y - b.x * a.y
		);
	}
	
	//--------------------------------------------------------------------------
	
	inline vec3 interp(const vec3 &v1, const vec3 &v2, float t) {
		return v1 + (v2 - v1) * t;
	}
	
	//------------------------------------------------------------------------------
	
	template <class T>
	class ANANDAMIDE_API Vector2x {
	
	public:
	
		union {
			struct { T x, y; };
			struct { T v[2]; };
		};
	
		Vector2x(T nx = 0, T ny = 0) : x(nx), y(ny) {}
	
		T &operator[] (int i) { return v[i]; }
		T operator[] (int i) const { return v[i]; }
	
	};
	
	//------------------------------------------------------------------------------

	template <class T>
	class ANANDAMIDE_API Vector4x {
	
	public:
	
		union {
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			struct { T v[4]; };
		};
	
		Vector4x(T nx = 0, T ny = 0, T nz = 0, T nw = 0) : x(nx), y(ny), z(nz), w(nw) {}
	
		T &operator[] (int i) { return v[i]; }
		T operator[] (int i) const { return v[i]; }
	
	};
	
	//------------------------------------------------------------------------------
	
	namespace math {
		vec3 abs(const vec3 &a);
	}
	
	typedef Vector2x <float> Vector2f;
	typedef Vector4x <float> Vector4f;

	//------------------------------------------------------------------------------
	
	class Quaternion;
	
	//------------------------------------------------------------------------------
	
	class ANANDAMIDE_API Matrix {
	
	public:
	
		union {
			struct { float data[4][4]; };
			struct { float v[16]; };
		};
	
		Matrix();
	
		void identity();
		bool inverse();
		void transpose();
	
		void basis(const vec3 &, const vec3 &, const vec3 &, const vec3 &);
	
		Matrix operator* (const Matrix &) const;
		vec3 operator* (const vec3 &) const;
		Vector4f operator* (const Vector4f &) const;
	
		bool operator== (const Matrix &o);
		bool operator!= (const Matrix &o);
	
		bool equals(const Matrix &o, float eps = EPSILON);
		static Matrix rotateX(float);
		static Matrix rotateY(float);
		static Matrix rotateZ(float);
		static Matrix translate(const vec3 &);
		static Matrix scale(const vec3 &);
	
		vec3 getPos() const;
		void setPos(const vec3 &pos);
		void setScale(const vec3 &scale);
	
		void mult(const Matrix &m);
		void multOrder(const Matrix &m);
		void normalize();
		void decompose(vec3 &pos, Quaternion &rot, vec3 &scale);
	
		Vector4f getRow(int i) const;
		Vector4f getColumn(int i) const;
	
		operator float *() { return (float *)data; }
		operator const float *() const { return (const float *)data; }
	
	};
	
	//------------------------------------------------------------------------------
	
	class Line {
	
	public:
	
		vec3 src;
		vec3 dst;
	
		Line() { }
		Line(const vec3 &s, const vec3 &d) : src(s), dst(d) { }
	
		inline vec3 getDirection() const { return (dst - src); }
	};
	
	//------------------------------------------------------------------------------
	
	class ANANDAMIDE_API Plane {
	
	public:
		vec3 normal;
		float dist;
	
		Plane() : normal(0.0f, 0.0f, 1.0f), dist(0.0f) {}
		Plane(const vec3 &n, float d) : normal(n), dist(d) {}
		Plane(const vec3 &, const vec3 &);
	
		vec3 getOrigin();
		void setOrigin(const vec3 &);
	
		void normalize();
		friend Plane normalize(const Plane &);
		//bool trace(const Line &, TracePoint &, bool = false, bool = false) const;
	};
	
	//------------------------------------------------------------------------------
	
	#undef min
	#undef max
	
	class ANANDAMIDE_API BBox {
	
	public:
	
		vec3 min;
		vec3 max;
	
		BBox() { }
		BBox(const vec3 &mn, const vec3 &mx);
	
		vec3 getOrigin() const { return min + (max - min) * 0.5f; }
		vec3 getExtents() const { return (max - min) * 0.5; }
	
		vec3 getVertex(int) const;
		Plane getPlane(int) const;
	
		//bool trace(const Line &, TracePoint &, bool = false, bool = false) const;
		void include(const vec3 &, float = 0.0f);
	
	};
	
	//------------------------------------------------------------------------------
	
	class ANANDAMIDE_API Frustum {
	
		Plane planes[6];
	
		int viewport[4];
		Matrix modelview;
		Matrix projection;
	
		void invalidate();
	
	public:
	
		Frustum();
		Frustum(const float *mv, const float *p, int *vp) { set(mv, p, vp); }
		void set(const float *mv, const float *p, int *vp);
	
		const Matrix &getModelview() const { return modelview; }
		const Matrix &getProjection() const { return projection; }
	
		const int *getViewport() const { return viewport; }
		const Plane &getPlane(int i) const { return planes[i]; }
	
		void setModelview(const Matrix &);
		void setProjection(const Matrix &);
	
		void setViewport(int *);
	
	};
	
	//------------------------------------------------------------------------------

	bool overlaps(const BBox &, const BBox &);
	
	//------------------------------------------------------------------------------

}

namespace Nutmeg {

	const char *intToStr(int i);
	const char *floatToStr(float i);
	const char *doubleToStr(double i);
	const char *vectorToStr(const Anandamide::vec3 &v);
	const char *quaternionToStr(const Anandamide::Quaternion &q);
	const char *matrixToStr(const Anandamide::Matrix &m);
	
	inline const char *toString(int i) { return intToStr(i); }
	inline const char *toString(float f) { return floatToStr(f); }
	inline const char *toString(double f) { return doubleToStr(f); }
	inline const char *toString(const Anandamide::vec3 &v) { return vectorToStr(v); }
	inline const char *toString(const Anandamide::Quaternion &q) { return quaternionToStr(q); }
	inline const char *toString(const Anandamide::Matrix &m) { return matrixToStr(m); }
	
	int strToInt(const char *str);
	float strToFloat(const char *str);
	double strToDouble(const char *str);
	Anandamide::vec3 strToVector(const char *str);
	Anandamide::Quaternion strToQuaternion(const char *str);
	Anandamide::Matrix strToMatrix(const char *str);
	
	void fromString(const char *str, int &i);
	void fromString(const char *str, float &f);
	void fromString(const char *str, double &f);
	void fromString(const char *str, Anandamide::vec3 &v);
	void fromString(const char *str, Anandamide::Quaternion &q);
	void fromString(const char *str, Anandamide::Matrix &m);

}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
