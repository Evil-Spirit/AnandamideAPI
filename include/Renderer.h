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

#ifndef RendererH
#define RendererH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "MathCore.h"
#include <QMap>

//------------------------------------------------------------------------------

class QPainter;
class QImage;
class QPixmap;

namespace Anandamide {
	
	class ANANDAMIDE_API Renderer {
	
		bool initialized;
	
		float fontSize;
	
		Matrix modelMatrix;
		Matrix viewMatrix;
		Matrix modelviewMatrix;
		Matrix projectionMatrix;
	
		double dModelviewMatrix[16];
		double dProjectionMatrix[16];
	
		int width;
		int height;
	
		vec3 up;
		vec3 right;
		vec3 currentColor;
		vec3 ambientColor;
		vec3 cameraPos;
		
		float currentAlpha;
		float pointSize;
		bool lineSmooth;
	
		QPainter *painter;
		
		static QMap<QString, QImage*> staticImages;
		static void addStaticImage(const QString& filename);
		static QImage* getStaticImage(const QString& filename);
	
	public:
		
		static void initStaticImages();
		static void doneStaticImages();
	
		Renderer();
		virtual ~Renderer();
	
		void setPainter(QPainter *painter) {
			this->painter = painter;
		}
	
		QPainter *getPainter() {
			return painter;
		}
	
		//--------------------------------------------------------------------------
		// common
		//--------------------------------------------------------------------------
	
		virtual void init();
	
		//--------------------------------------------------------------------------
		// base
		//--------------------------------------------------------------------------
	
		virtual void color(float r, float g, float b, float a = 1.0f);
		virtual void color(const vec3 &, float a = 1.0f);
	
		virtual vec3 getColor() const;
	
		//--------------------------------------------------------------------------
		//
		//--------------------------------------------------------------------------
	
		virtual void clear(bool = true, bool = true, const vec3 &c = vec3(0.0f, 0.0f, 0.0f));
		virtual void begin();
		virtual void end();
	
		//--------------------------------------------------------------------------
		// 2d mode
		//--------------------------------------------------------------------------
	
		//virtual void set2d(int w = -1, int h = -1);
		virtual void setResolution(int w, int h);
		virtual int getWidth() const;
		virtual int getHeight() const;
		virtual void rect(float x, float y, float w, float h);
		virtual void rectLine(float x, float y, float w, float h);
		void rect3d(float x, float y, float w, float h, bool selected = false);
		virtual void rectFill(float x, float y, float w, float h);

		virtual void linkSpline(const vec3 &p0, const vec3 &p1, const vec3 &c0, const vec3 &c1, bool selected = false, int num = -1);
		
		///
		/// \brief linkSpline Drawing link spline with user-defined angles [use in CustomNeurone]
		/// \param a0 src angle
		/// \param a1 dst angle
		///
		virtual void linkSpline(const vec3 &p0, const vec3 &p1, float a0, float a1, const vec3 &c0, const vec3 &c1, bool selected, int num);
		virtual void linkSpline(const vec3 &p0, const vec3 &p1, const vec3 &d0, const vec3 &d1, const vec3 &c0, const vec3 &c1, bool selected = false, int num = -1);
		
		static bool selectSpline(const vec3 &p0, const vec3 &p1, const vec3 &by, float radius);
		static bool selectSpline(const vec3 &p0, const vec3 &p1, const vec3 &d0, const vec3 &d1, const vec3 &by, float radius);
		static bool selectSpline(const vec3 &p0, const vec3 &p1, float a0, float a1, const vec3 &by, float radius);		
		
		virtual void drawImage(float x, float y, float w, float h, const QImage &image);
		virtual void drawStaticImage(float x, float y, float w, float h, const QString &image_name);
	
		void clearPolygonPoints();
		void addPolygonPoint(float x, float y);
		void polygonDraw();
		
		void text(float x, float y, const char *text, ...);
		void textInRect(float left, float top, float width, float height, int flags, const char *text, ...);
		float textWidth(const char *text, ...);
		float textHeight(const char *text, ...);
		
		void textBold(float x, float y, const char *text, ...);
		void textBoldInRect(float left, float top, float width, float height, int flags, const char *text, ...);		
		float textBoldWidth(const char *text, ...);
		float textBoldHeight(const char *text, ...);
		
		void textToWidth(float left, float width, float topCenter, bool leftToRight, const char *text, ...);
		float getTextPointSizeByWidth(float width, const char *text, ...);
		float textWidth(float textSize, const char *text, ...);
		float textHeight(float textSize, const char *text, ...);
		void text(float textSize, float x, float y, const char *text, ...);
	
		//--------------------------------------------------------------------------
		// 3d mode
		//--------------------------------------------------------------------------
	
		//virtual void set3d();
	
		virtual void render(const Line &);
	
		//--------------------------------------------------------------------------
		// resources
		//--------------------------------------------------------------------------
	
		virtual void setMatrix(const Matrix &);
		virtual const Matrix &getMatrix() const;
	
		virtual void setViewMatrix(const Matrix &);
		virtual const Matrix &getViewMatrix() const;
	
		int *getViewport();
		void setFrustum(const Frustum &f);
		virtual void getFrustum(Frustum &f);
	
		virtual vec3 unproject(const vec3 &v, const Frustum &f);
		virtual vec3 project(const vec3 &v, const Frustum &f);
		
	};
	
	//------------------------------------------------------------------------------
}

#endif
