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

#ifndef ANANDAMIDETYPEINFO_H
#define ANANDAMIDETYPEINFO_H

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "Str.h"
#include "Map.h"
#include "Anandamide.h"
#include <type_traits>

//------------------------------------------------------------------------------

namespace Anandamide {
	
	///-------------------------------------------------------------------------
	///
	/// \class Enum
	///
	/// \brief Класс для внутреннего хранения данных для перечисляемых (enum) типов
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Enum {
			
		public:
			
			Enum();
			virtual ~Enum();
			
			///
			/// \brief Получение количества элементов перечисления
			/// \return 
			///
			virtual int getItemsCount() const;
			
			///
			/// \brief Получение строкового представления элемента перечисления
			/// \param i индекс элемента перечисления
			/// \return строковое представление элемента перечисления
			///
			virtual const char *getItemName(int i) const;
			
			///
			/// \brief Получение значения элемента перечисления
			/// \param i индекс элемента перечисления
			/// \return значение элемента перечисления
			///
			virtual int getItemId(int i) const;
			
			///
			/// \brief Получение имени элемента пречисления по значению
			/// \param id значение элемента перечисления
			/// \return имя элемента перечисления со значением id
			///
			virtual const char *getItemNameById(int id) const;
			
			///
			/// \brief Получение значение элемента перечисления по имени
			/// \param name имя элемента перечисления
			/// \return значение элемента перечисления с именем name
			///
			virtual const int *getItemIdByName(const char *name) const;
			
	};
	
	///-------------------------------------------------------------------------
	///
	/// \brief Шаблонный класс перечисления по умолчанию
	///
	///-------------------------------------------------------------------------
	
	template <typename T>
	class EnumTemplate : public Enum { };
	
	#define ENUM_BEGIN(ENUM_TYPE)\
	namespace Anandamide {\
		template <>\
		class EnumTemplate <ENUM_TYPE> : public Enum {\
			public:\
				virtual int getItemsCount() const {\
					return items().count();\
				}\
				virtual const char *getItemName(int i) const {\
					return items()[i].getValue();\
				}\
				virtual int getItemId(int i) const {\
					if(i >= items().count()) return 0;\
					return items()[i].getKey();\
				}\
				virtual const char *getItemNameById(int id) const {\
					Str *item = items().get(id);\
					if(item == NULL) return NULL;\
					return item->str();\
				}\
				virtual const int *getItemIdByName(const char *name) const {\
					return items().findValue(Str(name));\
				}\
				virtual Enum *clone() const {\
					return new EnumTemplate <ENUM_TYPE> ();\
				}\
				static Enum *create() {\
					return new EnumTemplate <ENUM_TYPE> ();\
				}\
			private:\
				static Map <int, Str> &items() {\
					static Map <int, Str> static_items;\
					static int init = initItems(static_items);\
					(void)init;\
					return static_items;\
				}\
				static int initItems(Map <int, Str> &items) {
					#define ENUM_ITEM_NAMED(ID, NAME) items.append(new int(int(ID)), new Str(NAME));
					#define ENUM_ITEM(ID) items.append(new int(int(ID)), new Str(#ID));
					#define ENUM_END(ENUM_TYPE, ENUM_ID) ENUM_END_NAMED(ENUM_TYPE,#ENUM_TYPE,ENUM_ID)
					#define ENUM_END_NAMED(ENUM_TYPE,ENUM_NAME,ENUM_ID)\
					return 1;\
				}\
		};\
		template <> class TypeToVariable <ENUM_TYPE> { \
		public: \
			TypeToVariable(ENUM_TYPE t, Variable &value) { value.setEnum <ENUM_TYPE> (t); }\
		};\
		template <> class VariableToType <ENUM_TYPE> { \
		public: \
			ENUM_TYPE value; \
			VariableToType(const Variable &v) : value(ENUM_TYPE(v.getInt())) { }\
		};\
		template <>\
		struct TypeID <ENUM_TYPE> {\
			static int id() { static bool res = checkOrAddTypeID(ENUM_ID, ENUM_NAME); return ENUM_ID; }\
			static const char *name() { return ENUM_NAME; }\
			static const Enum *getEnum() { static EnumTemplate <ENUM_TYPE> enum_static; return &enum_static; }\
			static TypeContainer *newDefaultValue() { return new TypeContainerTemplate <ENUM_TYPE> ((ENUM_TYPE)getEnum()->getItemId(0)); }\
		};\
	}
		
