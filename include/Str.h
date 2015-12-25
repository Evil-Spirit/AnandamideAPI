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

#ifndef StrH
#define StrH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Array.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//--------------------------------------------------------------------------
	
	class ANANDAMIDE_API Str {
	
		char *data;
		int fsize;
	
		void newSize(int size);
	
	public:
	
		Str();
		Str(int size);
		Str(const char *str);
		Str(const Str &str);
		~Str();
		
		void set(const char *str);
		void clear();
		void setSize(int size);
		int size() const;
	
		operator char *();
		operator const char *() const;
	
		const char *str() const;
	
		char &operator[](int i);
		char operator[](int i) const;
	
		Str &operator= (const char *str);
		Str &operator= (const Str &str);
		bool operator== (const char *str) const;
		bool operator== (const Str &str) const;
		bool operator!= (const char *str) const;
		bool operator!= (const Str &str) const;
		Str operator+ (const char *str) const;
		Str operator+ (char c) const;
		
		bool operator> (const Str &o) const;
		bool operator> (const char *str) const;
	
		const char *format(const char *fmt, ...);
	
		void del(int p, int n = 1);
		void ins(int ps, const char *s);
		void ins(int ps, char s);
	
		int getInt() const;
		float getFloat() const;
		int getDigits() const;
		Str getLetters() const;
		
		void split(char c, Array <Str> &strings) const;
		int find(const char *str) const;
		
		void toLowerCase();
		Str getLowerCase() const;
		
		void replace(const char *f, const char *r);
	
	};
	
	//--------------------------------------------------------------------------
	
	ANANDAMIDE_API const char *format(const char *fmt, ...);
	
	//--------------------------------------------------------------------------

	template class ANANDAMIDE_API Array <Str>;
	
	//--------------------------------------------------------------------------

}
//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
