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

#ifndef AnandamideNeuroneH
#define AnandamideNeuroneH

//------------------------------------------------------------------------------

#include "Map.h"

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideVariable.h"
#include "AnandamideAction.h"
#include "AnandamideEvent.h"
#include "AnandamideInput.h"
#include "AnandamideOutput.h"
#include "AnandamideParameter.h"

///-----------------------------------------------------------------------------
///
/// \namespace Anandamide
///
///-----------------------------------------------------------------------------

namespace Anandamide {
	
	class Xml;
	class Logic;
	class NeuroneDef;
	class Library;
	class Renderer;
	class NeuroneGroup;
	class LogicEditor;
	class Libraries;

	//------------------------------------------------------------------------------
	
	template <class Parent, class Child, class Param>
	class Creator {
		public:
			static Parent *create(Param param) {
				return new Child(param);
			}
	};
	
	//------------------------------------------------------------------------------
	
	template <class Parent, class Child>
	class Creator <Parent, Child, void> {
		public:
			static Parent *create() {
				return new Child();
			}
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class Neurone
	///
	/// \brief Базовый класс для всех блоков
	///  
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API Neurone {

		friend class Logic;
		friend class NeuroneGroup;
		friend class LogicEditor;

		int blockIndex;
		
		struct DesignInfo {
			Str name;										//!< имя блока
			Str comment;									//!< комментарий блока
			bool hovered				= false;			//!< редактирование: флаг наведения мыши на блок
			bool selected				= false;			//!< редактирование: флаг выделеня блока
			int pos_x					= 0;				//!< визуализация: позиция блока по x
			int pos_y					= 0;				//!< визуализация: позиция блока по y
			int error_code				= 0;				//!< код ошбки (0 - если ошибки нет)
			
			bool need_delete			= false;			//!< флаг необходимости удаления блока
			Common::Action *on_destroy	= NULL;				//!< функция, которая вызывается при удалении блока
			int markedActionNumber		= -1;
		};
		
	protected:
		
		Neurone* dummyNode;
		Neurone* runTimeNode;
		
		static float head_h;				//!< визуализация: высота заголовка
		static float flow_h;				//!< визуализация: высота выходов блока
		static float flow_w;				//!< визуализация: ширина выходов блока
		static float width;					//!< визуализация: полная ширина блока

		Map <Str, Parameter> parameters;	//!< параметры блока
		Map <Str, Action> actions;			//!< действия блока
		Map <Str, Event> events;			//!< события блока
		Map <Str, Input> inputs;			//!< входы блока
		Map <Str, Output> outputs;			//!< выходы блока

		Logic *logic;						//!< указатель на Logic, к которму принадлежит блок
		const NeuroneDef *def;				//!< указатель на определение блока
		bool dummy					= false;//!< флаг, выставляемый, когда блок является пустышкой
		
		mutable DesignInfo *design_info;
		DesignInfo *di();
		const DesignInfo *di() const;

		///
		/// \brief Установка необходимости удаления блока
		/// \param state необходимость удаления
		///
		void setNeedDelete(bool state);
		
	public:
		///
		/// \brief Получение занимаемого размера памяти
		/// \return размер в байтах
		///
		virtual unsigned long long getSize();

	protected:
		//----------------------------------------------------------------------
		// parameters
		//----------------------------------------------------------------------
		
		///
		/// \brief Добавление параметра
		/// \param id имя параметра
		/// \param item параметр
		/// \return добавленный параметр (item)
		///
		Parameter *addParameter(const char *id, Parameter *item);
		
		///
		/// \brief Удаление параметра по индексу. Удаление происходит только из контейнера параметров.
		/// \param index индекс параметра
		///
		void removeParameterInternal(int index);
		
		///
		/// \brief Удаление параметра по указателю. Удаление происходит только из контейнера параметров.
		/// \param event указатель на параметр
		/// 
		void removeParameterInternal(Parameter *event);
		
		//----------------------------------------------------------------------
		// events
		//----------------------------------------------------------------------
		
		///
		/// \brief Добавление события
		/// \param id имя события
		/// \param item событие
		/// \return добавленное событие (item)
		///
		Event *addEvent(const char *id, Event *item);
		
		///
		/// \brief Удаление события по индексу. Удаление происходит только из контейнера событий.
		/// \param index индекс события
		///
		void removeEventInternal(int index);
		
		///
		/// \brief Удаление события по указателю. Удаление происходит только из контейнера событий.
		/// \param event указатель на событие
		/// 
		void removeEventInternal(Event *event);
		
		///
		/// \brief Переименование события. Переименование происходит только в контейнере событий.
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameEventInternal(const char *old_name, const char *new_name);

		//----------------------------------------------------------------------
		// inputs
		//----------------------------------------------------------------------
		
		///
		/// \brief Добавление входного параметра
		/// \param id имя входного параметра
		/// \param item входной параметр для добавления
		/// \return добавленный входной параметр (item)
		///
		Input *addInput(const char *id, Input *item);
		
		///
		/// \brief Удаление входного параметра по индексу.  Удаление происходит только из контейнера входных параметров.
		/// \param index индекс входного параметра
		///
		void removeInputInternal(int index);
		
		///
		/// \brief Удаление входного параметра по указателю. Удаление происходит только из контейнера входных параметров.
		/// \param item указатель на входной параметр
		///
		void removeInputInternal(Input *item);
		
		///
		/// \brief Переименование входного параметра. Переименование происходит только в контейнере входных параметров.
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameInputInternal(const char *old_name, const char *new_name);

		//----------------------------------------------------------------------
		// outputs
		//----------------------------------------------------------------------
		
		///
		/// \brief Добавление выходного параметра
		/// \param id имя выходного параметра
		/// \param item выходной параметр для добавления
		/// \return добавленный выходной параметр (item)
		///
		Output *addOutput(const char *id, Output *item);
		
		///
		/// \brief Удаление выходного параметра по индексу.  Удаление происходит только из контейнера выходных параметров.
		/// \param index индекс выходного параметра
		///
		void removeOutputInternal(int index);
		
		///
		/// \brief Удаление выходного параметра по указателю. Удаление происходит только из контейнера выходных параметров.
		/// \param item указатель на выходной параметр
		///
		void removeOutputInternal(Output *item);
		
		///
		/// \brief Переименование выходного параметра. Переименование происходит только в контейнере выходных параметров.
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameOutputInternal(const char *old_name, const char *new_name);

		//----------------------------------------------------------------------
		// actions
		//----------------------------------------------------------------------
		
		///
		/// \brief Добавление действия
		/// \param id имя действия
		/// \param item действие для добавления
		/// \return добавленное действие (item)
		///
		Action *addAction(const char *id, Action *item);
		
		///
		/// \brief Удаление действия по индексу.  Удаление происходит только из контейнера действий.
		/// \param index индекс действия параметра
		///
		void removeActionInternal(int index);
		
		///
		/// \brief Удаление действия по указателю. Удаление происходит только из контейнера действий.
		/// \param item указатель на действие
		///
		void removeActionInternal(Action *action);
		
		///
		/// \brief Переименование действия. Переименование происходит только в контейнере действий.
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameActionInternal(const char *old_name, const char *new_name);
		
		///-------------------------------------------------------------------------------------------------------------
		///
		/// derived Neurone interface
		///
		///-------------------------------------------------------------------------------------------------------------
		
		///
		/// \brief Перегружаемая функция для пользовательской отрисовки блоков
		/// \param renderer рендерер
		///
		virtual void onRender(Renderer *renderer);
		
		///
		/// \brief Функция, котороая вызывается, когда состояние выделения блока меняется
		///
		virtual void onSelected();
		
		///
		/// \brief Функция, котороая вызывается, когда состояние наведения мыши на блок меняется
		///
		virtual void onHovered();
		
		///
		/// \brief Функция, котороая вызывается при инициализации нейрона
		/// \param libraries список библиотек скрипта, в котором создается нейрон
		///
		virtual void onInit(Libraries *libraries);
		
		virtual void onShutdown();
		
		///
		/// \brief Функция, котороая вызывается для валидации блока
		///
		virtual void onInvalidate();

		///
		/// \brief Функция, которая автоматически вызывается для получения второй строки заголовка блока.
		/// \details Функция может быть перегружена для того, чтобы возвращать вторую строку расширенного заголовка блока,
		/// тем самым расширяя визуальное представление блока дополнительной информацией. Вторая строка заголовка будет
		/// выводиться только когда перегруженная функция hasCaption() возвращает true.
		/// \return должна возвращать строку, котрая будет выводиться второй сторкой в заголовке блока.
		///
		virtual Str onGetCaption() const;

		///
		/// \brief Запись блока в xml
		/// \param xml объект Xml, куда производится запись
		///
		virtual void onWrite(Xml *xml) const;

		///
		/// \brief Чтение параметров из Xml
		/// \param xml Xml
		///
		virtual void onReadParameters(const Xml *xml);

		///
		/// \brief Чтение событий из Xml
		/// \param xml Xml
		///
		virtual void onReadEvents(const Xml *xml);
		
	public:
		
		Neurone();
		
		virtual ~Neurone();
		
		///
		/// \brief Функция, которая автоматически вызывается перед созданием блока.
		/// \param def определение блока
		///
		void create(Libraries *libraries, const NeuroneDef *def);
		
		//----------------------------------------------------------------------
		// name
		//----------------------------------------------------------------------

		///
		/// \brief Получение имени блока
		/// \return имя блока
		///
		const char *getName() const;
		
		///
		/// \brief Получение заголовка блока
		/// \return имя блока
		///
		const char *getHeader() const;
		
		///
		/// \brief Установка имени блока
		/// \param name новое имя блока
		///
		void setName(const char *name);
		
		//----------------------------------------------------------------------
		// 
		//----------------------------------------------------------------------
		
		///
		/// \brief Установка кода ошибки
		/// \param code код ошибки
		///
		void setError(int code);
		
		///
		/// \brief Получение кода ошибки
		/// \return код ошибки
		///
		int getError();
		
		///
		/// \brief Получение имени экземпляра блока
		/// \return имя экземпляра блока
		///
		virtual const char *getInstance() const;
		
		///
		/// \brief Получение типа блока
		/// \return тип блока
		///
		virtual const char *getType() const;

		///
		/// \brief Функция, которая автоматически вызвается для объявления определения блока. 
		/// \details Наследуемые блоки должны перегружать эту функцию для определения количества и типов выводов блока.
		/// <br>Для этого необходимо заполнить объект класса NeuroneDef, указатель на который передается в виде параметра.
		/// <br>Функция вызывается один раз при регистрации типа блока в библиотеке. Вручную не взывать.
		/// \param definition указатель на объект класса NeuroneDef, в котором нужно сформировать определение блока. 
		///
		static void define(NeuroneDef *definition);\
		
		///
		/// \brief Функция, которая автоматически вызывается при создании нового экземпляра блока.
		/// \details Вызывается для каждого нового созданного блока. Может быть
		/// перегружена с целью инициализации пользовательского блока. Вручную не вызывать.
		/// \deprecated используйте или создайте protected Anandamide::Neurone::onInit
		///
		virtual void init();
		
		///
		/// \brief Функция, которая автоматически вызвается для обеспечения работы логики блока во времени. 
		/// \details Функция может быть перегружена, если пользовательскому
		/// типу блоков требуется логика работы во времени (например: таймеры, асинхронные события, факт появления 
		/// которых проверяется в главном цикле приложения)
		/// \param dt время в секундах, прошедшее с предыдущего вызова update
		/// \deprecated используйте или создайте protected Anandamide::Neurone::onUpdate
		///
		virtual void update(float dt);
		
		///
		/// \brief Функция, которая автоматически вызывается для отображения блока
		/// \param renderer объект, через который осуществляется прорисовка
		///
		void render(Renderer *renderer);
		
		///
		/// \brief Функция, которая автоматически вызывается для валидации пользовательского блока.
		/// \details Может быть перегружена в случае, когда количество или тип
		/// аргументов блока зависят от параметра. Для того, чтобы функция вызывалась в редакторе блоков, необходимо также,
		/// чтобы функция NeuroneDef::setCreateDummy(true) была вызвана при определении блока в функции define()
		/// \deprecated используйте или создайте protected Anandamide::Neurone::onInvalidate
		///
		virtual void invalidate();
		
		///
		/// \brief afterCompile Функция, которая автоматически вызывается после успешной компиляции скрипта
		/// \deprecated используйте или создайте protected Anandamide::Neurone::onAfterCompile
		///
		virtual void afterCompile();
		
		///
		/// \brief Функция, которая автоматически вызывается для получения второй строки заголовка блока.
		/// \details Функция может быть перегружена для того, чтобы возвращать вторую строку расширенного заголовка блока,
		/// тем самым расширяя визуальное представление блока дополнительной информацией. Вторая строка заголовка будет
		/// выводиться только когда перегруженная функция hasCaption() возвращает true.
		/// \return должна возвращать строку, котрая будет выводиться второй сторкой в заголовке блока.
		/// \deprecated используйте или создайте protected Anandamide::Neurone::onGetCaption
		///
		virtual const char *getCaption();
		
		///
		/// \brief Функция, которая автоматически вызвается для определения, имеет ли текущий блок вторую строку заголовка.
		/// \return должна возвращать true, в случае, когда необходимо выводить во вторую строку заголовка текст,
		/// возвращаемый функцией getCaption()
		///
		virtual bool hasCaption();
		
		const char *getComment() const;
		bool hasComment() const;
		void setComment(const char *comment);
		
		///
		/// \brief Функция, которая вызвается, когда необходимо сделать клонирование блока.
		/// \details Может быть перегружена, когда необходимо сделать нетривиальное клонирование блока.
		/// \return новый блок-клон текущего
		///
		virtual Neurone *clone() const;
		
		///
		/// \brief Функция, которая вызвается, когда необходимо сделать клонирование блока в блок-пустышку.
		/// \details Может быть перегружена, когда необходимо сделать нетривиальное клонирование блока.
		/// \return новый блок-пустышка, клон интерфейса текущего
		///
		virtual Neurone *dummyClone() const;
		
		///
		/// \brief Функция, которая вызывается, когда блоку назначается объект логики
		/// \param объект Logic, в который помещается данный блок
		///
		virtual void onSetLogic(Logic *logic);
		
		///
		/// \brief Функция, которая автоматически вызывается, когда нужно установить указатель на Logic, 
		/// к которому приндлежит текущий блок. Врчуную не вызывать.
		/// \param logic указатель на Logic
		///
		void setLogic(Logic *logic);
		
		///
		/// \brief Получение указателя на Logic, к которому принадлежит текущий блок.
		/// \return указатель на Logic
		///
		Logic *getLogic();
		
		///
		/// \brief Получение корневого указателя на Logic, к которому принадлежит текущий блок
		/// \return указатель на Logic
		///
		Logic *getRootLogic();
		
		///
		/// \brief Константная версия получения указателя на Logic, к которому принадлежит текущий блок.
		/// \return указатель на Logic
		///
		const Logic *getLogic() const;
		
		// Проверка, является ли данный блок блоком-пустышкой
		bool isDummy() const;
		
		///
		/// \brief Функция, которая автоматически вызывается в случае, когда необходимо указать, что данный блок является
		/// блоком-пустышкой. Вручную не взывать.
		/// \param state флаг блока-пустышки
		///
		void setDummy(bool state);
		
		void setDummyNode(Neurone* dummyNode);
		
		Neurone* getDummyNode() const;
		
		void setRunTimeNode(Neurone* runTimeNode);
		
		Neurone* getRunTimeNode() const;
		
		///
		/// \brief Функция для определения, является ли блок кастомным
		/// \return true - если блок кастомный
		///
		virtual bool isCustom();
		
		///
		/// \brief Функция для определения, попадает ли курсор мыши на активный элемент кастомного блока
		/// \param x координата курсора
		/// \param y коррдината курсора
		/// \return true - если попадает
		///
		virtual bool isActive(int x, int y);

		//----------------------------------------------------------------------
		// parameters
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение количества параметров блока
		/// \return количество параметров блока
		///
		int getParametersCount() const;
		
		///
		/// \brief Получение параметра по индексу (константная версия)
		/// \param индекс параметра
		/// \return параметр с индексом i
		///
		const Parameter &getParameter(int i) const;
		
		///
		/// \brief Получение параметра по индексу
		/// \param индекс параметра
		/// \return параметр с индексом i
		///
		Parameter &getParameter(int i);
		
		///
		/// \brief Получение параметра по имени (константная версия)
		/// \param id имя параметра
		/// \return параметр с именем id, или NULL в случае, если параметра с таким именем не существует
		///
		const Parameter *getParameter(const char *id) const;
		
		///
		/// \brief Получение параметра по имени
		/// \param id имя параметра
		/// \return параметр с именем id, или NULL в случае, если параметра с таким именем не существует
		///
		Parameter *getParameter(const char *id);
		
		///
		/// \brief Получение имени параметра по индексу
		/// \param i индекс параметра
		/// \return имя параметра с индексом i
		///
		const char *getParameterName(int i) const;

		//----------------------------------------------------------------------
		// actions
		//----------------------------------------------------------------------

		///
		/// \brief Получение индекса действия по указателю на действие
		/// \param action указатель на действие
		/// \return индекс действия, либо -1 в случае, если элемент не найден
		///
		int indexOf(const Action *action) const;
		
		///
		/// \brief Получнеие количества действий блока
		/// \return количество действий блока
		///
		int getActionsCount() const;
		
		///
		/// \brief Полуение действия по индексу (константная версия)
		/// \param i индекс действия
		/// \return действие с индексом i
		///
		const Action &getAction(int i) const;
		
		///
		/// \brief Полуение действия по индексу
		/// \param i индекс действия
		/// \return действие с индексом i
		///
		Action &getAction(int i);
		
		///
		/// \brief Получение действия по имени
		/// \param id имя действия
		/// \return действие с именем id, или NULL, в случае, если элемент не найден
		///
		const Action *getAction(const char *id) const;
		
		///
		/// \brief Получение действия по имени (константная версия)
		/// \param id имя действия
		/// \return действие с именем id, или NULL, в случае, если элемент не найден
		///
		Action *getAction(const char *id);
		
		///
		/// \brief Получнеие имени действия по индексу
		/// \param i индекс действия
		/// \return имя действия с индексом i
		///
		const char *getActionName(int i) const;
		
		// for Unigine
		Action *getActionByName(const char *id);
		
		//----------------------------------------------------------------------
		// events
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение индекса события по указателю на него
		/// \param event указатель на событие
		/// \return индекс события или -1 в случае, если элемент не найден
		///
		int indexOf(const Event *event) const;
		
		///
		/// \brief Получение количества событий блока
		/// \return количество событий блока
		///
		int getEventsCount() const;
		
		///
		/// \brief Получение события по индексу (константная версия)
		/// \param i индекс события
		/// \return событие с индексом i
		///
		const Event &getEvent(int i) const;
		
		///
		/// \brief Получение события по индексу
		/// \param i индекс события
		/// \return событие с индексом i
		///
		Event &getEvent(int i);
		
		///
		/// \brief Получение события по имени (константная версия)
		/// \param id имя событие
		/// \return событие с именем id или NULL в случае, если элемент не найден
		///
		const Event *getEvent(const char *id) const;
		
		///
		/// \brief Получение события по имени
		/// \param id имя событие
		/// \return событие с именем id или NULL в случае, если элемент не найден
		///
		Event *getEvent(const char *id);
		
		///
		/// \brief Получение имени события по индексу
		/// \param i индекс события
		/// \return имя события с индексом i
		///
		const char *getEventName(int i) const;

		//----------------------------------------------------------------------
		// inputs
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение индекса входного параметра по указателю на него
		/// \param input указатель на входной параметр
		/// \return индекс входного параметра или -1 в случае, если элемент не найден
		///
		int indexOf(const Input *input) const;	
		
		///
		/// \brief Получение количества входных параметров
		/// \return количество входных параметров
		///
		int getInputsCount() const;
		
		///
		/// \brief Получение входного параметра по индексу (константная версия)
		/// \param i индекс входного параметра
		/// \return входной параметр с индексом i
		///
		const Input &getInput(int i) const;
		
		///
		/// \brief Получение входного параметра по индексу
		/// \param i индекс входного параметра
		/// \return входной параметр с индексом i
		///
		Input &getInput(int i);
		
		///
		/// \brief Получение входного параметра по имени (константная версия)
		/// \param id имя входного параметра
		/// \return входной параметр с именем id или NULL в случе, если элемент не найден
		///
		const Input *getInput(const char *id) const;
		
		///
		/// \brief Получение входного параметра по имени
		/// \param id имя входного параметра
		/// \return входной параметр с именем id или NULL в случе, если элемент не найден
		///
		Input *getInput(const char *id);
		
		///
		/// \brief Получение имени входного параметра по индексу
		/// \param i индекс входного параметра
		/// \return имя входного параметра с индексом i
		///
		const char *getInputName(int i) const;

		//----------------------------------------------------------------------
		// outputs
		//----------------------------------------------------------------------

		///
		/// \brief Получение индекса выходного параметра по указателю на него
		/// \param output указатель на выходной параметр
		/// \return индекс выходного параметра или -1 в случае, если элемент не найден
		///
		int indexOf(const Output *output) const;
		
		///
		/// \brief Получение количества выходных параметров
		/// \return количество выходных параметров
		///
		int getOutputsCount() const;
		
		///
		/// \brief Получение выходного параметра по индексу (константная версия)
		/// \param i индекс выходного параметра
		/// \return выходной параметр с индеком i
		///
		const Output &getOutput(int i) const;
		
		///
		/// \brief Получение выходного параметра по индексу
		/// \param i индекс выходного параметра
		/// \return выходной параметр с индеком i
		///
		Output &getOutput(int i);
		
		///
		/// \brief Получение выходного параметра по имени (константная версия)
		/// \param id имя выходного параметра
		/// \return выходной параметр с именем id или NULL в случае, если элемент не найден
		///
		const Output *getOutput(const char *id) const;
		
		///
		/// \brief Получение выходного параметра по имени
		/// \param id имя выходного параметра
		/// \return выходной параметр с именем id или NULL в случае, если элемент не найден
		///
		Output *getOutput(const char *id);
		
		///
		/// \brief Получение имени выходного параметра по индексу
		/// \param i индекс выходного параметра
		/// \return имя выходного параметра с индексом i
		///
		const char *getOutputName(int i) const;

		//----------------------------------------------------------------------
		// geometry
		//----------------------------------------------------------------------

		///
		/// \brief Получение позиции блока по координате x
		/// \return x координату позиции блока
		///
		int getPosX() const;
		
		///
		/// \brief Установка позиции блока по координате x
		/// \param x новая позиция блока по координате x
		///
		void setPosX(int x);
		
		///
		/// \brief Получение позиции блока по координате y
		/// \return y координату позиции блока
		///
		int getPosY() const;
		
		///
		/// \brief Установка позиции блока по координате y
		/// \param y новая позиция блока по координате y
		///
		void setPosY(int pos_);
		
		///
		/// \brief Получение ширины блока
		/// \return ширину блока
		///
		virtual int getWidth();
		
		///
		/// \brief Получение высоты блока
		/// \return высоту блока
		///
		virtual int getHeight();
		
		int getRealPosX();
		int getRealPosY();
		int getRealWidth();
		int getRealHeight();
		
		///
		/// \brief Получение высоты заголовка блока
		/// \return высоту заголовка блока
		///
		int getHeaderHeight();
		
		///
		/// \brief Получение высоты действий блока
		/// \return высоту дейсвтвий блока
		///
		int getActionsHeight();
		
		///
		/// \brief Получение высоты входных параметров блока
		/// \return высоту входных параметров блока
		///
		int getInputsHeight();
		
		///
		/// \brief Получение высоты событий блока
		/// \return высоту событий блока
		///
		int getEventsHeight();
		
		///
		/// \brief Получение высоты выходных параметров блока
		/// \return высоту выходных параметров блока
		///
		int getOutputsHeight();
		
		///
		/// \brief Установка позиции блока
		/// \param x значение координаты x новой позиции блока
		/// \param y значение координаты y новой позиции блока
		///
		void setPos(int x, int y);
		
		///
		/// \brief Получение начального угла для отрисовки соединительного сплайна
		/// \param index номер действия
		/// \return угол в градусах
		///
		virtual float getActionWireAngle(int index);
		
		///
		/// \brief Получение конечного угла для отрисовки соединительного сплайна
		/// \param index номер события
		/// \return угол в градусах
		///
		virtual float getEventWireAngle(int index);
		
		///
		/// \brief Получение начального угла для отрисовки соединительного сплайна
		/// \param index номер входа
		/// \return угол в градусах
		///
		virtual float getInputWireAngle(int index);
		
		///
		/// \brief Получение конечного угла для отрисовки соединительного сплайна
		/// \param index номер выхода
		/// \return угол в градусах
		///
		virtual float getOutputWireAngle(int index);
		
		//----------------------------------------------------------------------
		// selection
		//----------------------------------------------------------------------
		
		///
		/// \brief Установка состояния наведения мыши на блок
		/// \param state состояние наведения мыши на блок. true - блок выделен
		///
		void setHovered(bool state);
		
		///
		/// \brief Проверка наведения мыши на блок
		/// \return true, если блок выделен
		///
		bool isHovered() const;
		
		///
		/// \brief Установка состояния выделения блока
		/// \param state состояние выделния блока. true - блок выделен
		///
		void setSelected(bool state);
		
		///
		/// \brief Проверка выдления блока
		/// \return true, если блок выделен
		///
		bool isSelected() const;
		
		///
		/// \brief Проверка выдления блока по координатам
		/// \param x кооридната x точки
		/// \param y координата y точки
		/// \return true, если точка с координатами x,y лежит внутри блока и не попадает в действия, события, входные
		/// или выходные параметры
		///
		virtual bool isSelected(int x, int y);
		
		Action *getSelectedAction(int x, int y);
		
		///
		/// \brief Проверка выделения действия по координатам
		/// \param x кооридната x точки
		/// \param y координата y точки
		/// \return индекс действия, если точка попадает в действие или -1 в случае, если не попадает
		///
		virtual int isActionSelected(int x, int y);
		
		///
		/// \brief Проверка выделения события по координатам
		/// \param x кооридната x точки
		/// \param y координата y точки
		/// \return индекс события, если точка попадает в событие или -1 в случае, если не попадает
		///
		virtual int isEventSelected(int x, int y);
		
		///
		/// \brief Проверка выделения входного параметра по координатам
		/// \param x кооридната x точки
		/// \param y координата y точки
		/// \return индекс входного парметра, если точка попадает во входной параметр или -1 в случае, если не попадает
		///
		virtual int isInputSelected(int x, int y);
		
		///
		/// \brief Проверка выделения выходного параметра по координатам
		/// \param x кооридната x точки
		/// \param y координата y точки
		/// \return индекс выходного парметра, если точка попадает в выходной параметр или -1 в случае, если не попадает
		///
		virtual int isOutputSelected(int x, int y);

		//----------------------------------------------------------------------
		// link positions
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение позиции связи для действия
		/// \param i индекс действия
		/// \return координаты позиции связи
		///
		virtual vec3 getActionPos(int i);
		
		///
		/// \brief Получение позиции связи для события
		/// \param i индекс действия
		/// \return координаты позиции связи
		///
		virtual vec3 getEventPos(int i);
		
		///
		/// \brief Получение позиции связи для входного параметра
		/// \param i индекс действия
		/// \return координаты позиции связи
		///
		virtual vec3 getInputPos(int i);
		
		///
		/// \brief Получение позиции связи для выходного параметра
		/// \param i индекс действия
		/// \return координаты позиции связи
		///
		virtual vec3 getOutputPos(int i);

		//----------------------------------------------------------------------
		// serialization
		//----------------------------------------------------------------------
		
		///
		/// \brief Запись блока в xml
		/// \param xml объект Xml, куда производится запись
		///
		void write(Xml *xml) const;
		
		///
		/// \brief Чтение параметров из Xml
		/// \param xml Xml
		///
		void readParameters(const Xml *xml);
		
		///
		/// \brief Чтение событий из Xml
		/// \param xml Xml
		///
		void readEvents(const Xml *xml);

		//----------------------------------------------------------------------
		// 
		//----------------------------------------------------------------------
		
		///
		/// \brief Проверка, помечен ли блок для удаления
		/// \return true, если блок помечен, как удаленный
		///
		bool isNeedDelete();
		
		///
		/// \brief Установка функции, которую необходимо вызывать, когда блок удаляется
		/// \param action функция, которая вызывается при удалении блока
		/// \todo возможно, не используется
		///
		void setOnDestroyAction(Common::Action *action);
		
		/// 
		/// \brief Получение функции, которая вызвается при удалении блока
		/// \return функцию, которая вызывается при удалении блока
		/// \todo возможно, не используется
		///
		Common::Action *getOnDestroyAction();
		
		///
		/// \brief Получение определения блока
		/// \return опредление блока
		///
		const NeuroneDef *getDef() const;
		
		///
		/// \brief Получение библиотеки блока
		/// \return библиотека блока
		///
		const Library *getLibrary() const;
		
		///
		/// \brief Получение имени библиотеки блока
		/// \return имя библиотеки блока
		///
		virtual const char *getLibraryName() const;
				
		//----------------------------------------------------------------------
		// Runtime events
		//----------------------------------------------------------------------
		
		///
		/// \brief Событие движения мыши, вызывается, если блок выделен
		/// \param x координата курсора
		/// \param y координата курсора
		/// \param button кнопка мыши
		/// \param buttons кнопки мыши
		/// \param keyboard_modifiers модификаторы клавиатуры
		///
		virtual void onMouseMove(int x, int y, int button, int buttons, int keyboard_modifiers);
		
		///
		/// \brief Событие нажатия клавиши мыши, вызывается, если блок выделен
		/// \param x координата курсора
		/// \param y координата курсора
		/// \param button кнопка мыши
		/// \param buttons кнопки мыши
		/// \param keyboard_modifiers модификаторы клавиатуры
		///
	    virtual void onMousePress(int x, int y, int button, int buttons, int keyboard_modifiers);
		
		///
		/// \brief Событие отпускания клавиши мыши, вызывается, если блок выделен
		/// \param x координата курсора
		/// \param y координата курсора
		/// \param button кнопка мыши
		/// \param buttons кнопки мыши
		/// \param keyboard_modifiers модификаторы клавиатуры
		///
	    virtual void onMouseRelease(int x, int y, int button, int buttons, int keyboard_modifiers);
		
		///
		/// \brief Событие двойного нажатия клавиши мыши, вызывается, если блок выделен
		/// \param x координата курсора
		/// \param y координата курсора
		/// \param button кнопка мыши
		/// \param buttons кнопки мыши
		/// \param keyboard_modifiers модификаторы клавиатуры
		///
	    virtual void onMouseDoubleClick(int x, int y, int button, int buttons, int keyboard_modifiers);
		
		///
		/// \brief Событие нажатия клавиши клавиатуры, вызывается, если блок выделен
		/// \param key клавиши
		/// \param keyboard_modifiers модификаторы клавиатуры
		/// \param data текст
		///
	    virtual void onKeyPress(int key, int keyboard_modifiers, const char *data);
		
		///
		/// \brief Событие отпускания клавиши клавиатуры, вызывается, если блок выделен
		/// \param key клавиши
		/// \param keyboard_modifiers модификаторы клавиатуры
		/// \param data текст
		///
		virtual void onKeyRelease(int key, int keyboard_modifiers, const char *data);
		
		int getBlockIndex() const;
		void setBlockIndex(int value);
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class NeuroneGroup
	///
	/// \brief Класс группы блоков
	///  
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API NeuroneGroup {
		
		friend class Logic;
		
		// Text sizes cache
		struct ANANDAMIDE_API NeuroneTextCache {
			NeuroneTextCache() { height = 0; width = 0; }
			Str text;
			float height;
			float width;
		};
		
		NeuroneTextCache commentCache;
		
		void checkTextSizes(Renderer *renderer);
		
	protected:
		Str comment;						//!< комментарий группы
		
		Array <Neurone> nodes;				//!< блоки группы
		
		Logic *logic;						//!< указатель на Logic, к которму принадлежит группа
		
		bool selected;						//!< редактирование: флаг выделеня блока
		bool need_delete;					//!< флаг необходимости удаления блока
		
		void setNeedDelete(bool state);
		void getGeometry(int &left, int &top, int &width, int &height) const;
		
		float colorR, colorG, colorB;
		
	public:
		
		NeuroneGroup();
		
		~NeuroneGroup();
		
		///
		/// \brief Получение занимаемого размера памяти
		/// \return размер в байтах
		///
		virtual unsigned long long getSize();
		
		void setLogic(Logic *logic);
		Logic *getLogic();
		
		void render(Renderer *renderer);
		
		const char *getComment();
		bool hasComment();
		void setComment(const char *comment);
		
		NeuroneGroup *clone() const;
		
		int getPosX() const;
		int getPosY() const;
		int getWidth() const;
		int getHeight() const;
		
		void setSelected(bool state);
		bool isSelected() const;
		bool isSelected(int x, int y) const;
		
		void write(Xml *xml) const;
		void read(const Xml *xml);
		
		bool isNeedDelete();
		
		bool addNode(Neurone* node);
		bool removeNode(Neurone* node);
		bool removeNode(int index);
		bool isNodeContains(Neurone* node) const;
		
		int getNodesCount() const;
		int getNodeIndex(Neurone* node);
		Neurone &getNode(int index);
		
		void getColor(float &r, float &g, float &b);
		void setColor(float r, float g, float b);
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class NeuroneDummy
	/// 
	/// \brief Класс блока-пустышки
	///
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API NeuroneDummy : public Neurone {
					
		public:
			
			NeuroneDummy();
			NeuroneDummy(const Neurone &neurone);
			
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class NeuroneDef
	///	
	/// \brief Класс определения блока
	/// 
 	///-------------------------------------------------------------------------

	class ANANDAMIDE_API NeuroneDef {
			
		typedef Neurone*(*NeuroneCreateFunction)();
		friend class Neurone;
		friend class Library;

		Str name;
		Str kind;
		Str header;
		
		bool can_create_dummy;
		bool hidden;
		
		NeuroneCreateFunction create_function;

		struct ActionDef {
			Str name;
		};

		struct EventDef {
			Str name;
		};

		struct ParameterDef {
			Str name;
			Str value;
			int type_id;
		};

		struct InputDef {
			Str name;
			Str value;
			int type_id;
		};

		struct OutputDef {
			Str name;
			Str value;
			int type_id;
		};

		void *data;
		void *container;
		
		Array <ParameterDef> parameters;
		Array <ActionDef> actions;
		Array <EventDef> events;
		Array <InputDef> inputs;
		Array <OutputDef> outputs;
		
		Library *library;
		
	protected:
		
		NeuroneDef();
		
	public:
		
		///
		/// \brief Создание опредления блока
		/// \return новое опредление блока
		///
		static NeuroneDef *create();
		
		virtual ~NeuroneDef();
		
		///
		/// \brief Получение занимаемого размера памяти
		/// \return размер в байтах
		///
		virtual unsigned long long getSize();
		
		///
		/// \brief Получение имени определения блока
		/// \return имя определения блока
		///
		const char *getName() const;
		
		///
		/// \brief Установка имени определения блока
		/// \param name новое имя определения блока
		///
		void setName(const char *name);
		
		///
		/// \brief Получение строки заголовка блока
		/// \return строку заголовка блока
		///
		const char *getHeader() const;
		
		///
		/// \brief Установка строки заголовка блока
		/// \param name новая строка заголовка определения блока
		///
		void setHeader(const char *header);
		
		///
		/// \brief Получение строки типа-классификатора блока
		/// \return строку типа-классификатора блока
		///
		const char *getKind() const;
		
		///
		/// \brief Установка типа-классификатора блока
		/// \param строка тип-классификатор
		///
		void setKind(const char *kind);
		
		void setHidden(bool hidden);
		
		bool isHidden() const;
		
		///
		/// \brief Функция, которая вызвается для создания нового блока
		/// \param dummy флаг создания блока-пустышки
		/// \return новый блок
		///
		virtual Neurone *createNeurone(Libraries *libraries, bool dummy = false, Neurone* dummyNode = NULL) const;
		
		///
		/// \brief Получение функции создания блока
		/// \return указатель на функцию создания блока
		///
		NeuroneCreateFunction getCreateFunction() const;
		
		///
		/// \brief Установка функции создания блока
		/// \param указатель на функцию создания блока
		///
		void setCreateFunction(NeuroneCreateFunction func);
		
		///
		/// \brief Добавление входного параметра
		/// \param name название параметра
		/// \param value значение по умолчанию
		/// \param type тип параметра
		///
		void addInput(const char *name, const char *value, const TypeInfo &type);
		
		///
		/// \brief Получнеие количества входных параметров
		/// \return количество входных параметров
		///
		int getInputsCount() const;
		
		///
		/// \brief Получение входного параметра по индексу
		/// \param i индекс входного параметра
		/// \return входной параметр с индексом i
		///
		const InputDef &getInput(int i) const;
		
		///
		/// \brief добавление выходного параметра
		/// \param name название параметра
		/// \param value значение по умолчанию
		/// \param type тип параметра
		///
		void addOutput(const char *name, const char *value, const TypeInfo &type);
		
		///
		/// \brief Получение количства выходных параметров
		/// \return количество выходных параметров
		///
		int getOutputsCount() const;
		
		///
		/// \brief Получение выходного параметра по индексу
		/// \param i индекс выходного параметра
		/// \return выходной параметр с индексом i
		///
		const OutputDef &getOutput(int i) const;
		
		///
		/// \brief Добавление скрытого параметра
		/// \param name имя параметра
		/// \param value значение параметра по умолчанию
		/// \param type тип параметра
		///
		void addParameter(const char *name, const char *value, const TypeInfo &type);
		
		///
		/// \brief Получение количства параметров
		/// \return количество параметров
		///
		int getParametersCount() const;
		
		///
		/// \brief Получение параметра по индексу
		/// \param i индекс параметра
		/// \return параметр с индексом i
		///
		const ParameterDef &getParameter(int i) const;
		
		///
		/// \brief Получение параметра по имени
		/// \param name имя параметра
		/// \return параметр с именем name
		///
		const ParameterDef *getParameter(const char *name);
		
		///
		/// \brief Добавление входного параметра
		/// \param name имя параметра
		/// \param value значение по умолчанию
		///	\param T тип параметра
		/// 
		template <typename T>
		int addInput(const char *name, const char *value) {
			addInput(name, value, TYPEID(T));
			return getInputsCount() - 1;
		}
		
		///
		/// \brief Добавление выходного параметра
		/// \param name имя параметра
		/// \param value значение по умолчанию
		///	\param T тип параметра
		/// 
		template <typename T>
		void addOutput(const char *name, const char *value) {
			addOutput(name, value, TYPEID(T));
		}
		
		///
		/// \brief Добавление скрытого параметра
		/// \param name имя параметра
		/// \param value значение по умолчанию
		///	\param T тип параметра
		/// 
		template <typename T>
		void addParameter(const char *name, const char *value) {
			addParameter(name, value, TYPEID(T));
		}
		
		///
		/// \brief Добавление действия
		/// \param name имя действия
		///
		void addAction(const char *name);
		
		int getActionsCount() const;

		///
		/// \brief Получение действия по индексу
		/// \param i индекс действия
		/// \return действие с индексом i
		///
		const ActionDef &getAction(int i) const;
		
		///
		/// \brief Добавление события
		/// \param name имя события
		///
		void addEvent(const char *name);
		
		int getEventsCount() const;

		///
		/// \brief Получение события по индексу
		/// \param i индекс события
		/// \return событие с индексом i
		///
		const EventDef &getEvent(int i) const;
		
		///
		/// \brief Установка данных
		/// \param data данные
		///
		void setData(void *data);
		
		///
		/// \brief Получение данных
		/// \return данные
		///
		const void *getData() const;
		
		///
		/// \brief Установка данных
		/// \param data данные
		///
		template <typename T>
		void setUserData(T data) {
			delete container;
			container = (void *)new T(data);
		}
		
		///
		/// \brief Получение данных
		/// \return данные
		///
		template <typename T>
		const T getUserData() const {
			if(container == NULL) return T();
			return *((T *)container);
		}
		
		///
		/// \brief Получение флага необходимости создания блока-пустышки в режиме редактирования. По умолчанию true.
		/// \return флаг необходимости создания блока-пустышки
		///
		bool canCreateDummy() const;
		
		///
		/// \brief Установка флага необходимости создания блока-пустышки
		/// \param value флаг необходимости создания блока-пустышки
		///
		void setCreateDummy(bool value);
		
		///
		/// \brief Получение библитотеки, в которой содержится данное определение блока
		/// \return 
		///
		const Library *getLibrary() const;
		
	};
	
	//#define FOR_EACH_VARIDATIC(FUNC) { int t[] = { 0, ((void)FUNC, 1)... }; (void)t; }
	#define FOR_EACH_VARIDATIC(FUNC) { int t[] = { 0, FUNC... }; (void)t; }
	//#define FOR_EACH_VARIDATIC_I(FUNC) { int t[] = { 0, (FUNC, i++)... }; (void)t; }
	//#define FOR_EACH_VARIDATIC_I(FUNC) { int t = (((FUNC), i++)..., 0); (void)t; }
	//#define PARSE_DEF_VALUES _getNameFromParam(names.begin()[i]).str(), _getValueFromParam(names.begin()[i]).str()
	//#define PARSE_DEF_VALUES names.begin()[i++], ""
	
	//--------------------------------------------------------------------------
	//
	// template class NeuroneFunction
	//
	//--------------------------------------------------------------------------
	
	template <typename Ret, typename ...Args>
	class NeuroneFunction : public Neurone {
	
		typedef Ret (*Func) (Args...);
		typedef NeuroneFunction <Ret, Args...> This;
			
		Func func;
		
		Event *out;
		
		void onValueGet() {
			int i = sizeof...(Args) - 1;
			ANANDAMIDE_UNUSED(i);
            TypeToVariable <Ret> (func(VariableToType <Args> (inputs[i--].getValue().getValue()).value...), outputs[0].getValue().getVariable());
			if(out) out->run();
		}

	public:
		
		static void define(NeuroneDef *def, const char *name, Func func_, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneFunction::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
				return;
			}
			
			def->setData((void *)func_);
			def->setName(name);
			
			Str ret_n = ret_name;
			if(ret_n[0] == '*') {
				ret_n.del(0);
				def->addAction("in");
				def->addEvent("out");
			}
			def->addOutput <Ret> (ret_n.str(), "");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
		}

		void init() {
			func = (Func)def->getData();
			Output *output = &getOutput(0);
			output->getVariable().setType <Ret> ();
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());

			
			out = getEvent("out");
			Action *in = getAction("in");
			if(in) {
				in->setAction(Common::Action::create(this, &This::onValueGet));
			} else {
				output->setOnGetValueAction(Common::Action::create(this, &This::onValueGet));
			}
		}
	};
	
	template <typename ...Args>
	class NeuroneFunction <void, Args...> : public Neurone {

		typedef void (*Func) (Args...);
		typedef NeuroneFunction <void, Args...> This;
			
		Anandamide::Action *in;
		Anandamide::Event *out;
		Func func;

		void run() {
			int i = sizeof...(Args) - 1;
			ANANDAMIDE_UNUSED(i);
			func(VariableToType <Args> (getInput(i--).getValue()).value...);
			out->run();
		}

	public:

		static void define(NeuroneDef *def, const char *name, Func func_, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneFunction::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
				return;
			}
			
			def->setData((void *)func_);
			def->setName(name);
			def->addAction("in");
			def->addEvent("out");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
		}

		void init() {
			func = (Func)def->getData();
			in = getAction("in");
			out = getEvent("out");
			in->setAction(Common::Action::create(this, &This::run));

			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());
			
		}

	};
	
