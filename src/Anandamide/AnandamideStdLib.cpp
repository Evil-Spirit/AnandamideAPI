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

#include "AnandamideStdLib.h"

//------------------------------------------------------------------------------

#include "AnandamideNeurone.h"
#include "AnandamideLibrary.h"
#include "AnandamideScript.h"
//#include <QImage>
#include <MathCore.h>

//------------------------------------------------------------------------------

ConsoleMessageCallback consoleMessageCallback = 0;
ConsoleMessageExtCallback consoleMessageExtCallback = 0;

namespace Anandamide {
	
	//----------------------------------------------------------------------
	//
	// class Mux
	//
	//----------------------------------------------------------------------

	class Mux : public Neurone
	{
		Output *oResult;
		Parameter *pTypeInputs;
		Parameter *pTypeCases;
		Parameter *pCases_count;
		Input *iCase;
		Input *iDefault;

		QVector <Input *> mux_inputs;

	public:

		~Mux() {
			pCases_count->getVariable().setOnChangeAction(NULL);
			pTypeCases->getVariable().setOnChangeAction(NULL);
			pTypeInputs->getVariable().setOnChangeAction(NULL);
			oResult->setOnGetValueAction(NULL);
		}

		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->setCreateDummy(false);

			def->addParameter <TypeInfo>("type_inputs", "1002");
			def->addParameter <TypeInfo>("type_cases", "1002");
			def->addParameter <int>("cases_count", "3");
			def->addInput <int>("case", "0");
			def->addInput <int>("default", "0");
			def->addOutput <int>("result", "");
		}

		void init() {

			pTypeInputs = getParameter("type_inputs");
			pTypeInputs->getVariable().setOnChangeAction(Common::Action::create(this, &Mux::onInvalidate));

			pTypeCases = getParameter("type_cases");
			pTypeCases->getVariable().setOnChangeAction(Common::Action::create(this, &Mux::onInvalidate));

			pCases_count = getParameter("cases_count");
			pCases_count->getVariable().setOnChangeAction(Common::Action::create(this, &Mux::onInvalidate));

			iCase = getInput("case");
			iDefault = getInput("default");

			oResult = getOutput("result");
			oResult->setOnGetValueAction(Common::Action::create(this, &Mux::onGetValue));
		}

		const TypeDef *getParameterTypeDef(int type_id) const {
			if(getLogic() == NULL) return NULL;
			if(getLogic()->getScript() == NULL) return NULL;
			return getLogic()->getScript()->getLibraries()->getTypeDefById(type_id);
		}

		Variable getValueFromType(TypeInfo type_info) {

			Variable vv;
			vv.setType(type_info);

			if(type_info == TYPEID(int) || type_info == TYPEID(unsigned int)) {
				vv.setInt(mux_inputs.size());
			}

			const Enum *enums = type_info.getEnum();

			//автозаполнение значений энумоув
			if(enums) {
				int id = enums->getItemId(mux_inputs.size() % enums->getItemsCount());
				vv.setEnum(type_info, id);
			}

			if(type_info == TYPEID(bool))
				vv.setBool((bool) (mux_inputs.size() % 2));

			return vv;
		}

		void onGetValue() {
			Variable case_value = iCase->getValue();
			for(int i = 0; i < mux_inputs.size(); i++){
				Variable vv = getInput(format("case_%02d", i))->getValue();
				if(case_value == vv) {
					oResult->setValue(mux_inputs.at(i)->getValue());
					return;
				}
			}
			oResult->setValue(getInput("default")->getValue());
		}

		void onSetLogic(Logic *logic) {
			onInvalidate();
		}

	protected:

