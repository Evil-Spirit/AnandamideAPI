//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// author:		Georgiy Kostarev
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

#ifndef ANANDAMIDEUILIB_H
#define ANANDAMIDEUILIB_H

#include "AnandamideLibrary.h"

class QObject;
struct QMetaObject;

namespace Anandamide {


	///------------------------------------------------------------------
	///
	/// @class ObjectLibrary
	/// @brief Класс библиотеки Qt объекта
	///
	///------------------------------------------------------------------


	class ANANDAMIDE_API QObjectLibrary : public Library
	{

			Str filename;
			QObject *object;
			QObjectLibrary(QObject *object, const Str &filename);

        public:

			~QObjectLibrary() override;

			///
			/// \brief Функция для создания библиотеки из .ui файла
			/// \param filename имя файла
			/// \return библиотеку объекта
			///
			static QObjectLibrary *create(const char *filename);

			///
			/// \brief Функция для создания библиотеки из экземпляра объекта
			/// \param object объект
			/// \return библиотеку объекта
			///
			static QObjectLibrary *create(QObject *object);

			///
			/// \brief Функция для автоматического экспорта объекта в любую библиотеку
			/// \param library библиотека
			/// \param object объект
			///
			static void parseDefs(Library *library, QObject *object);

			///
			/// \brief Функция для автоматического экспорта методов и свойств мета-объекта в любую библиотеку
			/// \param library библиотека
			/// \param metaobject мета-объект
			///
			static void parseDefs(Library *library, const QMetaObject *metaobject);

			void setFileName(const char *filename) override;
			const char *getFileName() const override;			
			const char *getLibFileName() const override;
    };

	typedef QObjectLibrary Ui;

	///
	/// @macro Макрос, для определения типов, экспортируемых при помощи механизма QObjectLibrary
	/// @note используется вместо DEFINE_TYPEID
	///
	#define DEFINE_TYPEID_OBJECT(TYPE, INDEX) \
			DEFINE_TYPEID(TYPE, INDEX) \
			Q_DECLARE_METATYPE(TYPE) \
			namespace Anandamide { \
				template <> \
				class VariableToType <TYPE> { \
				public: \
					TYPE value; \
					VariableToType(const Variable &v) { \
						auto value_ = QVariant(QMetaType::type(v.getType().name()), v.getDataPtr()); \
						if (!value_.isValid() || !value_.canConvert(qMetaTypeId <TYPE>())) { \
							warningMessage(format("Undefined conversion between <%s> and <%s> types. Check DEFINE_TYPEID_OBJECT macro.", v.getType().name(), #TYPE)); \
							return; \
						} \
						value_.convert(qMetaTypeId <TYPE>()); \
						value = value_.value <TYPE>(); \
					} \
				}; \
				namespace __DEFS { \
					struct __DEFINE_TYPE_##INDEX { inline __DEFINE_TYPE_##INDEX() { qRegisterMetaType <TYPE>(); } }; \
					static __DEFINE_TYPE_##INDEX __TYPE_DEFINITION_##INDEX; \
				} \
			}
}

#endif // ANANDAMIDEUILIB_H
