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

#ifndef ANANDAMIDESTDLIB_H
#define ANANDAMIDESTDLIB_H

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideLibrary.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	ANANDAMIDE_API VariableArray arrayCreate();
	ANANDAMIDE_API int arraySize(VariableArray array);
	ANANDAMIDE_API void arrayAppend(VariableArray array, const Variable &v);
	ANANDAMIDE_API void arrayAppend(VariableArray array_dst, VariableArray array_src);
	ANANDAMIDE_API int arrayIndexOf(VariableArray array, const Variable &v);
	ANANDAMIDE_API void arrayResize(VariableArray array, int size);
	ANANDAMIDE_API const Variable &arrayGet(VariableArray array, int index);
	ANANDAMIDE_API void arraySet(VariableArray array, int index, const Variable &value);
	ANANDAMIDE_API void arrayRemove(VariableArray array, int index);
	ANANDAMIDE_API void arrayClear(VariableArray array);
	
	ANANDAMIDE_API VariableMap mapCreate();
	ANANDAMIDE_API int mapSize(VariableMap map);
	ANANDAMIDE_API void mapAppend(VariableMap map, const Variable &key, const Variable &v);
	ANANDAMIDE_API const Variable &mapGet(VariableMap map, const Variable &key);
	ANANDAMIDE_API const Variable &mapGet(VariableMap map, int index);
	ANANDAMIDE_API const Variable &mapKey(VariableMap map, int index);
	ANANDAMIDE_API bool mapContains(VariableMap map, const Variable &key);
	ANANDAMIDE_API void mapRemove(VariableMap map, const Variable &key);
	ANANDAMIDE_API void mapClear(VariableMap map);
}

enum CompareOperation {
	IF_EQUAL,
	IF_NOTEQUAL,
	IF_GREATER,
	IF_GEQUAL,
	IF_LESS,
	IF_LEQUAL,
};

ENUM_BEGIN(CompareOperation)
	ENUM_ITEM_NAMED(IF_EQUAL,		"==")
	ENUM_ITEM_NAMED(IF_NOTEQUAL,	"!=")
	ENUM_ITEM_NAMED(IF_GREATER,		">")
	ENUM_ITEM_NAMED(IF_GEQUAL,		">=")
	ENUM_ITEM_NAMED(IF_LESS,		"<")
	ENUM_ITEM_NAMED(IF_LEQUAL,		"<=")
ENUM_END(CompareOperation, 2000)


ENUM_BEGIN(Qt::GlobalColor);
ENUM_ITEM(Qt::white);
ENUM_ITEM(Qt::black);
ENUM_ITEM(Qt::red);
ENUM_ITEM(Qt::darkRed);
ENUM_ITEM(Qt::green);
ENUM_ITEM(Qt::darkGreen);
ENUM_ITEM(Qt::blue);
ENUM_ITEM(Qt::darkBlue);
ENUM_ITEM(Qt::cyan);
ENUM_ITEM(Qt::darkCyan);
ENUM_ITEM(Qt::magenta);
ENUM_ITEM(Qt::darkMagenta);
ENUM_ITEM(Qt::yellow);
ENUM_ITEM(Qt::darkYellow);
ENUM_ITEM(Qt::gray);
ENUM_ITEM(Qt::darkGray);
ENUM_ITEM(Qt::lightGray);
ENUM_END(Qt::GlobalColor, 1210)

typedef void (*ConsoleMessageCallback)(const char*);
typedef void (*ConsoleMessageExtCallback)(const char*, bool, Qt::GlobalColor, bool, bool);

ANANDAMIDE_API void setMsgFunction(ConsoleMessageCallback callBack);
ANANDAMIDE_API void setMsgExtFunction(ConsoleMessageExtCallback callBack);

#endif // ANANDAMIDESTDLIB_H
