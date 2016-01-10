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

#include "Renderer.h"

//------------------------------------------------------------------------------

#define QT_RENDER

#ifdef QT_RENDER
	#include <QDebug>
	#include <QPainter>
	#include <QGraphicsTextItem>
#endif

//------------------------------------------------------------------------------

#ifdef WIN32
#include "windows.h"
#endif

#include <GL/gl.h>

//------------------------------------------------------------------------------

namespace Anandamide {
	
#ifdef QT_RENDER

	QPolygonF rendererPolygon;

	float getRendererTextWidth(const QString& text, const QFont& font)
	{
		QGraphicsTextItem item(text);
		item.setFont(font);
		return item.boundingRect().width();
	}

	float getRendererTextHeight(const QString& text, const QFont& font)
	{
		QGraphicsTextItem item(text);
		item.setFont(font);
		return item.boundingRect().height();
	}

#endif
	
	QMap<QString, QImage*> Renderer::staticImages;
	
	void Renderer::addStaticImage(const QString &filename)
	{
		staticImages[filename] = new QImage(filename);
	}
	
	QImage *Renderer::getStaticImage(const QString &filename)
	{
		if(!staticImages.contains(filename)) return NULL;
		return staticImages[filename];
	}
	
	void Renderer::initStaticImages() {
	}
	
	void Renderer::doneStaticImages()
	{
		foreach (QString key, staticImages.keys()) {
			delete staticImages.take(key);
		}
	}
	
	Renderer::Renderer() {
		width = 640;
		height = 480;
		fontSize = 16;
	
		initialized = false;
	
		currentColor = vec3(1.0f, 1.0f, 1.0f);
		currentAlpha = 1.0f;
		ambientColor = vec3(0.3f, 0.3f, 0.3f);
	
		lineSmooth = false;
		painter = NULL;
	}
	
	//------------------------------------------------------------------------------
	
