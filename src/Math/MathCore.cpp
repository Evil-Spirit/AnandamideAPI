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

#include "MathCore.h"

//------------------------------------------------------------------------------

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "Str.h"

#if _MSC_VER < 1900 
#define snprintf _snprintf
#endif

//------------------------------------------------------------------------------
//
// namespace math
//
//------------------------------------------------------------------------------

float math::abs(float a) {
	return fabs(a);
}

//------------------------------------------------------------------------------

float math::sqrt(float a) {
	return sqrtf(a);
}

//------------------------------------------------------------------------------

float math::cos(float a) {
	return cosf(a);
}

//------------------------------------------------------------------------------

float math::sin(float a) {
	return sinf(a);
}

//------------------------------------------------------------------------------

void math::sincos(float alpha, float &sn, float &cs) {
	sn = sinf(alpha);
	cs = cosf(alpha);
}

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//------------------------------------------------------------------------------
	//
	// class vec3
	//
	//------------------------------------------------------------------------------
	
	void vec3::normalize() {
		float len = length();
		if (len > EPSILON) {
			float inv = 1.0f / len;
			x *= inv;
			y *= inv;
			z *= inv;
		}
	}
	
	void vec3::operator<<(const Str &str) {
		Nutmeg::fromString(str, *this);
	}
	
	void vec3::operator>>(Str &str) const {
		str = Nutmeg::toString(*this);
	}
	
	//------------------------------------------------------------------------------
	//
	// class Matrix
	//
	//------------------------------------------------------------------------------
	
	Matrix::Matrix() {
		identity();
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::identity() {
		for (int i=0; i<4; i++) {
			for (int j=0; j<4; j++) {
				data[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
	}
	
	//------------------------------------------------------------------------------
	
	bool Matrix::operator== (const Matrix &o) {
		for (int i=0; i<4; i++) {
			for (int j=0; j<4; j++) {
				if (::math::abs(data[i][j] - o.data[i][j]) > EPSILON) {
					return false;
				}
			}
		}
		return true;
	}
	//------------------------------------------------------------------------------
	
	bool Matrix::equals(const Matrix &o, float eps) {
		for (int i=0; i<4; i++) {
			for (int j=0; j<4; j++) {
				if (::math::abs(data[i][j] - o.data[i][j]) > eps) {
					return false;
				}
			}
		}
		return true;
	}
	
	
	//------------------------------------------------------------------------------
	
	bool Matrix::operator!= (const Matrix &o) {
		return !operator== (o);
	}
	
	//------------------------------------------------------------------------------
	
	
	bool Matrix::inverse() {
	
		#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
		#define MAT(m,r,c) (m)[(c)*4+(r)]
	
		Matrix m_src = *this;
		float *m = (float *)&m_src;
		float *out = (float *)this;
		float wtmp[4][8];
		float m0, m1, m2, m3, s;
		float *r0, *r1, *r2, *r3;
	
		r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
	
		r0[0] = MAT(m, 0, 0); r0[1] = MAT(m, 0, 1); r0[2] = MAT(m, 0, 2); r0[3] = MAT(m, 0, 3); r0[4] = 1.0; r0[5] = r0[6] = r0[7] = 0.0;
		r1[0] = MAT(m, 1, 0); r1[1] = MAT(m, 1, 1), r1[2] = MAT(m, 1, 2); r1[3] = MAT(m, 1, 3), r1[5] = 1.0; r1[4] = r1[6] = r1[7] = 0.0;
		r2[0] = MAT(m, 2, 0); r2[1] = MAT(m, 2, 1); r2[2] = MAT(m, 2, 2); r2[3] = MAT(m, 2, 3); r2[6] = 1.0; r2[4] = r2[5] = r2[7] = 0.0;
		r3[0] = MAT(m, 3, 0); r3[1] = MAT(m, 3, 1); r3[2] = MAT(m, 3, 2); r3[3] = MAT(m, 3, 3); r3[7] = 1.0; r3[4] = r3[5] = r3[6] = 0.0;
	
		/* choose pivot - or die */
		if (::math::abs(r3[0]) > ::math::abs(r2[0])) SWAP_ROWS(r3, r2);
		if (::math::abs(r2[0]) > ::math::abs(r1[0])) SWAP_ROWS(r2, r1);
		if (::math::abs(r1[0]) > ::math::abs(r0[0])) SWAP_ROWS(r1, r0);
		if (0.0 == r0[0]) return 0;
	
		/* eliminate first variable	 */
		m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];
	
		s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
		s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
		s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	
		s = r0[4]; if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
		s = r0[5]; if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
		s = r0[6]; if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
		s = r0[7]; if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }
	
		/* choose pivot - or die */
		if (::math::abs(r3[1]) > ::math::abs(r2[1])) SWAP_ROWS(r3, r2);
		if (::math::abs(r2[1]) > ::math::abs(r1[1])) SWAP_ROWS(r2, r1);
		if (0.0 == r1[1]) return 0;
	
		/* eliminate second variable */
		m2 = r2[1] / r1[1];
		m3 = r3[1] / r1[1];
		r2[2] -= m2 * r1[2];
		r3[2] -= m3 * r1[2];
		r2[3] -= m2 * r1[3];
		r3[3] -= m3 * r1[3];
	
		s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
		s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
		s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
		s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }
	
		/* choose pivot - or die */
		if (::math::abs(r3[2]) > ::math::abs(r2[2])) SWAP_ROWS(r3, r2);
		if (0.0 == r2[2]) return false;
	
		/* eliminate third variable */
		m3 = r3[2] / r2[2];
		r3[3] -= m3 * r2[3]; r3[4] -= m3 * r2[4];
		r3[5] -= m3 * r2[5]; r3[6] -= m3 * r2[6]; r3[7] -= m3 * r2[7];
	
		/* last check */
		if (0.0 == r3[3]) return false;
	
		/* now back substitute row 3 */
		s = 1.0f / r3[3]; r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;
	
		/* now back substitute row 2 */
		m2 = r2[3];
		s = 1.0f / r2[2];
		r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
		m1 = r1[3];
		r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
		m0 = r0[3];
		r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
	
		m1 = r1[2];			/* now back substitute row 1 */
		s = 1.0f / r1[1];
		r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
		m0 = r0[2];
		r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
	
		m0 = r0[1];			/* now back substitute row 0 */
		s = 1.0f / r0[0];
		r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);
	
		MAT(out, 0, 0) = r0[4];
		MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
		MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
		MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
		MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
		MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
		MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
		MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
		MAT(out, 3, 3) = r3[7];
	
		return true;
	
		#undef MAT
		#undef SWAP_ROWS
	}
	
	//------------------------------------------------------------------------------
	
	Matrix Matrix::operator*(const Matrix &m) const {
		Matrix r;
	
		r.data[0][0] = data[0][0] * m.data[0][0] + data[0][1] * m.data[1][0] + data[0][2] * m.data[2][0] + data[0][3] * m.data[3][0];
		r.data[0][1] = data[0][0] * m.data[0][1] + data[0][1] * m.data[1][1] + data[0][2] * m.data[2][1] + data[0][3] * m.data[3][1];
		r.data[0][2] = data[0][0] * m.data[0][2] + data[0][1] * m.data[1][2] + data[0][2] * m.data[2][2] + data[0][3] * m.data[3][2];
		r.data[0][3] = data[0][0] * m.data[0][3] + data[0][1] * m.data[1][3] + data[0][2] * m.data[2][3] + data[0][3] * m.data[3][3];
	
		r.data[1][0] = data[1][0] * m.data[0][0] + data[1][1] * m.data[1][0] + data[1][2] * m.data[2][0] + data[1][3] * m.data[3][0];
		r.data[1][1] = data[1][0] * m.data[0][1] + data[1][1] * m.data[1][1] + data[1][2] * m.data[2][1] + data[1][3] * m.data[3][1];
		r.data[1][2] = data[1][0] * m.data[0][2] + data[1][1] * m.data[1][2] + data[1][2] * m.data[2][2] + data[1][3] * m.data[3][2];
		r.data[1][3] = data[1][0] * m.data[0][3] + data[1][1] * m.data[1][3] + data[1][2] * m.data[2][3] + data[1][3] * m.data[3][3];
	
		r.data[2][0] = data[2][0] * m.data[0][0] + data[2][1] * m.data[1][0] + data[2][2] * m.data[2][0] + data[2][3] * m.data[3][0];
		r.data[2][1] = data[2][0] * m.data[0][1] + data[2][1] * m.data[1][1] + data[2][2] * m.data[2][1] + data[2][3] * m.data[3][1];
		r.data[2][2] = data[2][0] * m.data[0][2] + data[2][1] * m.data[1][2] + data[2][2] * m.data[2][2] + data[2][3] * m.data[3][2];
		r.data[2][3] = data[2][0] * m.data[0][3] + data[2][1] * m.data[1][3] + data[2][2] * m.data[2][3] + data[2][3] * m.data[3][3];
	
		r.data[3][0] = data[3][0] * m.data[0][0] + data[3][1] * m.data[1][0] + data[3][2] * m.data[2][0] + data[3][3] * m.data[3][0];
		r.data[3][1] = data[3][0] * m.data[0][1] + data[3][1] * m.data[1][1] + data[3][2] * m.data[2][1] + data[3][3] * m.data[3][1];
		r.data[3][2] = data[3][0] * m.data[0][2] + data[3][1] * m.data[1][2] + data[3][2] * m.data[2][2] + data[3][3] * m.data[3][2];
		r.data[3][3] = data[3][0] * m.data[0][3] + data[3][1] * m.data[1][3] + data[3][2] * m.data[2][3] + data[3][3] * m.data[3][3];
	
		return r;
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::mult(const Matrix &m) {
	
		Matrix r;
	
		r.data[0][0] = data[0][0] * m.data[0][0] + data[0][1] * m.data[1][0] + data[0][2] * m.data[2][0] + data[0][3] * m.data[3][0];
		r.data[0][1] = data[0][0] * m.data[0][1] + data[0][1] * m.data[1][1] + data[0][2] * m.data[2][1] + data[0][3] * m.data[3][1];
		r.data[0][2] = data[0][0] * m.data[0][2] + data[0][1] * m.data[1][2] + data[0][2] * m.data[2][2] + data[0][3] * m.data[3][2];
		r.data[0][3] = data[0][0] * m.data[0][3] + data[0][1] * m.data[1][3] + data[0][2] * m.data[2][3] + data[0][3] * m.data[3][3];
	
		r.data[1][0] = data[1][0] * m.data[0][0] + data[1][1] * m.data[1][0] + data[1][2] * m.data[2][0] + data[1][3] * m.data[3][0];
		r.data[1][1] = data[1][0] * m.data[0][1] + data[1][1] * m.data[1][1] + data[1][2] * m.data[2][1] + data[1][3] * m.data[3][1];
		r.data[1][2] = data[1][0] * m.data[0][2] + data[1][1] * m.data[1][2] + data[1][2] * m.data[2][2] + data[1][3] * m.data[3][2];
		r.data[1][3] = data[1][0] * m.data[0][3] + data[1][1] * m.data[1][3] + data[1][2] * m.data[2][3] + data[1][3] * m.data[3][3];
	
		r.data[2][0] = data[2][0] * m.data[0][0] + data[2][1] * m.data[1][0] + data[2][2] * m.data[2][0] + data[2][3] * m.data[3][0];
		r.data[2][1] = data[2][0] * m.data[0][1] + data[2][1] * m.data[1][1] + data[2][2] * m.data[2][1] + data[2][3] * m.data[3][1];
		r.data[2][2] = data[2][0] * m.data[0][2] + data[2][1] * m.data[1][2] + data[2][2] * m.data[2][2] + data[2][3] * m.data[3][2];
		r.data[2][3] = data[2][0] * m.data[0][3] + data[2][1] * m.data[1][3] + data[2][2] * m.data[2][3] + data[2][3] * m.data[3][3];
	
		r.data[3][0] = data[3][0] * m.data[0][0] + data[3][1] * m.data[1][0] + data[3][2] * m.data[2][0] + data[3][3] * m.data[3][0];
		r.data[3][1] = data[3][0] * m.data[0][1] + data[3][1] * m.data[1][1] + data[3][2] * m.data[2][1] + data[3][3] * m.data[3][1];
		r.data[3][2] = data[3][0] * m.data[0][2] + data[3][1] * m.data[1][2] + data[3][2] * m.data[2][2] + data[3][3] * m.data[3][2];
		r.data[3][3] = data[3][0] * m.data[0][3] + data[3][1] * m.data[1][3] + data[3][2] * m.data[2][3] + data[3][3] * m.data[3][3];
	
		*this = r;
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::normalize() {
		vec3 x(data[0][0],data[0][1],data[0][2]);
		vec3 y(data[1][0],data[1][1],data[1][2]);
		vec3 z;
		x.normalize();
		z = cross(x,y);
		z.normalize();
		y = cross(z,x);
		y.normalize();
		data[0][0] = x.x; data[1][0] = y.x; data[2][0] = z.x;
		data[0][1] = x.y; data[1][1] = y.y; data[2][1] = z.y;
		data[0][2] = x.z; data[1][2] = y.z; data[2][2] = z.z;
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::multOrder(const Matrix &m) {
	
		Matrix r;
	
		r.data[0][0] = m.data[0][0] * data[0][0] + m.data[0][1] * data[1][0] + m.data[0][2] * data[2][0] + m.data[0][3] * data[3][0];
		r.data[0][1] = m.data[0][0] * data[0][1] + m.data[0][1] * data[1][1] + m.data[0][2] * data[2][1] + m.data[0][3] * data[3][1];
		r.data[0][2] = m.data[0][0] * data[0][2] + m.data[0][1] * data[1][2] + m.data[0][2] * data[2][2] + m.data[0][3] * data[3][2];
		r.data[0][3] = m.data[0][0] * data[0][3] + m.data[0][1] * data[1][3] + m.data[0][2] * data[2][3] + m.data[0][3] * data[3][3];
	
		r.data[1][0] = m.data[1][0] * data[0][0] + m.data[1][1] * data[1][0] + m.data[1][2] * data[2][0] + m.data[1][3] * data[3][0];
		r.data[1][1] = m.data[1][0] * data[0][1] + m.data[1][1] * data[1][1] + m.data[1][2] * data[2][1] + m.data[1][3] * data[3][1];
		r.data[1][2] = m.data[1][0] * data[0][2] + m.data[1][1] * data[1][2] + m.data[1][2] * data[2][2] + m.data[1][3] * data[3][2];
		r.data[1][3] = m.data[1][0] * data[0][3] + m.data[1][1] * data[1][3] + m.data[1][2] * data[2][3] + m.data[1][3] * data[3][3];
	
		r.data[2][0] = m.data[2][0] * data[0][0] + m.data[2][1] * data[1][0] + m.data[2][2] * data[2][0] + m.data[2][3] * data[3][0];
		r.data[2][1] = m.data[2][0] * data[0][1] + m.data[2][1] * data[1][1] + m.data[2][2] * data[2][1] + m.data[2][3] * data[3][1];
		r.data[2][2] = m.data[2][0] * data[0][2] + m.data[2][1] * data[1][2] + m.data[2][2] * data[2][2] + m.data[2][3] * data[3][2];
		r.data[2][3] = m.data[2][0] * data[0][3] + m.data[2][1] * data[1][3] + m.data[2][2] * data[2][3] + m.data[2][3] * data[3][3];
	
		r.data[3][0] = m.data[3][0] * data[0][0] + m.data[3][1] * data[1][0] + m.data[3][2] * data[2][0] + m.data[3][3] * data[3][0];
		r.data[3][1] = m.data[3][0] * data[0][1] + m.data[3][1] * data[1][1] + m.data[3][2] * data[2][1] + m.data[3][3] * data[3][1];
		r.data[3][2] = m.data[3][0] * data[0][2] + m.data[3][1] * data[1][2] + m.data[3][2] * data[2][2] + m.data[3][3] * data[3][2];
		r.data[3][3] = m.data[3][0] * data[0][3] + m.data[3][1] * data[1][3] + m.data[3][2] * data[2][3] + m.data[3][3] * data[3][3];
	
		*this = r;
	}
	
	//------------------------------------------------------------------------------
	
	vec3 Matrix::operator* (const vec3 &v) const {
	
		return vec3(
			data[0][0] * v.x + data[1][0] * v.y + data[2][0] * v.z + data[3][0],
			data[0][1] * v.x + data[1][1] * v.y + data[2][1] * v.z + data[3][1],
			data[0][2] * v.x + data[1][2] * v.y + data[2][2] * v.z + data[3][2]);
	
	}
	
	//------------------------------------------------------------------------------
	
	Vector4f Matrix::operator* (const Vector4f &v) const {
	
		return Vector4f(
			data[0][0] * v.x + data[1][0] * v.y + data[2][0] * v.z + data[3][0] * v.w,
			data[0][1] * v.x + data[1][1] * v.y + data[2][1] * v.z + data[3][1] * v.w,
			data[0][2] * v.x + data[1][2] * v.y + data[2][2] * v.z + data[3][2] * v.w, 
			data[0][3] * v.x + data[1][3] * v.y + data[2][3] * v.z + data[3][3] * v.w);
	
	}
	
	//------------------------------------------------------------------------------
	
	vec3 Matrix::getPos() const {
		return vec3(data[3][0], data[3][1], data[3][2]);
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::setPos(const vec3 &pos) {
		data[3][0] = pos.x;
		data[3][1] = pos.y;
		data[3][2] = pos.z;
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::setScale(const vec3 &scale) {
		data[0][0] = scale.x;
		data[1][1] = scale.y;
		data[2][2] = scale.z;
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::basis(const vec3 &x, const vec3 &y, const vec3 &z, const vec3 &d) {
	
		data[0][0] = x.x;
		data[1][0] = x.y;
		data[2][0] = x.z;
		data[3][0] = 0;
	
		data[0][1] = y.x;
		data[1][1] = y.y;
		data[2][1] = y.z;
		data[3][1] = 0;
	
		data[0][2] = z.x;
		data[1][2] = z.y;
		data[2][2] = z.z;
		data[3][2] = 0;
	
		data[3][0] = d.x;
		data[3][1] = d.y;
		data[3][2] = d.z;
		data[3][3] = 1;
	}
	
	//------------------------------------------------------------------------------
	
	Matrix Matrix::rotateX(float a) {
	
		Matrix r;
		float cs ,sn;
		::math::sincos(a, sn, cs);
	
		r.data[1][1] = cs;
		r.data[1][2] = -sn;
		r.data[2][1] = sn;
		r.data[2][2] = cs;
	
		return r;
	}
	
	//------------------------------------------------------------------------------
	
	Matrix Matrix::rotateY(float a) {
	
		Matrix r;
		float cs ,sn;
		::math::sincos(a, sn, cs);
	
		r.data[0][0] = cs;
		r.data[0][2] = -sn;
		r.data[2][0] = sn;
		r.data[2][2] = cs;
	
		return r;
	}
	
	//------------------------------------------------------------------------------
	
	Matrix Matrix::rotateZ(float a) {
	
		Matrix r;
		float cs ,sn;
		::math::sincos(a, sn, cs);
	
		r.data[0][0] = cs;
		r.data[0][1] = -sn;
		r.data[1][0] = sn;
		r.data[1][1] = cs;
	
		return r;
	}
	
	//------------------------------------------------------------------------------
	
	Matrix Matrix::translate(const vec3 &d) {
	
		Matrix r;
		
		r.data[3][0] = d.x;
		r.data[3][1] = d.y;
		r.data[3][2] = d.z;
	
		return r;
	}
	
	//------------------------------------------------------------------------------
	
	Matrix Matrix::scale(const vec3 &s) {
		Matrix r;
	
		r.data[0][0] = s.x;
		r.data[1][1] = s.y;
		r.data[2][2] = s.z;
	
		return r;
	}
	
	//------------------------------------------------------------------------------
	
	void Matrix::transpose() {
		float temp;
		for (int i=0; i<3; i++) {
			for (int j=i+1; j<4; j++) {
				temp = data[i][j];
				data[i][j] = data[j][i];
				data[j][i] = temp;
			}
		}
	}
	//------------------------------------------------------------------------------
	
	Vector4f Matrix::getRow(int i) const {
		return Vector4f(data[i][0], data[i][1], data[i][2], data[i][3]);
	}
	
	//------------------------------------------------------------------------------
	
	Vector4f Matrix::getColumn(int i) const {
		return Vector4f(data[0][i], data[1][i], data[2][i], data[3][i]);
	}
	
	//------------------------------------------------------------------------------
	//
	// class Plane
	//
	//------------------------------------------------------------------------------
	
	Plane::Plane(const vec3 &n, const vec3 &o) {
		normal = n;
		setOrigin(o);
	}
	
	//------------------------------------------------------------------------------
	
	void Plane::setOrigin(const vec3 &o) {
		dist = -dot(normal, o);
	}
	
	//------------------------------------------------------------------------------
	
	void Plane::normalize() {
		float len = normal.length();
	
		if (::math::abs(len) > EPSILON ) {
			normal /= len;
			dist /= len;
		}
	}
	
	//------------------------------------------------------------------------------
	
	Plane normalize(const Plane &p) {
		float len = p.normal.length();
	
		if (::math::abs(len) > EPSILON ) {
			float ilen = 1.0f / len;
			return Plane(p.normal * ilen, p.dist * ilen);
		}
		return p;
	}
	
	//------------------------------------------------------------------------------
	//
	// class BBox
	//
	//------------------------------------------------------------------------------
	
	BBox::BBox(const vec3 &p0, const vec3 &p1) {
		min.x = ::math::min_(p0.x, p1.x);
		min.y = ::math::min_(p0.y, p1.y);
		min.z = ::math::min_(p0.z, p1.z);
	
		max.x = ::math::max_(p0.x, p1.x);
		max.y = ::math::max_(p0.y, p1.y);
		max.z = ::math::max_(p0.z, p1.z);
	}

	//------------------------------------------------------------------------------
	
	vec3 BBox::getVertex(int i) const {
	
		if (i < 0 || i > 7) throw "BBox::getVertex : index out of bounds.";
	
		const vec3 *bounds = &min;
	
		return vec3(bounds[i % 2].x, bounds[(i / 2) % 2].y, bounds[(i / 4) % 2].z);
	}
	
	//------------------------------------------------------------------------------
	
	Plane BBox::getPlane(int i) const {
	
		if (i < 0 || i > 5) throw "BBox::getPlane : index out of bounds.";
	
		const vec3 *bounds = &min;
	
		Plane ret;
		ret.normal[i % 3] = float((i / 3) % 2) * 2.0f - 1.0f;
		ret.setOrigin(bounds[(i / 3) % 2]);
	
		return ret;
	}
	
	//------------------------------------------------------------------------------
	
	void BBox::include(const vec3 &v, float r) {
		if (v.x - r < min.x) min.x = v.x - r;
		if (v.y - r < min.y) min.y = v.y - r;
		if (v.z - r < min.z) min.z = v.z - r;
	
		if (v.x + r > max.x) max.x = v.x + r;
		if (v.y + r > max.y) max.y = v.y + r;
		if (v.z + r > max.z) max.z = v.z + r;
	}
	
	//------------------------------------------------------------------------------
	//
	//	distance
	//
	//------------------------------------------------------------------------------
	
	float distance(const vec3 &a, const vec3 &b) {
		return (a - b).length();
	}
	
	//------------------------------------------------------------------------------
	
	float distance(const Plane &p, const vec3 &v) {
		return dot(v, p.normal) + p.dist;
	}
	
	//------------------------------------------------------------------------------
	
	bool overlaps(const BBox &b0, const BBox &b1) {
	
		vec3 t = b1.getOrigin() - b0.getOrigin();
		vec3 e = b1.getExtents() + b0.getExtents();
	
		return ::math::abs(t.x) < e.x && ::math::abs(t.y) < e.y && ::math::abs(t.z) < e.z;
	}

	//------------------------------------------------------------------------------
	//
	// class Frustum
	//
	//------------------------------------------------------------------------------
	
	Frustum::Frustum() {
	}
	
	//------------------------------------------------------------------------------
	
	void Frustum::invalidate() {
	
		float clip[16];
	
		for (int i=0; i<4; i++) {
			for (int j=0; j<4; j++) {
				clip[i*4 + j] =
					modelview.v[i*4 + 0] * projection.v[0*4 + j] +
					modelview.v[i*4 + 1] * projection.v[1*4 + j] +
					modelview.v[i*4 + 2] * projection.v[2*4 + j] +
					modelview.v[i*4 + 3] * projection.v[3*4 + j];
			}
		}
	
	//0
		planes[0].normal.x = clip[ 3] - clip[ 0];
		planes[0].normal.y = clip[ 7] - clip[ 4];
		planes[0].normal.z = clip[11] - clip[ 8];
		planes[0].dist = clip[15] - clip[12];
	//1
		planes[1].normal.x = clip[ 3] + clip[ 0];
		planes[1].normal.y = clip[ 7] + clip[ 4];
		planes[1].normal.z = clip[11] + clip[ 8];
		planes[1].dist = clip[15] + clip[12];
	//2
		planes[2].normal.x = clip[ 3] + clip[ 1];
		planes[2].normal.y = clip[ 7] + clip[ 5];
		planes[2].normal.z = clip[11] + clip[ 9];
		planes[2].dist = clip[15] + clip[13];
	//3
		planes[3].normal.x = clip[ 3] - clip[ 1];
		planes[3].normal.y = clip[ 7] - clip[ 5];
		planes[3].normal.z	= clip[11] - clip[ 9];
		planes[3].dist = clip[15] - clip[13];
	//4
		planes[4].normal.x = clip[ 3] - clip[ 2];
		planes[4].normal.y = clip[ 7] - clip[ 6];
		planes[4].normal.z = clip[11] - clip[10];
		planes[4].dist = clip[15] - clip[14];
	//5
		planes[5].normal.x = clip[ 3] + clip[ 2];
		planes[5].normal.y = clip[ 7] + clip[ 6];
		planes[5].normal.z = clip[11] + clip[10];
		planes[5].dist = clip[15] + clip[14];
	
		for (int i =0; i<6; i++) {
			planes[i].normalize();
		}
	}
	
	//------------------------------------------------------------------------------
	
	void Frustum::set(const float *mv, const float *p, int *vp) {
		float clip[16];
		float *modelviewF = modelview;
		float *projectionF = projection;
	
	
		for (int i=0; i<16; i++) {
			modelview.v[i] = mv[i];
			projection.v[i] = p[i];
			modelviewF[i] = mv[i];
			projectionF[i] = p[i];
		}
	
		for (int i=0; i<4; i++) {
			viewport[i] = vp[i];
		}
	
		for (int i=0; i<4; i++) {
			for (int j=0; j<4; j++) {
				clip[i*4 + j] =
					modelview.v[i*4 + 0] * projection.v[0*4 + j] +
					modelview.v[i*4 + 1] * projection.v[1*4 + j] +
					modelview.v[i*4 + 2] * projection.v[2*4 + j] +
					modelview.v[i*4 + 3] * projection.v[3*4 + j];
			}
		}
	
	//0
		planes[0].normal.x = clip[ 3] - clip[ 0];
		planes[0].normal.y = clip[ 7] - clip[ 4];
		planes[0].normal.z = clip[11] - clip[ 8];
		planes[0].dist = clip[15] - clip[12];
	//1
		planes[1].normal.x = clip[ 3] + clip[ 0];
		planes[1].normal.y = clip[ 7] + clip[ 4];
		planes[1].normal.z = clip[11] + clip[ 8];
		planes[1].dist = clip[15] + clip[12];
	//2
		planes[2].normal.x = clip[ 3] + clip[ 1];
		planes[2].normal.y = clip[ 7] + clip[ 5];
		planes[2].normal.z = clip[11] + clip[ 9];
		planes[2].dist = clip[15] + clip[13];
	//3
		planes[3].normal.x = clip[ 3] - clip[ 1];
		planes[3].normal.y = clip[ 7] - clip[ 5];
		planes[3].normal.z	= clip[11] - clip[ 9];
		planes[3].dist = clip[15] - clip[13];
	//4
		planes[4].normal.x = clip[ 3] - clip[ 2];
		planes[4].normal.y = clip[ 7] - clip[ 6];
		planes[4].normal.z = clip[11] - clip[10];
		planes[4].dist = clip[15] - clip[14];
	//5
		planes[5].normal.x = clip[ 3] + clip[ 2];
		planes[5].normal.y = clip[ 7] + clip[ 6];
		planes[5].normal.z = clip[11] + clip[10];
		planes[5].dist = clip[15] + clip[14];
	
		for (int i =0;i<6;i++) {
			planes[i].normalize();
		}
	}
	
	//------------------------------------------------------------------------------
	
	void Frustum::setModelview(const Matrix &m) {
		modelview = m;
		float *modelviewF = modelview;
	
	
		for (int i=0; i<16; i++) {
			modelview.v[i] = modelviewF[i];
		}
	
		invalidate();
	}
	
	//------------------------------------------------------------------------------
	
	void Frustum::setProjection(const Matrix &m) {
	
		projection = m;
		invalidate();
	}
	
	//------------------------------------------------------------------------------
	
	void Frustum::setViewport(int *vp) {
		for(int i=0; i<4; i++) {
			viewport[i] = vp[i];
		}
		invalidate();
	}
	
}
//------------------------------------------------------------------------------

namespace Nutmeg {
	
	//--------------------------------------------------------------------------

	const char *intToStr(int i) {
		static char res[512];
		*res = '\0';
		snprintf(res, 512, "%d", i);
		return res;
	}

	//--------------------------------------------------------------------------

	const char *floatToStr(float f) {
		static char res[512];
		*res = '\0';
		snprintf(res, 512, "%f", f);
		return res;
	}

	//--------------------------------------------------------------------------

	const char *vectorToStr(const Anandamide::vec3 &v) {
		static char res[512];
		*res = '\0';

		strcat(res, floatToStr(v.x));
		strcat(res, " ");
		strcat(res, floatToStr(v.y));
		strcat(res, " ");
		strcat(res, floatToStr(v.z));

		return res;
	}

	//--------------------------------------------------------------------------
	const char *matrixToStr(const Anandamide::Matrix &m) {
		static char res[1024];
		*res = '\0';
		const float *mat = m;

		for(int i=0; i<15; i++) {
			strcat(res, floatToStr(mat[i]));
			strcat(res, " ");
		}
		strcat(res, floatToStr(mat[15]));

		return res;
	}

	//--------------------------------------------------------------------------

	int strToInt(const char *str) {
		static int res = 0;
		fromString(str, res);
		return res;
	}

	//--------------------------------------------------------------------------

	float strToFloat(const char *str) {
		static float res = 0;
		fromString(str, res);
		return res;
	}

	//--------------------------------------------------------------------------

	Anandamide::vec3 strToVector(const char *str) {
		static Anandamide::vec3 res;
		fromString(str, res);
		return res;
	}

	//--------------------------------------------------------------------------

	Anandamide::Matrix strToMatrix(const char *str) {
		static Anandamide::Matrix res;
		fromString(str, res);
		return res;
	}

	//--------------------------------------------------------------------------

	void fromString(const char *str, int &i) {
		i = atoi(str);
	}

	//--------------------------------------------------------------------------

	void fromString(const char *str, float &f) {
		f = (float)atof(str);
	}

	//--------------------------------------------------------------------------

	void fromString(const char *str, Anandamide::vec3 &v) {

		v.x = (float)atof(str);

		while(*str != ' ' && *str != '\t') { if (*str == '\0') return; str ++; }
		while(*str == ' ' || *str == '\t') str ++;
		if (*str == '\0') return;

		v.y = (float)atof(str);

		while(*str != ' ' && *str != '\t') { if (*str == '\0') return; str ++; }
		while(*str == ' ' || *str == '\t') str ++;
		if (*str == '\0') return;

		v.z = (float)atof(str);
	}

	//--------------------------------------------------------------------------

	void fromString(const char *str, Anandamide::Matrix &m) {
		float *mat = m;

		for(int i=0; i<15; i++) {
			mat[i] = (float)atof(str);

			while(*str != ' ' && *str != '\t') { if (*str == '\0') return; str ++; }
			while(*str == ' ' || *str == '\t') str ++;
			if (*str == '\0') return;
		}

		mat[15] = (float)atof(str);
	}
	
	double strToDouble(const char *str) {
		double res = 0;
		fromString(str, res);
		return res;
	}
	
	void fromString(const char *str, double &f) {
		f = atof(str);
	}
	
	const char *doubleToStr(double f) {
		static char res[512];
		*res = '\0';
		snprintf(res, 512, "%g", f);
		return res;
	}
	
	//--------------------------------------------------------------------------
	
}


int math::random(int __num) { return __num ? (int)(rand() % (__num)) : 0; }
