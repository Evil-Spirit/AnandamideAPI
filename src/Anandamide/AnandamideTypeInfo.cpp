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

#include "AnandamideTypeInfo.h"
#include "Anandamide.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//--------------------------------------------------------------------------
	
	int nextTypeID() {
		static int next_id = 9000;
		return next_id ++;
	}
	
	bool checkOrAddTypeID(int id, const char *type) {
		static Map <int, Str> ids;
		Str *item = ids.get(id);
		if(item == NULL) {
			ids.append(new int(id), new Str(type));
			return true;
		}
		if(*item == type) return true;
		errorMessage(format("Attempt to assign type id \"%d\" to type \"%s\", id already taken by type \"%s\".", id, type, item->str()));
		return false;
	}
	
	//--------------------------------------------------------------------------
	//
	// сlass Enum
	//
	//--------------------------------------------------------------------------
	
	Enum::Enum() { }
	
	Enum::~Enum() { }
	
	int Enum::getItemsCount() const {
		return 0;
	}
	
	const char *Enum::getItemName(int i) const {
		return NULL;
	}
	
	int Enum::getItemId(int i) const {
		return 0;
	}
	
	const char *Enum::getItemNameById(int id) const {
		return NULL;
	}
	
	const int *Enum::getItemIdByName(const char *name) const {
		return NULL;
	}
	
	void operator>>(const TypeInfo &t, Str &str) {
		str.format("%d", t.id());
	}
	
	void operator<<(TypeInfo &t, const Str &str) {
		t.setId(str.getInt());
	}

	//--------------------------------------------------------------------------

}

#include <QString>

namespace Anandamide {

	void operator>>(unsigned int t, Anandamide::Str &str)
	{
		str.set(QString::number(t).toLocal8Bit().constData());
	}
	
	void operator<<(unsigned int &t, const Anandamide::Str &str)
	{
		t = QString::fromLocal8Bit(str.str()).toUInt();
	}
	
	void operator>>(long long t, Str &str)
	{
		str.set(QString::number(t).toLocal8Bit().constData());
	}
	
	void operator<<(long long &t, const Str &str)
	{
		t = QString::fromLocal8Bit(str.str()).toLongLong();
	}
	
	void operator>>(unsigned long long t, Str &str)
	{
		str.set(QString::number(t).toLocal8Bit().constData());
	}
	
	void operator<<(unsigned long long &t, const Str &str)
	{
		t = QString::fromLocal8Bit(str.str()).toULongLong();
	}
	
	void operator>>(char t, Str &str)
	{
		str.set(QString(QChar(t)).toLocal8Bit().constData());
	}
	
	void operator<<(char &t, const Str &str)
	{
		if(str.size() == 0)
			t = '?';
		else
			t = str[0];
	}
}
