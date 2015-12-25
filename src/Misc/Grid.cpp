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

#include "Grid.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	float Grid::snap(float x, float step) const {
		return floor(x / step + 0.5f) * step;
	}
	
	//------------------------------------------------------------------------------
	
	vec3 Grid::snap(const vec3 &v, float step) const {
		return vec3(snap(v.x, step), snap(v.y, step), 0.5f);
	}
	
	//------------------------------------------------------------------------------
	
	float Grid::snap(float x) const {
		return snap(x, step);
	}
	
	//------------------------------------------------------------------------------
	
	vec3 Grid::snap(const vec3 &v) const {
		return snap(v, step);
	}
	
	//------------------------------------------------------------------------------
	
	void Grid::render(Renderer *renderer, const Frustum &frustum) const {
	
		int *v;
		v = renderer->getViewport();
		vec3 min = renderer->unproject(vec3(float(v[0]), float(v[1]), 0.5f), frustum);
		vec3 max = renderer->unproject(vec3(float(v[2]), float(v[3]), 0.5f), frustum);
	
		//Vector scale_v = renderer->project(Vector(1.0f, 1.0f, 0.0f), frustum) - renderer->project(Vector(0.0f, 0.0f, 0.0f), frustum);
		vec3 scale_v = renderer->project(min + vec3(1.0f, 1.0f, 0.0f), frustum) - renderer->project(min, frustum);
		//Log::message("scale_v.x %f, scale_v.y %f\n", scale_v.x, scale_v.y);
		float scale = ::math::min_(::math::abs(scale_v.x), ::math::abs(scale_v.y));
	
		//return;
		
		if (::math::abs(scale) < EPSILON) {
			throw "Grid::render(): division by zero.";
		}
	
		float step = ::math::max_(1.0f, float(pow(10.0f, ceil(log10(2.0f / 3.0f / scale))))) * this->step;
	
	
		float ln;
		float z = 1.0f;
	
		ln = floor(min.y / step) * step;
		while (ln < max.y) {
	
			if (int(floor(ln / step + 0.5f)) == 0) {
				renderer->color(0.3f, 0.3f, 0.05f);
				z = 0.1f;
			} else
			if (int(floor(ln / step + 0.5f)) % 10 == 0 ) {
				renderer->color(0.3f, 0.3f, 0.3f);
				z = 0.2f;
			} else
			if (int(floor(ln / step + 0.5f)) % 5 == 0 ) {
				renderer->color(0.15f, 0.15f, 0.15f);
				z = 0.3f;
			} else {
				renderer->color(0.1f, 0.1f, 0.1f);
				z = 0.4f;
			}
	
			renderer->render(Line(vec3(min.x, ln, z), vec3(max.x, ln, z)));
			ln += step;
		}
	
		ln = floor(min.x / step) * step;
		while (ln < max.x) {
	
			if (int(floor(ln / step + 0.5f)) == 0) {
				renderer->color(0.3f, 0.3f, 0.05f);
				z = 0.1f;
			} else
			if (int(floor(ln / step + 0.5f)) % 10 == 0 ) {
				renderer->color(0.3f, 0.3f, 0.3f);
				z = 0.2f;
			} else
			if (int(floor(ln / step + 0.5f)) % 5 == 0) {
				renderer->color(0.15f, 0.15f, 0.15f);
				z = 0.3f;
			} else {
				renderer->color(0.1f, 0.1f, 0.1f);
				z = 0.4f;
			}
	
			renderer->render(Line(vec3(ln, min.y, z), vec3(ln, max.y, z)));
			ln += step;
		}
		
	
	}
	
	//------------------------------------------------------------------------------

}
