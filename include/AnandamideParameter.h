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

#ifndef ANANDAMIDEPARAMETER_H
#define ANANDAMIDEPARAMETER_H

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideVariable.h"

//------------------------------------------------------------------------------

namespace Anandamide {

	///-------------------------------------------------------------------------
	///
	/// \class Parameter
	///
	/// \brief Класс параметров блоков
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Parameter {
	
		Variable value;
	
	public:
	
		///
		/// \brief Конструктор класса параметра
		/// \param value значение параметра
		///
		Parameter(const Variable &value);
		
		///
		/// \brief Получение значения параметра
		/// \return переменную-значение параметра
		///
		const Variable &getValue() const;
		
		///
		/// \brief Установка значения параметра
		/// \param новое значение параметра
		///
		void setValue(const Variable &value);
		
		///
		/// \brief Получение ссылки на переменную, в которой хранится значение параметра
		/// \return ссылку на переменную, в которой хранится значение параметра
		///
		Variable &getVariable();
		
	};
	
}

#endif // ANANDAMIDEPARAMETER_H
