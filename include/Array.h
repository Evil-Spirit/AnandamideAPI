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

#ifndef ArrayH
#define ArrayH

//------------------------------------------------------------------------------

//#include "MathCore.h"
#include "AnandamideLibAPI.h"

//------------------------------------------------------------------------------

#include <stdio.h>
#include <typeinfo>

//------------------------------------------------------------------------------

#define CHECK_RANGE(index)	{											\
								static char error[0x200];				\
								if (index >= size || index < 0) {		\
									sprintf(error, "Array <%s> : index out of bounds (index = %d, count = %d)", typeid(Class).name(), index, size); \
									throw (const char *)error;			\
								}										\
							}
namespace Anandamide {
	
	template <class Class>
	class ANANDAMIDE_API Array {
	
		int size;
		int capacity;
		bool own;
	
	protected:
		
		Class **items;
	
		void setCapacity(int newCapacity, bool own) {
			if (own) {
				while (size > newCapacity) {
					delete items[--size];
				}
			} else {
				if (size > newCapacity) size = newCapacity;
			}
	
			capacity = newCapacity;
			Class **newItems = new Class *[capacity];
			for (int i=0; i<capacity; i++) {
				newItems[i] = 0;
			}
			for (int i=0; i<size; i++) {
				newItems[i] = items[i];
			}
			delete []items;
			items = newItems;
		}
	
	public:
	
		Array() : size(0), capacity(0), own(true), items(0) {}
		Array(bool o) : size(0), capacity(0), own(o), items(0) {}
	
		Array(int n, bool o = true) : size(0), capacity(0), items(0), own(o) {
			setCapacity(n, own);
		}
	
		virtual ~Array() {
			clear();
		}
	
		int count() const { return size; }
		
		/*
		void setCount(int nC) {
	
			while (size < nC)
				append(new Class());
	
			while (size > nC)
				remove(size - 1);
		}
		*/
		
		virtual void clear() {
			if (size <= 0) return;
	
			setCapacity(0, own);
	
			delete []items;
			capacity = 0;
			size = 0;
			items = 0;
		}
		
		virtual void destroy() {
			if (size <= 0) return;
	
			setCapacity(0, true);
	
			delete []items;
			capacity = 0;
			size = 0;
			items = 0;
		}
		
		Class &operator[](int index) {
			CHECK_RANGE(index);
			return *items[index];
		}
	
		const Class &operator[](int index) const {
			CHECK_RANGE(index);
			return *items[index];
		}
	
		Class &last() {
			CHECK_RANGE(size - 1);
			return *items[size - 1];
		}
	
		const Class &last() const  {
			CHECK_RANGE(size - 1);
			return *items[size - 1];
		}
	
		Class &append(Class *e) {
	
			if (size + 1 > capacity) {
				setCapacity(size + 51, own);
			}
			items[size] = e;
			size ++;
			return *items[size - 1];
		}
	
		int seek(const Class *e) const {
			for (int i=0; i<size; i++) {
				if (items[i] == e) {
					return i;
				}
			}
			return -1;
		}
	
		int seek(const Class &e) const {
			for (int i=0; i<size; i++) {
				if (*items[i] == e) {
					return i;
				}
			}
			return -1;
		}
	
		void setItem(int index, Class *e) {
			if (own) {
				delete items[index];
			}
			items[index] = e;
		}
	
		Class *getItem(int index) {
			return items[index];
		}
	
		const Class *getItem(int index) const {
			return items[index];
		}
		
		void insert(Class *e, int after) {
			CHECK_RANGE(after);
			append(e);
	
			for (int i=size-1; i>after; i--) {
				items[i + 1] = items[i];
			}
			items[after + 1] = e;
		}
		
		void insertAt(Class *e, int at) {
			append(e);
			if(at == size - 1) return;
			for (int i = size - 1; i>at; i--) {
				items[i] = items[i - 1];
			}
			items[at] = e;
		}
		
	
		virtual void remove(int index) {
			CHECK_RANGE(size - 1);
	
			if (own) delete items[index];
	
			for (int i=index; i<size-1; i++) {
				items[i] = items[i + 1];
			}
			size --;
			if (capacity - size > 100) {
				setCapacity(size + 50, own);
			}
		}
	
		void remove(const Class *e) {
			int index = seek(e);
			if (index != -1) {
				remove(index);
			}
		}
	
		bool is(int index) const {
			return items[index] != NULL;
		}
	
	/*	Array <Class> &operator= (const Array <Class> &o) {
			clear();
			own = o.own;
			if (own) {
			} else {
				for (int i=0; i<o.count(); i++) {
					append(o.items[i]);
				}
			}
			return *this;
		}
	*/
		void exchange(int i, int j) {
			Class *temp = items[i];
			items[i] = items[j];
			items[j] = temp;
		}
	
		bool isOwner() const {
			return own;
		}
	};
	
	//------------------------------------------------------------------------------
}

#endif

//------------------------------------------------------------------------------
