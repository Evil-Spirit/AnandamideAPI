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

/// \file AnandamideVariable.h
/// 
#ifndef AnandamideVariableH
#define AnandamideVariableH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Str.h"
#include "AnandamideTypeInfo.h"
#include "Action.h"

//------------------------------------------------------------------------------

#define varDebugOut(...)
//#define varDebugOut(...) qDebug(__VA_ARGS__)

/// \namespace Anandamide
namespace Anandamide {
	
    //--------------------------------------------------------------------------

    template <typename T> class TypeToVariable;
    template <typename T> class VariableToType;

	///-------------------------------------------------------------------------
	///
	/// \class Variable
	///
	/// \brief Класс переменной (variant)
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Variable {
		
		TypeInfo type;
		
		int i;
		double f;
		Str s;
		TypeContainer *unify;
		
		Common::Action *on_change;
		
		void clear();
		void reset();
		void notifyChanged();
		
	public:
		
		template <typename T>
		explicit Variable(const T &value) {
			clear();
			TypeToVariable <T> (value, *this);
		}
		
		///
		/// \brief Конструктор по умолчанию. Инициализирует переменную значением int(0)
		///
		Variable();
		
		///
		/// \brief Конструктор переменной типа int
		/// \param value значение переменной
		///
		explicit Variable(int value);
		
		///
		/// \brief Конструктор переменной типа bool
		/// \param value значение переменной
		///
		explicit Variable(bool value);
		
		///
		/// \brief Конструктор переменной типа float
		/// \param value значение переменной
		///
		explicit Variable(float value);
		
		///
		/// \brief Конструктор переменной типа double
		/// \param value значение переменной
		///
		explicit Variable(double value);
		
		///
		/// \brief Конструктор переменной типа string
		/// \param value значение переменной
		///
		explicit Variable(const char *value);
		
		///
		/// \brief Конструктор переменной типа string
		/// \param value значение переменной
		///
		explicit Variable(const Str &value);

		///
		/// \brief Конструктор копирования
		/// \param o другая переменная
		///
		Variable(const Variable &o);
		
		///
		/// \brief operator =
		/// \param o другая переменнная
		/// \return ссылку на теущий объект
		///
		Variable &operator=(const Variable &o);
		
		//
		~Variable();
		
		//----------------------------------------------------------------------
		// variable
		//----------------------------------------------------------------------
		
		const Variable &get() const;
		
		//
		void set(const Variable &v);
		
		//----------------------------------------------------------------------
		// bool
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение значения переменной в виде bool. 
		/// \details Если в переменной содержится значение другого типа, произойдет автоматическое преобразование в требуемый тип
		/// \return значение требуемого типа
		///
		bool getBool() const;
		
		///
		/// \brief Установка значения переменной типа int
		/// \param value новое значение переменной
		///
		void setBool(bool value);
		
		//----------------------------------------------------------------------
		// int
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение значения переменной в виде int. 
		/// \details Если в переменной содержится значение другого типа, произойдет автоматическое преобразование в требуемый тип
		/// \return значение требуемого типа
		///
		int getInt() const;
		
		///
		/// \brief Установка значения переменной типа int
		/// \param value новое значение переменной
		///
		void setInt(int value);
		
		//----------------------------------------------------------------------
		// float
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение значения переменной в виде float. 
		/// \details Если в переменной содержится значение другого типа, произойдет автоматическое преобразование в требуемый тип
		/// \return значение требуемого типа
		///
		float getFloat() const;
		
		///
		/// \brief Установка значения переменной типа float
		/// \param value новое значение переменной
		///
		void setFloat(float value);
		
		//----------------------------------------------------------------------
		// double
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение значения переменной в виде double. 
		/// \details Если в переменной содержится значение другого типа, произойдет автоматическое преобразование в требуемый тип
		/// \return значение требуемого типа
		///
		double getDouble() const;
		
		///
		/// \brief Установка значения переменной типа double
		/// \param value новое значение переменной
		///
		void setDouble(double value);
		
		//----------------------------------------------------------------------
		// string
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение значения переменной в виде строки. 
		/// \details Если в переменной содержится значение другого типа, произойдет автоматическое преобразование в требуемый тип.<br>
		/// Для преобразования пользовательского типа Type будет использована функция <br>
		/// void operator>>(const Type &t, Str &str) или Type::opertarator>>(Str &str).
		/// Если такой оператор не объявлен для типа, результатом будет строка "{0}"
		/// \return значение переменной в строковом представлении
		///
		const char *getStr() const;
		
		///
		/// \brief Установка значения переменной типа строка
		/// \param value новое значение переменной
		///
		void setStr(const char *value);
		
		///
		/// \brief Конвертация в значение переменной из строки. 
		/// \details Результат операции зависит от фактического типа текущей переменной и не изменяет ее тип.
		/// Если тип переменной - пользовательский Type, то для преобразования будет использована функция<br>
		/// void operator<<(Type &t, const Str &str) или Type::operator<<(const Str &str)<br>
		/// В случае, если такой оператор не объявлен, значение переменной не изменится.
		/// \param value новое значение переменной в строковм представлении
		///
		void setFromStr(const char *value);

		//----------------------------------------------------------------------
		// data ptr
		//----------------------------------------------------------------------

		///
		/// \brief getDataPtr
		/// \return
		///
		void *getDataPtr() const;
		
		//----------------------------------------------------------------------
		// enum
		//----------------------------------------------------------------------
		
		///
		/// \brief Установка значения переменной перечисляемого типа
		/// \param EnumType перечисляемый тип 
		/// \param value значение перечисляемого типа
		///
		template <typename EnumType>
		void setEnum(EnumType value) {
			reset();
			type = TYPEID(EnumType);
			i = int(value);
		}
		
		///
		/// \brief Установка значения переменной перечисляемого типа
		/// \param EnumType перечисляемый тип
		/// \param value значение перечисляемого типа
		///
		void setEnum(TypeInfo ti, int value);
		
		///
		/// \brief Установка значения переменной перечисляемого типа
		/// \param EnumType перечисляемый тип
		/// \param value значение перечисляемого типа
		///
		void setEnum(int value);
		
		///
		/// \brief Проверка, является тип переменной перечисляемым типом
		/// \return true, если тип переменной является перечисляемым
		///
		bool isEnum() const;
		
		//----------------------------------------------------------------------
		// unify
		//----------------------------------------------------------------------
		
		///
		/// \brief Установка значения переменной пользовательского типа
		///	\param Type пользовательский тип
		/// \param value значение
		///
		template <typename Type>
		void setUnify(Type &value) {
			reset();
			type = TYPEID(Type);
			unify = new TypeContainerTemplate <Type> (value);
		}

		///
		/// \brief Установка значения переменной пользовательского типа
		///	\param type_info тип значения
		/// \param unify контейнер
		///

		void setUnify(const TypeInfo &type_info, TypeContainer *unify);

		///
		/// \brief Установка значения переменной произвольного типа
		/// \param value указатель на значение
		/// \param type_info тип значения
		/// \param deallocator объект, освобождающий память в случае уничтожения контейнера типа
		///
		void setUnify(void *value, const TypeInfo &type_info, Deleter *deleter);
		
		template <typename T>
		class CheckUnify {
			public:
				bool check(const Variable *v) {
					
//					if(TYPEID(T) != v->type) {
//						errorMessage(format("Type mismatch while attempting to get \"%s\" from variable of type \"%s\".", TYPEID(T).name(), v->type.name()));
//						return false;
//					}
					
					if(v->unify == NULL || v->unify->getValue() == NULL) {
						errorMessage(format("Value is not assigned to variable of type \"%s\".", v->type.name()));
						return false;
					}
					
					T *value = NULL;
					try {
						// There is no way to do shit like this :(
						value = static_cast<T*>(v->unify->getValue());
					} catch (const std::bad_cast &e) {
						ANANDAMIDE_UNUSED(e);
						errorMessage(format("Can not convert variable of type \"%s\" to type \"%s\".", v->type.name(), TYPEID(T).name()));
						return false;
					}
					
					if(value == NULL) {
						errorMessage(format("Can not convert NULL variable of type \"%s\" to type \"%s\".", v->type.name(), TYPEID(T).name()));
						return false;
					}
					return true;
				}
		};
		
		/// \todo Пиздецк как need
		template <typename T>
		class CheckUnify <T *> {
			public:
				bool check(const Variable *v) {
					if(v->unify == NULL || v->unify->getValue() == NULL) {
						/// \todo
						errorMessage(format("Value is not assigned to variable of type \"%s\".", v->type.name()));
						return false;
					}
					T *value = NULL;
					try {
						// There is no way to do shit like this :(
						value = *static_cast<T**>(v->unify->getValue());
					} catch (const std::bad_cast &e) {
						ANANDAMIDE_UNUSED(e);
						errorMessage(format("Can not convert variable ptr of type \"%s\" to type \"%s\".", v->type.name(), TYPEID(T *).name()));
						return false;
					}
					
					if(value == NULL && v->unify->getValue() != NULL) {
						//errorMessage(format("Can not convert nullptr variable of type \"%s\" to type \"%s\".", v->type.name(), TYPEID(T *).name()));
						return false;
					}
					return true;
				}
		};

		
		///
		/// \brief Получение значения переменной пользовательского типа
		/// \param Type пользовательский тип переменной.
		/// \return значение типа Type
		///
		template <typename Type>
		Type &getUnify() const {
			CheckUnify <Type>().check(this);
			varDebugOut("Variable getUnify <T>: T = %s", typeid(Type).name());
			return *((Type*)unify->getValue());
		}
		
		///
		/// \brief Получение значения переменной пользовательского типа
		/// \param Type пользовательский тип переменной.
		/// \param result возвращаемое значение типа Type
		///
		template <typename T>
		void getUnify(T &result) const {
			result = getUnify <T> ();
		}
		
		template <typename T>
		T toType() const {
			return VariableToType <T> (*this).value;
		}
		
		template <typename T>
		void toType(T &result) const {
			result = VariableToType <T> (*this).value;
		}
		
		template <typename T>
		void fromType(const T &value) {
			TypeToVariable <T> (value, *this);
		}
		
		template <typename T>
		static Variable create(const T &value) {
			Variable result;
			return TypeToVariable <T> (value, result);
			return result;
		}
		
		///
		/// \brief Получение типа переменной
		/// \return TypeInfo типа переменной
		///
		TypeInfo getType() const;
		
		///
		/// \brief Получение имени типа переменной в строковом представлении
		/// \return имя типа переменной в строковом представлении
		///
		const char *getTypeName() const;

		///
		/// \brief Установка типа переменной
		/// \param Type новый тип переменной
		///
		template <class Type> 
		int setType() {
			TypeInfo new_type = TYPEID(Type);
			if(type == new_type) return 0;
			reset();
			type = new_type;
			unify = type.newDefaultValue();
			if(type.getEnum() != NULL) {
				i = type.getEnum()->getItemId(0);
			}
			if (on_change != NULL) on_change->run();
			return 1;
		}
		
		///
		/// \brief Установка типа переменной по TypeInfo
		/// \param new_type новый тип переменной
		///
		void setType(const TypeInfo &new_type);
		
		///
		/// \brief Получение текущей функции-события изменения переменной
		/// \return функция-событие изменения переменной
		///
		Common::Action *getOnChangeAction();

		///
		/// \brief Установка функции-события изменения переменной
		/// \param action функция, которая будет вызвана, когда значение переменной меняется
		///
		void setOnChangeAction(Common::Action *action);
		
		/// \brief operator==
		///	\details Результат зависит от типов переменных. 
		/// <br>В случае, если это простые типы (int, float, double, enum, string)
		/// осуществляется конвертация типов. Если текущая переменная имеет пользовательский тип,
		/// вызывается соответствующий оператор для пользовательского типа. В таком случае
		/// второй аргумент оператора должен быть точно такого же типа, как и текущая переменная.
		/// Автоматический вызов операторов для двух разных пользовательских типов не поддерживается.
		/// \param o второй агрумент оператора. Если переменная пользовательского типа, тип переменной
		/// второго агрумента должен соотвествтвоать типу текущей переменной.
		/// \return результат выполнения оператора
		///
		bool operator==(const Variable &o) const;

		/// \brief operator>
		///	\details Результат зависит от типов переменных. 
		/// <br>В случае, если это простые типы (int, float, double, enum, string)
		/// осуществляется конвертация типов. Если текущая переменная имеет пользовательский тип,
		/// вызывается соответствующий оператор для пользовательского типа. В таком случае
		/// второй аргумент оператора должен быть точно такого же типа, как и текущая переменная.
		/// Автоматический вызов операторов для двух разных пользовательских типов не поддерживается.
		/// \param o второй агрумент оператора. Если переменная пользовательского типа, тип переменной
		/// второго агрумента должен соотвествтвоать типу текущей переменной.
		/// \return результат выполнения оператора
		///
		bool operator>(const Variable &o) const;
		
		/// \brief operator ==
		///	\details Результат зависит от типов переменных. 
		/// <br>В случае, если это простые типы (int, float, double, enum, string)
		/// осуществляется конвертация типов. Если текущая переменная имеет пользовательский тип,
		/// вызывается соответствующий оператор для пользовательского типа. В таком случае
		/// второй аргумент оператора должен быть точно такого же типа, как и текущая переменная.
		/// Автоматический вызов операторов для двух разных пользовательских типов не поддерживается.
		/// \param o второй агрумент оператора. Если переменная пользовательского типа, тип переменной
		/// второго агрумента должен соотвествтвоать типу текущей переменной.
		/// \return результат выполнения оператора
		///
		Variable operator+(const Variable &o) const;
		
		/// \brief operator-
		///	\details Результат зависит от типов переменных. 
		/// <br>В случае, если это простые типы (int, float, double, enum, string)
		/// осуществляется конвертация типов. Если текущая переменная имеет пользовательский тип,
		/// вызывается соответствующий оператор для пользовательского типа. В таком случае
		/// второй аргумент оператора должен быть точно такого же типа, как и текущая переменная.
		/// Автоматический вызов операторов для двух разных пользовательских типов не поддерживается.
		/// \param o второй агрумент оператора. Если переменная пользовательского типа, тип переменной
		/// второго агрумента должен соотвествтвоать типу текущей переменной.
		/// \return результат выполнения оператора
		///
		Variable operator-(const Variable &o) const;
		
		/// \brief operator*
		///	\details Результат зависит от типов переменных. 
		/// <br>В случае, если это простые типы (int, float, double, enum, string)
		/// осуществляется конвертация типов. Если текущая переменная имеет пользовательский тип,
		/// вызывается соответствующий оператор для пользовательского типа. В таком случае
		/// второй аргумент оператора должен быть точно такого же типа, как и текущая переменная.
		/// Автоматический вызов операторов для двух разных пользовательских типов не поддерживается.
		/// \param o второй агрумент оператора. Если переменная пользовательского типа, тип переменной
		/// второго агрумента должен соотвествтвоать типу текущей переменной.
		/// \return результат выполнения оператора
		///
		Variable operator*(const Variable &o) const;
		
		/// \brief operator/
		///	\details Результат зависит от типов переменных. 
		/// <br>В случае, если это простые типы (int, float, double, enum, string)
		/// осуществляется конвертация типов. Если текущая переменная имеет пользовательский тип,
		/// вызывается соответствующий оператор для пользовательского типа. В таком случае
		/// второй аргумент оператора должен быть точно такого же типа, как и текущая переменная.
		/// Автоматический вызов операторов для двух разных пользовательских типов не поддерживается.
		/// \param o второй агрумент оператора. Если переменная пользовательского типа, тип переменной
		/// второго агрумента должен соотвествтвоать типу текущей переменной.
		/// \return результат выполнения оператора
		///
		Variable operator/(const Variable &o) const;
		Variable operator%(const Variable &o) const;
		
		bool operator<(const Variable &o) const;
		bool operator>=(const Variable &o) const;
		bool operator<=(const Variable &o) const;
		bool operator!=(const Variable &o) const;
		
		void create();
		
		void destroy();

	};


