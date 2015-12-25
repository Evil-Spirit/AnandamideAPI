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

#ifndef ANANDAMIDE_SCRIPT_H
#define ANANDAMIDE_SCRIPT_H

#include "AnandamideLibAPI.h"
#include "AnandamideLogic.h"
#include "Array.h"
#include "Str.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	class Xml;
	class Logic;
	class Neurone;
	class Libraries;
	
	enum ScriptCompileState {
		SCS_NOT_COMPILED,
		SCS_COMPILED_OK,
		SCS_COMPILED_FAILED
	};
	
	///------------------------------------------------------------------------------
	///
	/// @class Script
	/// 
	/// @brief Класс скрипта.
	/// Класс Script служит для загрузки, компиляции и запуска скриптов
	///
	///------------------------------------------------------------------------------
	
	class ANANDAMIDE_API Script {
			
			friend class Logic;
			friend class ScriptLibrary;
			
			Array <Logic> logics;
			Logic *main;
			Libraries *libraries;

			bool is_shutdown;
			Action *shutdown_action;
			
			void *user_data;
			const Xml *loading;
			
			void preLoadLogic(const char *name_str);
			Str name;
			Str id;
			Str fileName;
			ScriptCompileState scriptCompileState;
			
		protected:
			
			Script();
			virtual ~Script();
			void generateId();
			
		public:
			
			///
			/// \brief Получение занимаемого размера памяти
			/// \return размер в байтах
			///
			virtual unsigned long long getSize();
			
			const char *getId() const;
			
			///
			/// \brief Функция создания экземпляра класса Script
			/// \return новый экземпляр класса Script
			/// 
			static Script *create();
			
			Script* clone();
			
			///
			/// \brief Уничтожение экземпляра класса Script
			///
			void destroy();
			
			///
			/// \brief Очистка содержимого скрипта (блоков, загруженных библиотек, уничтожение скомпилированного скрита)
			///
			void clear();
			
			///
			/// \brief Компиляция скрипта
			/// \return true, если операция успешна
			///
			bool compile();
			
			///
			/// \brief getCompileState возвращает состояние скомпилированности сценария
			/// \return 
			///
			ScriptCompileState getCompileState() const;
			
			///
			/// \brief Запуск скрипта (запуск Action "entry" у блока \"Main\").
			/// \return true, если операция успешна
			///
			bool run();

			///
			/// \brief Установка коллбэк-функции, выполняющейся при завершении скрипта
			/// \param action коллбэк-функция
			///
			void setShutdownAction(Action *action);
			
			///
			/// \brief Завершение скрипта.
			///
			void shutdown();

			///
			/// \brief Проверка на завершение работы
			/// \return состояние завершенности скрипта
			///
			bool isShutdown() const;
			
			///
			/// \brief Вызов update() у всех блоков
			/// \param dt время в секундах, прошедшее с предыдущего момента вызова update()
			///
			void update(float dt);
			
			///
			/// \brief Создание "нового" скрипта (очистка и создание блока "Main" и Action "entry")
			///
			void newScript();
			
			///
			/// \brief Получение количества исходных скриптовых блоков
			/// \return количество исходных скриптовых блоков внутри скрипта
			///
			int getLogicsCount();
			
			///
			/// \brief Получение скомпилированного главного блока.
			/// \return скомпилированный главный блок \"Main\". В случае, если скрипт не скопмпилирован, вернет NULL. 
			///
			Logic *getMain();

			///
			/// \brief Получение имени блока по индексу
			/// \param i индекс блока
			/// \return исходный скриптовых блок с индексом i
			///
			Logic *getLogic(int i);
			
			///
			/// \brief Добавление нового исходного блока в скрипт.
			/// \param name имя нового блока. 
			/// \return новый исходный блок или NULL в случае, если исходный блок с таким именем уже существует.
			///
			Logic *addLogic(const char *name);
			
			///
			/// \brief Добавление существующего исходного блока в скрипт
			/// \param logic существующий исходный блок
			/// \return false, если блок не был добавлен по причине того, что блок с таким именем уже существует
			///
			bool addLogic(Logic *logic);
			
			///
			/// \brief Поиск исходного блока по имени
			/// \param name_str имя исходного блока
			/// \return найденный исходный блок, либо NULL в случае, если исходный блок не найден
			///
			Logic *findLogic(const char *name_str);
			
			///
			/// \brief Поиск исходного блока по имени и получение его индекса
			/// \param name имя исходного блока
			/// \return индекс найденного исходного блока, либо -1 в случае, если исходный блок не найден
			///
			int indexOfLogic(const char *name);
			
			///
			/// \brief Удаление исходного блока
			/// \param logic исходный блок, который следует удалить
			///
			void removeLogic(Logic *logic);
			
			///
			/// \brief Создание блока (создается либо новый скриптовый блок из этого скрипта, либо блок из библиотеки)
			/// \param type имя типа блока
			/// \param dummy флаг создания блока-пустышки
			/// \return новый созданный блок, либо NULL в случае неудачи
			///
			Neurone *createNeurone(const char *type, const char *library, bool dummy = false, Neurone* dummyNode = NULL);
			
			///
			/// \brief Чтение скрипта из формата Xml
			/// \param xml объект Xml
			/// \param version версия файла
			/// \return false, в случае возникновения ошибок
			///
			bool readXml(const Xml *xml, int version, const char* scriptFileName);
			
			///
			/// \brief readXmlLibraries Чтение библиотек скрипта из формата Xml (этап 1)
			/// \param xml объект Xml
			/// \param version версия файла
			/// \return false, в случае возникновения ошибок
			///
			bool readXmlLibraries(const Xml *xml, int version, const char* scriptFileName);
			
			///
			/// \brief readXmlLogics Чтение логик скрипта из формата Xml (этап 2)
			/// \param xml объект Xml
			/// \param version версия файла
			/// \return false, в случае возникновения ошибок
			///
			bool readXmlLogics(const Xml *xml, int version);
			
			///
			/// \brief Запись скрипта в формат Xml
			/// \param xml объект Xml
			///
			void writeXml(Xml *xml, const char *script_file = NULL) const;
			
			///
			/// \brief Запись скприта в файл
			/// \param name имя файла
			///
			void save(const char *name);
			
			///
			/// \brief Загрузка скрипта из файла
			/// \param name имя файла
			/// \return false, в случае возникновения ошибок
			///
			bool load(const char *filename);
			
			///
			/// \brief loadLibraries загрузка библиотек из файла AND (этап 1)
			/// \param filename
			/// \return false, в случае возникновения ошибок
			///
			bool loadLibraries(const char *filename);
			
			///
			/// \brief loadLogics загрузка логик из файла AND (этап 2)
			/// \param filename
			/// \return false, в случае возникновения ошибок
			///
			bool loadLogics(const char *filename);
			
			bool loadScriptName(const char *filename);
			
			///
			/// \brief Получение загруженных библиотек
			/// \return загруженные библиотеки скрипта
			///
			Libraries *getLibraries();
			
			///
			/// \brief Получение имени скрипта
			/// \return имя скрипта
			///
			const char *getName() const;
			
			///
			/// \brief Установка нового имени скрипта. В случае, если скрипт используется как библиотека,
			/// имя скрипта будет являться именем библиотеки.
			/// \param name новое имя скрипта
			///
			void setName(const char *name);
			
			///
			/// \brief Установка пользовательских данных скрипта.
			/// \param пользовательский данные
			///
			void setUserData(void *data);
			
			///
			/// \brief Получение пользовательских данных скрипта
			/// \return пользовательский данные
			///
			void *getUserData() const;
			
			void renameNodes(const char* oldName, const char* newName);
			
			void removeLibraryNodes(Anandamide::Library* lib);
	};

}

#endif // ANANDAMIDESCRIPT_H
