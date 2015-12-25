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

#ifndef ResourceH
#define ResourceH

//------------------------------------------------------------------------------

#include "Str.h"
#include "Map.h"
#include "Xml.h"

//------------------------------------------------------------------------------
//
// namespace Nutmeg
//
//------------------------------------------------------------------------------

namespace Anandamide {

	//--------------------------------------------------------------------------

	class ResourceItemBase;
	class ResourceManBase;
	template <class T> class ResourceMan;

	//--------------------------------------------------------------------------
	//
	// class ResourceItemBase
	//
	//--------------------------------------------------------------------------

	class ResourceItemBase {

	public:

		Str name;
		Str file_name;

		int ref_count;
		int time_stamp;
		int file_size;
		float loading_time;

		ResourceItemBase() {
			ref_count = 0;
			time_stamp = 0;
			file_size = 0;
			loading_time = 0.0f;
		}

		virtual ~ResourceItemBase() { }

	};

	//--------------------------------------------------------------------------
	//
	// class ResourceItem
	//
	//--------------------------------------------------------------------------

	template <class T>
	class ResourceItem : public ResourceItemBase {

	public:

		T *data;

		ResourceItem(T *data_) {
			data = data_;
		}

		virtual ~ResourceItem() {
			delete data;
		}

	};

	//--------------------------------------------------------------------------
	//
	// class Resource
	//
	//--------------------------------------------------------------------------

	template <class T>
	class Resource {

		friend class ResourceMan <T>;

		static ResourceMan <T> &manager() {
			static ResourceMan <T> value(new T());
			return value;
		}
		
		ResourceItem <T> *resource_item;

		Resource(ResourceItem <T> *resource_item_) : resource_item(NULL) {
			resource_item = resource_item_;
			assert(resource_item != NULL);
			resource_item->ref_count ++;
		}

	public:

		Resource() : resource_item(NULL) {
			resource_item = manager().getDefaultResourceItem();
			resource_item->ref_count ++;
		}

		//
		Resource(const Resource &o) {
			resource_item = o.resource_item;
			resource_item->ref_count ++;
		}

		//
		Resource(const char *name) {
			resource_item = NULL;
			*this = manager().load(name);
		}

		//
		~Resource() {
			if (resource_item == NULL) return;
			resource_item->ref_count --;
			manager().freeUnused();
		}

		bool is() const {
			return resource_item != NULL && resource_item->data != NULL;
		}

		bool isDefault() const {
			return resource_item->data == manager().getDefaultResource();
		}

		void load(const char *name) {
			*this = manager().load(name);
		}

		Resource <T> &operator=(const Resource <T> &o) {
			if (&o == this) return *this;
			if (resource_item != NULL) resource_item->ref_count --;
			resource_item = o.resource_item;
			resource_item->ref_count ++;
			manager().freeUnused();
			return *this;
		}

		//
		const T *operator->() const {
			if (resource_item == NULL || resource_item->data == NULL) {
				return manager().getDefaultResourceItem()->data;
			}
			return resource_item->data;
		}

		//
		const T &get() const {
			if (resource_item == NULL || resource_item->data == NULL) {
				return *manager().getDefaultResourceItem()->data;
			}
			return *resource_item->data;
		}

		//
		operator const T &() const {
			if (resource_item == NULL) {
				ResourceItem <T> *&item = const_cast <ResourceItem <T> *&> (resource_item);
				item = manager().getDefaultResourceItem();
				item->ref_count ++;
			}

			if (resource_item->data == NULL) {
				return *manager().getDefaultResourceItem()->data;
			}

			return *resource_item->data;
		};

		bool operator==(const Resource <T> &o) const {
			return resource_item == o.resource_item;
		}

		//
		const char *getName() const {
			if (resource_item == NULL) {
				ResourceItem <T> *&item = const_cast <ResourceItem <T> *&> (resource_item);
				item = manager().getDefaultResourceItem();
				item->ref_count ++;
			}
			return resource_item->name;
		}

		//
		void writeXml(const char *name, Xml *xml) const {
			if (resource_item == NULL) {
				ResourceItem <T> *&item = const_cast <ResourceItem <T> *&> (resource_item);
				item = manager().getDefaultResourceItem();
				item->ref_count ++;
			}
			xml->setChildData(name, resource_item->file_name);
		}

		void readXml(const char *name, const Xml *xml) {
			Str file;
			xml->getChildData(name, file);
			load(file);
		}

		static ResourceMan <T> &getManager() {
			return manager();
		}

		void setDefault() {
			*this = manager().load(manager().getDefaultResourceItem()->name);
		}

		int getRefsCount() const {
			if (resource_item != NULL) {
				return resource_item->ref_count;
			} else {
				return 0;
			}
		}

	};

	//--------------------------------------------------------------------------
	//
	// class ResourceMan
	//
	//--------------------------------------------------------------------------

	template <class T>
	class ResourceMan {

		Map <Str, ResourceItem <T> > items;
		T *default_resource;
		ResourceItem <T> *default_item;

		ResourceItem <T> *getItemByName(const char *id) {
			return items.get(id);
		}

		bool freeing;
		bool free_unused;

	public:

		//
		ResourceMan(T *def_res) {

			default_resource = def_res;
			assert(default_resource != NULL);

			default_item = new ResourceItem <T> (default_resource);
			default_item->name = "default";
			items.append(new Str(default_item->name), default_item);

			freeing = false;
			free_unused = true;
		}

		Resource <T> load(const char *name) {

			ResourceItem <T> *item = getItemByName(name);
			if (item == NULL) {
				T *data = T::loadResource(name);
				if(data == NULL) return default_item;
				item = new ResourceItem <T> (data);
				item->name = name;
				items.append(new Str(name), item);
			}

			return Resource <T> (item);
		}

		Resource <T> create(const char *name, T *resource) {
			ResourceItem <T> *item = items.get(name);

			if (item == NULL) {
				item = new ResourceItem <T> (resource);
				item->file_name = "";
				item->name = name;
				item->data = resource;
				items.append(new Str(item->name), item);
			}

			return Resource <T> (item);
		}

		int getItemsCount() const {
			return items.count();
		}

		const ResourceItem <T> &getItem(int index) const {
			return items[index].getValue();
		}

		const ResourceItem <T> *getItemByName(const char *id) const {
			return items.get(id);
		}

		T *getDefaultResource() {
			return default_resource;
		}

		ResourceItem <T> *getDefaultResourceItem() {
			return default_item;
		}

		void freeUnused() {
			if (free_unused == false) return;
			if (freeing == true) return;

			bool found = true;

			freeing = true;
			while (found) {
				found = false;
				for (int i=1; i<items.count(); i++) {
					if (items[i].getValue().ref_count == 0) {
						items.remove(i--);
						found = true;
						break;
					}
				}
			}
			freeing = false;

		}

	 	const ResourceItem <T> &getItem(int i) {
			return items[i].getValue();
		}

		bool isRemoveUnused() {
			return free_unused;
		}

		void setRemoveUnused(bool state) {
			free_unused = state;
			if (free_unused) freeUnused();
		}

	};

}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