	///-------------------------------------------------------------------------
	///
	/// \class VariableToType
	///
	/// \brief Шаблон для преобразования переменной в требуемый тип 
	/// \param Type требуемый тип переменной
	/// 
	///-------------------------------------------------------------------------
	
	template <class Type>
	class VariableToType {
	
	public:
		
		/// результат требуемого типа
		Type value;
		
		VariableToType(const Variable &v) : value(v.getUnify <Type> ()) {
			varDebugOut("VariableToType <T>: T = %s", typeid(Type).name());
		}
		
	};
	
	template <class Type>
	class VariableToType <const Type &> {
	
	public:
		
		const Type &value;
		
		VariableToType(const Variable &v) : value(v.getUnify <Type> ()) {
			varDebugOut("VariableToType <const T &>: T = %s", typeid(Type).name());
		}
		
	};
	
	template <class Type>
	class VariableToType <Type &> {
	
	public:
		
		Type &value;
		
		VariableToType(const Variable &v) : value(v.getUnify <Type> ()) {
			varDebugOut("VariableToType <T &>: T = %s", typeid(Type).name());
		}
		
	};
	
	template <> class VariableToType <Variable &> {
	
	public:
		Variable &value;
		
		VariableToType(Variable &v) : value(v) { }
	};
	
	template <> class VariableToType <const Variable &> {
	
	public:
		const Variable &value;
		
		VariableToType(const Variable &v) : value(v) { }
	};
	
