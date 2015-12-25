//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// authors:		Alexey Egorov (FadeToBlack aka EvilSpirit)
// 				Zakhar Shelkovnikov
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

#ifndef ANANDAMIDELIBRARY_H
#define ANANDAMIDELIBRARY_H

#define ADD_TYPE(lib,type,col_r,col_g,col_b,img_path) \
lib->addType <type>(col_r,col_g,col_b, new QImage(img_path), img_path);

#define ADD_IMG(img_path) \
new QImage(img_path), img_path

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideNeurone.h"
#include "Resource.h"
#include <QImage>

//------------------------------------------------------------------------------

namespace Anandamide {
	
	class Neurone;
	class Libraries;
	class Script;
	
	typedef void (*FormApiLoadCallback)(const QString&, Anandamide::Libraries*);
	
	///-------------------------------------------------------------------------
	///
	/// \class TypeDef
	///
	/// \brief Класс для регистрации параметров типа.
	///
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API TypeDef {
			
			TypeDef() : r(0.4f), g(0.4f), b(0.4f), image(NULL), type(TYPEID(int)) { }
		
			/// красный
			float r;
			
			/// зеленый
			float g;
			
			/// синий
			float b;		
			
			/// иконка
			QImage *image;
			
			/// тип
			TypeInfo type;
			
			static QMap <QString, QImage*> def_images;
			
		public:
			
			~TypeDef();
			
			///
			/// \brief Получение занимаемого размера памяти
			/// \return размер в байтах
			///
			virtual unsigned long long getSize();
			
			///
			/// \brief Создание нового экземпляра класса TypeDef
			/// \return новый экземпляр класса TypeDef
			///
			static TypeDef *create();
			
			void setColor(float r, float g, float b);
			void setImage(QImage *image, const QString& image_key = QString());
			void setTypeInfo(const TypeInfo &type);
			
			float getR() const;
			float getG() const;
			float getB() const;
			
			const TypeInfo &getTypeInfo() const;
			
			const QImage *getImage() const;
				
			static QImage* getDefImage(const QString& key);
			static void addDefImage(const QString& key, QImage* image);
			static void clearDefImages();
			
	};
				
	///-------------------------------------------------------------------------
	///
	/// \class Library
	///
	/// \brief Класс библиотеки блоков
	///
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Library {
			
			bool external;
			Str name;
			Str fileName;
			Str pluginName;
			Map <Str, NeuroneDef> node_defs;
			Map <int, TypeDef> type_defs;
			
			void addTypeDefinition(TypeDef *def);
			
		public:
			
			Library();
			
			virtual ~Library();
			
			///
			/// \brief Получение занимаемого размера памяти
			/// \return размер в байтах
			///
			virtual unsigned long long getSize();
			
			///
			/// \brief Получение количества определений блоков
			/// \return количество определений блоков, которые содержатся в библиотеке
			///
			int getDefsCount() const;
			
			///
			/// \brief Получение определения блока по индексу
			/// \param i индекс определения
			/// \return определение блока с индексом i
			///
			const NeuroneDef *getDef(int i) const;
			
			///
			/// \brief Получение определения блока по имени
			/// \param id имя определения блока
			/// \return определение блока с именем id или NULL в случае, если определение с заданным именем не существует
			///
			const NeuroneDef *getDef(const char *id) const;
			
			///
			/// \brief Получение имени бибилиотеки
			/// \return имя библиотеки
			///
			const char *getName() const;
			
			///
			/// \brief Установка имени библиотеки
			/// \param new_name новое имя библиотеки
			///
			void setName(const char *new_name);
			
			const char* getPluginName() const;
			
			void setPluginName(const char* new_plugin_name);
			
			///
			/// \brief Добавление определения блока
			/// \param def определение блока
			/// \return false, если имя блока не задано, либо блок с таким именем уже существует
			///
			bool addDefinition(NeuroneDef *def);

			///
			/// \brief Добавление определения блока с указанием подтипа классификации
			/// \param kind подтип классификации
			/// \param def определение блока
			/// \return false, если имя блока не задано, либо блок с таким именем уже существует
			///
			bool addDefinition(const char *kind, NeuroneDef *def);
			
			///
			/// \brief Добавление определения блока с указанием подтипа классификации
			/// \param kind подтип классификации
			/// \param header имя заголовка блока
			/// \param def определение блока
			/// \return false, если имя блока не задано, либо блок с таким именем уже существует
			///
			bool addDefinition(const char *kind, const char *header, NeuroneDef *def);
			
			//--------------------------------------------------------------------------
			//
			// addFunction
			//
			//--------------------------------------------------------------------------
			
			///
			/// \brief Создание определения функционального блока по указателю на функцию
			/// \param name имя блока
			/// \param ret_name имя возвращаемого параметра
			/// \param names список инициализации имен для параметров функции {"name1", "name2", "name"... }
			/// \return новое определение функционального блока
			///
			template <typename Ret, typename ...Args>
			NeuroneDef *addFunction(const char * kind, const char *name, Ret (*func)(Args...), const char *ret_name, std::initializer_list <const char *> names) {
//				qDebug() << "addFunction" << kind << name << ret_name;
				typedef NeuroneFunction <Ret, Args...> NeuroneType;
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def, name, func, ret_name, names);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(kind, def);
				return def;
			}
			