	//--------------------------------------------------------------------------
	//
	// template class NeuroneClassConstructor
	//
	//--------------------------------------------------------------------------
	
	template <typename Ret, typename ...Args>
	class NeuroneClassConstructor : public Neurone {
	
		typedef Ret (*Func) (Args...);
		typedef NeuroneClassConstructor <Ret, Args...> This;
			
		Func func;
		
		Event *out;
		
		Output *o_pointer;
		Output *o_instance;
		
		void onValueGet() {
			int i = sizeof...(Args) - 1;
			ANANDAMIDE_UNUSED(i);
			TypeToVariable <Ret> (func(VariableToType <Args> (inputs[i--].getValue().getValue()).value...), o_instance->getVariable());
			TypeToVariable <Ret*> ( &(VariableToType <Ret> (o_instance->getVariable()).value), o_pointer->getVariable());
			if(out) out->run();
		}

	public:
		
		static void define(NeuroneDef *def, const char *name, Func func_, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneClassConstructor::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
				return;
			}
			
			def->setData((void *)func_);
			def->setName(name);
			
			def->addAction("in");
			def->addEvent("out");
			
			def->addOutput <Ret*> ("Pointer", "");
			def->addOutput <Ret> ("Instance", "");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
		}

		void init() {
			func = (Func)def->getData();
			
			o_pointer = &getOutput(0);
			o_pointer->getVariable().setType <Ret*> ();
			
			o_instance = &getOutput(1);
			o_instance->getVariable().setType <Ret> ();
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());
			