	template <> class VariableToType <Variable> {
	
	public:
		const Variable value;
		
		VariableToType(const Variable &v) : value(v) { }
	};
	
	
	/// \def DEFINE_VARIABLE_TO_TYPE(TYPE,VALUE_TYPE,OPERATOR,CONVERSION)
	/// макрос для объвления преобразования переменной в требуемый тип
	#define DEFINE_VARIABLE_TO_TYPE(TYPE,VALUE_TYPE,OPERATOR,CONVERSION) \
	namespace Anandamide {\
		template <> class VariableToType <TYPE> { \
		public: \
			VALUE_TYPE value; \
			VariableToType(const Variable &v) : value(CONVERSION(v.OPERATOR())) { } \
		};\
	}
	
	//--------------------------------------------------------------------------
	//
	// TypeToVariable
	//
	//--------------------------------------------------------------------------
	
	template <class Type>
	class TypeToVariable {
	
	public:
		
		TypeToVariable(Type t, Variable &value) {
			varDebugOut("Begin TypeToVariable <T>: T = %s", typeid(Type).name());
			value.setUnify(t);
			varDebugOut("End TypeToVariable <T>: T = %s", typeid(Type).name());
		}
		
	};
	
	template <class Type>
	class TypeToVariable <const Type &> {
	
