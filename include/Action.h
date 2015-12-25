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

#ifndef ActionH
#define ActionH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Array.h"

//------------------------------------------------------------------------------
//
// namespace Common
//
//------------------------------------------------------------------------------

namespace Common {

	class Event;

	//--------------------------------------------------------------------------
	//
	// class Action
	//
	//--------------------------------------------------------------------------
	
	class ANANDAMIDE_API Action {

		public:
	
			virtual ~Action() { }
	
			virtual void *getInstance() = 0;
			virtual const void *getInstance() const = 0;
			virtual Action *clone() const = 0;
			virtual void run() = 0;
		
			template <class T>
			static Action *create(T *instance, void (T::*func)(void));
			template <class T>
			static Action *create(T &function);
	
	};

	//--------------------------------------------------------------------------
	//
	// class Action
	//
	//--------------------------------------------------------------------------

	template <class Instance>
	class ActionTemplate : public Action {
	
		typedef void (Instance::*ActionFunc)(void);
	
		Instance *instance;
		ActionFunc func;
	
	
	public:
	
		//
		ActionTemplate(Instance *instance_, ActionFunc func_) {
			instance = instance_;
			func = func_;
		}
	
		//
		virtual void run() {
			(instance->*func)();
		}
	
		//
		virtual void *getInstance() {
			return instance;
		}

		//
		virtual const void *getInstance() const {
			return instance;
		}
		
		//
		virtual Action *clone() const {
			return new ActionTemplate(instance, func);
		}
	
	};

	template <class T>
	class ActionFunction : public Action {

		T function;

	public:

		//
		ActionFunction(T &function) : function(function) {
		}

		//
		virtual void run() {
			function();
		}

		//
		virtual void *getInstance() {
			return nullptr;
		}

		//
		virtual const void *getInstance() const {
			return nullptr;
		}

		//
		virtual Action *clone() const {
			return new ActionFunction <T>(const_cast <T&>(function));
		}

	};

	//--------------------------------------------------------------------------
	
	template <class T>
	Action *Action::create(T *instance, void (T::*func)(void)) {
		return new ActionTemplate <T> (instance, func);
	}

	template <class T>
	Action *Action::create(T &function)
	{
		return new ActionFunction <T> (function);
	}

}

#endif

//------------------------------------------------------------------------------