	///
	/// \brief Генерация нового id типа
	/// \return новый id типа
	///
	ANANDAMIDE_API int nextTypeID();
	
	ANANDAMIDE_API bool checkOrAddTypeID(int id, const char * type);
	
	//--------------------------------------------------------------------------
	//
	// TypeID
	//
	//--------------------------------------------------------------------------
	
	class TypeContainer;
    template <typename T> class TypeContainerTemplate;
	
	typedef TypeContainer *(*TypeDefaultValueCreator)();

	template <typename T>
	struct TypeID {
		static int id() { /*static_assert(false, "Anandamide error: type id is not assigned.");*/ return T();}
		static const char *name() { return typeid(T).name(); }
		static const Enum *getEnum() { return NULL; }
		static TypeContainer *newDefaultValue() { return new TypeContainerTemplate <T> (); }
	};
	
	template <typename T>
	struct TypeID <const T *> {
		static int id() { return TypeID <T *>().id(); }
		static const char *name() { return TypeID <T *>().name(); }
		static const Enum *getEnum() { return TypeID <T *>().getEnum(); }
		static TypeContainer *newDefaultValue() { return new TypeContainerTemplate <T *> (); }
	};
	
	template <typename T>
	struct TypeID <const T> {
		static int id() { return TypeID <T>().id(); }
		static const char *name() { return TypeID <T>().name(); }
		static const Enum *getEnum() { return TypeID <T>().getEnum(); }
		static TypeContainer *newDefaultValue() { return new TypeContainerTemplate <T> (); }
	};
	
	template <typename T>
	struct TypeID <T &> {
		static int id() { return TypeID <T>().id(); }
		static const char *name() { return TypeID <T>().name(); }
		static const Enum *getEnum() { return TypeID <T>().getEnum(); }
		static TypeContainer *newDefaultValue() { return new TypeContainerTemplate <T> (); }
	};
	
	template <typename T>
	struct TypeID <const T &> {
		static int id() { return TypeID <T>().id(); }
		static const char *name() { return TypeID <T>().name(); }
		static const Enum *getEnum() { return NULL; }
		static TypeContainer *newDefaultValue() { return new TypeContainerTemplate <T> (); }
	};
	
	
	template<bool B, class T>
	struct create_if {
		static T *create() {
			return NULL;
		}
	};
	 
	template<class T>
	struct create_if <true, T> {
		static T *create() {
			return new T();
		}
	};
	
	#define DEFINE_TYPEID_NAMED(TYPE,NAME,ID_VALUE)\
	namespace Anandamide {\
		template <>\
		struct TypeID <TYPE> {\
			enum { ID = ID_VALUE };\
			typedef TypeContainerTemplate <TYPE> ContainerType;\
			static int id() { static bool res = checkOrAddTypeID(ID_VALUE, NAME); (void)res; return ID_VALUE; }\
			static const char *name() { return NAME; }\
			static const Enum *getEnum() { return NULL; }\
			static TypeContainer *newDefaultValue() { \
				return create_if <	std::is_default_constructible <TYPE>::value && \
									std::is_abstract <TYPE>::value == false, \
									TypeContainerTemplate <TYPE> >::create(); \
			}\
		};\
	}
	#define DEFINE_TYPEID(TYPE,ID) DEFINE_TYPEID_NAMED(TYPE,#TYPE,ID)
	
	#define TYPEID(T) Anandamide::TypeInfo(Anandamide::TypeID <T>().id(), Anandamide::TypeID <T>().name(), Anandamide::TypeID <T>().getEnum(), &Anandamide::TypeID <T>::newDefaultValue)
	#define CASEID(T) Anandamide::TypeID <T>::ID
	
	///-------------------------------------------------------------------------
	///
	/// \class TypeInfo
	///
	/// \brief Класс, служащий для хранения информации о типе
	///
	///-------------------------------------------------------------------------
 
	class ANANDAMIDE_API TypeInfo {
		