	public:
		
		TypeToVariable(const Type &t, Variable &value) {
			varDebugOut("Begin TypeToVariable <T>: T = %s", typeid(Type).name());
			value.setUnify(const_cast <Type &>(t));
			varDebugOut("End TypeToVariable <T>: T = %s", typeid(Type).name());
		}
		
	};
	
	template <> 
	class TypeToVariable <const Variable &> {
	
	public:
			
		TypeToVariable(const Variable &v, Variable &value) { value = v; }
	};
	
	template <> 
	class TypeToVariable <Variable &> {
	
	public:
		
		TypeToVariable(Variable &v, Variable &value) { value = v; }
	};
	
	template <> 
	class TypeToVariable <Variable> {
	
	public:
		
		TypeToVariable(const Variable &v, Variable &value) { value = v; }
	};
	
	#define DEFINE_TYPE_TO_VARIABLE(TYPE,OPERATOR,CONVERSION) \
	namespace Anandamide {\
		template <> class TypeToVariable <TYPE> { \
		public: \
			TypeToVariable(TYPE t, Variable &value) { value.OPERATOR(CONVERSION(t)); } \
		};\
	}

	//--------------------------------------------------------------------------

	template <class T>
	T fromString(const char *str) { Variable v; v.setType <T>(); v.setFromStr(str); return v.toType <T>(); }

