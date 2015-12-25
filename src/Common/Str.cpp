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

#include "Str.h"

//------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//------------------------------------------------------------------------------
	
	const char *format(const char *fmt, ...) {
		static char	text[4096];
		va_list	ap;
		va_start(ap, fmt);
		vsnprintf_s(text, 4096, fmt, ap);
		va_end(ap);
		return text;
	}
	
	//------------------------------------------------------------------------------
	
	void Str::newSize(int size) {
		static const char *empty = "\0";
		if(fsize != 0) delete [] data;
		fsize = size;
		if(size == 0) {
			data = const_cast <char *> (empty);
			return;
		}
		data = new char[size + 1];
		data[size] = 0;
	}
	
	Str::Str() : fsize(0) { newSize(0); }
	
	Str::Str(int size) : data(NULL) { newSize(size); }
	
	Str::Str(const char *str) : data(NULL) { set(str); }
	
	Str::Str(const Str &str) : data(NULL) { set(str.data); }
	
	Str::~Str() { clear(); }
	
	void Str::set(const char *str) {
		if (data == str) return;
		int new_size = 0;
		if (str != NULL) new_size = strlen(str);
		newSize(new_size);
		if(new_size != 0) strcpy(data, str);
	}
	
	void Str::setSize(int size) {
		if(fsize == 0) {
			newSize(size);
			return;
		}
		fsize = size;
		char *d = data;
		data = new char[size + 1];
		strncpy(data, d, size);
		data[size] = 0;
		delete [] d;
		
	}
	
	void Str::clear() {
		newSize(0);
	}
	
	int Str::size() const { return fsize; }
	
	const char *Str::str() const { return data; }
	
	char Str::operator[](int i) const { return data[i]; }
	
	Str &Str::operator=(const Str &str) { set(str.data); return *this; }
	
	bool Str::operator==(const Str &str) const { return fsize == str.fsize && !strcmp(data, str.data); }
	
	bool Str::operator!=(const Str &str) const { return !operator== (str); }
	
	Str Str::operator+(char c) const {
		Str res(size() + 1);
		for (int i=0; i<size(); i++) {
			res[i] = data[i];
		}
		res[res.size() - 1] = c;
		res[res.size()] = '\0';
		return res;
	}
	
	bool Str::operator>(const char *str) const {
		return strcmp(data, str) > 0;
	}
	
	bool Str::operator>(const Str &o) const {
		return strcmp(data, o.data) > 0;
	}
	
	const char *Str::format(const char *fmt, ...) {
		static char	text[1024] = "\0";
		va_list	ap;
		va_start(ap, fmt);
		vsprintf_s(text, 1024, fmt, ap);
		va_end(ap);
		set(text);
		return data;
	}
	
	void Str::del(int p, int n) {
		if (fsize <= 0 || n < 1 || n > fsize) return;
		if (p + n < fsize) {
			strcpy(&data[p], &data[p + n]);
			setSize(fsize - n);
		} else {
			setSize(p);
			data[fsize] = '\0';
		}
	}
	
	void Str::ins(int ps, const char *s) {
		if (ps > fsize) return;
		
		int len = strlen(s);
		setSize(fsize + len);
		
		for (int i=fsize-len; i>=ps; i--) {
			data[i + len] = data[i];
		}
		strncpy(&data[ps], s, len);
	}
	
	void Str::ins(int ps, char s) {
		if (ps > fsize) return;
		
		setSize(fsize + 1);
		
		for (int i=fsize-1; i>=ps; i--) {
			data[i + 1] = data[i];
		}
		data[ps] = s;
	}
	
	int Str::getInt() const {
		return atoi(data);
	}
	
	float Str::getFloat() const {
		return float(atof(data));
	}
	
	int Str::getDigits() const {
		Str temp;
		for (int i=0; i<fsize; i++) {
			if (data[i] >= '0' && data[i] <= '9') {
				temp = temp + data[i];
			}
		}
		return temp.getInt();
	}
	
	Str Str::getLetters() const {
		Str temp;
		for (int i=0; i<fsize; i++) {
			if (data[i] < '0' || data[i] > '9') {
				temp = temp + data[i];
			}
		}
		return temp;
	}
	
	void Str::split(char c, Anandamide::Array<Str> &strings) const {
		Str cur_str;
		for(int i=0; i<fsize; i++) {
			if(data[i] == c) {
				strings.append(new Str(cur_str));
				cur_str = "";
				continue;
			}
			cur_str = cur_str + data[i];
		}
	}
	
	int Str::find(const char *str) const {
		const char *found = strstr(data, str);
		if (found) {
			return int(found - data);
		}
		return -1;
	}
	
	void Str::toLowerCase() {
		if (data == NULL) return;
		strlwr(data);
	}
	
	Str Str::getLowerCase() const {
		Str res = *this;
		res.toLowerCase();
		return res;
	}
	
	void Str::replace(const char *f, const char *r) {
		size_t fs = strlen(f);
		while(true) {
			int i = find(f);
			if(i == -1) break;
			del(i, fs);
			ins(i, r);
		}
	}
	
	Str Str::operator+(const char *str) const {
		Str res(size() + strlen(str));
		for (int i=0; i<size(); i++) {
			res[i] = data[i];
		}
		for (int i=size(); i<res.size(); i++) {
			res[i] = str[i - size()];
		}
		res[res.size()] = '\0';
		return res;
	}
	
	bool Str::operator!=(const char *str) const { return !operator== (str); }
	
	bool Str::operator==(const char *str) const { return fsize == (int)strlen(str) && !strcmp(data, str); }
	
	Str &Str::operator=(const char *str) { set(str); return *this; }
	
	char &Str::operator[](int i) { return data[i]; }
	
	Str::operator const char *() const { return data; }
	
	Str::operator char *() { return data; }
}
