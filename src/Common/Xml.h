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

#ifndef XmlH
#define XmlH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Array.h"
#include "Str.h"

//------------------------------------------------------------------------------
namespace Anandamide {
	
	class ANANDAMIDE_API Xml {
	
		struct Arg {
	
			Str name;
			Str data;
	
			Arg(const char *name_, const char *data_) {
				name = name_;
				data = data_;
			}
	
		};
	
		Str name;
		Str data;
		Str comment;
	
		Anandamide::Array <Xml> children;
		Anandamide::Array <Arg> args;
	
	protected:
	
		const char *parseXml(const char *str);
		void saveToFile(FILE *f, int depth = 0) const;
	
	public:
	
		Xml(const char *name);
		virtual ~Xml();
	
		const char *getName() const;
		void setName(const char *name_);
	
		//--------------------------------------------------------------------------
		// children
		//--------------------------------------------------------------------------
	
		int getChildrenCount() const;
	
		Xml *getChild(int i);
		Xml *getChild(const char *child);
	
		const Xml *getChild(int i) const;
		const Xml *getChild(const char *child) const;
	
		Xml *addChild(Xml *xml);
		Xml *addChild(const char *);
	
		void removeChild(int i);
		void removeChild(Xml *xml);
	
		//--------------------------------------------------------------------------
		// args
		//--------------------------------------------------------------------------
	
		int getArgsCount() const;
	
		const char *getArg(int i) const;
		void setArg(int i, const char *name, const char *data);
		bool isArg(const char *name) const;
	
		int findArg(const char *name) const;
	
		const char *getArg(const char *name) const;
	
		bool getArg(const char *name, int &res) const;
		void setArg(const char *name, int data);
	
		bool getArg(const char *name, bool &res) const;
		void setArg(const char *name, bool data);
	
		bool getArg(const char *name, float &res) const;
		void setArg(const char *name, float data);
	
		bool getArg(const char *name, Str &res) const;
		void setArg(const char *name, const char *data);
	
		//--------------------------------------------------------------------------
		// data
		//--------------------------------------------------------------------------
	
		const char *getData() const;
	
		void getData(int &res) const;
		void setData(int data);
	
		void getData(bool &res) const;
		void setData(bool data);
	
		void getData(float &res) const;
		void setData(float data);
	
		void getData(Str &res) const;
		void setData(const char *data_);
	
		//--------------------------------------------------------------------------
		// data
		//--------------------------------------------------------------------------
	
		bool getChildData(const char *child, int &res) const;
		void setChildData(const char *child, int data);
	
		bool getChildData(const char *child, bool &res) const;
		void setChildData(const char *child, bool data);
	
		bool getChildData(const char *child, float &res) const;
		void setChildData(const char *child, float data);
	
		bool getChildData(const char *child, Str &res) const;
		void setChildData(const char *child, const char *data_);
	
		//--------------------------------------------------------------------------
		// comment
		//--------------------------------------------------------------------------
	
		void setComment(const char *comment);
		const char *getComment() const;
	
		//--------------------------------------------------------------------------
		// save / load
		//--------------------------------------------------------------------------
	
		bool load(const char *name);
		void save(const char *name) const;
		void saveToStr(Str &s) const;
	
		//--------------------------------------------------------------------------
	
		Xml &operator= (const Xml &xml);
	
		//
		void clear();
		
		void parse(const char *xml);
	
	};

}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
