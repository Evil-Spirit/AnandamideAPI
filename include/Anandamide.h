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

#ifndef AnandamideH
#define AnandamideH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"

///-----------------------------------------------------------------------------
///
/// \brief Пространство имен библиотеки AnandamideAPI
///
///-----------------------------------------------------------------------------

namespace Anandamide {
	
	///-------------------------------------------------------------------------
	///
	/// \class Messager 
	///
	/// \brief Класс для вывода предупреждений и сообщений об ошибках компиляции 
	/// и времени выполнения скриптов.
	/// \details Используется для вывода ошибок компиляции 
	/// и времени выполнения скрипта.
	/// <br>Для того, чтобы ошибки выводились с помощью пользовательских функций
	/// (например: qDebug(), printf, Log::message(), и т. д.), необходимо:
	/// 1. Наследовать класс Message
	/// 2. Перегрузить функции message, warning, error
	/// 3. Создать экземпляр наследника класса Message
	/// 4. Установить текущий Messager функцией setMessager
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Messager {
		public:
			virtual ~Messager();
			
			/// \brief Функция вывода информационного сообщения
			/// \param text выводимый текст
			virtual void message(const char *text);
			
			/// \brief Функция вывода предупреждающего сообщения
			/// \param text выводимый текст
			virtual void warning(const char *text);
			
			/// \brief Функция вывода сообщения об ошибке
			/// \param text выводимый текст
			virtual void error(const char *text);
	};
	
	///
	/// \brief вывод сообщения об ошибке, используя текущий Messager
	/// \param message выводимый текст
	/// 
	ANANDAMIDE_API void errorMessage(const char *message);
	
	/// \brief вывод предупреждающего сообщения, используя текущий  Messager
	/// \param message выводимый текст
	ANANDAMIDE_API void warningMessage(const char *message);
	
	/// \brief вывод информационного сообщения, используя текущий Messager
	/// \param message выводимый текст
	ANANDAMIDE_API void messageMessage(const char *message);
	
	/// \brief установка текущего Messager`а
	/// \param messager выводимый текст
	ANANDAMIDE_API void setMessager(Messager *messager);
	
	class Input;
	ANANDAMIDE_API void errorPointerMessage(Input* input);
}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