		int m_id;
		const char *m_name;
		const Enum *m_enum;
		TypeDefaultValueCreator m_creator;
			
	public:
		
		TypeInfo() {
			m_id = 0;
			m_name = NULL;
			m_enum = NULL;
			m_creator = NULL;
		}
		
		TypeInfo(const TypeInfo &o) {
			m_id = o.m_id;
			m_name = o.m_name;
			m_enum = o.m_enum;
			m_creator = o.m_creator;
		}
		
		TypeInfo(int n_id, const char *n_name, const Enum *n_enum, TypeDefaultValueCreator n_creator) {
			m_id = n_id;
			m_name = n_name;
			m_enum = n_enum;
			m_creator = n_creator;
		}
		
		///
		/// \brief Получение уникального идентификатора типа
		/// \return уникальный идентификатор типа
		///
		int id() const {
			return m_id;
		}
		
		///
		/// \brief Получение строки-имени типа
		/// \return стоку-имя типа
		///
		const char *name() const {
			return m_name;
		}
		
		///
		/// \brief Получение информации о перечисляемом типе
		/// \return определение перечисляемого типа или NULL, если тип не является перечисляемым
		///
		const Enum *getEnum() const {
			return m_enum;
		}
		
		bool operator==(const TypeInfo &o) const {
			return m_id == o.m_id;
		}
		
		bool operator!=(const TypeInfo &o) const {
			return m_id != o.m_id;
		}
		
		TypeInfo &operator=(const TypeInfo &o) {
			m_id = o.m_id;
			m_name = o.m_name;
			m_enum = o.m_enum;
			m_creator = o.m_creator;
			return *this;
		}
		
		TypeContainer *newDefaultValue() {
			return m_creator();
		}
		
		void setId(int id) {
			m_id = id;
			m_name = "##Implicit##";
			m_enum = NULL;
			m_creator = NULL;
		}
		
	};
	
	//-------------------------------------------------------------------------
	//
	// namespace DefaultOperators
	//
	//-------------------------------------------------------------------------
	
	namespace DefaultOperators {
		template <class A, class B>
		bool operator>(const A &a, const B &b) {
			errorMessage(format("Operator \"%s\" > \"%s\" is not defined.", typeid(A).name(), typeid(B).name()));
			return false;
		}
		
		template <class A, class B>
		bool operator==(const A &a, const B &b) {
			errorMessage(format("Operator \"%s\" == \"%s\" is not defined.", typeid(A).name(), typeid(B).name()));
			return false;
		}
		
		template <class T>
		T operator+(const T &a, const T &b) {
			errorMessage(format("Operator \"%s\" + \"%s\" is not defined.", typeid(T).name(), typeid(T).name()));
			return T();
		}
		
		template <class T>
		T operator-(const T &a, const T &b) {
			errorMessage(format("Operator \"%s\" - \"%s\" is not defined.", typeid(T).name(), typeid(T).name()));
			return T();
		}
		
		template <class T>
		T operator*(const T &a, const T &b) {
			errorMessage(format("Operator \"%s\" * \"%s\" is not defined.", typeid(T).name(), typeid(T).name()));
			return T();
		}
		
		template <class T>
		T operator/(const T &a, const T &b) {
			errorMessage(format("Operator \"%s\" / \"%s\" is not defined.", typeid(T).name(), typeid(T).name()));
			return T();
		}
		
		template <class T>
		void operator>>(const T &t, Str &str) {
			//errorMessage(format("Operator string  << \"%s\" is not defined.", typeid(T).name()));
			str = "{?}";
		}
		
		template <class T>
		void operator<<(T &t, const Str &str) {
			//errorMessage(format("Operator \"%s\" << string is not defined.", typeid(T).name()));
		}
	}
	
	//-------------------------------------------------------------------------
	//
	// class TypeContainer
	//
	//-------------------------------------------------------------------------
	
	class TypeContainer {
	
		public:
			
			virtual ~TypeContainer() { }
			virtual void *getValue() = 0;
			virtual TypeContainer *clone() const = 0;
			
