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

#ifndef AnandamideLogicH
#define AnandamideLogicH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideNeurone.h"

//------------------------------------------------------------------------------

namespace Anandamide {

	class Script;
	class Renderer;

	///-------------------------------------------------------------------------
	///
	/// \class ActionNeurone
	///
	/// \brief Класс блока, представляющий Action внутри Logic блока
	/// 
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API ActionNeurone : public Neurone {

		Action *action;
		Event *event;

	public:

		ActionNeurone(const char *instance, Logic *logic_, Neurone* dummyNode = NULL);
		~ActionNeurone();

		const char *getType() const;
		const char *getInstance() const;
		const char *getCaption();
		bool hasCaption();
		
		Action *getLogicAction();

	};

	///-------------------------------------------------------------------------
	///
	/// \class EventNeurone
	///
	/// \brief Класс блока, представляющий Event внутри Logic блока
	/// 
	///-------------------------------------------------------------------------
  
	class ANANDAMIDE_API EventNeurone : public Neurone {

		Action *action;
		Event *event;

	public:

		EventNeurone(const char *inst, Logic *logic_, Neurone* dummyNode = NULL);
		~EventNeurone();

		const char *getType() const;
		const char *getInstance() const;
		const char *getCaption();
		bool hasCaption();
		
		Event *getLogicEvent();
		
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class GetVariableNeurone
	///
	/// \brief Класс блока, представляющий получение значения переменной внутри Logic блока
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API GetVariableNeurone : public Neurone {

		Variable *variable;
		Output *output;
		
		void onValueGet();

	public:

		GetVariableNeurone(const char *instance, Logic *logic_, Neurone* dummyNode = NULL);
		~GetVariableNeurone();

		const char *getType() const;
		const char *getInstance() const;
		const char *getCaption();
		bool hasCaption();
		
		Variable *getLogicVariable();
		
		void updateType();
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class GetVariablePtrNeurone
	///
	/// \brief Класс блока, представляющий получение значения переменной внутри Logic блока
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API GetVariablePtrNeurone : public Neurone {

		Variable *variable;
		Output *output;
		
		void onValueGet();

	public:

		GetVariablePtrNeurone(const char *instance, Logic *logic_, Neurone* dummyNode = NULL);
		virtual ~GetVariablePtrNeurone();

		virtual const char *getType() const;
		virtual const char *getInstance() const;
		virtual const char *getCaption();
		virtual bool hasCaption();
		
		Variable *getLogicVariable();
		
	};

	
	///-------------------------------------------------------------------------
	///
	/// \class SetVariableNeurone
	///
	/// \brief Класс блока, представляющий установку значения переменной внутри Logic блока
	/// 
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API SetVariableNeurone : public Neurone {

		Variable *variable;
		Event *out;
		Input *input;
			
		void actionSet();
		
	public:

		SetVariableNeurone(const char *instance, Logic *logic_, Neurone* dummyNode = NULL);
		~SetVariableNeurone();

		const char *getType() const;
		const char *getInstance() const;
		const char *getCaption();
		bool hasCaption();
		
		Variable *getLogicVariable();
		
		void updateType();

	};
	
	///-------------------------------------------------------------------------
	///
	/// \class InputNeurone
	///
	/// \brief Класс блока, представляющий Input внутри Logic блока
	/// 
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API InputNeurone : public Neurone {

		Input *input;
		Output *output;
		Parameter *type;
		Parameter *default_value;

		void onValueGet();
		void onSetDefault();
		void onSetType();
		
	public:

		InputNeurone(const char *instance, Logic *logic_, Neurone* dummyNode = NULL);
		~InputNeurone();

		const char *getType() const;
		const char *getInstance() const;
		const char *getCaption();
		bool hasCaption();
		
		Input *getLogicInput();
		
		void invalidate();
		
	};
	
	///-------------------------------------------------------------------------
	///
	/// \class OutputNeurone
	///
	/// \brief Класс блока, представляющий Output внутри Logic блока
	/// 
	///-------------------------------------------------------------------------

	class ANANDAMIDE_API OutputNeurone : public Neurone {

		Output *output;
		Input *input;
		Event *get;
		Parameter *type;
		
		void onValueGet();
		void onValueSet();
		void onSetType();
		
	public:

		OutputNeurone(const char *instance, Logic *logic_, Neurone* dummyNode = NULL);
		~OutputNeurone();

		const char *getType() const;
		const char *getInstance() const;
		const char *getCaption();
		bool hasCaption();
		
		Output *getLogicOutput();
		
		void invalidate();
		
	};
	
	///-------------------------------------------------------------------------
	///
	/// \struct Link
	/// 
	/// \brief Класс, сохраняющий в себе информацию о связи блока
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Link {
		
	public:
			
		Neurone *src;		//!< источник связи
		Neurone *dst;		//!< приемник связи
		
		int src_index;		//!< индекс элемента в источнике
		int dst_index;		//!< индекс элемента в приемнике
		
		bool event_action;	//!< если true - элемент в источнике - Event, в приемнике - Action, иначе - Output, Input
		
		Link();
		
		Link(Neurone *src, int src_index, Neurone *dst, int dst_index, bool event_action);
		
		///
		/// \brief Отрисовка связи
		/// \param renderer Renderer отрисовки
		/// \param selected true - рисовать, как выбранный
		///
		void render(Renderer *renderer, bool selected);
		
		///
		/// \brief Выбор связи по точке
		/// \param point точка, которая проверяется на попадание в линию связи
		/// \return true, если точка попадает в линию связи
		///
		bool select(const vec3 &point);
		
		///
		/// \brief Удаление связи. После удаления текущий объект очищается.
		///
		void remove();
		
		///
		/// \brief Проверка, является текущая связь валидной
		/// \return true, если связь валидна
		///
		bool is();
	};
	
	///-------------------------------------------------------------------------
	///
	/// class Logic
	/// 
	/// \brief Класс блока логики, который содержит в себе блоки,
	/// которые связаны между собой.
	/// 
	///-------------------------------------------------------------------------
	
	class ANANDAMIDE_API Logic : public Neurone {

		friend class Neurone;
		friend class NeuroneGroup;
		friend class ActionNeurone;
		friend class EventNeurone;
		friend class InputNeurone;
		friend class OutputNeurone;

	protected:

		Array <Neurone> nodes;
		Array <NeuroneGroup> nodeGroups;
		Map <Str, Variable> variables;
		mutable Script *script;
		Str m_kind;

		virtual Neurone *createNeurone(const char *type_, const char *instance, const char *library, bool dummy = false, Neurone *dummyNode = NULL);

	public:

		///
		/// \brief Конструктор
		/// \param script указатель на объект Script, к которому приндлежит данный объект
		///
		Logic(Script *script);
		
		Logic();
		
		virtual ~Logic();
		
		///
		/// \brief Получение занимаемого размера памяти
		/// \return размер в байтах
		///
		unsigned long long getSize() override;
		
		///
		/// \brief Получение типа блока
		/// \return строку-тип блока
		///
		const char *getType() const;

		///
		/// \brief Вызов update() у всех Neurone внутри блока
		/// \param dt время в секундах, прошедшее с предыдущего вызова update()
		///
		void update(float dt);
		
		///
		/// \brief Отрисовка содержимого в виде блоков со связями
		/// \param renderer указатель на Renderer, через который осуществляется отрисовка
		///
		void renderContent(Anandamide::Renderer *renderer);
		
		///
		/// \brief Клонирование блока
		/// \return клон блока
		///
		Neurone *clone(Anandamide::Neurone* dummyNode = NULL);
		
		///
		/// \brief afterCompile Функция, которая автоматически вызывается после успешной компиляции скрипта
		///
		void afterCompile();
		
		///
		/// \brief Клонирование Logic. То же самое, что и clone(), только результат типа Logic
		/// \return клон Logic
		///
		Logic *cloneLogic();
		
		///
		/// \brief Клонирование списка блоков в target_logic
		/// \param nodes cписок блоков
		/// \param target_logic указатель на объект, куда будут склонированы блоки
		/// \param dummy флаг клонирования в блоки-пустышки
		/// \param set_selected флаг установки выделения для вновь склонировнных блоков
		///
		void cloneNodes(Array <Neurone> &nodes, Logic *target_logic, bool dummy = false, bool set_selected = false);
		
		void cloneNodesAndGroups(Array <Neurone> &nodes, Array <NeuroneGroup> &groups, Logic *target_logic, bool dummy = false, bool set_selected = false);
		
		///
		/// \brief Получения количества блоков, которые содержатся внутри Logic
		/// \return колчиество блоков
		///
		int getNodesCount() const;
		
		///
		/// \brief Получение блока по индексу
		/// \param i индекс блока
		/// \return блок с индексом i
		///
		Neurone &getNode(int i);
		
		///
		/// \brief Константная версия получения блока по индексу
		/// \param i индекс блока
		/// \return блок с индексом i
		///
		const Neurone &getNode(int i) const;
		
		///
		/// \brief Добавление существующего блока. 
		/// \param node существующий блок. При добавлении у node будет задан новый Logic
		///
		void addNode(Neurone *node);
		
		///
		/// \brief Добавление нового блока
		/// \param type тип блока в строковом представлении
		/// \param instance экземпляр (для блоков, которые представляют собой выводы текущего блока Logic - NeuroneAction, NeuroneEvent, ...)
		/// \param dummy флаг, обозначающий создание блока-пустышки
		/// \return новый созданный и добавленный блок
		///
		Neurone *addNode(const char *type, const char *library, const char *instance, bool dummy, Neurone* dummyNode = NULL);
		
		Neurone* getNodeByPos(int X, int Y);
		Action* getActionByPos(int X, int Y);
		
		void addNodesGroup(NeuroneGroup* group);
		bool canCreateNewGroup();
		void addNodesGroupBySelection();
		int getNodeGroup(Neurone *node);
		int getNodeGroupCount() const;
		NeuroneGroup &getNodeGroup(int index);
		NeuroneGroup* getSelectedGroup();
		NeuroneGroup* getGroupOfSelectedNodes();
		bool removeSelectedFromGroup();
		NeuroneGroup* getGroupByPos(int X, int Y);
		bool removeGroup(NeuroneGroup* group);
		void removeGroup(int i);
		bool removeGroupWithNodes(NeuroneGroup* group);
		bool addSelectedToGroup(NeuroneGroup* group);
		
		bool isSomethingSelected();
		int getSelectedGroupsCount();
		
		void renameNodes(const char* oldName, const char* newName);
		
		///
		/// \brief Удаление блока с индеком index
		/// \param index индекс удаляемого блока
		///
		virtual void removeNode(int index);
		
		void removeNodesGroup(int index);
		
		///
		/// \brief Удаление блока по указателю
		/// \param node указатель на удаляемый блок
		///
		void removeNode(Neurone *node);
		
		///
		/// \brief Перемещение блока в конец массива с целью отображения его поверх всех блоков
		/// \param node указатель на блок
		/// \bug вызывает ошибки и несоответствия
		///
		void bringToFront(Neurone *node);
		
		///
		/// \brief Получение индекса блока по указателю
		/// \param node указатель на блок
		/// \return индекс блока с указателем node либо -1, если не найдено
		///
		int indexOf(const Neurone *node) const;

		///
		/// \brief Удаление всех блоков из Logic.
		///
		virtual void clear();
		
		///
		/// \brief Запись Logic в xml
		/// \param xml
		///
		void writeXml(Xml *xml) const;
		
		///
		/// \brief Чтение Logic из xml
		/// \param xml
		/// \param dummy флаг создание блоков-пустышек
		/// \return true, если операция успешна
		///
		bool readXml(const Xml *xml, bool dummy = false);
		
		///
		/// \brief Получение указателя на Script, к которому приндлежит данный блок
		/// \return указатель на объект Script
		///
		Script *getScript() const;
		
		//----------------------------------------------------------------------
		// actions
		//----------------------------------------------------------------------
		
		///
		/// \brief Переименование Action внутри Logic
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameAction(const char *old_name, const char *new_name);
		
		///
		/// \brief Удаление Action по указателю
		/// \param action указатель на Action
		///
		void removeAction(Action *action);
		
		///
		/// \brief Получение блока ActionNeurone, представляющего Action внутри Logic
		/// \param action указатель на Action
		/// \return указатель на блок ActionNeurone, соответствующий Action или NULL, если не найдено
		///
		ActionNeurone *getActionNode(Action *action);
		
		//----------------------------------------------------------------------
		// events
		//----------------------------------------------------------------------
		
		///
		/// \brief Переименование Event внутри Logic
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameEvent(const char *old_name, const char *new_name);
		
		///
		/// \brief Удаление Event по указателю
		/// \param event указатель на Event
		///
		void removeEvent(Event *event);
		
		///
		/// \brief Получение блока EventNeurone, представляющего Event внутри Logic
		/// \param event указатель на Event
		/// \return указатель на блок EventNeurone, соответствующий Event или NULL, если не найдено
		///
		EventNeurone *getEventNode(Event *event);
		
		//----------------------------------------------------------------------
		// inputs
		//----------------------------------------------------------------------
		
		///
		/// \brief Переименование Input внутри Logic
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameInput(const char *old_name, const char *new_name);
		
		///
		/// \brief Удаление Input по указателю
		/// \param input указатель на Input
		///
		void removeInput(Input *input);

		///
		/// \brief Получение блока InputNeurone, представляющего Input внутри Logic
		/// \param input указатель на Input
		/// \return указатель на блок InputNeurone, соответствующий Input или NULL, если не найдено
		///
		InputNeurone *getInputNode(Input *input);
		
		//----------------------------------------------------------------------
		// outputs
		//----------------------------------------------------------------------
		
		///
		/// \brief Переименование Output внутри Logic
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если new_name уже существует
		///
		bool renameOutput(const char *old_name, const char *new_name);
		
		///
		/// \brief Удаление Output по указателю
		/// \param output указатель на Output
		///
		void removeOutput(Output *output);
		
		///
		/// \brief Получение блока OutputNeurone, представляющего Output внутри Logic
		/// \param output указатель на Output
		/// \return указатель на блок OutputNeurone, соответствующий Output или NULL, если не найдено
		///
		OutputNeurone *getOutputNode(Output *output);
		
		//----------------------------------------------------------------------
		// variables
		//----------------------------------------------------------------------
		
		///
		/// \brief Получение количества локальных переменных внутри Logic
		/// \return количество локальных переменных внутри Logic
		///
		int getVariablesCount() const;
		
		///
		/// \brief Получение переменной по индексу
		/// \param i индекс переменной
		/// \return переменная с индексом i
		///
		Variable &getVariable(int i);
		
		///
		/// \brief Константная версия получения переменной по индексу
		/// \param i индекс переменной
		/// \return переменная с индексом i
		///
		const Variable &getVariable(int i) const;
		
		///
		/// \brief Получение переменной по имени
		/// \param id имя переменной
		/// \return переменная с именем id или NULL, если переменной не существует
		///
		Variable *getVariable(const char *id);
		
		///
		/// \brief Константная версия получения переменной по имени
		/// \param id имя переменной
		/// \return переменная с именем id или NULL, если переменной не существует
		///
		const Variable *getVariable(const char *id) const;
		
		///
		/// \brief Получение имени переменной по индексу
		/// \param i индекс переменной
		/// \return имя переменнной
		///
		const char *getVariableName(int i) const;
		
		///
		/// \brief Получение имени переменной по указателю
		/// \param variable указатель на переменную
		/// \return имя переменнной или "", если переменной не существует
		///
		const char *getVariableName(const Variable *variable) const;
		
		///
		/// \brief Переименование переменной
		/// \param old_name старое имя
		/// \param new_name новое имя
		/// \return false, если имя new_name уже существует
		///
		bool renameVariable(const char *old_name, const char *new_name);
		
		///
		/// \brief Установка типа переменной. Обновляет все блоки GetVariableNeurone, SetVariableNeurone
		/// \param variable указатель на перменную
		/// \param type новый тип переменной
		///
		void setVariableType(Variable *variable, const TypeInfo &type);
		
		///
		/// \brief Добавление новой переменной с именем id
		/// \param id имя переменной
		/// \return новую созданную переменную или NULL, если переменная с таким именем уже существует
		///
		Variable *addVariable(const char *id);
		
		///
		/// \brief Удаление переменной по указателю. Удаляются также соотвествующие перменной блоки.
		/// \param variable указатель на переменную
		///
		void removeVariable(Variable *variable);
		
		///
		/// \brief Получение индекса переменной по указателю
		/// \param variable указатель на переменную
		/// \return индекс переменной с указателем variable, либо -1, если не переменная не найдена
		///
		int indexOf(const Variable *variable) const;
		
		//for Unigine
		Variable *getVariableByName(const char *id);
		
		// Neurone interface
		void onSetLogic(Logic *logic);
		
		void setDef(const NeuroneDef *def);
		
		// Kind methods
		void setKind(const Str& kind);
		Str getKind() const;
		
	};

	///--------------------------------------------------------------------------
	///
	/// \class LogicDummy
	///
	/// \brief Класс блока-пустышки Logic.
	/// 
	///--------------------------------------------------------------------------
	
	class ANANDAMIDE_API LogicDummy : public NeuroneDummy {
			
			const Logic *source;
			
		public:
			
			LogicDummy(const Logic &logic, const NeuroneDef *def);
			
			void invalidate();
			
			const Logic *getSource() const;
			
	};
	
	//--------------------------------------------------------------------------
	
}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
