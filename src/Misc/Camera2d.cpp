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

#include "Camera2d.h"
#include "Renderer.h"
#include "Anandamide.h"
#include "Str.h"

#include <QDebug>
#include <QTime>

//------------------------------------------------------------------------------

namespace Anandamide {
	
	Camera2d::Camera2d() : scroll(false), zoom(1.0f), renderer(NULL), ratio(1.0) { }
	
	void Camera2d::setup(Renderer *renderer, float w, float h) {
		
		this->renderer = renderer;
		Matrix matrix;
	
		/*
		float zoom_s = floor(zoom * 10.0f) / 10.0f;
		Vector k = Vector(float(w) * zoom_s / 2.0f, float(h) * zoom_s / -2.0f, 1.0f);
		Vector pos_s = Vector(floor(pos.x * k.x) / k.x, floor(pos.y * k.y) / k.y, 0.0f);
		*/
	
		float zoom_s = getZoom();
		vec3 pos_s = pos;
	
		//matrix = Matrix::getTranslate(pos) * Matrix::getScale(Vector(1.0f / float(w), -1.0f / float(h), 1.0f) / zoom) * Matrix::getTranslate(Vector(-1.0f, 1.0f, 0.0f));
	
		/*
		if (math::abs(w) < EPSILON || math::abs(h) < EPSILON || math::abs(zoom_s) < EPSILON) {
			int stop = true;
		}
		*/
		
		matrix = Matrix::translate(pos_s) * Matrix::scale(vec3(2.0f / float(w) / zoom_s, -2.0f / float(h) / zoom_s, 1.0f));
	
		//matrix = Matrix::getTranslate(pos) * Matrix::getScale(Vector(2.0f / float(w), -2.0f / float(h), 1.0f) / zoom)/* * Matrix::getTranslate(Vector(-1.0f, 1.0f, 0.0f))*/;
		renderer->setViewMatrix(matrix);
		renderer->getFrustum(frustum);
	}
	
	//------------------------------------------------------------------------------
	
	void Camera2d::mouseDown(int x, int y) {
		scroll	= true;
		click = cursor;
	}
	
	//------------------------------------------------------------------------------
	
	void Camera2d::mouseUp(int x, int y) {
		scroll = false;
	}
	
	//------------------------------------------------------------------------------
	
	bool Camera2d::mouseMove(int x, int y) {
		if(renderer == NULL) return false;
		cursor = renderer->unproject(vec3(float(x), float(y), 0.5),frustum);
		if (scroll) {
			pos += cursor - click;
			//click = cursor;
			return true;
		}
		return false;
	
	}
	
	vec3 Camera2d::fromMouse(int x, int y) {
		if(renderer == NULL) return vec3();
		return renderer->unproject(vec3(float(x), float(y), 0.5),frustum);
	}
	
	//------------------------------------------------------------------------------
	
	void Camera2d::zoomIn(float delta, bool shift) {
		float old_zoom = zoom;
		zoom += delta;
		if (zoom < 0.2f) zoom = 0.2f;
		if (zoom > 7.0f) zoom = 7.0f;
		if(shift) pos += (cursor + pos) * (zoom - old_zoom) * 0.65f;
	}
	
	vec3 Camera2d::getCursorPos() const { return cursor; }
	
	const Frustum &Camera2d::getFrustum() { return frustum; }
	
	//------------------------------------------------------------------------------
	
	void Camera2d::setRatio(float r) {
		ratio = r;
	}
	
	float Camera2d::getZoom() const { return pow(2.0f, zoom) * 1.0f / 8.0f; }
	
	const vec3 &Camera2d::getPos() const { return pos; }
	
	void Camera2d::setPos(vec3 pos)
	{
		this->pos = pos;
	}
	
	void Camera2d::setTarget(vec3 t) {
		pos.x = -t.x;
		pos.y = -t.y;
	}
	
	bool Camera2d::getScroll() const {return scroll;}
	
	//------------------------------------------------------------------------------
	
	void Camera2d::clear() {
		pos = vec3();
		zoom = 3.0f;
	
	}
	
	//------------------------------------------------------------------------------

}