			//virtual bool canCast(const UnifyContainer *o) const = 0;
			virtual bool isGreater(const TypeContainer *o) const = 0;
			virtual bool isEqual(const TypeContainer *o) const = 0;
			virtual void toString(Str &str) const = 0;
			virtual void fromString(const Str &str) = 0;
			virtual TypeContainer *add(const TypeContainer *o) const = 0;
			virtual TypeContainer *sub(const TypeContainer *o) const = 0;
			virtual TypeContainer *mul(const TypeContainer *o) const = 0;
			virtual TypeContainer *div(const TypeContainer *o) const = 0;
			
			virtual void create() = 0;
			virtual void destroy() = 0;
		
	};
	
	//-------------------------------------------------------------------------
	//
	// class TypeContainerTemplate
	//
	//-------------------------------------------------------------------------
	
	template <typename T>
	class TypeContainerTemplate : public TypeContainer {
			
			T value;
			
		public:
		
			TypeContainerTemplate() : value(T()) { }
			
			TypeContainerTemplate(const T &v) : value(v) { }
			
			virtual void *getValue() {
				return const_cast <void *> ((void *)&value);
			}
			
			virtual TypeContainer *clone() const {
				return new TypeContainerTemplate <T> (value);
			}

			virtual bool isGreater(const TypeContainer *o) const {
				using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return false;
				return value > this_type->value;
			}
			
			virtual bool isEqual(const TypeContainer *o) const {
				using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return false;
				return value == this_type->value;
			}
			
			virtual void toString(Str &str) const {
				using namespace DefaultOperators;
				value >> str;
			}
			
			virtual void fromString(const Str &str) {
				using namespace DefaultOperators;
				value << str;
			}
			
			virtual TypeContainer *add(const TypeContainer *o) const {
				using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return NULL;
				return new TypeContainerTemplate <decltype(value + this_type->value)> (value + this_type->value);
			}
			
			virtual TypeContainer *sub(const TypeContainer *o) const {
				using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return NULL;
				return new TypeContainerTemplate <decltype(value - this_type->value)> (value - this_type->value);
			}
			
			virtual TypeContainer *mul(const TypeContainer *o) const {
				using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return NULL;
				return new TypeContainerTemplate <decltype(value * this_type->value)> (value * this_type->value);
			}
			
			virtual TypeContainer *div(const TypeContainer *o) const {
				using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return NULL;
				return new TypeContainerTemplate <decltype(value / this_type->value)> (value / this_type->value);
			}
			
			virtual void create() { }
			virtual void destroy() { }
			
	};
	
	//-------------------------------------------------------------------------
	//
	// class TypeContainerTemplate
	//
	//-------------------------------------------------------------------------
	
	template <typename T>
	class TypeContainerTemplate <T *> : public TypeContainer {
			
			T *value;
					
		public:
		
			TypeContainerTemplate() : value(NULL) { }
			TypeContainerTemplate(T *v) : value(v) { }
		
			virtual void *getValue() {
				return &value;
			}
			
			virtual TypeContainer *clone() const {
				return new TypeContainerTemplate <T *> (value);
			}

			virtual bool isGreater(const TypeContainer *o) const {
				return false;
			}
			
			virtual bool isEqual(const TypeContainer *o) const {
				/*using namespace DefaultOperators;
				decltype(this) this_type = dynamic_cast <decltype(this)> (o);
				if(this_type == NULL) return false;
				return value == this_type->value;*/
				return value == *((T **)((TypeContainer *)o)->getValue());
			}
			
			virtual void toString(Str &str) const {
				str.format("%p", value);
			}
			
			virtual void fromString(const Str &str) {
				using namespace DefaultOperators;
				value << str;
			}
			
