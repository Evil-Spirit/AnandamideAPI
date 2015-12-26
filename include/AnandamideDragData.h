//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// author:		Sergey Almighty
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

#ifndef DragDataH
#define DragDataH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideNeurone.h"
#include "AnandamideScript.h"
#include "QString"
#include "QMap"
#include "QVector"
#include "QMimeData"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//--------------------------------------------------------------------------
	//
	// class DragData
	//
	//--------------------------------------------------------------------------

	class ANANDAMIDE_API BlockDragDataContent {
	public:
		QString type;
		QString library;
		QString instance;
		
		QString metaType;
		QString ownerName;
		QString itemValue;
		
		QMap <QString, QString> params;
		QMap <QString, QString> inputs;
	};
	
	class ANANDAMIDE_API BlockDragData {
		
	public:
		
		QVector<BlockDragDataContent> contents;
		BlockDragDataContent content;
	
		BlockDragData();
		~BlockDragData();
		
		void addToMimeData(QMimeData* data);
		bool fillFromMimedata(const QMimeData *data);
		
		QMimeData *toMimeData();
		bool fromMimeData(const QMimeData *data);
		void fillNeurone(Neurone *neurone);
		
		static void fillBlocksNum(QMimeData *data, int num);
		
		void print();
	};
}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