	Renderer::~Renderer() {
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::setResolution(int w, int h) {
		width = w;
		height = h;
		glViewport(0, 0, w, h);
		if (height == 0) height = 1;
		if (width == 0) width = 1;
	}
	
	//------------------------------------------------------------------------------
	
	int Renderer::getWidth() const {
		return width;
	}
	
	//------------------------------------------------------------------------------
	
	int Renderer::getHeight() const {
		return height;
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::rectLine(float x, float y, float w, float h) {
		if(painter != NULL) {
			#ifdef QT_RENDER
			QBrush b = painter->brush();
			b.setStyle(Qt::NoBrush);
			painter->setBrush(b);
			painter->drawRect(QRectF(x, y, w, h));
			#endif
		} else {
			glDisable(GL_TEXTURE_2D);
	
			glBegin(GL_LINE_LOOP);
	
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
	
			glEnd();
		}
	
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::rect(float x, float y, float w, float h) {
	
		if(painter != NULL) return;
	
		float x0 = x;
		float y0 = y;
		float x1 = x0 + w;
		float y1 = y0 + h;
	
	
		if (x1 > x0) ::math::swap(x0, x1);
		if (y1 > y0) ::math::swap(y0, y1);
	
		x0 = x0 - 1.0f;
		y0 = y0 - 1.0f;
		x1 = x1 + 1.0f;
		y1 = y1 + 1.0f;
	
		glBegin(GL_TRIANGLES);
	
		glVertex2f(x0, y0);
		glVertex2f(x1, y0);
		glVertex2f(x0 + 1.0f, y0 + 1.0f);
	
		glVertex2f(x0 + 1.0f, y0 + 1.0f);
		glVertex2f(x1, y0);
		glVertex2f(x1 - 1.0f, y0 + 1.0f);
	
		glVertex2f(x0, y0);
		glVertex2f(x0 + 1.0f, y0 + 1.0f);
		glVertex2f(x0, y1);
	
		glVertex2f(x0 + 1.0f, y0 + 1.0f);
		glVertex2f(x0 + 1.0f, y1 - 1.0f);
		glVertex2f(x0, y1);
	
		glVertex2f(x0 + 1.0f, y1 - 1.0f);
		glVertex2f(x1, y1);
		glVertex2f(x0, y1);
	
		glVertex2f(x1 - 1.0f, y1 - 1.0f);
		glVertex2f(x1, y1);
		glVertex2f(x0 + 1.0f, y1 - 1.0f);
	
		glVertex2f(x1 , y1);
		glVertex2f(x1 - 1.0f, y0 + 1.0f);
		glVertex2f(x1, y0);
	
		glVertex2f(x1, y1);
		glVertex2f(x1 - 1.0f, y1 - 1.0f);
		glVertex2f(x1 - 1.0f, y0 + 1.0f);
	
		glEnd();
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::rectFill(float x, float y, float w, float h) {
		if(painter != NULL) {
			#ifdef QT_RENDER
			QBrush b = painter->brush();
			b.setStyle(Qt::SolidPattern);
			painter->setBrush(b);
			painter->drawRect(QRectF(x, y, w, h));
			#endif
		} else {
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
			glEnd();
		}
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::rect3d(float x, float y, float w, float h, bool selected) {
		vec3 old_color = currentColor;
		float oldAlpha = currentAlpha;
	
		if(painter != NULL) {
			#ifdef QT_RENDER
			QBrush b = painter->brush();
			b.setStyle(Qt::SolidPattern);
			painter->setBrush(b);
			QPen pen;
			pen.setColor(QColor(120, 120, 120));
			QPen penBack(pen);
			if(selected) {
				pen.setStyle(Qt::DashLine);
				pen.setWidth(5);
			}
			painter->setPen(pen);
			painter->drawRect(QRectF(x, y, w, h));
			painter->setPen(penBack);
			#endif
			/*
			static QPushButton *button = NULL;
			if (button == NULL) button = new QPushButton();
			button->setGeometry(x, y, w, h);
			button->render(painter, QPoint(x, y));
			
			color(currentColor, 0.3f);
			QBrush b = painter->brush();
			b.setStyle(Qt::SolidPattern);
			painter->setBrush(b);
			QPen pen;
			pen.setStyle(Qt::NoPen);
			painter->setPen(pen);
			painter->drawRect(x, y, w, h);
			*/
		} else {
	
	
			color(old_color * 0.75f);
	
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
			glEnd();
	
	
			float x0 = x;
			float y0 = y;
			float x1 = x0 + w;
			float y1 = y0 + h;
	
	
			if (x1 > x0) ::math::swap(x0, x1);
			if (y1 > y0) ::math::swap(y0, y1);
	
			x0 = x0 - 1.0f;
			y0 = y0 - 1.0f;
			x1 = x1 + 1.0f;
			y1 = y1 + 1.0f;
	
	
			glBegin(GL_TRIANGLES);
	
			//
			color(old_color * 0.5f);
	
			glVertex2f(x0, y0);
			glVertex2f(x1, y0);
			glVertex2f(x0 + 1.0f, y0 + 1.0f);
	
			glVertex2f(x0 + 1.0f, y0 + 1.0f);
			glVertex2f(x1, y0);
			glVertex2f(x1 - 1.0f, y0 + 1.0f);
	
			glVertex2f(x0, y0);
			glVertex2f(x0 + 1.0f, y0 + 1.0f);
			glVertex2f(x0, y1);
	
			glVertex2f(x0 + 1.0f, y0 + 1.0f);
			glVertex2f(x0 + 1.0f, y1 - 1.0f);
			glVertex2f(x0, y1);
	
			//
			color(old_color);
	
			glVertex2f(x0 + 1.0f, y1 - 1.0f);
			glVertex2f(x1, y1);
			glVertex2f(x0, y1);
	
			glVertex2f(x1 - 1.0f, y1 - 1.0f);
			glVertex2f(x1, y1);
			glVertex2f(x0 + 1.0f, y1 - 1.0f);
	
			glVertex2f(x1 , y1);
			glVertex2f(x1 - 1.0f, y0 + 1.0f);
			glVertex2f(x1, y0);
	
			glVertex2f(x1, y1);
			glVertex2f(x1 - 1.0f, y1 - 1.0f);
			glVertex2f(x1 - 1.0f, y0 + 1.0f);
	
			glEnd();
	
			/*
			glBegin(GL_LINES);
	
			color(old_color);
			glVertex2f(x + 1, y + h);
			glVertex2f(x + 1, y + 1);
	
			glVertex2f(x + 1, y);
			glVertex2f(x + w, y);
	
			color(old_color * 0.5f);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
	
			glVertex2f(x + w, y + h);
			glVertex2f(x + 1, y + h);
	
			glEnd();
			*/
		}
		color(old_color, oldAlpha);
	
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::render(const Line &l) {
		if(painter != NULL) return;
		glBegin(GL_LINES);
		glVertex3fv(l.src.v);
		glVertex3fv(l.dst.v);
		glEnd();
	}
	
	void Renderer::color(float r, float g, float b, float a) {
		color(vec3(r, g, b), a);
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::color(const vec3 &c, float a) {
		currentColor = c;
		currentAlpha = a;
	
		if (painter != NULL) {
			#ifdef QT_RENDER
			QPen pen;
			vec3 cc = c * 255.0f;
			QColor col(cc.x, cc.y, cc.z, a * 255.0f);
			pen.setColor(col);
			painter->setPen(col);
			QBrush brush;
			brush.setStyle(Qt::SolidPattern);
			brush.setColor(col);
			painter->setBrush(brush);
			#endif
		} else {
			glColor4f(c.x, c.y, c.z, a);
		}
	}
	
	//------------------------------------------------------------------------------
	
	vec3 Renderer::getColor() const {
		return currentColor;
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::text(float x, float y, const char *fmt, ...) {
		if (!fmt) return;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
	
		if(painter != NULL) {
			#ifdef QT_RENDER
			painter->setFont(QFont());
			painter->drawText(QPointF(x, y + 10), QString::fromLocal8Bit(text));
			#endif
		}	
	}
	
	void Renderer::textInRect(float left, float top, 
							  float width, float height, 
							  int flags, 
							  const char *fmt, ...)
	{
		if (!fmt) return;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
	
		if(painter != NULL) {
			#ifdef QT_RENDER
			painter->setFont(QFont());
			QRectF rect(left, top, width, height);
			painter->drawText(rect, flags, QString::fromLocal8Bit(text));
			#endif
		}
	}
	
	float Renderer::textHeight(const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			return getRendererTextHeight(QString::fromLocal8Bit(text), QFont());
			#endif
			return 0;
		}
		else return 0;
	}
	
	float Renderer::textWidth(const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			return getRendererTextWidth(QString::fromLocal8Bit(text), QFont());
			#endif
			return 0;
		}
		else return 0;
	}

	void Renderer::textBold(float x, float y, const char *fmt, ...)
	{
		if (!fmt) return;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
	
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setBold(true);
			painter->setFont(f);
			painter->drawText(QPointF(x, y + 10), QString::fromLocal8Bit(text));
			#endif
		}
	}
	
	void Renderer::textBoldInRect(float left, float top, 
								  float width, float height, 
								  int flags, 
								  const char *fmt, ...)
	{
		if (!fmt) return;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
	
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setBold(true);
			painter->setFont(f);
			QRectF rect(left, top, width, height);
			painter->drawText(rect, flags, QString::fromLocal8Bit(text));
			#endif
		}
	}
	
	float Renderer::textBoldWidth(const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setBold(true);
			return getRendererTextWidth(QString::fromLocal8Bit(text), f);
			#endif
			return 0;
		}
		else return 0;
	}
	
	float Renderer::textBoldHeight(const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setBold(true);
			return getRendererTextHeight(QString::fromLocal8Bit(text), f);
			#endif
			return 0;
		}
		else return 0;
	}
	
	struct FontAndHeight {
		QFont font;
		float textHeight = 0.0;
	};
	
	void Renderer::textToWidth(float left, float width, 
							   float topCenter, bool leftToRight, 
							   const char *fmt, ...)
	{
		if (!fmt) return;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			QString title = QString::fromLocal8Bit(text);
			static QMap <QString, FontAndHeight> fonts_for_text;
			
			FontAndHeight fh;
			if(!fonts_for_text.contains(title)) {
				int fontSize = fh.font.pointSize();
				float textWidth;
				do{
					fh.font.setPointSize(fontSize--);
					textWidth = getRendererTextWidth(title, fh.font);
				}while(textWidth > width && fontSize > 3);
				
				fh.textHeight = getRendererTextHeight(title, fh.font);
				
				fonts_for_text[title] = fh;
				if(fonts_for_text.size() > 20000) fonts_for_text.clear();
				
			} else {
				fh = fonts_for_text[title];
			}
			
			painter->setFont(fh.font);
			QRectF rect(left, topCenter-fh.textHeight/2, width, fh.textHeight);
			painter->drawText(rect, leftToRight ? 0x81 : 0x82, title);
			#endif
		}
	}
	
	float Renderer::getTextPointSizeByWidth(float width, const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			QString title = QString::fromLocal8Bit(text);
			QFont f;
			float fontSizeDelta = 0.3f;
			float fontSize = f.pointSizeF() + fontSizeDelta;
			float textWidth;
			do{
				fontSize -= fontSizeDelta;
				f.setPointSizeF(fontSize);
				textWidth = getRendererTextWidth(title, f);
			}while(textWidth > width && fontSize > 4.5f);
			return fontSize;
			#endif
		}
		return 0;
	}
	