		void onInvalidate() {

			//установка типа case
			const TypeDef *def = getParameterTypeDef(pTypeInputs->getValue().getInt());
			TypeInfo type_info_inputs = TYPEID(Variable);

			if(def != NULL) {
				type_info_inputs = def->getTypeInfo();
			} else {
				errorMessage(format("Mux: can`t set object type id \"%d\".", pTypeInputs->getValue().getInt()));
			}

			const TypeDef *def_cases = getParameterTypeDef(pTypeCases->getValue().getInt());
			TypeInfo type_info_cases = TYPEID(Variable);
			if(def_cases != NULL) {
				type_info_cases = def_cases->getTypeInfo();
			} else {
				errorMessage(format("Mux: can`t set object type id \"%d\".", pTypeCases->getValue().getInt()));
			}

			iCase->getVariable().setType(type_info_cases);
			iDefault->getVariable().setType(type_info_inputs);
			oResult->getVariable().setType(type_info_inputs);

			//настройка
			const Enum *enums_cases = type_info_cases.getEnum();
			const Enum *enums_inputs = type_info_inputs.getEnum();

			int cases_count = pCases_count->getValue().getInt();

			if(cases_count < 0)
				cases_count = 0;

			if(enums_cases) {
				if(cases_count > enums_cases->getItemsCount())
					cases_count = enums_cases->getItemsCount();
			}

			if(type_info_cases == TYPEID(bool))
				if(cases_count > 2)
					cases_count = 2;

			if(pCases_count->getValue().getInt() != cases_count)
				pCases_count->setValue(Variable(cases_count));

			//перестройка входов
			while(mux_inputs.size() > cases_count) {
				Input *input = mux_inputs[mux_inputs.size()-1];
				removeInputInternal(input);
				removeInputInternal(getInputsCount() - 1);
				mux_inputs.remove(mux_inputs.size()-1);
			}

			while (mux_inputs.size() < cases_count) {
				Input *input = new Input(getValueFromType(type_info_inputs));
				Input *input_case = new Input(getValueFromType(type_info_cases));
				addInput(format("input_%02d", mux_inputs.size()), input);
				addInput(format("case_%02d", mux_inputs.size()), input_case);
				mux_inputs.append(input);
			}

			for(int i = 0; i < mux_inputs.size(); i++) {
				Variable v_input = mux_inputs[i]->getValue();
				if(v_input.getType() != type_info_inputs && enums_inputs) {
					int id = enums_inputs->getItemId(i % enums_inputs->getItemsCount());
					v_input.setEnum(type_info_inputs, id);
				}
				v_input.setType(type_info_inputs);
				mux_inputs[i]->setValue(v_input);

				Input *input_case = getInput(format("case_%02d", i));
				Variable v_cases = input_case->getValue();
				if(v_cases.getType() != type_info_cases && enums_cases) {
					int id = enums_cases->getItemId(i % enums_cases->getItemsCount());
					v_cases.setEnum(type_info_cases, id);
				}
				v_cases.setType(type_info_cases);
				input_case->setValue(v_cases);
			}
		}
	};

	//----------------------------------------------------------------------
	//
	// class RandomSwitch
	//
	//----------------------------------------------------------------------

	class RandomSwitch : public Neurone {

		Parameter *fp_outs_count;

	public:

		~RandomSwitch() {
			fp_outs_count->getVariable().setOnChangeAction(NULL);
		}

		void invalidate() {

			int outs_count = fp_outs_count->getValue().getInt();

			while (getEventsCount() > outs_count) {
				removeEventInternal(getEventsCount() - 1);
			}

			while (getEventsCount() < outs_count) {
				Event *event = new Event();
				addEvent(format("out%d", getEventsCount()), event);
			}
		}

		//
		void init() {

			// parameters
			fp_outs_count = getParameter("outs_count");
			fp_outs_count->getVariable().setOnChangeAction(Common::Action::create(this, &RandomSwitch::invalidate));

			// actions
			Action *action = getAction("in");
			action->setAction(Common::Action::create(this, &RandomSwitch::in));
		}
		
		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->addParameter <int> ("outs_count", "5");
			def->addAction("in");
			def->setCreateDummy(false);
		}

		//
		void in() {
			int out_index = ::math::random(getEventsCount());
			getEvent(out_index).run();

		}

	};
	
	//----------------------------------------------------------------------
	//
	// class Connector
	//
	//----------------------------------------------------------------------

	class Connector : public Neurone {

	public:

		//
		void init() {
			// actions
			Action *action = getAction("in");
			action->setAction(Common::Action::create(this, &Connector::in));
		}
		
		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->addAction("in");
			def->addEvent("out");
		}

		//
		void in() {
			getEvent(0).run();
		}

	};

	//----------------------------------------------------------------------
	//
	// class Sequence
	//
	//----------------------------------------------------------------------

	class Sequence : public Neurone {

		Parameter *fp_outs_count;

	public:

		~Sequence() {
			fp_outs_count->getVariable().setOnChangeAction(NULL);
		}

		void invalidate() {

			int outs_count = fp_outs_count->getValue().getInt();

			while (getEventsCount() > outs_count) {
				removeEventInternal(getEventsCount() - 1);
			}

			while (getEventsCount() < outs_count) {
				Event *event = new Event();
				addEvent(format("out%d", getEventsCount()), event);
			}
		}

		//
		void init() {

			// parameters
			fp_outs_count = getParameter("outs_count");
			fp_outs_count->getVariable().setOnChangeAction(Common::Action::create(this, &Sequence::invalidate));

			// actions
			Action *action = getAction("in");
			action->setAction(Common::Action::create(this, &Sequence::in));
		}
		
		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->addParameter <int> ("outs_count", "5");
			def->addAction("in");
			def->setCreateDummy(false);
		}

		//
		void in() {
			for(int i=0; i<getEventsCount(); i++) {
				getEvent(i).run();
			}
		}

	};
	
	//----------------------------------------------------------------------
	//
	// class Switch
	//
	//----------------------------------------------------------------------

	class Switch : public Neurone {

		Parameter *fp_outs_count;
		Input *in_case;
		Event *e_default;

	public:

		~Switch() {
			fp_outs_count->getVariable().setOnChangeAction(NULL);
		}

		void invalidate() {
			int outs_count = fp_outs_count->getValue().getInt();

			while (getEventsCount() - 1 > outs_count) {
				removeEventInternal(getEventsCount() - 1);
			}

			while (getEventsCount() - 1 < outs_count) {
				Event *event = new Event();
				addEvent(format("case%d", getEventsCount() - 1), event);
			}
		}

		//
		void init() {
			// parameters
			fp_outs_count = getParameter("cases_count");
			fp_outs_count->getVariable().setOnChangeAction(Common::Action::create(this, &Switch::invalidate));
			
			// inputs
			in_case = getInput("case");
			
			// events
			e_default = getEvent("default");

			// actions
			Action *action = getAction("in");
			action->setAction(Common::Action::create(this, &Switch::in));
		}
		
		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->setCreateDummy(false);
			
			def->addParameter <int> ("cases_count", "5");
			def->addAction("in");
			def->addEvent("default");
			def->addInput <int> ("case", "0");
		}

		//
		void in() {
			int case_value = in_case->getValue().getInt() + 1;
			if(case_value > 0 && case_value < getEventsCount()) {
				getEvent(case_value).run();
				return;
			}
			e_default->run();
		}

	};
	
	//----------------------------------------------------------------------
	//
	// class UberSwitch
	//
	//----------------------------------------------------------------------

	class UberSwitch : public Neurone {

		Parameter *p_type;
		Parameter *fp_outs_count;
		Input *in_case;
		Event *e_default;
		QVector<Input*> switch_inputs;

	public:

		~UberSwitch() {
			fp_outs_count->getVariable().setOnChangeAction(NULL);
		}
		
		const TypeDef *getParameterTypeDef() const {
			int type_id = p_type->getValue().getInt();
			if(getLogic() == NULL) return NULL;
			if(getLogic()->getScript() == NULL) return NULL;
			return getLogic()->getScript()->getLibraries()->getTypeDefById(type_id);
		}

		void invalidate() {
			//установка типа case
			if(getLogic() == NULL) return;
			if(getLogic()->getScript() == NULL) return;
			
			const TypeDef *def = getParameterTypeDef();
			TypeInfo type_info = TYPEID(Variable);
			if(def != NULL) {
				type_info = def->getTypeInfo();
			} else {
				errorMessage(format("UberSwitch: can`t set object type id \"%d\".", p_type->getValue().getInt()));
			}
			in_case->getVariable().setType(type_info);
			
			//настройка
			const Enum *enums = type_info.getEnum();
			
			int outs_count = fp_outs_count->getValue().getInt();
			if(outs_count < 0) outs_count = 0;
			if(enums) {
				if(outs_count > enums->getItemsCount()) outs_count = enums->getItemsCount();
			}
			
			//перестройка входов
			while(switch_inputs.size() > outs_count) {
				Input *input = switch_inputs[switch_inputs.size()-1];
				removeInputInternal(input);
				switch_inputs.remove(switch_inputs.size()-1);
			}
			
			while (switch_inputs.size() < outs_count) {
				Variable vv;
				vv.setType(type_info);
				
				if(type_info == TYPEID(int) || type_info == TYPEID(unsigned int)) {
					vv.setInt(switch_inputs.size());
				}
				
				//автозаполение значений энумоув
				if(enums) {
					int id = enums->getItemId(switch_inputs.size() % enums->getItemsCount());
					vv.setEnum(type_info, id);
				}
				
				Input *input = new Input(vv);
				Str key = format("case%d", switch_inputs.size());
				addInput(key, input);
				switch_inputs.append(input);
			}
			
			for(int i=0; i<switch_inputs.size(); i++) {
				Variable vv = switch_inputs[i]->getValue();
				if(vv.getType() != type_info && enums) {
					int id = enums->getItemId(i % enums->getItemsCount());
					vv.setEnum(type_info, id);
				}
				vv.setType(type_info);
				switch_inputs[i]->setValue(vv);
			}
			
			//перестройка событий
			while (getEventsCount() - 1 > outs_count) {
				removeEventInternal(getEventsCount() - 1);
			}

			while (getEventsCount() - 1 < outs_count) {
				Event *event = new Event();
				addEvent(format("case%d", getEventsCount() - 1), event);
			}
		}

		void init() {
			// parameters
			p_type = getParameter("type");
			p_type->getVariable().setOnChangeAction(Common::Action::create(this, &UberSwitch::invalidate));
			fp_outs_count = getParameter("cases_count");
			fp_outs_count->getVariable().setOnChangeAction(Common::Action::create(this, &UberSwitch::invalidate));
			
			// inputs
			in_case = getInput("case");
			
			// events
			e_default = getEvent("default");

			// actions
			Action *action = getAction("in");
			action->setAction(Common::Action::create(this, &UberSwitch::in));
		}
		
		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->setCreateDummy(false);
			
			def->addParameter <TypeInfo> ("type", "1002");
			def->addParameter <int> ("cases_count", "5");
			def->addAction("in");
			def->addEvent("default");
			def->addInput <int> ("case", "0");
		}
		
		void in() {
			Variable case_value = in_case->getValue();
			for(int i=0; i<switch_inputs.size(); i++) {
				Variable vv = switch_inputs[i]->getValue();
				if(case_value == vv) {
					getEvent(i+1).run();
					return;
				}
			}
			e_default->run();
		}
		
		void onSetLogic(Logic *logic) {
			invalidate();
		}
	};
	
	//----------------------------------------------------------------------
	//
	// class Switcher
	//
	//----------------------------------------------------------------------

	class Switcher : public Neurone {

		Parameter *p_type;
		Parameter *fp_outs_count;
		Input *in_case;
		Event *e_default;
		QVector<Parameter*> switch_inputs;

	public:

		~Switcher() {
			fp_outs_count->getVariable().setOnChangeAction(NULL);
		}
		
		const TypeDef *getParameterTypeDef() const {
			int type_id = p_type->getValue().getInt();
			if(getLogic() == NULL) return NULL;
			if(getLogic()->getScript() == NULL) return NULL;
			return getLogic()->getScript()->getLibraries()->getTypeDefById(type_id);
		}

		void invalidate() {
			//установка типа case
			if(getLogic() == NULL) return;
			if(getLogic()->getScript() == NULL) return;
			
			const TypeDef *def = getParameterTypeDef();
			TypeInfo type_info = TYPEID(Variable);
			if(def != NULL) {
				type_info = def->getTypeInfo();
			} else {
				errorMessage(format("Switcher: can`t set object type id \"%d\".", p_type->getValue().getInt()));
			}
			in_case->getVariable().setType(type_info);
			
			//настройка
			const Enum *enums = type_info.getEnum();
			
			int outs_count = fp_outs_count->getValue().getInt();
			if(outs_count < 0) outs_count = 0;
			if(enums) {
				if(outs_count > enums->getItemsCount()) outs_count = enums->getItemsCount();
			}
			
			//перестройка входов
			while(switch_inputs.size() > outs_count) {
				Parameter *input = switch_inputs[switch_inputs.size()-1];
				removeParameterInternal(input);
				switch_inputs.remove(switch_inputs.size()-1);
			}
			
			while (switch_inputs.size() < outs_count) {
				Variable vv;
				vv.setType(type_info);
				
				if(type_info == TYPEID(int) || type_info == TYPEID(unsigned int)) {
					vv.setInt(switch_inputs.size());
				}
				
				//автозаполение значений энумоув
				if(enums) {
					int id = enums->getItemId(switch_inputs.size() % enums->getItemsCount());
					vv.setEnum(type_info, id);
				}
				
				Parameter *input = new Parameter(vv);
				Str key = format("case%d", switch_inputs.size());
				addParameter(key, input);
				switch_inputs.append(input);
			}
			
			for(int i=0; i<switch_inputs.size(); i++) {
				Variable vv = switch_inputs[i]->getValue();
				if(vv.getType() != type_info && enums) {
					int id = enums->getItemId(i % enums->getItemsCount());
					vv.setEnum(type_info, id);
				}
				vv.setType(type_info);
				switch_inputs[i]->setValue(vv);
			}
			
			//перестройка событий
			while (getEventsCount() - 1 > outs_count) {
				removeEventInternal(getEventsCount() - 1);
			}

			while (getEventsCount() - 1 < outs_count) {
				Event *event = new Event();
				addEvent(format("case%d", getEventsCount() - 1), event);
			}
		}

		void init() {
			// parameters
			p_type = getParameter("type");
			p_type->getVariable().setOnChangeAction(Common::Action::create(this, &Switcher::invalidate));
			fp_outs_count = getParameter("cases_count");
			fp_outs_count->getVariable().setOnChangeAction(Common::Action::create(this, &Switcher::invalidate));
			
			// inputs
			in_case = getInput("case");
			
			// events
			e_default = getEvent("default");

			// actions
			Action *action = getAction("in");
			action->setAction(Common::Action::create(this, &Switcher::in));
		}
		
		static void define(NeuroneDef *def) {
			def->setKind("FlowControl");
			def->setCreateDummy(false);
			
			def->addParameter <TypeInfo> ("type", "1002");
			def->addParameter <int> ("cases_count", "5");
			def->addAction("in");
			def->addEvent("default");
			def->addInput <int> ("case", "0");
		}
		
		void in() {
			Variable case_value = in_case->getValue();
			for(int i=0; i<switch_inputs.size(); i++) {
				Variable vv = switch_inputs[i]->getValue();
				if(case_value == vv) {
					getEvent(i+1).run();
					return;
				}
			}
			e_default->run();
		}
		
		void onSetLogic(Logic *logic) {
			invalidate();
		}
	};
	
	//----------------------------------------------------------------------
	//
	// class If
	//
	//----------------------------------------------------------------------

	class If : public Neurone {

		Parameter *p_op;
		Parameter *p_type;
		Input *i_a;
		Input *i_b;
		Event *e_then;
		Event *e_else;
		Output *o_res;

	public:

		~If() {
		}

		//
		void init() {

			// parameters
			p_op = getParameter("op");
			p_type = getParameter("type");
			
			// inputs
			i_a = getInput("a");
			i_b = getInput("b");
			
			// outputs
			o_res = getOutput("result");

			// events
			e_then = getEvent("then");
			e_else = getEvent("else");

			// actions
			getAction("check")->setAction(Common::Action::create(this, &If::in));
			p_type->getVariable().setOnChangeAction(Common::Action::create(this, &If::invalidate));
		}
		
		bool hasCaption() {
			return true;
		}
		
		const char *getCaption() {
			return format("a %s b", p_op->getValue().getStr());
		}
		
		static void define(NeuroneDef *def) {
			
			def->setKind("FlowControl");
			def->setCreateDummy(false);
			
			// parameters
			def->addParameter <TypeInfo> ("type", "1007");
			def->addParameter <CompareOperation> ("op", "==");
			
			// outputs
			def->addOutput <bool >("result", "false");
			
			// inputs
			def->addInput <Variable> ("a", "0");
			def->addInput <Variable> ("b", "0");
			
			// events
			def->addEvent("then");
			def->addEvent("else");
			
			// actions
			def->addAction("check");
		}
		
		bool check(const Variable &a, const Variable &b, CompareOperation op) {
			switch (op)	{
				case IF_EQUAL:		return a == b;
				case IF_GEQUAL:		return a == b || a > b;
				case IF_GREATER:	return a > b;
				case IF_LEQUAL:		return !(a > b);
				case IF_LESS:		return !(a > b) && !(a == b);
				case IF_NOTEQUAL:	return !(a == b);
			}
			return false;
		}
		
		void in() {
			CompareOperation op = VariableToType <CompareOperation> (p_op->getValue()).value;
			
			bool res = check(i_a->getValue(), i_b->getValue(), op);
			o_res->getVariable().setBool(res);
			if (res) {
				e_then->run();
			} else {
				e_else->run();
			}
		}
		
		const TypeDef *getParameterTypeDef() const {
			int type_id = p_type->getValue().getInt();
			if(getLogic() == NULL) return NULL;
			if(getLogic()->getScript() == NULL) return NULL;
			return getLogic()->getScript()->getLibraries()->getTypeDefById(type_id);
		}
		
		void invalidate() {
			if(getLogic() == NULL) return;
			if(getLogic()->getScript() == NULL) return;
			const TypeDef *def = getParameterTypeDef();
			TypeInfo type_info = TYPEID(Variable);
			if(def != NULL) {
				type_info = def->getTypeInfo();
			} else {
				errorMessage(format("If: can`t set object type id \"%d\".", p_type->getValue().getInt()));
			}
			i_a->getVariable().setType(type_info);
			i_b->getVariable().setType(type_info);
		}
		
		void onSetLogic(Logic *logic) {
			invalidate();
		}
		
	};

	//----------------------------------------------------------------------
	//
	// class For
	//
	//----------------------------------------------------------------------

	class For : public Neurone {

		Input *i_from;
		Input *i_count;
		Output *o_index;
		Event *e_end;
		Event *e_loop;
		bool need_break;
		
		//
		void start() {
			need_break = false;
			int from = i_from->getValue().getInt();
			for (int i=0; i<i_count->getValue().getInt(); i++) {
				o_index->getVariable().setInt(from + i);
				e_loop->run();
				if(need_break) break;
			}
			e_end->run();
		}
		
		//
		void break_() {
			need_break = true;
		}
		
	public:

		//
		void init() {

			// inputs
			i_from = getInput("from");
			i_count = getInput("count");

			// outputs
			o_index = getOutput("index");

			// actions
			getAction("start")->setAction(Common::Action::create(this, &For::start));
			getAction("break")->setAction(Common::Action::create(this, &For::break_));

			// events
			e_end = getEvent("end");
			e_loop = getEvent("loop");
			
			// variables
			need_break = false;
		}

		static void define(NeuroneDef *def) {
			
			def->setKind("FlowControl");
			
			// inputs
			def->addInput <int> ("from", "0");
			def->addInput <int> ("count", "10");

			// outputs
			def->addOutput <int >("index", "0");

			// actions
			def->addAction("start");
			def->addAction("break");

			// events
			def->addEvent("end");
			def->addEvent("loop");
		}
		
	};
	
	//----------------------------------------------------------------------
	//
	// class ForWait
	//
	//----------------------------------------------------------------------

	class ForWait : public Neurone {

		Input *i_from;
		Input *i_count;
		Output *o_index;
		Event *e_end;
		Event *e_loop;
		bool need_break;
		int counter;
		
		void forProcess() {
			if(counter >= i_count->getValue().getInt() || need_break) {
				e_end->run();
			}
			else {
				o_index->getVariable().setInt(counter);
				e_loop->run();
			}
		}
		
		//
		void start() {
			need_break = false;
			counter = 0;
			forProcess();
		}
		
		void next() {
			need_break = false;
			counter++;
			forProcess();
		}
		
		//
		void break_() {
			need_break = true;
		}
		
	public:

		//
		void init() {

			// inputs
			i_from = getInput("from");
			i_count = getInput("count");

			// outputs
			o_index = getOutput("index");

			// actions
			getAction("start")->setAction(Common::Action::create(this, &ForWait::start));
			getAction("next")->setAction(Common::Action::create(this, &ForWait::next));
			getAction("break")->setAction(Common::Action::create(this, &ForWait::break_));

			// events
			e_end = getEvent("end");
			e_loop = getEvent("loop");
			
			// variables
			need_break = false;
		}

		static void define(NeuroneDef *def) {
			
			def->setKind("FlowControl");
			
			// inputs
			def->addInput <int> ("from", "0");
			def->addInput <int> ("count", "10");

			// outputs
			def->addOutput <int >("index", "0");

			// actions
			def->addAction("start");
			def->addAction("next");
			def->addAction("break");

			// events
			def->addEvent("end");
			def->addEvent("loop");
		}
		
	};
	
	//----------------------------------------------------------------------
	//
	// class While
	//
	//----------------------------------------------------------------------

	class While : public Neurone {

		Input *i_condition;
		Event *e_end;
		Event *e_loop;
		bool need_break;
		
		//
		void start() {
			need_break = false;
			while(i_condition->getValue().getBool()) {
				e_loop->run();
				if(need_break) break;
			}
			e_end->run();
		}
		
		//
		void break_() {
			need_break = true;
		}
		
	public:

		//
		void init() {

			// inputs
			i_condition = getInput("condition");

			// actions
			getAction("start")->setAction(Common::Action::create(this, &While::start));
			getAction("break")->setAction(Common::Action::create(this, &While::break_));

			// events
			e_end = getEvent("end");
			e_loop = getEvent("loop");
			
			// variables
			need_break = false;
		}

		static void define(NeuroneDef *def) {
			
			def->setKind("FlowControl");
			
			// inputs
			def->addInput <bool> ("condition", "true");

			// actions
			def->addAction("start");
			def->addAction("break");

			// events
			def->addEvent("end");
			def->addEvent("loop");
		}
		
	};
	
	//----------------------------------------------------------------------
	//
	// class New
	//
	//----------------------------------------------------------------------

	class New : public Neurone {

		Parameter *p_type;
		Event *e_out;
		Output *o_new;

	public:

		~New() {}

		//
		void init() {

			// parameters
			p_type = getParameter("type");
			
			// events
			e_out = getEvent("out");
			
			// outputs
			o_new = getOutput("object");

			// actions
			getAction("in")->setAction(Common::Action::create(this, &New::in));
			p_type->getVariable().setOnChangeAction(Common::Action::create(this, &New::invalidate));
		}
		
		bool hasCaption() {
			return true;
		}
		
		const TypeDef *getParameterTypeDef() const {
			int type_id = p_type->getValue().getInt();
			if(getLogic() == NULL) return NULL;
			if(getLogic()->getScript() == NULL) return NULL;
			return getLogic()->getScript()->getLibraries()->getTypeDefById(type_id);
		}
		
		const char *getCaption() {
			const TypeDef *def = getParameterTypeDef();
			if(def == NULL) return "{Invalid}";
			return def->getTypeInfo().name();
		}
		
		static void define(NeuroneDef *def) {
			
			def->setKind("Data");
			def->setCreateDummy(false);
			
			// parameters
			def->addParameter <TypeInfo> ("type", "1002");
			
			// output
			def->addOutput <int> ("object", "0");
			
			// events
			def->addEvent("out");
			
			// actions
			def->addAction("in");
		}
		
		void in() {
			/*
			const TypeDef *def = getParameterTypeDef();
			if(def == NULL) {
				return;
			}
			*/
			o_new->getVariable().create();
			e_out->run();
		}
		
		void invalidate() {
			if(getLogic() == NULL) return;
			if(getLogic()->getScript() == NULL) return;
			const TypeDef *def = getParameterTypeDef();
			TypeInfo type_info = TYPEID(int);
			if(def != NULL) {
				type_info = def->getTypeInfo();
			} else {
				errorMessage(format("New: can`t set object type id \"%d\".", p_type->getValue().getInt()));
			}
			o_new->getVariable().setType(type_info);
		}
		
		void onSetLogic(Logic *logic) {
			invalidate();
		}
	};

	//----------------------------------------------------------------------
	//
	// class Shutdown
	//
	//----------------------------------------------------------------------

	class Shutdown : public Neurone
	{

		void in() {
			getRootLogic()->getScript()->shutdown();
		}

	public:

		~Shutdown() {}

		void init() {
			getAction("in")->setAction(Common::Action::create(this, &Shutdown::in));
		}

		static void define(NeuroneDef *def) {
			def->setKind("Core");
			def->addAction("in");
		}
	};
	
	//----------------------------------------------------------------------
	
	void Delete(Variable variable) {
		variable.destroy();
	}
	
	//----------------------------------------------------------------------

	int intAdd(int a, int b) { return a + b; }
	int intSub(int a, int b) { return a - b; }
	int intMul(int a, int b) { return a * b; }
	int intDiv(int a, int b) { return a / b; }
	int intMod(int a, int b) { return a % b; }
	int intAbs(int a) { return abs(a); }
	int intSqrt(int a) { return sqrt(a); }
	
	double doubleAdd(double a, double b) { return a + b; }
	double doubleSub(double a, double b) { return a - b; }
	double doubleMul(double a, double b) { return a * b; }
	double doubleDiv(double a, double b) { return a / b; }
	double doubleAbs(double a) { return abs(a); }
	double doubleSqrt(double a) { return sqrt(a); }

	int intEqual(int a, int b) { return a == b; }
	void intNothing(int a) {}
	
	Variable connectionVar(const Variable &a) { return a; }
	
	Variable addVar(const Variable &a, const Variable &b) { return a + b; }
	Variable subVar(const Variable &a, const Variable &b) { return a - b; }
	Variable mulVar(const Variable &a, const Variable &b) { return a * b; }
	Variable divVar(const Variable &a, const Variable &b) { return a / b; }
	
	Variable strConst(const char *s) { return Variable(s); }
	int intConst(int v) { return v; }
	float floatConst(float v) { return v; }
	double doubleConst(double v) { return v; }
	bool boolConst(bool v) { return v; }

	VariableArray arrayCreate() {
		return new VariableArrayData();
	}
	
	int arraySize(VariableArray array) {
		return array->count();
	}
	
	void arrayAppend(VariableArray array, const Variable &v) {
		array->append(new Variable(v));
	}
	
	void arrayAppend(VariableArray array_dst, VariableArray array_src) {
		for(int i=0; i<array_src->count(); i++) {
			array_dst->append(new Variable(*array_src->getItem(i)));
		}
	}
	
	int arrayIndexOf(VariableArray array, const Variable &v) {
		for(int i=0; i<array->count(); i++) {
			if(v == *array->getItem(i)) return i;
		}
		return -1;
	}

	void arrayResize(VariableArray array, int size){
		if (array->count() == size) return;
		while (array->count() < size) {
			array->append(new Variable());
		}
		while (array->count() > size) {
			array->remove(array->count() - 1);
		}
	}
	
	const Variable &arrayGet(VariableArray array, int index) {
		if(index < 0 || index >= array->count()) {
			errorMessage(format("arrayGet(index = %d) : index out of range (count = %d).", index, array->count()));
			static Variable dummy;
			dummy = Variable();
			return dummy;
		}
		return *array->getItem(index);
	}
	
	void arraySet(VariableArray array, int index, const Variable &value) {
		if(index < 0 || index >= array->count()) {
			errorMessage(format("arraySet(index = %d) : index out of range (count = %d).", index, array->count()));
			return;
		}
		*array->getItem(index) = value;
	}
	
	void arrayRemove(VariableArray array, int index) {
		if(index < 0 || index >= array->count()) {
			errorMessage(format("arrayRemove(index = %d) : index out of range (count = %d).", index, array->count()));
			return;
		}
		array->remove(index);
	}
	
	void arrayClear(VariableArray array) {
		array->clear();
	}
	
	void message(const char *text) {
		if(consoleMessageCallback)
			consoleMessageCallback(text);
	}
	
	void messageExt(const char *message, bool endl, Qt::GlobalColor color, bool bold, bool italic) {
		if(consoleMessageExtCallback)
			consoleMessageExtCallback(message, endl, color, bold, italic);
	}
	
	TypeInfo type_id(const Variable &var) {
		return var.getType();
	}
	
	const char *type_name(const TypeInfo &type_info) {
		return type_info.name();
	}

	//Map
	VariableMap mapCreate() {
		return new VariableMapData();
	}
	
	int mapSize(VariableMap map) {
		return map->count();
	}
	
	void mapAppend(VariableMap map, const Variable &key, const Variable &v) {
		map->append(new Variable(key), new Variable(v));
	}
	
	const Variable &mapGet(VariableMap map, const Variable &key) {
		return *map->get(key);
	}
	
	const Variable &mapGet(VariableMap map, int index) {
		return map->operator [] (index).getValue();
	}
	
	const Variable &mapKey(VariableMap map, int index) {
		return map->operator [] (index).getKey();
	}
	
	bool mapContains(VariableMap map, const Variable &key) {
		return map->indexOf(key) != -1;
	}
	
	void mapRemove(VariableMap map, const Variable &key) {
		map->remove(key);
	}
	
	void mapClear(VariableMap map) {
		map->clear();
	}
	
	//----------------------------------------------------------------------
	//
	// initStdLib
	//
	//----------------------------------------------------------------------
	
	void initStdLib(Library *library) {
		library->setName("Std");
		library->addType <char>						(0.3f, 1.0, 0.3f, NULL);
		library->addType <int>						(0.3f, 1.0, 0.3f, NULL);
		library->addType <unsigned int>				(0.3f, 1.0, 0.3f, NULL);
		library->addType <long long int>			(0.3f, 1.0, 0.3f, NULL);
		library->addType <unsigned long long int>	(0.3f, 1.0, 0.3f, NULL);
		library->addType <float>					(0.3f, 0.5f, 1.0f, NULL);
		library->addType <double>					(0.5f, 0.6f, 1.0f, NULL);
		library->addType <double *>					(0.4f, 0.5f, 1.0f, NULL);
		library->addType <const char *>				(1.0f, 1.0, 0.3f, NULL);
		library->addType <bool>						(1.0f, 0.5f, 0.8f, NULL);
		library->addType <void *>					(1.0f, 0.0f, 0.0f, NULL);
		library->addType <TypeInfo>					(0.7f, 0.5f, 0.7f, ADD_IMG(":/types/types/pinion.png"));
		library->addType <Variable>					(0.7f, 0.5f, 0.7f, ADD_IMG(":/types/types/variable.png"));
		library->addType <Variable*>				(0.2f, 0.6f, 0.3f, ADD_IMG(":/types/types/variable.png"));
		library->addType <VariableArray>			(0.4f, 1.0f, 0.3f, ADD_IMG(":/types/types/array.png"));
		library->addType <VariableMap>				(0.7f, 1.0f, 0.1f, ADD_IMG(":/types/types/array.png"));
		library->addType <CompareOperation>			(1.0f, 0.3f, 0.3f, NULL);
		library->addType <Undefined>				(1.0f, 0.0f, 0.0f, ADD_IMG(":/types/types/undefined.png"));
		library->addType <Qt::GlobalColor> (0.1f, 1.0f, 0.3f, NULL);
		
		library->addTemplate <If>			("If");
		library->addTemplate <For>			("For");
		library->addTemplate <ForWait>		("ForWait");
		library->addTemplate <While>		("While");
		library->addTemplate <Connector>	("FlowConnector");
		library->addTemplate <Sequence>		("Sequence");
		library->addTemplate <Switch>		("Switch");
		library->addTemplate <RandomSwitch>	("RandomSwitch");
		library->addTemplate <Switcher>		("Switcher");
		library->addTemplate <UberSwitch>	("UberSwitch");
		library->addTemplate <Mux>			("Mux");

		library->addTemplate <Shutdown>		("Shutdown");
		
		library->addTemplate <New>			("New");
		library->addFunction("Data", "Delete", "Delete", &Delete, "", {"object"});
		library->addDefinition("Data", makeNeuroneFunction1("ValueConnector", &connectionVar, "out", "in"));
		library->addFunction("Data", "TypeID", "TypeID", &type_id, "type_info", {"variable"});
		library->addFunction("Data", "TypeName", "TypeName", &type_name, "type_name", {"type_info"});
		
		//------------------------------------------------------------------
		//
		// int
		//
		//------------------------------------------------------------------
		
		library->addDefinition("Math", makeNeuroneFunction2("IntAdd", &intAdd, "a + b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("IntSub", &intSub, "a - b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("IntMul", &intMul, "a * b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("IntDiv", &intDiv, "a / b", "a", "b = 1"));
		library->addDefinition("Math", makeNeuroneFunction2("IntMod", &intMod, "a mod b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction1("IntAbs", &intAbs, "|a|", "a"));
		library->addDefinition("Math", makeNeuroneFunction1("IntSqrt", &intSqrt, "sqrt(a)", "a"));
		library->addDefinition("Math", makeNeuroneFunction2("IntEqual", &intEqual, "a == b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction1("IntNothing", &intNothing, "", "a"));
		
		//------------------------------------------------------------------				
		//																					
		// double																			
		//																					
		//------------------------------------------------------------------				
																							
		library->addDefinition("Math", makeNeuroneFunction2("DoubleAdd", &doubleAdd, "a + b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("DoubleSub", &doubleSub, "a - b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("DoubleMul", &doubleMul, "a * b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("DoubleDiv", &doubleDiv, "a / b", "a", "b = 1"));
		library->addDefinition("Math", makeNeuroneFunction1("DoubleAbs", &doubleAbs, "|a|", "a"));
		library->addDefinition("Math", makeNeuroneFunction1("DoubleSqrt", &doubleSqrt, "sqrt(a)", "a"));
		//------------------------------------------------------------------				
		//																					
		// var																			
		//																					
		//------------------------------------------------------------------				
																							
		library->addDefinition("Math", makeNeuroneFunction2("AddVar", &addVar, "a + b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("SubVar", &subVar, "a - b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("MulVar", &mulVar, "a * b", "a", "b"));
		library->addDefinition("Math", makeNeuroneFunction2("DivVar", &divVar, "a / b", "a", "b = 1"));

		//------------------------------------------------------------------				
		//																					
		// const																			
		//																					
		//------------------------------------------------------------------				
																							
		library->addDefinition("Math", makeNeuroneFunction1("StrConst", &strConst, "value", "result"));
		library->addDefinition("Math", makeNeuroneFunction1("IntConst", &intConst, "value", "result"));
		library->addDefinition("Math", makeNeuroneFunction1("FloatConst", &floatConst, "value", "result"));
		library->addDefinition("Math", makeNeuroneFunction1("DoubleConst", &doubleConst, "value", "result"));
		library->addDefinition("Math", makeNeuroneFunction1("BoolConst", &boolConst, "value", "result"));

		//------------------------------------------------------------------				
		//																					
		// Array																			
		//																					
		//------------------------------------------------------------------				
		
		library->addDefinition("Array", makeNeuroneFunction0("arrayCreate", &arrayCreate, "*new"));
		library->addDefinition("Array", makeNeuroneFunction1("arraySize", &arraySize, "size", "array"));
		library->addFunction<void, VariableArray, const Variable&>("Array", "arrayAppend", &arrayAppend, "", {"array", "value"});
		library->addFunction<void, VariableArray, VariableArray>("Array", "arrayAppendArr", &arrayAppend, "", {"array_dst", "array_src"});
		library->addDefinition("Array", makeNeuroneFunction2("arrayIndexOf", &arrayIndexOf, "index", "array", "value"));
		library->addDefinition("Array", makeNeuroneFunction2("arrayRemove", &arrayRemove, "", "array", "index"));
		library->addDefinition("Array", makeNeuroneFunction2("arrayGet", &arrayGet, "value", "array", "index"));
		library->addDefinition("Array", makeNeuroneFunction3("arraySet", &arraySet, "", "array", "index", "value"));
		library->addDefinition("Array", makeNeuroneFunction1("arrayClear", &arrayClear, "", "array"));
		library->addDefinition("Array", makeNeuroneFunction("arrayResize", &arrayResize, "", { "array", "size" } ));
		
		//------------------------------------------------------------------				
		//																					
		// Map																			
		//																					
		//------------------------------------------------------------------				
		
		library->addFunction("Map", "mapCreate", &mapCreate, "*new", {});
		library->addFunction("Map", "mapSize", &mapSize, "size", {"map"});
		library->addFunction("Map", "mapAppend", &mapAppend, "", {"map", "key", "value"});
		library->addFunction<const Variable &, VariableMap, const Variable &>("Map", "mapGet", &mapGet, "value", {"map", "key"});
		library->addFunction<const Variable &, VariableMap, int>("Map", "mapGetByIndex", &mapGet, "value", {"map", "index"});
		library->addFunction<const Variable &, VariableMap, int>("Map", "mapKey", &mapKey, "value", {"map", "index"});
		library->addFunction("Map", "mapContains", &mapContains, "res", {"map", "key"});
		library->addFunction("Map", "mapRemove", &mapRemove, "", {"map", "key"});
		library->addFunction("Map", "mapClear", &mapClear, "", {"map"});

		//------------------------------------------------------------------				
		//																					
		// Console																			
		//																					
		//------------------------------------------------------------------	
		
		library->addDefinition("Console", Anandamide::makeNeuroneFunction1("Message", &message, "result", "text"));
		library->addDefinition("Console", Anandamide::makeNeuroneFunction5("messageExt", &messageExt, "result", "text", "endLine=true", "color=Qt::black", "bold=false", "italic=false"));
		
		//------------------------------------------------------------------				
		
	}
}

extern "C" ANANDAMIDE_API void anandamideDefineLibrary(Anandamide::Library *library) {
	initStdLib(library);
}

void setMsgFunction(ConsoleMessageCallback callBack)
{
	consoleMessageCallback = callBack;
}

void setMsgExtFunction(ConsoleMessageExtCallback callBack)
{
	consoleMessageExtCallback = callBack;
}
