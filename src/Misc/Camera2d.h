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

#ifndef Camera2dH
#define Camera2dH

#include "AnandamideLibAPI.h"

//------------------------------------------------------------------------------

#include "MathCore.h"

namespace Anandamide {
	
	class Renderer;
	
	class ANANDAMIDE_API Camera2d {
	
		vec3 click;
		bool scroll;
		vec3 pos;
		float zoom;
		float ratio;
		Frustum frustum;
		vec3 cursor;
		Renderer *renderer;
	
	public:
	
		Camera2d();
	
		void setup(Renderer *renderer, float w, float h);
		void clear();
	
		void mouseDown(int x, int y);
		void mouseUp(int x, int y);
		bool mouseMove(int x, int y);
		
		vec3 fromMouse(int x, int y);
		
		void zoomIn(float delta, bool shift = false);
		vec3 getCursorPos() const;
		const Frustum &getFrustum();
	
		void setRatio(float r);
		float getZoom() const;
		const vec3 &getPos() const;
		void setPos(vec3 pos);
		void setTarget(vec3 t);
	
		bool getScroll() const;
	};

//------------------------------------------------------------------------------

}

#endif

//------------------------------------------------------------------------------