	float Renderer::textWidth(float textSize, const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setPointSizeF(textSize);
			return getRendererTextWidth(QString::fromLocal8Bit(text), f);
			#endif
			return 0;
		}
		else return 0;
	}
	
	float Renderer::textHeight(float textSize, const char *fmt, ...)
	{
		if (!fmt) return 0;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setPointSizeF(textSize);
			return getRendererTextHeight(QString::fromLocal8Bit(text), f);
			#endif
			return 0;
		}
		else return 0;
	}
	
	void Renderer::text(float textSize, float x, float y, const char *fmt, ...)
	{
		if (!fmt) return;
	
		static char	text[4096] = "\0\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
	
		if(painter != NULL) {
			#ifdef QT_RENDER
			QFont f;
			f.setPointSizeF(textSize);
			painter->setFont(f);
			painter->drawText(QPointF(x, y + 10), QString::fromLocal8Bit(text));
			#endif
		}	
	}
	
	//------------------------------------------------------------------------------
	
	vec3 getSplineValue(const vec3 &a, const vec3 &b, const vec3 &ra, const vec3 &rb, float t) {
		float t2 = t*t;
		float t3 = t2*t;
		return	a * ( 2*t3 - 3*t2 + 1) + ra * (t3 - 2*t2 + t) +
				b * (-2*t3 + 3*t2    ) + rb * (t3 - t2      );
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::linkSpline(const vec3 &p0, const vec3 &p1, const vec3 &d0, const vec3 &d1, const vec3 &c0, const vec3 &c1, bool selected, int num) {
	
		vec3 old_color = currentColor;
		float oldAlpha = currentAlpha;
		const int approxPoints = 32;
		float delta = 1.0f / (approxPoints-1) ;
		
		if(painter != NULL) {
			
			#ifdef QT_RENDER
			
			vec3 vdir = normalize(d1);
			
			vec3 pp1 = p1 - vdir * 8.0f;
			
			QLinearGradient gradient(QPointF(p0.x, p0.y), QPointF(pp1.x, pp1.y));
			vec3 col0 = c0 * 255.0f;
			vec3 col1 = c1 * 255.0f;
			gradient.setColorAt(0, QColor(col0.r, col0.g, col0.b));
			gradient.setColorAt(1, QColor(col1.r, col1.g, col1.b));
			QBrush brush(gradient);
			QPen pen;
			if(selected) {
				pen.setWidthF(4.0);
				pen.setStyle(Qt::DotLine);
			} else {
				pen.setWidthF(2.0);
			}
			pen.setBrush(brush);
			painter->setPen(pen);
			
			QPointF points[approxPoints];
			float t = 0.0f;
			for(int i=0; i<approxPoints; i++) {
				vec3 pos = getSplineValue(p0, pp1, d0, d1, t);
				points[i] = QPointF(pos.x, pos.y);
				t += delta;
			}
			
			painter->drawPolyline(points, approxPoints);
			
			/*
			Vector old;
			Vector cur;
			for(float t=0.0f; t<=1.0f; t+=delta) {
				old = cur;
				cur = getSplineValue(p0, pp1, Vector(200, 0, 0), Vector(200, 0, 0), t);
				color(m ath::lerp(c0, c1, t));
				if(t != 0.0f) painter->drawLine(QPointF(old.x, old.y), QPointF(cur.x, cur.y));
			}
			*/
			
			color(c1);
			pen = painter->pen();
			pen.setJoinStyle(Qt::MiterJoin);
			pen.setStyle(Qt::SolidLine);
			pen.setMiterLimit(5.0);
			painter->setPen(pen);
			

			
			QPointF dir = QPointF(vdir.x, vdir.y) * 10.0;
			QPointF normal = QPointF(vdir.y, -vdir.x) * 3.0;
			points[0] = QPointF(p1.x, p1.y);
			points[1] = points[0] - dir + normal;
			points[2] = QPointF(pp1.x, pp1.y);
			points[3] = points[0] - dir - normal;
			
			
						
			painter->drawConvexPolygon(points, 4);
			
			
			
			if(num >= 0) {
				QFont f;
				f.setPixelSize(10);
				f.setBold(true);
				f.setItalic(true);
				painter->setFont(f);
				painter->drawText(QPointF(p1.x-25, p1.y-10), QString::number(num));
			}
			#endif
		} else {
	
			glBegin(GL_LINE_STRIP);
	
			for (float t=0.0f; t<=1.0f; t+=delta) {
				vec3 cur = getSplineValue(p0, p1, vec3(200, 0, 0), vec3(200, 0, 0), t);
				color(::math::lerp(c0, c1, t));
				glVertex3fv(cur.v);
			}
			glEnd();
	
			glBegin(GL_LINE_STRIP);
			glVertex3fv((p1 + vec3(-1.2f, -1.2f, 0.0f) * 5.0f).v);
			glVertex3fv(p1.v);
			glVertex3fv((p1 + vec3(-1.2f, +1.2f, 0.0f) * 5.0f).v);
			glEnd();
		}
		color(old_color, oldAlpha);
	}

	void Renderer::linkSpline(const vec3 &p0, const vec3 &p1, const vec3 &c0, const vec3 &c1, bool selected, int num) {
		vec3 vdir = vec3(1.0f, 0.0f, 0.0f);
		vec3 pp1 = p1 - vdir * 8.0f;
		float derivative = 500.0f;
		float length = (p0 - pp1).length();
		if(length < derivative) derivative = length;
		vec3 d0(derivative, 0, 0);
		vec3 d1(derivative, 0, 0);
		linkSpline(p0, p1, d0, d1, c0, c1, selected, num);
	}

	void Renderer::linkSpline(const vec3 &p0, const vec3 &p1, float a0, float a1, const vec3 &c0, const vec3 &c1, bool selected, int num) {
		vec3 vdir = vec3(1.0f, 0.0f, 0.0f);
		vec3 pp1 = p1 - vdir * 8.0f;
		float derivative = 500.0f;
		float length = (p0 - pp1).length();
		if(length < derivative) derivative = length;

		vec3 d0 = vec3(cos(a0 * 3.14 / 180), sin(a0 * 3.14 / 180), 0.0f) * derivative;
		vec3 d1 = vec3(cos(a1 * 3.14 / 180), sin(a1 * 3.14 / 180), 0.0f) * derivative;
		
		linkSpline(p0, p1, d0, d1, c0, c1, selected, num);
	}
	
	float getPointLineDistance2d(const vec3 &p0, const vec3 &p1, vec3 point) {
		vec3 direction = p1 - p0;
		float len = dot(direction,direction);
		if(abs(len) < EPSILON) return (p0 - point).length();
		float k = dot(point - p0,direction) / len;
		if(k > 1.0f) k = 1.0f;
		if(k < 0.0f) k = 0.0f;
		return (p0 + direction * k - point).length();
	}
	
	bool Renderer::selectSpline(const vec3 &p0, const vec3 &p1, const vec3 &by, float radius) {
		
		vec3 vdir = vec3(1.0f, 0.0f, 0.0f);
		vec3 pp1 = p1 - vdir * 8.0f;
		
		float derivative = 500.0f;
		float length = (p0 - pp1).length();
		if(length < derivative) derivative = length;
		
		vec3 d0 = vec3(derivative, 0, 0);
		vec3 d1 = vec3(derivative, 0, 0);
		
		return selectSpline(p0, p1, d0, d1, by, radius);
	}
	
	bool Renderer::selectSpline(const vec3 &p0, const vec3 &p1, const vec3 &d0, const vec3 &d1, const vec3 &by, float radius)
	{
		const int approxPoints = 32;
		float delta = 1.0f / (approxPoints-1) ;
		
		vec3 vdir = vec3(1.0f, 0.0f, 0.0f);
		vec3 pp1 = p1 - vdir * 8.0f;
		
		float t = 0.0f;
		vec3 old_pos;
		for(int i=0; i<approxPoints; i++) {
			vec3 pos = getSplineValue(p0, pp1, d0, d1, t);
			if(i > 0 && getPointLineDistance2d(pos, old_pos, by) < radius) {
				return true;
			}
			old_pos = pos;
			t += delta;
		}
		return false;
	}
	
	bool Renderer::selectSpline(const vec3 &p0, const vec3 &p1, float a0, float a1, const vec3 &by, float radius)
	{
		vec3 vdir = vec3(1.0f, 0.0f, 0.0f);
		vec3 pp1 = p1 - vdir * 8.0f;
		float derivative = 500.0f;
		float length = (p0 - pp1).length();
		if(length < derivative) derivative = length;

		vec3 d0 = vec3(cos(a0 * 3.14 / 180), sin(a0 * 3.14 / 180), 0.0f) * derivative;
		vec3 d1 = vec3(cos(a1 * 3.14 / 180), sin(a1 * 3.14 / 180), 0.0f) * derivative;
		
		return selectSpline(p0, p1, d0, d1, by, radius);
	}
	
	void Renderer::drawImage(float x, float y, float w, float h, const QImage &image) {
		if(painter != NULL) {
			painter->drawImage(QRectF(x, y, w, h), image);
		}
	}
	
	void Renderer::drawStaticImage(float x, float y, float w, float h, const QString &image_name)
	{
		QImage* img = Renderer::getStaticImage(image_name);
		if(img == NULL) return;
		drawImage(x, y, w, h, *img);
	}
	
	void Renderer::clearPolygonPoints()
	{
		if(painter != NULL) {
		#ifdef QT_RENDER
			rendererPolygon.clear();
		#endif
		}
	}
	
	void Renderer::addPolygonPoint(float x, float y)
	{
		if(painter != NULL) {
		#ifdef QT_RENDER
			rendererPolygon.append(QPointF(x, y));
		#endif
		}
	}
	
	void Renderer::polygonDraw()
	{
		if(painter != NULL) {
		#ifdef QT_RENDER
			if(!rendererPolygon.isEmpty()) {
				QBrush b = painter->brush();
				b.setStyle(Qt::SolidPattern);
				painter->setBrush(b);
				QPen pen;
				pen.setColor(QColor(120, 120, 120));
				painter->setPen(pen);
				painter->drawPolygon(rendererPolygon);
				rendererPolygon.clear();
			}
		#endif
		}
	}
	
	
	//------------------------------------------------------------------------------
	
	void Renderer::setMatrix(const Matrix &m) {
		modelviewMatrix = m;
		glLoadMatrixf(viewMatrix);
		glMultMatrixf(modelviewMatrix);
	}
	
	//------------------------------------------------------------------------------
	
	const Matrix &Renderer::getMatrix() const {
		return modelMatrix;
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::setViewMatrix(const Matrix &m) {
		viewMatrix = m;
		Matrix im(m);
		im.inverse();
		vec3 o = im * vec3(0, 0, 0);
		up = im * vec3(0, 1, 0) - o;
		right = im * vec3(1, 0, 0) - o;
		glLoadMatrixf(viewMatrix);
	}
	
	//------------------------------------------------------------------------------
	
	const Matrix &Renderer::getViewMatrix() const {
		return viewMatrix;
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::setFrustum(const Frustum &f) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(f.getProjection().v);
		glMatrixMode(GL_MODELVIEW);
	
		//TODO: get the proj matrix
		setViewMatrix(f.getModelview());
	}
	
	//------------------------------------------------------------------------------
	
	int *Renderer::getViewport() {
		static int viewport[4];
	
		glGetIntegerv(GL_VIEWPORT, viewport);
		return viewport;
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::getFrustum(Frustum &f) {
	
		static float modelView[16];
		static float projection[16];
		static int viewport[4];
	
		glGetFloatv(GL_PROJECTION_MATRIX, projection);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
		glGetIntegerv(GL_VIEWPORT, viewport);
	
		f.set(modelView, projection, viewport);
	}
	
	
	
	//------------------------------------------------------------------------------
	
	vec3 Renderer::project(const vec3 &v, const Frustum &f) {
	
		vec3 r;
		const int *viewport = f.getViewport();
		const Matrix &projection = f.getProjection();
		const Matrix &modelview = f.getModelview();
		
		//Transformation vectors
		float fTempo[8];
	
		//Modelview transform
		fTempo[0] = modelview.v[0] * v.x + modelview.v[4] * v.y + modelview.v[8] * v.z + modelview.v[12]; 
		fTempo[1] = modelview.v[1] * v.x + modelview.v[5] * v.y + modelview.v[9] * v.z + modelview.v[13];
		fTempo[2] = modelview.v[2] * v.x + modelview.v[6] * v.y + modelview.v[10] * v.z + modelview.v[14];
		fTempo[3] = modelview.v[3] * v.x + modelview.v[7] * v.y + modelview.v[11] * v.z + modelview.v[15];
		
		fTempo[4] = projection.v[0] * fTempo[0] + projection.v[4] * fTempo[1] + projection.v[8] * fTempo[2] + projection.v[12] * fTempo[3];
		fTempo[5] = projection.v[1] * fTempo[0] + projection.v[5] * fTempo[1] + projection.v[9] * fTempo[2] + projection.v[13] * fTempo[3];
		fTempo[6] = projection.v[2] * fTempo[0] + projection.v[6] * fTempo[1] + projection.v[10] * fTempo[2] + projection.v[14] * fTempo[3];
		fTempo[7] = -fTempo[2];
	/*
		//The result normalizes between -1 and 1
		if (::math::abs(fTempo[7]) < EPSILON) {
			return Vector();
		}
		
		fTempo[7] = 1.0 / fTempo[7];
	
		//Perspective division
		fTempo[4] *= fTempo[7];
		fTempo[5] *= fTempo[7];
		fTempo[6] *= fTempo[7];
	 */   
		//Window coordinates
		//Map x, y to range 0-1
		r.v[0] = (fTempo[4] * 0.5 + 0.5) * viewport[2] + viewport[0];
		r.v[1] = (fTempo[5] * 0.5 + 0.5) * viewport[3] + viewport[1];
		r.v[2] = (1.0 + fTempo[6]) * 0.5;
		
		return vec3(r.v[0], float(viewport[3]) - r.v[1], r.v[2]);
	}
	
	//------------------------------------------------------------------------------
	
	vec3 Renderer::unproject(const vec3 &v, const Frustum &f) {
	
		
		//Transformation matrices
		Vector4f v4;
		const int *viewport = f.getViewport();
		
		//Calculation for inverting a matrix, compute projection x modelview
		//and store in A[16]
		
		Matrix m = f.getModelview() * f.getProjection();
		m.inverse();
		
		//Transformation of normalized coordinates between -1 and 1
		v4.x = (v.x - float(viewport[0])) / float(viewport[2]) * 2.0f - 1.0f;
		v4.y = viewport[3] - v.y;
		v4.y = (v4.y - float(viewport[1])) / float(viewport[3]) * 2.0f - 1.0f;
		v4.z = 2.0f * v.z - 1.0f;
		v4.w = 1.0f;
		
		//Objects coordinates
		v4 = m * v4;
		if (::math::abs(v4.w) < EPSILON) {
			return vec3();
		}
		v4.w = 1.0f / v4.w;
	
		vec3 result = vec3(v4.x * v4.w, v4.y * v4.w, v4.z * v4.w);
		return result;
	}
	
	//------------------------------------------------------------------------------
	
	
	void Renderer::clear(bool color, bool depth, const vec3 &c) {
		//if(painter != NULL) return;
		if (color == false && depth == false) return;
	
		int bit = 0;
	
		if (color) bit |= GL_COLOR_BUFFER_BIT;
		if (depth) bit |= GL_DEPTH_BUFFER_BIT;
	
		if (color) {
			glClearColor(c.x, c.y, c.z, 1.0);
		}
		glClear(bit);
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::begin() {
		glFinish();
	}
	
	//------------------------------------------------------------------------------
	
	void Renderer::end() {
		glFlush();
	}
	
	//------------------------------------------------------------------------------
	//
	// console
	//
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	
	void Renderer::init() {
	
		if (!initialized) {
	
			initialized = true;
		}
	
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
		glEnable(GL_CULL_FACE);
	//	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glEnable(GL_LIGHTING);
	//	glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	
		color(currentColor);
	
		glPointSize(7.0f);
		glLineWidth(1.0f);
	}
}
