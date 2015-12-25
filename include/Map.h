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

#ifndef MapH
#define MapH

//------------------------------------------------------------------------------

#include "Array.h"
#include "Assert.h"
#include "qDebug.h"

namespace Anandamide {
	
	//------------------------------------------------------------------------------
	//
	// class Map
	//
	//------------------------------------------------------------------------------
	
	template <class Key, class Value>
	class ANANDAMIDE_API Map {
		
		class Item {
			
			friend class Map <Key, Value>;
			
			Key *key;
			Value *value;
			int index;
			
		public:
		
			
			//
			Item(int i, Key *k = NULL, Value *v = NULL) {
				index = i;
				key = k;
				value = v;
			}
			
			//
			virtual ~Item() {
				delete key;
			}
			
			//
			Key &getKey() { return *key; }
			Value &getValue() { return *value; }
			
			const Key &getKey() const { return *key; }
			const Value &getValue() const { return *value; }
			
			bool operator == (const Item &o) const { return *key == *o.key; }
			bool operator == (const Key &o) const { return *key == o; }
		
		};
		
		Array <Item> items;
		Array <Item> index_items;
		bool own;
		
	public:
		
		//
		Map() : own(true), index_items(false) { }
		
		//
		Map(bool own_) : own(own_), index_items(false) { }
		
		//
		~Map() {
			if(own) {
				for(int i = 0; i < items.count(); ++i) {
					//qDebug() << items[i].getKey() << typeid(items[i].getKey()).raw_name() << /*items[i].getValue() <<*/ typeid(items[i].getValue()).raw_name();
					delete items[i].value;
				}
			}
		
		}
		
		//
		bool isOwn() {
			return own;
		}
		
		//
		Value *get(const Key &key) {
			int i = indexOf(key);
			if(i < 0) return NULL;
			return items[i].value;
		}
		
		const Value *get(const Key &key) const {
			int i = indexOf(key);
			if(i < 0) return NULL;
			return items[i].value;
		}
		
		Value get(const Key &key, const Value &def) const {
			Value *v = get(key);
			if(v != NULL) return *v;
			return def;
		}
		
		
		// ! здесь нужно заменять имеющееся значение.
		Item *append(Key *key, Value *value) {
			
			int i = indexOf(*key);
			
			if (i >= 0) {
				if(own) delete items[i].value;
				items[i].value = value;
				return &items[i];
			}
			
			int first = 0;
			int last = items.count();
			
			// We know that we must insert within the closed interval [first,last]
			while(first != last) {
				int mid = (first + last) / 2;
				const Key &k = *items[mid].key;
				if(k > *key) {
					last = mid;
				} else {
					first = mid + 1;
				}
			}
			
			Item *item = new Item(index_items.count(), key, value);
			index_items.append(item);
			items.insertAt(item, first);
			return item;
		}
		
	
		int count() const { return index_items.count(); }
	
		const Item &operator[](int index) const { return index_items[index]; }
		Item &operator[](int index) { return index_items[index]; }
	
		void remove(int index) {
			int i = indexOf(*index_items[index].key);
			if(i < 0) return;
			if(own) delete items[i].value;
			items.remove(i);
			index_items.remove(index);
			for(int j = index; j < index_items.count(); ++j) {
				index_items[j].index = j;
			}
		}
	
		void remove(const Key &key) {
			int i = indexOf(key);
			if(i < 0) return;
			if(own) delete items[i].value;
			index_items.remove(items[i].index);
			for(int j = items[i].index; j < index_items.count(); ++j) {
				index_items[j].index = j;
			}
			items.remove(i);
		}
		
		bool rename(const Key &key, const Key &new_key) {
			if(get(new_key) != NULL) return false;
			
			int i = indexOf(key);
			if(i < 0) return false;
			
			// store data
			Value *value = items[i].value;
			int old_index = items[i].index;
			
			// remove old item
			items.remove(i);
			
			// append new item
			Item *new_item = append(new Key(new_key), value);
			index_items.remove(index_items.count() - 1);
			
			// reindex
			index_items.remove(old_index);
			index_items.insertAt(new_item, old_index);
			new_item->index = old_index;
			
			return true;
		}
	
		void clear() {
			items.clear();
			index_items.clear();
		}
	
		int seek(const Value *value) const {
			for (int i=0; i<index_items.count(); i++) {
				if (&index_items[i].getValue() == value) {
					return i;
				}
			}
			return -1;
		}
		
		const Key *findValue(const Value &value) const {
			for (int i=0; i<items.count(); i++) {
				if (items[i].getValue() == value) {
					return &items[i].getKey();
				}
			}
			return NULL;
		}
		
		int indexOf(const Key &key) const {
			int first = 0;
			int last = items.count() - 1;
			
			while(first <= last) {
				int mid = (first + last) / 2;
				const Key &k = *items[mid].key;
				if(k == key) return mid;
				
				if(k > key) {
					last = mid - 1; // and first stays the same
				} else {
					first = mid + 1; // and last stays the same
				}
			}
			return -1;
		}
		
		bool isOwner() {
			return own;
		}
		
	
	};

}
//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