			virtual TypeContainer *add(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual TypeContainer *sub(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual TypeContainer *mul(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual TypeContainer *div(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual void create() {
				value = create_if < std::is_constructible <T>::value && !std::is_abstract <T>::value, T >::create();
				if(value == NULL) {
					errorMessage(format("Can`t create instance of the \"%s\" class. The class is not default-constructable (there is no default constructor, or class is abstract).", typeid(T).name()));
				}
			}
			virtual void destroy() {
				delete value;
				value = NULL;
			}
			
	};
	
	//-------------------------------------------------------------------------
	//
	// class TypeContainerTemplate
	//
	//-------------------------------------------------------------------------
	
	template <>
	class TypeContainerTemplate <void> : public TypeContainer {
			
		public:
		
			TypeContainerTemplate() { }
		
			virtual void *getValue() {
				return NULL;
			}
			
			virtual TypeContainer *clone() const {
				return NULL;
			}

			virtual bool isGreater(const TypeContainer *o) const {
				return false;
			}
			
			virtual bool isEqual(const TypeContainer *o) const {
				return false;
			}
			
			virtual void toString(Str &str) const {
				str = "{void}";
			}
			
			virtual void fromString(const Str &str) {
			}
			
			virtual TypeContainer *add(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual TypeContainer *sub(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual TypeContainer *mul(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual TypeContainer *div(const TypeContainer *o) const {
				return NULL;
			}
			
			virtual void create() { }
			virtual void destroy() { }
	};

	class Deleter
	{
	public:
		virtual void free(void *value) const = 0;
	};

	class TypeContainerVoid: public TypeContainer
	{

		void *value;

		Deleter *deleter;

		public:

			TypeContainerVoid() : value(nullptr), deleter(nullptr) { }
			TypeContainerVoid(void *value) : value(value), deleter(nullptr) { }
			TypeContainerVoid(void *value, Deleter *deleter) : value(value), deleter(deleter) { }

			virtual void *getValue() {
				return value;
			}

			virtual TypeContainer *clone() const {
				return new TypeContainerVoid(value, deleter);
			}

			virtual bool isGreater(const TypeContainer *o) const {
				return false;
			}

			virtual bool isEqual(const TypeContainer *o) const {
				return false;
			}

			virtual void toString(Str &str) const {
				str = "{void*}";
			}

			virtual void fromString(const Str &str) {

			}

			virtual TypeContainer *add(const TypeContainer *o) const {
				return nullptr;
			}

			virtual TypeContainer *sub(const TypeContainer *o) const {
				return nullptr;
			}

			virtual TypeContainer *mul(const TypeContainer *o) const {
				return nullptr;
			}

			virtual TypeContainer *div(const TypeContainer *o) const {
				return nullptr;
			}

			virtual void create() { value = nullptr; deleter = nullptr; }
			virtual void destroy() {
				if (deleter != nullptr) {
					deleter->free(value);
					delete deleter;
				}
			}
	};
	
	
}

namespace Anandamide {
	
	void ANANDAMIDE_API operator>>(const TypeInfo &t, Anandamide::Str &str);
	
	void ANANDAMIDE_API operator<<(TypeInfo &t, const Anandamide::Str &str);
	
	void ANANDAMIDE_API operator>>(unsigned int t, Anandamide::Str &str);
	void ANANDAMIDE_API operator<<(unsigned int &t, const Anandamide::Str &str);
	
	void ANANDAMIDE_API operator>>(long long int t, Anandamide::Str &str);
	void ANANDAMIDE_API operator<<(long long int &t, const Anandamide::Str &str);
	
	void ANANDAMIDE_API operator>>(unsigned long long int t, Anandamide::Str &str);
	void ANANDAMIDE_API operator<<(unsigned long long int &t, const Anandamide::Str &str);
	
	void ANANDAMIDE_API operator>>(char t, Anandamide::Str &str);
	void ANANDAMIDE_API operator<<(char &t, const Anandamide::Str &str);
	
}

DEFINE_TYPEID(Anandamide::TypeInfo		, 666)
DEFINE_TYPEID(void			, 1001)
DEFINE_TYPEID(int			, 1002)
DEFINE_TYPEID(float			, 1003)
DEFINE_TYPEID(double		, 1004)
DEFINE_TYPEID(const char *	, 1005)
DEFINE_TYPEID(unsigned int	, 1006)
DEFINE_TYPEID(char			, 1008)
DEFINE_TYPEID(bool			, 1009)
DEFINE_TYPEID(long long int , 1010)
DEFINE_TYPEID(unsigned long long int, 1011)
DEFINE_TYPEID(void *		, 1012)
DEFINE_TYPEID(double *		, 1013)



#endif // ANANDAMIDETYPEINFO_H