			template <typename Ret, typename ...Args>
			void addFunction(const char *kind, const char *name, const char *header, Ret (*func)(Args...), const char *ret_name, std::initializer_list <const char *> names) {
				NeuroneDef *def = addFunction(kind, name, func, ret_name, names);
				def->setHeader(header);
			}
			
			//--------------------------------------------------------------------------
			//
			// addFunction member const
			//
			//--------------------------------------------------------------------------
			
			///
			/// \brief Создание определения функционального блока по указателю на метод класса, вызываемого по указателю
			/// \param name имя блока
			/// \param inst_name имя входного параметра - указателя на экземпляр, для которого вызывается метод
			/// \param ret_name имя возвращаемого параметра
			/// \param names список инициализации имен для параметров функции { "name1", "name2", "name"... }
			/// \return новое определение блока - метода класса
			///
			template <typename Instance, typename Ret, typename ...Args>
			NeuroneDef *addFunction(const char *kind, const char *name, Ret (Instance::*func)(Args...) const, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				typedef Ret (Instance::*Func)(Args...) const;
				typedef NeuroneMember <Func, Instance, Ret, Args...> NeuroneType;
//				typedef NeuroneMember <Func, Ptr<Instance*>, Ret, Args...> NeuroneType;
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def, name, func, inst_name, ret_name, names);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(kind, def);
				return def;
			}
			
			template <typename Instance, typename Ret, typename ...Args>
			void addFunction(const char *kind, const char *name, const char *header, Ret (Instance::*func)(Args...) const, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				NeuroneDef *def = addFunction(kind, name, func, inst_name, ret_name, names);
				def->setHeader(header);
			}
			
			//--------------------------------------------------------------------------
			//
			// addFunction member
			//
			//--------------------------------------------------------------------------
			
			template <typename Instance, typename Ret, typename ...Args>
			NeuroneDef *addFunction(const char * kind, const char *name, Ret (Instance::*func)(Args...), const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				typedef Ret (Instance::*Func)(Args...);
				typedef NeuroneMember <Func, Instance, Ret, Args...> NeuroneType;
//				typedef NeuroneMember <Func, Ptr<Instance*>, Ret, Args...> NeuroneType;
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def, name, func, inst_name, ret_name, names);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(kind, def);
				return def;
			}
			
			template <typename Instance, typename Ret, typename ...Args>
			void addFunction(const char * kind, const char *name, const char *header, Ret (Instance::*func)(Args...), const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				NeuroneDef *def = addFunction(kind, name, func, inst_name, ret_name, names);
				def->setHeader(header);
			}
			
			//--------------------------------------------------------------------------
			//
			// addFunctionV const
			//
			//--------------------------------------------------------------------------
			
			///
			/// \brief Создание определения функционального блока по указателю на метод класса, вызываемого по значению
			/// \param name имя блока
			/// \param inst_name имя входного параметра - указателя на экземпляр, для которого вызывается метод
			/// \param ret_name имя возвращаемого параметра
			/// \param names список инициализации имен для параметров функции { "name1", "name2", "name"... }
			/// \return новое определение блока - метода класса
			///
			template <typename Instance, typename Ret, typename ...Args>
			NeuroneDef *addFunctionV(const char *kind, const char *name, Ret (Instance::*func)(Args...) const, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				typedef Ret (Instance::*Func)(Args...) const;
				typedef NeuroneMemberValue <Func, Instance, Ret, Args...> NeuroneType;
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def, name, func, inst_name, ret_name, names);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(kind, def);
				return def;
			}
			
			template <typename Instance, typename Ret, typename ...Args>
			void addFunctionV(const char *kind, const char *name, const char *header, Ret (Instance::*func)(Args...) const, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				NeuroneDef *def = addFunctionV(kind, name, func, inst_name, ret_name, names);
				def->setHeader(header);
			}
			
			//--------------------------------------------------------------------------
			//
			// addFunctionV
			//
			//--------------------------------------------------------------------------
			
			template <typename Instance, typename Ret, typename ...Args>
			NeuroneDef *addFunctionV(const char * kind, const char *name, Ret (Instance::*func)(Args...), const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				typedef Ret (Instance::*Func)(Args...);
				typedef NeuroneMemberValue <Func, Instance, Ret, Args...> NeuroneType;
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def, name, func, inst_name, ret_name, names);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(kind, def);
				return def;
			}
			
			template <typename Instance, typename Ret, typename ...Args>
			void addFunctionV(const char * kind, const char *name, const char *header, Ret (Instance::*func)(Args...), const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
				NeuroneDef *def = addFunctionV(kind, name, func, inst_name, ret_name, names);
				def->setHeader(header);
			}
			
			///
			/// \brief Добавление нового определения блока, основанного на наследнике класса Neurone
			/// \param NeuroneType имя класса блока, наследованного от Neurone
			/// \param kind строка классификатор блока
			/// \param name имя типа блока
			/// \param header строка заголовка блока
			/// 
			template <class NeuroneType>
			void addTemplate(const char *kind, const char *name, const char *header = "", bool hidden = false) {
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def);
				def->setName(name);
				def->setKind(kind);
				def->setHeader(header);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				def->setHidden(hidden);
				addDefinition(def);
			}
			
			///
			/// \brief Добавление нового определения блока, основанного на наследнике класса Neurone
			/// \param NeuroneType имя класса блока, наследованного от Neurone
			/// \param name имя типа блока
			/// 
			template <class NeuroneType>
			void addTemplate(const char *name) {
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def);
				def->setName(name);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(def);
			}
	
			///
			/// \brief Добавление нового определения блока, основанного на наследнике класса Neurone
			/// \param NeuroneType имя класса блока, наследованного от Neurone
			/// 
			template <class NeuroneType>
			void addTemplate() {
				NeuroneDef *def = NeuroneDef::create();
				NeuroneType::define(def);
				def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
				addDefinition(def);
			}
			
			
			///
			/// \brief Получение количества опредлений типов в библиотеке
			/// \return количество определений типов
			///
			int getTypeDefsCount() const;
			
			///
			/// \brief Получение определения типа по индексу
			/// \param i индекс определения типа
			/// \return определение типа с индексом i
			///
			const TypeDef *getTypeDef(int i) const;
			
			///
			/// \brief Получение определения типа по TypeInfo
			/// \param type TypeInfo типа, для которого нужно получить определние
			/// \return определение для типа TypeInfo, либо NULL, если опредление для такого типа не зарегистрировано в библиотеке
			///
			const TypeDef *getTypeDef(const TypeInfo &type) const;
			
			///
			/// \brief Получение определения типа по TypeInfo::id()
			/// \param id TypeInfo::id() типа, для которого нужно получить определние
			/// \return определение для типа TypeInfo::id(), либо NULL, если опредление для такого типа не зарегистрировано в библиотеке
			///
			const TypeDef *getTypeDefById(int id) const;
			
			///
			/// \brief Получение определения типа по TypeInfo::name()
			/// \param id TypeInfo::name() типа, для которого нужно получить определние
			/// \return определение для типа TypeInfo::name(), либо NULL, если опредление для такого типа не зарегистрировано в библиотеке
			///
			const TypeDef *getTypeDefByName(const char *name) const;
			
			///
			/// \brief Добавление опредления для имени типа T
			/// \param T имя типа, для которого добавляется определение
			/// \param r красный компонент цвета [0.0f, 1.0f]
			/// \param g зеленый компонент цвета [0.0f, 1.0f]
			/// \param b синий компонент цвета [0.0f, 1.0f]
			/// \param image иконка для типа
			///
			template <class T>
			void addType(float r, float g, float b, QImage *image = NULL, const QString& image_key = QString()) {
				TypeDef *def = TypeDef::create();
				def->setColor(r, g, b);
				if(image) {
					def->setImage(image, image_key);
//					delete image;
				}
				def->setTypeInfo(TYPEID(T));
				addTypeDefinition(def);
			}

			///
			/// \brief Добавление определения для имени типа
			/// \param type_info информация о типе
			/// \param r красный компонент цвета [0.0f, 1.0f]
			/// \param g зеленый компонент цвета [0.0f, 1.0f]
			/// \param b синий компонент цвета [0.0f, 1.0f]
			/// \param image иконка для типа
			///
			void addType(const TypeInfo &type_info, float r, float g, float b, QImage *image = NULL, const QString& image_key = QString()) {
				if (type_defs.get(type_info.id())) {
					warningMessage(format("An error occured while adding type <%s>. Type with same index already exist.", type_info.name()));
					return;
				}
				TypeDef *def = TypeDef::create();
				def->setColor(r, g, b);
				if(image) {
					def->setImage(image, image_key);
//					delete image;
				}
				def->setTypeInfo(type_info);
				addTypeDefinition(def);
			}
			
			///
			/// \brief Получение имени файла, из которого была загружена библиотека
			/// \return имя файла, из которого была загружена бибилиотека, либо пустая строка, если библиотека была создана вручную
			///
			virtual const char *getFileName() const;
			
			virtual void setFileName(const char* fileName);
			
			virtual const char *getLibFileName() const;
			
			static Library *loadResource(const char *file_name);
			
	};
	
	typedef Resource <Library> LibraryRef;

	///-------------------------------------------------------------------------
	///
	/// \class ScriptLibrary
	///
	/// \brief 
	///
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API ScriptLibrary : public Library {
			
			Str file_name;
			Script *script;
			
			ScriptLibrary();
			
			class ScriptLogicDef : public NeuroneDef {
					
					mutable Logic *logic;
					Str libName;
					
					ScriptLogicDef(Logic *logic);
					
				public:
			
					static ScriptLogicDef *create(Logic *logic, const char* libName);
					
					Neurone *createNeurone(Libraries *libraries, bool dummy, Neurone *dummyNode) const override;
			};
			
		public:
			
			static ScriptLibrary *create(const char *name);
			static ScriptLibrary *create(Script *script);
			
			~ScriptLibrary();
			
			virtual const char *getFileName() const;
			virtual void setFileName(const char* fileName);
			virtual const char *getLibFileName() const;
			
			Script* getScript();
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class Libraries
	///
	/// \brief Класс-контейнер для библиотек-зависимостей
	///
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API Libraries {

		Map <Str, LibraryRef> libraries;

	public:
		
		~Libraries();
		
		///
		/// \brief Получение занимаемого размера памяти
		/// \return размер в байтах
		///
		virtual unsigned long long getSize();
		
		///
		/// \brief Получнеие количества библиотек
		/// \return количество библиотек
		///
		int getLibrariesCount() const;
		
		///
		/// \brief Получение библиотеки по индексу
		/// \param i индекс библиотеки
		/// \return библиотеку с индексом i
		///
		const Library *getLibrary(int i) const;
		
		
		///
		/// \brief Получение библиотеки по имени
		/// \param name индекс библиотеки
		/// \return библиотеку с именем name
		///
		const Library *getLibraryByName(const char *name) const;

		const Library *getLibraryByFileName(const char *file_name) const;
		
		///
		/// \brief Получение имени библиотеки с индексом i
		/// \param i индекс библиотеки
		/// \return имя библиотеки с индексом i
		///
		const char *getLibraryName(int i) const;
		
		///
		/// \brief Получение библиотеки по имени
		/// \param id имя библиотеки
		/// \return библиотеку с именем id или NULL в случае, если библиотека с заданным именем не существует
		///
		const Library *getLibrary(const char *id);
		
		///
		/// \brief Добавление библиотеки
		/// \param library добавляемая библиотека
		/// \return false в случае, если библиотека с таким именем уже существует
		///
		bool addLibrary(LibraryRef library);
		
		///
		/// \brief Создание блока по имени библиотеки и имени определения блока
		/// \param lib_name имя библиотеки
		/// \param id имя опрдления блока
		/// \param dummy флаг создания блока-пустышки
		/// \return новый созданный блок, либо NULL, если не найдена библиотека или определение
		///
		Neurone *create(const char *lib_name, const char *id, bool dummy = false, Neurone* dummyNode = NULL);
		
		///
		/// \brief Создание блока по имени определения блока
		/// \param id имя опрдления блока
		/// \param dummy флаг создания блока-пустышки
		/// \return новый созданный блок, либо NULL, если не найдено определение с таким именем ни в одной библиотеке
		///
		Neurone *create(const char *id, bool dummy = false, Neurone* dummyNode = NULL);
		
		///
		/// \brief Получение определения типа по TypeInfo
		/// \param type TypeInfo типа, для которого нужно получить определение
		/// \return определение типа, либо NULL в случае, если определение для запрашеваемого типа не существует
		///
		const TypeDef *getTypeDef(const TypeInfo &type) const;
		
		///
		/// \brief Получение определения типа по TypeInfo::id()
		/// \param id TypeInfo::id() типа, для которого нужно получить определение
		/// \return определение типа, либо NULL в случае, если определение для запрашеваемого типа не существует
		///
		const TypeDef *getTypeDefById(int id) const;
		
		///
		/// \brief Получение определения типа по TypeInfo::name()
		/// \param id TypeInfo::name() типа, для которого нужно получить определение
		/// \return определение типа, либо NULL в случае, если определение для запрашеваемого типа не существует
		///
		const TypeDef *getTypeDefByName(const char *name) const;
		
		///
		/// \brief Создание переменной по id типа
		/// \param id id типа
		/// \return переменная с типом id
		///
		Variable createVariableByType(int type_id);
		
		///
		/// \brief Создание переменной по имени типа
		/// \param id имя типа
		/// \return переменная с типом name
		///
		Variable createVariableByTypeName(const char *name);
		
		///
		/// \brief Загрузка библиотеки из файла
		/// \param filename имя файла
		/// \return true, если библиотека загружена успешно
		///
		bool loadLibrary(const char *filename);
		
		///
		/// \brief Какая-то фигня, которую никто даже не удосужился объяснить
		/// \param pluginName какая-то фигня
		/// \return что-то там
		///
		bool loadLibraryFormApi(const char *pluginName);
		
		///
		/// \brief Выгрузка библиотеки по имени
		/// \param name имя библиотеки
		///
		bool unloadLibrary(const char *name);
		
		///
		/// \brief Выгрузка библиотеки по индексу
		/// \param index индекс библиотеки
		///
		bool unloadLibrary(int index);
		
		///
		/// \brief Выгрузка и удаление всех библиотек
		///
		void clear();
		
		///
		/// \brief Проверка, загружена ли библиотека
		/// \param fileName имя файла библиотеки
		/// \return true если загружена
		///
		bool isLibraryLoaded(const char* fileName);
		
		///
		/// \brief setFormApiCallback
		/// \param formApiLoadCallback
		/// \bug Еще одна фигня, которая фиг знает, зачем нужна
		///
		static void setFormApiCallback(FormApiLoadCallback formApiLoadCallback);
		
	};
	
}

#endif // ANANDAMIDELIBRARY_H
