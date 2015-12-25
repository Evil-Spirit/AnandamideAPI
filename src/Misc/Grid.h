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

#ifndef GridH
#define GridH

//------------------------------------------------------------------------------

#include "Renderer.h"
#include "MathCore.h"
#include <math.h>

//------------------------------------------------------------------------------

namespace Anandamide {
	
	class Renderer;
	
	class Grid {
	
	public:
	
		float step;
	
		Grid() : step(0.1f) {}
	
		float snap(float x) const;
		vec3 snap(const vec3 &v) const;
	
		float snap(float x, float step) const;
		vec3 snap(const vec3 &v, float step) const;
	
		void render(Renderer *renderer, const Frustum &frustum) const;
	};

}	
//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------

