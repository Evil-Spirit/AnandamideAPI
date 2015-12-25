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

#ifndef ANANDAMIDEACTION_H
#define ANANDAMIDEACTION_H

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Array.h"
#include "Action.h"
#include "AnandamideEvent.h"
#include "MathCore.h"

//------------------------------------------------------------------------------

namespace Anandamide {

	class Event;
	class Neurone;
	class Neurone;
	class LogicEditor;

	///-------------------------------------------------------------------------
	///
	/// \class Action
	///
	/// \brief Класс действий блоков
	/// 
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API Action {

	private:
		
		friend class Event;
		friend class Neurone;
		friend class LogicEditor;

		mutable Array <Event> events;
		Common::Action *action;
		Neurone *neurone;

		void addEvent(Event *event);
		void removeEvent(Event *event);
		
		void setNeurone(Neurone *neurone);

	public:

		Action();
		~Action();
		
		///
		/// \brief Получение указателя на блок, к которому принадлежит данное действие
		/// \return указатель на блок, к которому принадлежит данное событие
		///
		Neurone *getNeurone();
		
		///
		/// \brief Константная версия получения указателя на блок, к которому принадлежит данное действие
		/// \return указатель на блок, к которому принадлежит данное событие
		///
		const Neurone *getNeurone() const;
		
		///
		/// \brief run Запуск действия
		/// \param event событие, запустившее действие
		///
		void run();
		
		///
		/// \brief Установка функции, которая вызывается при вызове данного действия
		/// \param action функция
		///
		void setAction(Common::Action *action);
		
		///
		/// \brief Получение имени данного действия
		/// \return имя действия
		///
		const char *getName() const;

	};
	
}

#endif // ANANDAMIDEACTION_H
