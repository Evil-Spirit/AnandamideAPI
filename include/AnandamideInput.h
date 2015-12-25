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

#ifndef ANANDAMIDEINPUT_H
#define ANANDAMIDEINPUT_H

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideVariable.h"

//------------------------------------------------------------------------------
namespace Anandamide {
	
	class Output;
	class Neurone;
	class TypeDef;
	
	///-------------------------------------------------------------------------
	///
	/// \class Input
	///
	/// \brief Класс входных параметров блоков
	///  
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API Input {
		
		friend class Neurone;
		
		Variable default_value;
		Output *source;
		Neurone *neurone;
		
		void setNeurone(Neurone *neurone);
		
		float pos_x;
		float pos_y;
		float dir_x;
		float dir_y;
		
	public:
		
		Input(const Variable &default_value_);
		
		///
		/// \brief Получение значения входного параметра
		/// \return значение переменной по умолчанию в случае, если не задан выходной параметр, иначе значение выходного параметра
		///
		const Variable &getValue() const;
		
		///
		/// \brief Установка значения по умолчанию, которое будет получено в случае, если данный входной параметр не связан с выходным параметром
		/// \param default_value_
		///
		void setValue(const Variable &default_value_);
		
		///
		/// \brief Получение переменной
		/// \return переменная
		///
		Variable &getVariable();
		
		///
		/// \brief Получение переменной-значения по умолчанию
		/// \return переменная-значение по умолчанию
		///
		Variable &getDefaultValue();
		
		///
		/// \brief Константная версия получения переменной-значения по умолчанию
		/// \return переменная-значение по умолчанию
		///
		const Variable &getDefaultValue() const;
		
		///
		/// \brief Получение источника (выходного параметра), с которым связан данный входной параметр
		/// \return выходной параметр, с которым связан данный входной параметр
		///
		Output *getSource();
	
		///
		/// \brief Константная версия получения источника (выходного параметра), с которым связан данный входной параметр
		/// \return выходной параметр, с которым связан данный входной параметр
		///
		const Output *getSource() const;
		
		///
		/// \brief setSource
		/// \param source_
		///
		void setSource(Output *source_);
		
		///
		/// \brief Получение имени данного входного параметра
		/// \return имя входного параметра
		///
		const char *getName() const;
		
		///
		/// \brief Получение блока, в котором содержится данный входной параметр
		/// \return указатель на блок, в которм содержится данный входной параметр
		///
		Neurone *getNeurone();
		
		///
		/// \brief Константная версия получения блока, в котором содержится данный входной параметр
		/// \return указатель на блок, в которм содержится данный входной параметр
		///
		const Neurone *getNeurone() const;
		
		///
		/// \brief Получение определения типа для переменной данного входного параметра
		/// \return определение типа
		///
		const TypeDef *getTypeDef() const;
		
		float getPosX() const;
		float getPosY() const;
		void setPos(float x, float y);
	
		float getDirX() const;
		float getDirY() const;
		void setDir(float x, float y);
		
	};
	
}

#endif // ANANDAMIDEINPUT_H