			out = getEvent("out");
			Action *in = getAction("in");
			if(in) {
				in->setAction(Common::Action::create(this, &This::onValueGet));
			} 
		}
	};

	//--------------------------------------------------------------------------
	//
	// template class NeuroneMember
	//
	//--------------------------------------------------------------------------
	
	template <typename Func, typename Instance, typename Ret, typename ...Args>
	class NeuroneMember : public Neurone {
	
		//typedef Ret (Instance::*Func) (Args...);
		typedef NeuroneMember <Func, Instance, Ret, Args...> This;
			
		Func func;
		
		Event *out;
		Input *i_instance;
		
		void onValueGet() {
			Instance *instance = VariableToType <Instance *> (i_instance->getValue()).value;
			int i = sizeof...(Args);
			ANANDAMIDE_UNUSED(i);
			if(instance != NULL)
				TypeToVariable <Ret> (
					(instance->*func)(VariableToType <Args> (inputs[i--].getValue().getValue()).value...),
					getOutput(0).getVariable()
				);
			else {
				errorPointerMessage(i_instance);
				getOutput(0).getVariable().setInt(0);
			}
			if(out) out->run();
		}

	public:
		
		static void define(NeuroneDef *def, const char *name, Func func_, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneMember::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
				return;
			}
			
			def->setUserData(func_);
			def->setName(name);
			
			Str ret_n = ret_name;
			if(ret_n[0] == '*') {
				ret_n.del(0);
				def->addAction("in");
				def->addEvent("out");
			}
			def->addOutput <Ret> (ret_n.str(), "");
			
			//member
			def->addInput <Instance *> (inst_name, "");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
			
		}
		
		void init() {
			func = def->getUserData <Func> ();
			Output *output = &getOutput(0);
			output->getVariable().setType <Ret> ();
			
			int i = 1;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());
			
			out = getEvent("out");
			Action *in = getAction("in");
			if(in) {
				in->setAction(Common::Action::create(this, &This::onValueGet));
			} else {
				output->setOnGetValueAction(Common::Action::create(this, &This::onValueGet));
			}
			
			i_instance = &getInput(0);
		}
	};
	
	template <typename Func, typename Instance, typename ...Args>
	class NeuroneMember <Func, Instance, void, Args...> : public Neurone {

		//typedef void (Instance::*Func) (Args...);
		typedef NeuroneMember <Func, Instance, void, Args...> This;
			
		Func func;
		
		Anandamide::Action *in;
		Anandamide::Event *out;
		Input *i_instance;

		void run() {
			Instance *instance = VariableToType <Instance *> (i_instance->getValue()).value;
			int i = sizeof...(Args);
			ANANDAMIDE_UNUSED(i);
			if(instance != NULL)
				(instance->*func)(VariableToType <Args> (getInput(i--).getValue()).value...);
			else
				errorPointerMessage(i_instance);
			out->run();
		}

	public:

		static void define(NeuroneDef *def, const char *name, Func func_, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneMember::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
				return;
			}
			
			def->setUserData(func_);
			def->setName(name);
			def->addAction("in");
			def->addEvent("out");
			
			//member
			def->addInput <Instance *> (inst_name, "");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
		}

		void init() {
			func = def->getUserData <Func> ();
			in = getAction("in");
			out = getEvent("out");
			in->setAction(Common::Action::create(this, &This::run));

			int i = 1;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());
			i_instance = &getInput(0);
		}

	};
	
	//--------------------------------------------------------------------------
	//
	// template class NeuroneMemberValue
	//
	//--------------------------------------------------------------------------
	
	template <typename Func, typename Instance, typename Ret, typename ...Args>
	class NeuroneMemberValue : public Neurone {
	
		//typedef Ret (Instance::*Func) (Args...);
		typedef NeuroneMemberValue <Func, Instance, Ret, Args...> This;
			
		Func func;
		
		Event *out;
		Input *i_instance;
		
		void onValueGet() {
			Instance inst = VariableToType <Instance> (i_instance->getValue()).value;
			Instance *instance = &inst;
			int i = sizeof...(Args);
			ANANDAMIDE_UNUSED(i);
			TypeToVariable <Ret> ((instance->*func)(VariableToType <Args> (inputs[i--].getValue().getValue()).value...), getOutput(0).getVariable());
			TypeToVariable <Instance> (inst, getOutput(1).getVariable());
			if(out) out->run();
		}

	public:
		
		static void define(NeuroneDef *def, const char *name, Func func_, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneMemberValue::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
			}
			
			def->setUserData(func_);
			def->setName(name);
			
			Str ret_n = ret_name;
			if(ret_n[0] == '*') {
				ret_n.del(0);
				def->addAction("in");
				def->addEvent("out");
			}
			def->addOutput <Ret> (ret_n.str(), "");
			
			//Instance output
			def->addOutput <Instance> ("Instance", "");
			
			//member
			def->addInput <Instance> (inst_name, "");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
		}
		
		void init() {
			func = def->getUserData <Func> ();
			Output *output = &getOutput(0);
			output->getVariable().setType <Ret> ();
			
			//Instance output
			Output *outputI = &getOutput(1);
			outputI->getVariable().setType <Instance> ();
			
			int i = 1;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());
			
			out = getEvent("out");
			Action *in = getAction("in");
			if(in) {
				in->setAction(Common::Action::create(this, &This::onValueGet));
			} else {
				output->setOnGetValueAction(Common::Action::create(this, &This::onValueGet));
			}
			
			i_instance = &getInput(0);
		}
	};
	
	template <typename Func, typename Instance, typename ...Args>
	class NeuroneMemberValue <Func, Instance, void, Args...> : public Neurone {

		//typedef void (Instance::*Func) (Args...);
		typedef NeuroneMemberValue <Func, Instance, void, Args...> This;
			
		Func func;
		
		Anandamide::Action *in;
		Anandamide::Event *out;
		Input *i_instance;

		void run() {
			Instance inst = VariableToType <Instance> (i_instance->getValue()).value;
			Instance *instance = &inst;
			int i = sizeof...(Args);
			ANANDAMIDE_UNUSED(i);
			(instance->*func)(VariableToType <Args> (getInput(i--).getValue()).value...);
			TypeToVariable <Instance> (inst, getOutput(0).getVariable());
			out->run();
		}

	public:

		static void define(NeuroneDef *def, const char *name, Func func_, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
			
			if(names.size() != sizeof...(Args)) {
				errorMessage(format("NeuroneMemberValue::define() : block \"%s\" argument names number (%d) does not match function prototype arguments number (%d)", name, names.size(), sizeof...(Args)));
			}
			
			def->setUserData(func_);
			def->setName(name);
			def->addAction("in");
			def->addEvent("out");
			
			//member
			def->addInput <Instance> (inst_name, "");
			
			//Instance output
			def->addOutput <Instance> ("Instance", "");
			
			int i = 0;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(def->addInput <Args> (names.begin()[i++], ""));
		}

		void init() {
			func = def->getUserData <Func> ();
			in = getAction("in");
			out = getEvent("out");
			in->setAction(Common::Action::create(this, &This::run));

			int i = 1;
			ANANDAMIDE_UNUSED(i);
			FOR_EACH_VARIDATIC(getInput(i++).getVariable().template setType <Args> ());
			i_instance = &getInput(0);
			
			//Instance output
			Output *outputI = &getOutput(0);
			outputI->getVariable().setType <Instance> ();
		}

	};
	
	//--------------------------------------------------------------------------
	//
	// makeNeuroneFunction
	//
	//--------------------------------------------------------------------------
	
	///
	/// \brief makeNeuroneFunction Создание определения функционального блока по указателю на функцию
	/// \param name имя блока
	/// \param ret_name имя возвращаемого параметра
	/// \param names список инициализации имен для параметров функции {"name1", "name2", "name"... }
	/// \return новое определение функционального блока
	///
	template <typename Ret, typename ...Args>
	static NeuroneDef *makeNeuroneFunction(const char *name, Ret (*func)(Args...), const char *ret_name, std::initializer_list <const char *> names) {
		typedef NeuroneFunction <Ret, Args...> NeuroneType;
		NeuroneDef *def = NeuroneDef::create();
		NeuroneType::define(def, name, func, ret_name, names);
		def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
		return def;
	}
	
	//--------------------------------------------------------------------------
	//
	// makeNeuroneMember const
	//
	//--------------------------------------------------------------------------
	
	///
	/// \brief makeNeuroneMember Создание определения функционального блока по указателю на метод класса
	/// \param name имя блока
	/// \param inst_name имя входного параметра - указателя на экземпляр, для которого вызывается метод
	/// \param ret_name имя возвращаемого параметра
	/// \param names список инициализации имен для параметров функции { "name1", "name2", "name"... }
	/// \return новое определение блока - метода класса
	///
	template <typename Instance, typename Ret, typename ...Args>
	static NeuroneDef *makeNeuroneMember(const char *name, Ret (Instance::*func)(Args...) const, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
		typedef Ret (Instance::*Func)(Args...) const;
		typedef NeuroneMember <Func, Instance, Ret, Args...> NeuroneType;
		NeuroneDef *def = NeuroneDef::create();
		NeuroneType::define(def, name, func, inst_name, ret_name, names);
		def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
		return def;
	}
	
	//--------------------------------------------------------------------------
	//
	// makeNeuroneMember
	//
	//--------------------------------------------------------------------------
	
	template <typename Instance, typename Ret, typename ...Args>
	static NeuroneDef *makeNeuroneMember(const char *name, Ret (Instance::*func)(Args...), const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
		typedef Ret (Instance::*Func)(Args...);
		typedef NeuroneMember <Func, Instance, Ret, Args...> NeuroneType;
		NeuroneDef *def = NeuroneDef::create();
		NeuroneType::define(def, name, func, inst_name, ret_name, names);
		def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
		return def;
	}
	
	//--------------------------------------------------------------------------
	//
	// makeNeuroneMemberValue const
	//
	//--------------------------------------------------------------------------
	
	///
	/// \brief makeNeuroneMemberValue Создание определения функционального блока вызова метода класса по значению класса
	/// \param name имя блока
	/// \param inst_name имя входного параметра - указателя на экземпляр, для которого вызывается метод
	/// \param ret_name имя возвращаемого параметра
	/// \param names список инициализации имен для параметров функции { "name1", "name2", "name"... }
	/// \return новое определение блока - метода класса
	///
	template <typename Instance, typename Ret, typename ...Args>
	static NeuroneDef *makeNeuroneMemberValue(const char *name, Ret (Instance::*func)(Args...) const, const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
		typedef Ret (Instance::*Func)(Args...) const;
		typedef NeuroneMemberValue <Func, Instance, Ret, Args...> NeuroneType;
		NeuroneDef *def = NeuroneDef::create();
		NeuroneType::define(def, name, func, inst_name, ret_name, names);
		def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
		return def;
	}
	
	//--------------------------------------------------------------------------
	//
	// makeNeuroneMemberValue
	//
	//--------------------------------------------------------------------------
	
	template <typename Instance, typename Ret, typename ...Args>
	static NeuroneDef *makeNeuroneMemberValue(const char *name, Ret (Instance::*func)(Args...), const char *inst_name, const char *ret_name, std::initializer_list <const char *> names) {
		typedef Ret (Instance::*Func)(Args...);
		typedef NeuroneMemberValue <Func, Instance, Ret, Args...> NeuroneType;
		NeuroneDef *def = NeuroneDef::create();
		NeuroneType::define(def, name, func, inst_name, ret_name, names);
		def->setCreateFunction(Creator<Neurone, NeuroneType, void>::create);
		return def;
	}
	
	//--------------------------------------------------------------------------
	//
	// deprecated
	//
	//--------------------------------------------------------------------------
	
	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret>
	static NeuroneDef *makeNeuroneFunction0(const char *name, Ret (*func)(), const char *ret_name) {
		return makeNeuroneFunction <Ret> (name, func, ret_name, {});
	}
	
	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0>
	static NeuroneDef *makeNeuroneFunction1(const char *name, Ret (*func)(A0), const char *ret_name, const char *a0_name) {
		return makeNeuroneFunction <Ret, A0> (name, func, ret_name, {a0_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1>
	static NeuroneDef *makeNeuroneFunction2(const char *name, Ret (*func)(A0, A1), const char *ret_name, const char *a0_name, const char *a1_name) {
		return makeNeuroneFunction <Ret, A0, A1> (name, func, ret_name, {a0_name, a1_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2>
	static NeuroneDef *makeNeuroneFunction3(const char *name, Ret (*func)(A0, A1, A2), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2> (name, func, ret_name, {a0_name, a1_name, a2_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2, class A3>
	static NeuroneDef *makeNeuroneFunction4(const char *name, Ret (*func)(A0, A1, A2, A3), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name, const char *a3_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2, A3> (name, func, ret_name, {a0_name, a1_name, a2_name, a3_name});
	}
	
	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2, class A3, class A4>
	static NeuroneDef *makeNeuroneFunction5(const char *name, Ret (*func)(A0, A1, A2, A3, A4), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name, const char *a3_name, const char *a4_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2, A3, A4> (name, func, ret_name, {a0_name, a1_name, a2_name, a3_name, a4_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2, class A3, class A4, class A5>
	static NeuroneDef *makeNeuroneFunction6(const char *name, Ret (*func)(A0, A1, A2, A3, A4, A5), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name, const char *a3_name, const char *a4_name, const char *a5_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2, A3, A4, A5> (name, func, ret_name, {a0_name, a1_name, a2_name, a3_name, a4_name, a5_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2, class A3, class A4, class A5, class A6>
	static NeuroneDef *makeNeuroneFunction7(const char *name, Ret (*func)(A0, A1, A2, A3, A4, A5, A6), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name, const char *a3_name, const char *a4_name, const char *a5_name, const char *a6_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2, A3, A4, A5, A6> (name, func, ret_name, {a0_name, a1_name, a2_name, a3_name, a4_name, a5_name, a6_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	static NeuroneDef *makeNeuroneFunction8(const char *name, Ret (*func)(A0, A1, A2, A3, A4, A5, A6, A7), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name, const char *a3_name, const char *a4_name, const char *a5_name, const char *a6_name, const char *a7_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2, A3, A4, A5, A6, A7> (name, func, ret_name, {a0_name, a1_name, a2_name, a3_name, a4_name, a5_name, a6_name, a7_name});
	}

	///
	/// \brief Создание определения функционального блока по указателю на функцию
	/// \param name имя определения блока
	/// \param ret_name имя выходного параметра возвращаемого значения
	/// \param A.. тип параметра функции
	/// \param a.._name название входного параметра
	/// \return новое определение функционального блока
	/// \deprecated устарело, необходимо использовать makeNeuroneFunction
	///
	template <class Ret, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	static NeuroneDef *makeNeuroneFunction9(const char *name, Ret (*func)(A0, A1, A2, A3, A4, A5, A6, A7, A8), const char *ret_name, const char *a0_name, const char *a1_name, const char *a2_name, const char *a3_name, const char *a4_name, const char *a5_name, const char *a6_name, const char *a7_name, const char *a8_name) {
		return makeNeuroneFunction <Ret, A0, A1, A2, A3, A4, A5, A6, A7, A8> (name, func, ret_name, {a0_name, a1_name, a2_name, a3_name, a4_name, a5_name, a6_name, a7_name, a8_name});
	}
	
	template <typename T, typename ...Args>
	T constructor(Args ...args) {
		return T(args...);
	}
	
	template <typename T, typename ...Args>
	T* constructorPtr(Args ...args) {
		return new T(args...);
	}
	
}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
