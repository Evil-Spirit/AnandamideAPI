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

#ifndef ANANDAMIDEEVENT_H
#define ANANDAMIDEEVENT_H

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Array.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	class Action;
	class Neurone;
	
	///-------------------------------------------------------------------------
	///
	/// \class Event
	///
	/// \brief Класс событий блоков
	/// 
	///--------------------------------------------------------------------------

	class ANANDAMIDE_API Event {
		
		friend class Neurone;
		
		Array <Action> actions;
		Neurone *neurone;
		
		void setNeurone(Neurone *neurone);

	public:

		Event();
		~Event();

		///
		/// \brief Запуск всех Action, с которыми связано событие
		///
		void run();

		///
		/// \brief Добавление связи с Action при генерации данного события
		/// \param action действие, которые вызывается при генерации данного события
		///
		void addAction(Action *action);
		
		///
		/// \brief Удаление действия по индексу
		/// \param index индекс удаляемого действия
		///
		void removeAction(int index);
		
		///
		/// \brief Удаление связи с Action при генерации данного события
		/// \param action указатель на действие
		///
		void removeAction(Action *action);
		
		///
		/// \brief Получение количества действий, с которыми связано данное событие
		/// \return 
		///
		int getActionsCount() const;

		
		///
		/// \brief Получение действия, с которым связано данное событие по индексу
		/// \param i индекс действия
		/// \return действие с индексом i
		///
		Action &getAction(int i);
		
		///
		/// \brief Константная версия получения действия, с которым связано данное событие по индексу
		/// \param i индекс действия
		/// \return действие с индексом i
		///
		const Action &getAction(int i) const;
		
		///
		/// \brief Получение блока, в котором содержится данное событие
		/// \return указатель на блок, в которм содержится данное событие
		///
		Neurone *getNeurone();
		
		///
		/// \brief Константная версия получения блока, в котором содержится данное событие
		/// \return указатель на блок, в которм содержится данное событие
		///
		const Neurone *getNeurone() const;
		
		///
		/// \brief Получнеие имени даного события
		/// \return имя события
		///
		const char *getName() const;

	};
	
}

#endif // ANANDAMIDEEVENT_H