	//--------------------------------------------------------------------------
	
}

//------------------------------------------------------------------------------

DEFINE_TYPEID(Anandamide::Variable, 1007)
DEFINE_TYPEID(Anandamide::Variable*, 10013)

//------------------------------------------------------------------------------

//DEFINE_VARIABLE_TO_TYPE(Variable,Variable,get,)
DEFINE_VARIABLE_TO_TYPE(char,char,getInt,(char))
DEFINE_VARIABLE_TO_TYPE(bool,bool,getBool,)
DEFINE_VARIABLE_TO_TYPE(unsigned char,unsigned char,getInt,(unsigned char))
DEFINE_VARIABLE_TO_TYPE(int,int,getInt,)
DEFINE_VARIABLE_TO_TYPE(float,float,getFloat,)
DEFINE_VARIABLE_TO_TYPE(double,double,getDouble,)
DEFINE_VARIABLE_TO_TYPE(const char *,const char *,getStr,)

//DEFINE_TYPE_TO_VARIABLE(Variable,set,)
DEFINE_TYPE_TO_VARIABLE(char,setInt,(int))
DEFINE_TYPE_TO_VARIABLE(bool,setInt,(int))
DEFINE_TYPE_TO_VARIABLE(unsigned char,setInt,(int))
DEFINE_TYPE_TO_VARIABLE(int,setInt,)
DEFINE_TYPE_TO_VARIABLE(float,setFloat,)
DEFINE_TYPE_TO_VARIABLE(double,setDouble,)
DEFINE_TYPE_TO_VARIABLE(const char *,setStr,)

//------------------------------------------------------------------------------

namespace Anandamide {
	typedef Array <Variable> VariableArrayData;
	typedef Array <Variable> *VariableArray;
	
	typedef Map <Variable, Variable> VariableMapData;
	typedef Map <Variable, Variable> *VariableMap;
}

//------------------------------------------------------------------------------

DEFINE_TYPEID(Anandamide::VariableArray, 1100)
DEFINE_TYPEID(Anandamide::VariableMap, 1101)

class Undefined { 
	Anandamide::Str value;
public:
	Undefined();
	Undefined(const char *value);
	Anandamide::Str &getValue();
};

namespace Anandamide {
	void ANANDAMIDE_API operator>>(Undefined t, Str &str);
	void ANANDAMIDE_API operator<<(Undefined &t, const Str &str);
}

DEFINE_TYPEID(Undefined, 1099338)

#endif

//------------------------------------------------------------------------------
