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

#ifndef ANANDAMIDEOUTPUT_H
#define ANANDAMIDEOUTPUT_H

//------------------------------------------------------------------------------

#include "Action.h"
#include "AnandamideLibAPI.h"
#include "AnandamideVariable.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	class Neurone;
	class TypeDef;
	
	///-------------------------------------------------------------------------
	///
	/// \class Output
	///
	/// \brief Класс выходных параметров блоков
	/// 
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API Output {

		Variable value;
		Neurone *instance;
		Common::Action *on_get_value;

	public:
		
		///
		/// \brief Конструктор для создания нового выходного параметра
		/// \param neurone блок, для которого создается выходной параметр
		///
		Output(Neurone *neurone);
		
		///
		/// \brief Конструктор для создания нового выходного параметра
		/// \param neurone блок, для которого создается выходной параметр
		/// \param value значение по умолчанию
		///
		Output(Neurone *neurone, const Variable &value);
		
		~Output();

		///
		/// \brief Получение значения выходного параметра
		/// \return переменная-значение выходного параметра
		///
		const Variable &getValue() const;
		
		///
		/// \brief Установить значение выходного параметра
		/// \param value новое значение выходного параметра
		///
		void setValue(const Variable &value);
		
		///
		/// \brief Получение переменной выходного параметра
		/// \return ссылка на переменную выходного параметра 
		///
		Variable &getVariable();

		///
		/// \brief Получение блока, к которому принадлежит текущий выходной параметр
		/// \return указатель на блок, к которому принадлежит текущий выходной параметр
		///
		Neurone *getInstance();
		
		///
		/// \brief Константная версия получения блока, к которому принадлежит текущий выходной параметр
		/// \return указатель на блок, к которому принадлежит текущий выходной параметр
		///
		const Neurone *getInstance() const;
		
		///
		/// \brief Установка функции, которая будет вызываться при получении значения параметра
		/// \param action функция, которая будет вызвана, когда запрашивается значение выходного параметра через getValue()
		///
		void setOnGetValueAction(Common::Action *action);
		
		///
		/// \brief Установка функции, которая будет вызываться при получении значения параметра
		/// \return функцию, которая вызвается, когда запрашивается значение выходного параметра через getValue()
		///
		Common::Action *getOnGetValueAction();
		
		///
		/// \brief Получение имени выходного параметра
		/// \return имя выходного параметра
		///
		const char *getName() const;
		
		///
		/// \brief Получение определения для типа выходнго параметра
		/// \return определение для типа выходного параметра
		///
		const TypeDef *getTypeDef() const;
	};
	
}

#endif // ANANDAMIDEOUTPUT_H
