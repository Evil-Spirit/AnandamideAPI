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

#include "AnandamideNeurone.h"

//------------------------------------------------------------------------------

#include "AnandamideLogic.h"
#include "AnandamideLibrary.h"
#include "Renderer.h"
#include "Xml.h"

//------------------------------------------------------------------------------

#define TEXT_SIZE 12.0f

//------------------------------------------------------------------------------

namespace Anandamide {

	//--------------------------------------------------------------------------
	
	float Neurone::head_h = 16.0f;
	float Neurone::flow_h = 16.0f;
	float Neurone::flow_w = 75.0f;
	float Neurone::width =  160.0f;
	
	Neurone::DesignInfo *Neurone::di() {
		if(design_info == nullptr) {
			design_info = new DesignInfo();
		} 
		return design_info;
	}
	
	const Neurone::DesignInfo *Neurone::di() const {
		if(design_info == nullptr) {
			design_info = new DesignInfo();
		} 
		return design_info;
	}
	
	void Neurone::setNeedDelete(bool state) {
		di()->need_delete = state;
	}
	
	Parameter *Neurone::addParameter(const char *id, Parameter *item) {
		parameters.append(new Str(id), item);
		return item;
	}
	
	void Neurone::removeParameterInternal(int index)  {
		parameters.remove(index);
	}
	
	void Neurone::removeParameterInternal(Parameter *event)  {
		int index = parameters.seek(event);
		if (index == -1) return;
		parameters.remove(index);
	}
	
	Event *Neurone::addEvent(const char *id, Event *item) {
		item->setNeurone(this);
		events.append(new Str(id), item);
		return item;
	}
	
	void Neurone::removeEventInternal(int index)  {
		events.remove(index);
	}
	
	void Neurone::removeEventInternal(Event *event)  {
		int index = events.seek(event);
		if (index == -1) return;
		events.remove(index);
	}
	
	bool Neurone::renameEventInternal(const char *old_name, const char *new_name) {
		return events.rename(old_name, new_name);
	}
	
	Input *Neurone::addInput(const char *id, Input *item) {
		item->setNeurone(this);
		inputs.append(new Str(id), item);
		return item;
	}
	
	void Neurone::removeInputInternal(int index) {
		inputs.remove(index);
	}
	
	void Neurone::removeInputInternal(Input *item) {
		int index = inputs.seek(item);
		if (index == -1) return;
		inputs.remove(index);
	}
	
	bool Neurone::renameInputInternal(const char *old_name, const char *new_name) {
		return inputs.rename(old_name, new_name);
	}
	
	Output *Neurone::addOutput(const char *id, Output *item) {
		outputs.append(new Str(id), item);
		return item;
	}
	
	void Neurone::removeOutputInternal(int index) {
		outputs.remove(index);
	}
	
	void Neurone::removeOutputInternal(Output *item) {
		int index = outputs.seek(item);
		if (index == -1) return;
		outputs.remove(index);
	}
	
	bool Neurone::renameOutputInternal(const char *old_name, const char *new_name) {
		return outputs.rename(old_name, new_name);
	}
	
	Action *Neurone::addAction(const char *id, Action *item) {
		item->setNeurone(this);
		actions.append(new Str(id), item);
		return item;
	}
	
	void Neurone::removeActionInternal(int index)  {
		actions.remove(index);
	}
	
	void Neurone::removeActionInternal(Action *action)  {
		int index = actions.seek(action);
		if (index == -1) return;
		actions.remove(index);
	}
	
	bool Neurone::renameActionInternal(const char *old_name, const char *new_name) {
		return actions.rename(old_name, new_name);
	}
	
	void Neurone::onRender(Renderer *renderer)
	{
		float flow_height = ::math::max_(getActionsHeight(), getEventsHeight());
		float data_height = ::math::max_(getInputsHeight(), getOutputsHeight());
		//float height = getHeight() - flow_h;
		
		
//		if(markedActionNumber >= 0) {
//			if (selected) {
//				renderer->color(0.953f, 0.086f, 0.365f);
//			} else {
//				renderer->color(0.976f, 0.259f, 0.486f);
//			}
//		}
//		else {
			if (di()->selected) {
				//ошибка при загрузке блока
				if (di()->error_code != 0) {
					renderer->color(0.99f, 0.1f, 0.1f);
				} else renderer->color(0.5f, 0.3f, 1.0f);
			} else  
			if(di()->hovered) {
				if (di()->error_code != 0) {
					renderer->color(0.80f, 0.2f, 0.2f);
				} else renderer->color(0.7f, 0.6f, 0.9f);
			} else {
				if (di()->error_code != 0) {
					renderer->color(0.99f, 0.0f, 0.0f);
				} else renderer->color(0.9f, 0.9f, 0.9f);
			}
//		}

		float header = head_h;

		if (hasCaption())  {
			// caption
			header += head_h;
			renderer->rect3d(getPosX(), getPosY(), width, header);
			renderer->color(0.0f, 0.0f, 0.0f);
			renderer->text(getPosX() + 3, getPosY() + 2, getHeader());
			renderer->text(getPosX() + 3, getPosY() + head_h + 2, getCaption());
		} else {
			// header
			renderer->rect3d(getPosX(), getPosY(), width, head_h);
			renderer->color(0.0f, 0.0f, 0.0f);
			renderer->text(getPosX() + 3, getPosY() + 2, getHeader());
		}

		// body
		renderer->color(0.9f, 0.9f, 0.9f);
		renderer->rect3d(getPosX(), getPosY() + header, width, flow_height);

		if (data_height != 0) {
			//renderer->rect3d(pos_x, pos_y + head_h + flow_height, width, splitter);
			renderer->rect3d(getPosX(), getPosY() + header + flow_height, width, data_height);
		}

		//Actions
		for (int i=0; i<actions.count(); i++) {
			renderer->color(0.9f, 0.9f, 0.9f);
			renderer->rect3d(getPosX(), getPosY() + header + i * flow_h, flow_w, flow_h);
			renderer->color(0.0f, 0.0f, 0.0f);
//			renderer->text(pos_x + 3, pos_y + header + i * flow_h + 2, actions[i].getKey());
			renderer->textToWidth(getPosX() + 2, flow_w - 4, getPosY() + header + (i + 0.5) * flow_h, true, actions[i].getKey());
		}

		//Events
		for (int i=0; i<events.count(); i++) {
			renderer->color(0.9f, 0.9f, 0.9f);
			renderer->rect3d(getPosX() + width - flow_w, getPosY() + header + i * flow_h, flow_w, flow_h);
			renderer->color(0.0f, 0.0f, 0.0f);
//			renderer->text(pos_x + 3 + width - flow_w, pos_y + header + i * flow_h + 2, events[i].getKey());
			renderer->textToWidth(getPosX() + 2 + width - flow_w, flow_w - 4, getPosY() + header + (i + 0.5) * flow_h, false, events[i].getKey());
		}

		float y = getPosY() + header + flow_height /*+ splitter*/;

		//Inputs
		for (int i=0; i<inputs.count(); i++) {
			
			int icon_shift = 0;
			const TypeDef *type_def = inputs[i].getValue().getTypeDef();
			
			vec3 color(0.3f, 1.0, 1.0f);
			if(type_def) {
				//QImage instance
//				if(!type_def->getImage().isNull()) icon_shift = flow_h;
				//QImage pointer
				if(type_def->getImage()) icon_shift = flow_h;
				color = vec3(type_def->getR(), type_def->getG(), type_def->getB());
			}
			renderer->color(color);
			
			renderer->rect3d(getPosX(), y + i * flow_h, flow_w, flow_h);
			renderer->color(0.0f, 0.0f, 0.0f);
			
			//QImage instance
//			if(type_def != NULL && !type_def->getImage().isNull()) {
//				icon_shift = flow_h;
//				renderer->drawImage(pos_x, y + i * flow_h, flow_h, flow_h, type_def->getImage());
//			}
			//QImage pointer
			if(type_def != NULL && type_def->getImage()) {
				icon_shift = flow_h;
				renderer->drawImage(getPosX(), y + i * flow_h, flow_h, flow_h, *type_def->getImage());
			}
			
			if (inputs[i].getValue().getSource() != NULL) {
//				renderer->text(pos_x + 3 + icon_shift, y + i * flow_h + 2, inputs[i].getKey());
				renderer->textToWidth(getPosX() + 2 + icon_shift, flow_w - 4 - icon_shift, y + (i + 0.5) * flow_h, true, inputs[i].getKey());
			} else {
//				renderer->text(pos_x + 3 + icon_shift, y + i * flow_h + 2, format("%s = %s", inputs[i].getKey().str(), inputs[i].getValue().getValue().getStr()));
				renderer->textToWidth(getPosX() + 2 + icon_shift, flow_w - 4 - icon_shift, y + (i + 0.5) * flow_h, true, format("%s = %s", inputs[i].getKey().str(), inputs[i].getValue().getValue().getStr()));
			}
			
		}

		//Outputs
		for (int i=0; i<outputs.count(); i++) {
			int icon_shift = 0;
			const TypeDef *type_def = type_def = outputs[i].getValue().getTypeDef();
			
			vec3 color(0.3f, 1.0, 1.0f);
			if(type_def) color = vec3(type_def->getR(), type_def->getG(), type_def->getB());
			renderer->color(color);
			
			renderer->rect3d(getPosX() + width - flow_w, y + i * flow_h, flow_w, flow_h);
			renderer->color(0.0f, 0.0f, 0.0f);
			//QImage instance
//			if(type_def != NULL && !type_def->getImage().isNull()) {
//				renderer->drawImage(pos_x + width - flow_h, y + i * flow_h, flow_h, flow_h, type_def->getImage());
//			}
			//QImage pointer
			if(type_def != NULL && type_def->getImage()) {
				icon_shift = flow_h;
				renderer->drawImage(getPosX() + width - flow_h, y + i * flow_h, flow_h, flow_h, *type_def->getImage());
			}
//			renderer->text(pos_x + 3 + width - flow_w, y + i * flow_h + 2, outputs[i].getKey());
			renderer->textToWidth(getPosX() + 2 + width - flow_w, flow_w - 4 - icon_shift, y + (i + 0.5) * flow_h, false, outputs[i].getKey());
		}
	}
	
	void Neurone::onSelected() { }
	
	void Neurone::onHovered() {
		//messageMessage(format("%s: hovered %s\n", getType(), (hovered) ? "true" : "false"));
	}
	
	void Neurone::onInit(Libraries *libraries) {}
	
	void Neurone::onShutdown() {}
	
	void Neurone::onInvalidate() {}
	
	Neurone::Neurone() {
		design_info = NULL;
		logic = NULL;
		def = NULL;
		dummyNode = NULL;
		runTimeNode = NULL;
	}
	
	Neurone::~Neurone() {
		setNeedDelete(false);
		if (di()->on_destroy != NULL) di()->on_destroy->run();
		setLogic(NULL);
		delete design_info;
	}
	
	const char *Neurone::getName() const {
		return di()->name;
	}
	
	const char *Neurone::getHeader() const {
		if(def == NULL) return getType();
		return def->getHeader();
	}
	
	const char *Neurone::getInstance() const {
		return "";
	}
	
	const char *Neurone::getType() const {
		//if(def == NULL) return "Undefined";
		if(def == NULL) return getName();
		return def->getName();
	}
	
	void Neurone::setName(const char *name_) {
		di()->name = name_;
	}
	
	unsigned long long Neurone::getSize() {
		unsigned long long res = 0;
		
		//qDebug() << "Neurone" << name << sizeof(Neurone) << sizeof(*this);
		
		res += sizeof(Neurone);
		
		res += di()->name.size();
		res += di()->comment.size();
		
		//qDebug() << name << "params" << sizeof(parameters) << "actions" << sizeof(actions) << "events" << sizeof(events) << "inputs" << sizeof(inputs) << "outputs" << sizeof(outputs);
		
		res += sizeof(parameters);
		res += sizeof(actions);
		res += sizeof(events);
		res += sizeof(inputs);
		res += sizeof(outputs);
		
		res += parameters.count() * sizeof(Parameter);
		res += actions.count() * sizeof(Action);
		res += events.count() * sizeof(Event);
		res += inputs.count() * sizeof(Input);
		res += outputs.count() * sizeof(Output);
		
		return res;
	}
	
	void Neurone::setError(int code)
	{
		di()->error_code = code;
	}
	
	int Neurone::getError()
	{
		return di()->error_code;
	}
	
	void Neurone::update(float dt) { }
	
	void Neurone::define(NeuroneDef *definition) { }
	
	void Neurone::invalidate() { }
	
	void Neurone::afterCompile() { }
	
	//--------------------------------------------------------------------------
	
	void Neurone::write(Xml *xml) const {
		
		xml->setArg("type", getType());
		if(*getInstance() != '\0') xml->setArg("instance", getInstance());
		if(*getLibraryName() != '\0') xml->setArg("library", getLibraryName());
		xml->setArg("pos_x", getPosX());
		xml->setArg("pos_y", getPosY());
		xml->setArg("index", blockIndex);

		if(di()->comment != "") {
			xml->setArg("comment", di()->comment.str());
		}

		if (events.count() > 0) {
			Xml *events_node = NULL;
			for (int i=0; i<events.count(); i++) {

				const Event *event = &events[i].getValue();
				if (event->getActionsCount() < 1) continue;

				if (events_node == NULL) events_node = xml->addChild("events");

				Xml *event_node = events_node->addChild("event");
				event_node->setArg("name", events[i].getKey());

				for (int j=0; j<event->getActionsCount(); j++) {
					Xml *action_node = event_node->addChild("action");
					const Action *action = &event->getAction(j);
					action_node->setArg("node", logic->indexOf(action->getNeurone()));
					int action_index = action->getNeurone()->indexOf(action);
					action_node->setData(action->getNeurone()->getActionName(action_index));

				}

			}
		}

		if (parameters.count() > 0) {
			Xml *parameters_node = xml->addChild("parameters");
			for (int i=0; i<parameters.count(); i++) {

				Xml *parameter_node = parameters_node->addChild("parameter");
				parameter_node->setArg("name", parameters[i].getKey());
				parameter_node->setData(parameters[i].getValue().getValue().getStr());

			}
		}

		if (inputs.count() > 0) {
			Xml *inputs_node = xml->addChild("inputs");
			for (int i=0; i<inputs.count(); i++) {

				Xml *input_node = inputs_node->addChild("input");
				input_node->setArg("name", inputs[i].getKey());

				if (inputs[i].getValue().getSource() == NULL) {
					input_node->setArg("value", inputs[i].getValue().getValue().getStr());
				} else {
					input_node->setArg("source_node", logic->indexOf(inputs[i].getValue().getSource()->getInstance()));
					int output_index = inputs[i].getValue().getSource()->getInstance()->indexOf(inputs[i].getValue().getSource());
					input_node->setArg("source_output", inputs[i].getValue().getSource()->getInstance()->getOutputName(output_index));
				}

			}
		}

		onWrite(xml);

	}

	//--------------------------------------------------------------------------

	void Neurone::readParameters(const Xml *xml) {

		xml->getArg("pos_x", di()->pos_x);
		xml->getArg("pos_y", di()->pos_y);
		xml->getArg("comment", di()->comment);
		xml->getArg("index", blockIndex);

		const Xml *parameters_node = xml->getChild("parameters");

		if (parameters_node != NULL) {

			for (int i=0; i<parameters_node->getChildrenCount(); i++) {

				const Xml *child = parameters_node->getChild(i);
				Str name;
				child->getArg("name", name);
				Parameter *parameter = getParameter(name);

				//Checking for the corpse
				if ((parameter == NULL) && (getError() == 1)) {
					parameter = addParameter(name, new Parameter(Variable(Undefined())));
				} else if (parameter == NULL) continue;

				Str value;
				child->getData(value);
				parameter->getVariable().setFromStr(value);
			}
		}

		onReadParameters(xml);

		invalidate();

	}

	//--------------------------------------------------------------------------
	
	void Neurone::readEvents(const Xml *xml) {

		const Xml *events_node = xml->getChild("events");

		if (events_node != NULL) {

			for (int i=0; i<events_node->getChildrenCount(); i++) {

				const Xml *event_node = events_node->getChild(i);

				Str event_name;
				if (event_node->getArg("name", event_name) == false) continue;
				Event *event = getEvent(event_name);

				//Checking for the corpse
				if (event == NULL && (getError() == 1)) {
					event = addEvent(event_name, new Event());
				//Corpse injection
				} else if (event == NULL && (getError() == 0)) {
					setError(2);
					event = addEvent(event_name, new Event());
					Anandamide::errorMessage(format("Can not find event with name \"%s\" at the neurone \"%s\" definition.", event_name.str(), getType()));
				} else if (event == NULL) continue;

				for (int j=0; j<event_node->getChildrenCount(); j++) {

					const Xml *child = event_node->getChild(j);

					int node_index;
					if (child->getArg("node", node_index) == false) continue;

					Neurone *neurone = &logic->getNode(node_index);
					if (neurone == NULL) continue;

					Str action_name;
					child->getData(action_name);

					Action *action = neurone->getAction(action_name);

					//Corpse action
					if (action == NULL && (neurone->getError() == 1)) {
						action = neurone->addAction(action_name, new Action());
					//Corpse injection
					} else if (action == NULL && (neurone->getError() == 0)) {
						neurone->setError(2);
						action = neurone->addAction(action_name, new Action());
						Anandamide::errorMessage(format("Can not find action with name \"%s\" at the neurone \"%s\" definition.", action_name.str(), neurone->getType()));
					} else if (action == NULL) continue;

					event->addAction(action);

				}

			}
		}

		const Xml *inputs_node = xml->getChild("inputs");

		if (inputs_node != NULL) {

			for (int i=0; i<inputs_node->getChildrenCount(); i++) {

				const Xml *input_node = inputs_node->getChild(i);

				Str input_name;
				if (input_node->getArg("name", input_name) == false) continue;
				Input *input = getInput(input_name);

				//Checking for the corpse
				if (input == NULL && (getError() == 1)) {
					input = addInput(input_name, new Input(Variable(Undefined())));
				} else if (input == NULL && (getError() == 0)) {
					setError(2);
					input = addInput(input_name, new Input(Variable(Undefined())));
					Anandamide::errorMessage(format("Can not find input with name \"%s\" at the neurone \"%s\" definition.", input_name.str(), getType()));
				} else if (input == NULL) continue;

				if(input_node->isArg("value")) {
					input->getVariable().setFromStr(input_node->getArg("value"));
				} else {

					int source_node;
					if (input_node->getArg("source_node", source_node) == false) continue;

					Neurone *neurone = &logic->getNode(source_node);
					if (neurone == NULL) continue;

					Str source_output;
					if (input_node->getArg("source_output", source_output) == false) continue;

					Output *output = neurone->getOutput(source_output);

					//Corpse output
					if (output == NULL && (neurone->getError() == 1)) {
						output = neurone->addOutput(source_output, new Output(neurone, Variable(Undefined())));
					//Corpse injection
					} else if (output == NULL && (neurone->getError() == 0)) {
						neurone->setError(2);
						output = neurone->addOutput(source_output, new Output(neurone, Variable(Undefined())));
						Anandamide::errorMessage(format("Can not find output with name \"%s\" at the neurone \"%s\" definition.", source_output.str(), neurone->getType()));
					} else if (output == NULL) continue;

					input->setSource(output);
				}
			}
		}

		onReadEvents(xml);
	}
	
	bool Neurone::isNeedDelete() { return di()->need_delete; }
	
	const char *Neurone::getCaption() {
		static Str str;
		str = onGetCaption();
		return str.str();
	}
	
	Str Neurone::onGetCaption() const {
		return "Caption";
	}

	void Neurone::onWrite(Xml *xml) const { }

	void Neurone::onReadParameters(const Xml *xml) { }

	void Neurone::onReadEvents(const Xml *xml) { }
	
	bool Neurone::hasCaption() {
		return false;
	}
	
	const char *Neurone::getComment() const
	{
		return di()->comment.str();
	}
	
	bool Neurone::hasComment() const
	{
		return di()->comment != Str();
	}
	
	void Neurone::setComment(const char *comment)
	{
		di()->comment = comment;
	}
	
	void Neurone::setOnDestroyAction(Common::Action *action) {
		di()->on_destroy = action;
	}
	
	Common::Action *Neurone::getOnDestroyAction() {
		return di()->on_destroy;
	}
	
	const NeuroneDef *Neurone::getDef() const {
		return def;
	}
	
	const Library *Neurone::getLibrary() const {
		if(def == NULL) return NULL;
		return def->getLibrary();
	}
	
	const char *Neurone::getLibraryName() const {
		if(getLibrary() == NULL) return "";
		return getLibrary()->getName();
	}
	
	void Neurone::onMouseMove(int x, int y, int button, int buttons, int keyboard_modifiers)
	{
		//... for CustomNeurone
	}
	
	void Neurone::onMousePress(int x, int y, int button, int buttons, int keyboard_modifiers)
	{
		//... for CustomNeurone
	}
	
	void Neurone::onMouseRelease(int x, int y, int button, int buttons, int keyboard_modifiers)
	{
		//... for CustomNeurone
	}
	
	void Neurone::onMouseDoubleClick(int x, int y, int button, int buttons, int keyboard_modifiers)
	{
		//... for CustomNeurone
	}
	
	void Neurone::onKeyPress(int key, int keyboard_modifiers, const char *data)
	{
		//... for CustomNeurone
	}
	
	void Neurone::onKeyRelease(int key, int keyboard_modifiers, const char *data)
	{
		//... for CustomNeurone
	}
	
	Neurone *Neurone::clone() const {
		return NULL;
	}
	
	Neurone *Neurone::dummyClone() const {
		return new NeuroneDummy(*this);	
	}
	
	void Neurone::onSetLogic(Logic * logic) {
		
	}
	
	//--------------------------------------------------------------------------
	
	void Neurone::render(Renderer *renderer) {
		float commentCache_width = 50.0;
		float commentCache_height = 12.0;
		if(hasComment()) {
			renderer->color(1.0f, 0.937f, 0.639f);
			float hCenter = getPosX() + getWidth()/2.0f;
			float left = hCenter - commentCache_width/2;
			float top = getPosY() - 0.5*head_h - commentCache_height;
			renderer->clearPolygonPoints();
			renderer->addPolygonPoint(left - 1, top - 1);
			renderer->addPolygonPoint(left + commentCache_width + 1, top - 1);
			renderer->addPolygonPoint(left + commentCache_width + 1, top + commentCache_height + 1);
			renderer->addPolygonPoint(hCenter + 6, top + commentCache_height + 1);
			renderer->addPolygonPoint(hCenter, getPosY() - 2);
			renderer->addPolygonPoint(hCenter - 6, top + commentCache_height + 1);
			renderer->addPolygonPoint(left - 1, top + commentCache_height + 1);
			renderer->polygonDraw();
			renderer->color(0.204f, 0.082f, 0.592f);
			renderer->textBoldInRect(left, top, commentCache_width, commentCache_height, 0x84 , getComment());
		}
		
		onRender(renderer);

	}
	
	vec3 Neurone::getActionPos(int i) {
		vec3 res;
		float header = (hasCaption()) ? 2 * head_h : head_h;
		res.x = getPosX();
		res.y = getPosY() + header + i * flow_h + floor(flow_h / 2.0f);
		res.z = 1.0f;
		return res;
	}
	
	vec3 Neurone::getEventPos(int i) {
		vec3 res;
		float header = (hasCaption()) ? 2 * head_h : head_h;
		res.x = getPosX() + width;
		res.y = getPosY() + header + i * flow_h + floor(flow_h / 2.0f);
		res.z = 1.0f;
		return res;
	}
	
	vec3 Neurone::getInputPos(int i) {
		vec3 res;
		float header = (hasCaption()) ? 2 * head_h : head_h;
		res.x = getPosX();
		res.y = getPosY() + header + i * flow_h + floor(flow_h / 2.0f) + ::math::max_(getActionsHeight(), getEventsHeight());
		res.z = 1.0f;
		return res;
	}
	
	vec3 Neurone::getOutputPos(int i) {
		vec3 res;
		float header = (hasCaption()) ? 2 * head_h : head_h;
		res.x = getPosX() + width;
		res.y = getPosY() + header + i * flow_h + floor(flow_h / 2.0f) + ::math::max_(getActionsHeight(), getEventsHeight());
		res.z = 1.0f;
		return res;
	}
	
	bool Neurone::isSelected(int x, int y) {
		if(isInputSelected(x, y) != -1) return 0;
		if(isOutputSelected(x, y) != -1) return 0;
		if(isActionSelected(x, y) != -1) return 0;
		if(isEventSelected(x, y) != -1) return 0;
		//float header = (hasCaption()) ? 2 * head_h : head_h;
		return
				x > getPosX() &&
				y > getPosY() &&
				x < getPosX() + getWidth() &&
				y < getPosY() + getHeight();
	}
	
	Action *Neurone::getSelectedAction(int x, int y) 
	{
		int idx = isActionSelected(x, y);
		if(idx < 0) return NULL;
		return &getAction(idx);
	}
	
	int Neurone::isActionSelected(int x, int y) {
		float header = (hasCaption()) ? 2 * head_h : head_h;
		
		for (int i=0; i<actions.count(); i++) {
			if (x > getPosX() &&
				y > getPosY() + header + i * flow_h &&
				x < getPosX() + flow_w &&
				y < getPosY() + header + i * flow_h + flow_h
				) return i;
		}
		
		return -1;
	}
	
	int Neurone::isEventSelected(int x, int y) {
		
		float header = (hasCaption()) ? 2 * head_h : head_h;
		for (int i=0; i<events.count(); i++) {
			
			if (x > getPosX() + width - flow_w &&
				y > getPosY() + header + i * flow_h &&
				x < getPosX() + width &&
				y < getPosY() + header + i * flow_h + flow_h
				) return i;
		}
		
		return -1;
	}
	
	int Neurone::isInputSelected(int x, int y) {
		
		int actions_height = ::math::max_(getActionsHeight(), getEventsHeight());
		float header = (hasCaption()) ? 2 * head_h : head_h;
		
		for (int i=0; i<inputs.count(); i++) {
			
			if (x > getPosX() &&
				y > getPosY() + header + i * flow_h + actions_height &&
				x < getPosX() + flow_w &&
				y < getPosY() + header + i * flow_h + flow_h + actions_height
				) return i;
		}
		
		return -1;
	}
	
	int Neurone::isOutputSelected(int x, int y) {
		
		int events_height = ::math::max_(getActionsHeight(), getEventsHeight());
		float header = (hasCaption()) ? 2 * head_h : head_h;
		
		for (int i=0; i<outputs.count(); i++) {
			
			if (x > getPosX() + width - flow_w &&
				y > getPosY() + header + i * flow_h + events_height  &&
				x < getPosX() + width &&
				y < getPosY() + header + i * flow_h + flow_h + events_height
				) return i;
		}
		
		return -1;
	}
	
	//--------------------------------------------------------------------------
	
	//
	void Neurone::setLogic(Logic *logic_) {
		
		if (logic == logic_) {
			onSetLogic(logic_);
			
			return;
		}
		
		if (logic != NULL) {
			logic->removeNode(this);
		}
		
		logic = logic_;
		
		if (logic != NULL) {
			logic->nodes.append(this);
		}
		onSetLogic(logic_);
		
	}
	
	Logic *Neurone::getLogic() {
		return logic;
	}
	
	Logic *Neurone::getRootLogic() {
		if(logic == NULL) {
			if(dynamic_cast <Logic *> (this) != NULL) return (Logic *)this;
			return NULL;
		}
		return logic->getRootLogic();
	}
	
	const Logic *Neurone::getLogic() const {
		return logic;
	}
	
	bool Neurone::isDummy() const {
		return dummy;
	}
	
	void Neurone::setDummy(bool state) {
		dummy = state;
	}
	
	void Neurone::setDummyNode(Neurone *dummyNode)
	{
		this->dummyNode = dummyNode;
		if(dummyNode != NULL)
			dummyNode->setRunTimeNode(this);
	}
	
	Neurone *Neurone::getDummyNode() const
	{
		return dummyNode;
	}
	
	void Neurone::setRunTimeNode(Neurone *runTimeNode)
	{
		this->runTimeNode = runTimeNode;
	}
	
	Neurone *Neurone::getRunTimeNode() const
	{
		return runTimeNode;
	}
	
	bool Neurone::isCustom()
	{
		return false;
	}
	
	bool Neurone::isActive(int x, int y)
	{
		return false;
	}
	
	int Neurone::getParametersCount() const {
		return parameters.count();
	}
	
	const Parameter &Neurone::getParameter(int i) const {
		return parameters[i].getValue();
	}
	
	Parameter &Neurone::getParameter(int i) {
		return parameters[i].getValue();
	}
	
	const Parameter *Neurone::getParameter(const char *id) const {
		return parameters.get(id);
	}
	
	Parameter *Neurone::getParameter(const char *id) {
		return parameters.get(id);
	}
	
	const char *Neurone::getParameterName(int i) const {
		return parameters[i].getKey();
	}
	
	int Neurone::indexOf(const Action *action) const {
		return actions.seek(action);
	}
	
	int Neurone::getActionsCount() const {
		return actions.count();
	}
	
	const Action &Neurone::getAction(int i) const {
		return actions[i].getValue();
	}
	
	Action &Neurone::getAction(int i) {
		return actions[i].getValue();
	}
	
	const Action *Neurone::getAction(const char *id) const {
		return actions.get(id);
	}
	
	Action *Neurone::getAction(const char *id) {
		return actions.get(id);
	}
	
	Action *Neurone::getActionByName(const char *id) {
		return getAction(id);
	}
	
	const char *Neurone::getActionName(int i) const {
		return actions[i].getKey();
	}
	
	int Neurone::indexOf(const Event *event) const {
		return events.seek(event);
	}
	
	int Neurone::getEventsCount() const {
		return events.count();
	}
	
	const Event &Neurone::getEvent(int i) const {
		return events[i].getValue();
	}
	
	Event &Neurone::getEvent(int i) {
		return events[i].getValue();
	}
	
	const Event *Neurone::getEvent(const char *id) const {
		return events.get(id);
	}
	
	Event *Neurone::getEvent(const char *id) {
		return events.get(id);
	}
	
	const char *Neurone::getEventName(int i) const {
		return events[i].getKey();
	}
	
	int Neurone::indexOf(const Input *input) const {
		return inputs.seek(input);
	}
	
	int Neurone::getInputsCount() const {
		return inputs.count();
	}
	
	const Input &Neurone::getInput(int i) const {
		return inputs[i].getValue();
	}
	
	Input &Neurone::getInput(int i) {
		return inputs[i].getValue();
	}
	
	const Input *Neurone::getInput(const char *id) const {
		return inputs.get(id);
	}
	
	Input *Neurone::getInput(const char *id) {
		return inputs.get(id);
	}
	
	const char *Neurone::getInputName(int i) const {
		return inputs[i].getKey();
	}
	
	int Neurone::indexOf(const Output *output) const {
		return outputs.seek(output);
	}
	
	int Neurone::getOutputsCount() const {
		return outputs.count();
	}
	
	const Output &Neurone::getOutput(int i) const {
		return outputs[i].getValue();
	}
	
	Output &Neurone::getOutput(int i) {
		return outputs[i].getValue();
	}
	
	const Output *Neurone::getOutput(const char *id) const {
		return outputs.get(id);
	}
	
	Output *Neurone::getOutput(const char *id) {
		return outputs.get(id);
	}
	
	const char *Neurone::getOutputName(int i) const {
		return outputs[i].getKey();
	}
	
	int Neurone::getPosX() const {
		return di()->pos_x;
	}
	
	void Neurone::setPosX(int pos_) {
		di()->pos_x = pos_;
	}
	
	int Neurone::getPosY() const {
		return di()->pos_y;
	}
	
	void Neurone::setPosY(int pos_) {
		di()->pos_y = pos_;
	}
	
	int Neurone::getWidth() {
		return width;
	}
	
	int Neurone::getActionsHeight() {
		return actions.count() * flow_h;
	}
	
	int Neurone::getInputsHeight() {
		return inputs.count() * flow_h;
	}
	
	int Neurone::getEventsHeight() {
		return events.count() * flow_h;
	}
	
	int Neurone::getOutputsHeight() {
		return outputs.count() * flow_h;
	}
	
	int Neurone::getHeight() {
		float header = (hasCaption()) ? 2 * head_h : head_h;
		return	::math::max_(getActionsHeight(), getEventsHeight()) +
				::math::max_(getInputsHeight(), getOutputsHeight()) + header;
	}
	
	int Neurone::getRealPosX()
	{
		return ::math::min_(getPosX(), (int)(getPosX() + 0.5 * ((float)getWidth())));
	}
	
	int Neurone::getRealPosY()
	{
		if(hasComment())
			return getPosY() - 0.5*head_h;
		return getPosY();
	}
	
	int Neurone::getRealWidth()
	{
		return getWidth();
	}
	
	int Neurone::getRealHeight()
	{
		if(hasComment())
			return getHeight() + 0.5*head_h;
		return getHeight();
	}
	
	int Neurone::getHeaderHeight() {
		return head_h;
	}
	
	void Neurone::setPos(int pos_x_, int pos_y_) {
		setPosX(pos_x_);
		setPosY(pos_y_);
	}
	
	float Neurone::getActionWireAngle(int index)
	{
		return 0.0f;
	}
	
	float Neurone::getEventWireAngle(int index)
	{
		return 360.0f;
	}
	
	float Neurone::getInputWireAngle(int index)
	{
		return 360.0f;
	}
	
	float Neurone::getOutputWireAngle(int index)
	{
		return 0.0f;
	}
	
	void Neurone::setHovered(bool state) {
		if(di()->hovered == state) return;
		di()->hovered = state;
		onHovered();
	}
	
	bool Neurone::isHovered() const {
		return di()->hovered;
	}
	
	void Neurone::setSelected(bool state) {
		if(di()->selected == state) return;
		di()->selected = state;
		onSelected();
	}
	
	bool Neurone::isSelected() const {
		return di()->selected;
	}

	void Neurone::init() {}

	//--------------------------------------------------------------------------

	void Neurone::create(Libraries *libraries, const NeuroneDef *def_) {
		def = def_;
		if(def == NULL) return;
		
		// parameters
		for (int i=0; i<def->parameters.count(); i++) {
			Variable var = libraries->createVariableByType(def->parameters[i].type_id);
			var.setFromStr(def->parameters[i].value);
			Parameter *parameter = new Parameter(var);
			addParameter(def->parameters[i].name, parameter);
		}

		// events
		for (int i=0; i<def->events.count(); i++) {
			Event *event = new Event();
			addEvent(def->events[i].name, event);
		} 

		// inputs
		for (int i=0; i<def->inputs.count(); i++) {
			Variable var = libraries->createVariableByType(def->inputs[i].type_id);
			var.setFromStr(def->inputs[i].value);
			Input *input = new Input(var);
			addInput(def->inputs[i].name, input);
		}

		// outputs
		for (int i=0; i<def->outputs.count(); i++) {
			Variable var = libraries->createVariableByType(def->outputs[i].type_id);
			var.setFromStr(def->outputs[i].value);
			Output *output = new Output(this, var);
			addOutput(def->outputs[i].name, output);
		}

		// actions
		for (int i=0; i<def->actions.count(); i++) {
			addAction(def->actions[i].name, new Action());
		}
		
		init();
		onInit(libraries);
		
		invalidate();
		onInvalidate();
	}
	
	//--------------------------------------------------------------------------
	//
	// class NeuroneGroup
	//
	//--------------------------------------------------------------------------
	
	void NeuroneGroup::checkTextSizes(Renderer *renderer)
	{
		//Check comment size
		if(commentCache.text != comment) {
			commentCache.text = comment;
			commentCache.height = renderer->textBoldHeight(getComment());
			commentCache.width = renderer->textBoldWidth(getComment());
			if(commentCache.height == 0)
				commentCache.text = Str();
		}
	}
	
	void NeuroneGroup::setNeedDelete(bool state)
	{
		need_delete = state;
	}
	
	void NeuroneGroup::getGeometry(int &left, int &top, int &width, int &height) const
	{
		left = -1;
		top = -1;
		width = -1;
		height = -1;
		if(!nodes.count())
			return;
		int minX = 100000; 
		int minY = 100000;
		int maxX = -100000; 
		int maxY = -100000;
		for(int i = 0; i < nodes.count(); ++i) {
			Neurone* block = const_cast<Neurone*>(nodes.getItem(i));
			if(minX > block->getRealPosX())
				minX = block->getRealPosX();
			if(maxX < block->getRealPosX() + block->getRealWidth())
				maxX = block->getRealPosX() + block->getRealWidth();
			if(minY > block->getRealPosY())
				minY = block->getRealPosY();
			if(maxY < block->getRealPosY() + block->getRealHeight())
				maxY = block->getRealPosY() + block->getRealHeight();
		}
		left = minX - Neurone::head_h;
		top = minY - Neurone::head_h;
		width = maxX - minX + 2 * Neurone::head_h;
		height = maxY - minY + 2 * Neurone::head_h;
	}
	
	NeuroneGroup::NeuroneGroup() : nodes(false)
	{
		logic = NULL;
		selected = false;
		need_delete = true;
		
		colorR = 1.0f;
		colorG = 0.0f;
		colorB = 0.0f;
	}
	
	NeuroneGroup::~NeuroneGroup()
	{
		setNeedDelete(false);
		setLogic(NULL);
	}
	
	unsigned long long NeuroneGroup::getSize() {
		unsigned long long res = 0;
		
		res += sizeof(*this);
		res += comment.size();
		res += sizeof(nodes);
		
		return res;
	}
	
	void NeuroneGroup::setLogic(Logic *logic)
	{
		this->logic = logic;
		if(logic != NULL) {
			logic->nodeGroups.append(this);
		}
	}
	
	Logic *NeuroneGroup::getLogic()
	{
		return logic;
	}
	
	void NeuroneGroup::render(Renderer *renderer)
	{
		checkTextSizes(renderer);
		
		if(hasComment()) {
			renderer->color(1.0f, 0.937f, 0.639f);
			float pos_x = getPosX();
			float pos_y = getPosY();
			float width = getWidth();
			float hCenter = pos_x + width/2;
			float left = hCenter - commentCache.width/2;
			float top = pos_y - 0.5*Neurone::head_h - commentCache.height;
			renderer->clearPolygonPoints();
			renderer->addPolygonPoint(left - 1, top - 1);
			renderer->addPolygonPoint(left + commentCache.width + 1, top - 1);
			renderer->addPolygonPoint(left + commentCache.width + 1, top + commentCache.height + 1);
			renderer->addPolygonPoint(hCenter + 6, top + commentCache.height + 1);
			renderer->addPolygonPoint(hCenter, pos_y - 2);
			renderer->addPolygonPoint(hCenter - 6, top + commentCache.height + 1);
			renderer->addPolygonPoint(left - 1, top + commentCache.height + 1);
			renderer->polygonDraw();
			renderer->color(0.204f, 0.082f, 0.592f);
			renderer->textBoldInRect(left, top, commentCache.width, commentCache.height, 0x84 , getComment());
		}
		
		if (selected) 
			renderer->color(colorR, colorG, colorB, 0.4f); 
		else 
			renderer->color(colorR, colorG, colorB, 0.2f); 
		
		int l, t, w, h;
		getGeometry(l, t, w, h);
		renderer->rect3d(l, t, w, h, selected);
	}
	
	const char *NeuroneGroup::getComment()
	{
		return comment.str();
	}
	
	bool NeuroneGroup::hasComment()
	{
		return comment != Str();
	}
	
	void NeuroneGroup::setComment(const char *comment)
	{
		this->comment.set(comment);
	}
	
	NeuroneGroup *NeuroneGroup::clone() const
	{
		return NULL;
	}
	
	int NeuroneGroup::getPosX() const
	{
		int l, t, w, h;
		getGeometry(l, t, w, h);
		return l;
	}
	
	int NeuroneGroup::getPosY() const
	{
		int l, t, w, h;
		getGeometry(l, t, w, h);
		return t;
	}
	
	int NeuroneGroup::getWidth() const
	{
		int l, t, w, h;
		getGeometry(l, t, w, h);
		return w;
	}
	
	int NeuroneGroup::getHeight() const
	{
		int l, t, w, h;
		getGeometry(l, t, w, h);
		return h;
	}
	
	void NeuroneGroup::setSelected(bool state)
	{
		selected = state;
		if(state)
			for(int i = 0; i < nodes.count(); ++i)
				nodes[i].setSelected(state);
	}
	
	bool NeuroneGroup::isSelected() const
	{
		return selected;
	}
	
	bool NeuroneGroup::isSelected(int x, int y) const
	{
		int l, t, w, h;
		getGeometry(l, t, w, h);
		return
				x > l &&
				y > t &&
				x < l + w &&
				y < t + h;
	}
	
	void NeuroneGroup::write(Xml *xml) const
	{
		if(comment != "") {
			xml->setArg("comment", comment.str());
		}
		
		xml->setArg("colorR", colorR);
		xml->setArg("colorG", colorG);
		xml->setArg("colorB", colorB);
		Xml *nodes_xml = xml->addChild("nodes");
		for(int i = 0; i < nodes.count(); ++i) {
			Xml *node_xml = nodes_xml->addChild("node");
			node_xml->setArg("index", nodes[i].blockIndex);
		}
	}
	
	void NeuroneGroup::read(const Xml *xml)
	{
		xml->getArg("comment", comment);
		xml->getArg("colorR", colorR);
		xml->getArg("colorG", colorG);
		xml->getArg("colorB", colorB);
		if(logic == NULL)
			return;
		const Xml *nodes_xml = xml->getChild("nodes");
		if(nodes_xml) {
			for(int i = 0; i < nodes_xml->getChildrenCount(); ++i) {
				const Xml *node_xml = nodes_xml->getChild(i);
				if(node_xml) {
					int bi;
					if(node_xml->getArg("index", bi)) {
						for(int j = 0; j < logic->getNodesCount(); ++j) {
							if(bi == logic->getNode(j).blockIndex) {
								addNode(&logic->getNode(j));
								break;
							}
						}
					}
				}
			}
		}
	}
	
	bool NeuroneGroup::isNeedDelete()
	{
		return need_delete;
	}
	
	bool NeuroneGroup::addNode(Neurone *node)
	{
		if(nodes.seek(node) >= 0)
			return false;
		nodes.append(node);
		need_delete = false;
		return true;
	}
	
	bool NeuroneGroup::removeNode(Neurone *node)
	{
		if(nodes.seek(node) < 0)
			return false;
		nodes.remove(node);
		need_delete = nodes.count() == 0;
		return true;
	}
	
	bool NeuroneGroup::removeNode(int index)
	{
		if(index < 0 || index >= nodes.count())
			return false;
		nodes.remove(index);
		need_delete = nodes.count() == 0;
		return true;
	}
	
	bool NeuroneGroup::isNodeContains(Neurone *node) const
	{
		return nodes.seek(node) >= 0;
	}
	
	int NeuroneGroup::getNodesCount() const
	{
		return nodes.count();
	}
	
	int NeuroneGroup::getNodeIndex(Neurone *node)
	{
		return nodes.seek(node);
	}
	
	Neurone &NeuroneGroup::getNode(int index)
	{
		return nodes[index];
	}
	
	void NeuroneGroup::getColor(float &r, float &g, float &b)
	{
		r = colorR;
		g = colorG;
		b = colorB;
	}
	
	void NeuroneGroup::setColor(float r, float g, float b)
	{
		colorR = r;
		colorG = g;
		colorB = b;
	}

	//--------------------------------------------------------------------------
	//
	// class NeuroneDummy
	//
	//--------------------------------------------------------------------------

	/*
	void NeuroneDummy::dummy() {
		
	}*/
	
	NeuroneDummy::NeuroneDummy() {
		
	}
	
	NeuroneDummy::NeuroneDummy(const Neurone &neurone) {
		def = neurone.getDef();
		setName(neurone.getName());
		
		// parameters
		for (int i=0; i<neurone.getParametersCount(); i++) {
			Parameter *parameter = new Parameter(neurone.getParameter(i).getValue());
			addParameter(neurone.getParameterName(i), parameter);
		}

		// events
		for (int i=0; i<neurone.getEventsCount(); i++) {
			Event *event = new Event();
			addEvent(neurone.getEventName(i), event);
		}

		// inputs
		for (int i=0; i<neurone.getInputsCount(); i++) {
			Input *input = new Input(neurone.getInput(i).getValue());
			addInput(neurone.getInputName(i), input);
		}

		// outputs
		for (int i=0; i<neurone.getOutputsCount(); i++) {
			Output *output = new Output(this, neurone.getOutput(i).getValue());
			addOutput(neurone.getOutputName(i), output);
		}

		// actions
		for (int i=0; i<neurone.getActionsCount(); i++) {
			Action *action = new Action();
			addAction(neurone.getActionName(i), action);
		}
		
	}
	
	//--------------------------------------------------------------------------
	//
	// class NeuroneDef
	//
	//--------------------------------------------------------------------------
	
	NeuroneDef::NeuroneDef() {
		data = NULL;
		can_create_dummy = true;
		library = NULL;
		container = NULL;
		hidden = false;
	}
	
	NeuroneDef *NeuroneDef::create() {
		return new NeuroneDef();
	}
	
	NeuroneDef::~NeuroneDef() {
		delete container;
	}
	
	unsigned long long NeuroneDef::getSize() {
		unsigned long long res = 0;
		
		res += sizeof(*this);
		
		res += name.size();
		res += kind.size();
		res += header.size();
		
		res += sizeof(parameters);
		res += sizeof(actions);
		res += sizeof(events);
		res += sizeof(inputs);
		res += sizeof(outputs);
		
		return res;
	}
	
	const char *NeuroneDef::getName() const {
		return name;
	}
	
	void NeuroneDef::setName(const char *name) {
		this->name = name;
	}
	
	const char *NeuroneDef::getHeader() const {
		if(header == "") return name;
		return header;
	}
	
	void NeuroneDef::setHeader(const char *header) {
		this->header = header;
	}
	
	const char *NeuroneDef::getKind() const {
		return kind;
	}
	
	void NeuroneDef::setKind(const char *kind) {
		this->kind = kind;
	}
	
	void NeuroneDef::setHidden(bool hidden)
	{
		this->hidden = hidden;
	}
	
	bool NeuroneDef::isHidden() const
	{
		return hidden;
	}
	
	Neurone *NeuroneDef::createNeurone(Libraries *libraries, bool dummy, Neurone *dummyNode) const {
		Neurone *neurone;
		if(dummy && can_create_dummy) {
			neurone = new NeuroneDummy();
		} else {
			neurone = create_function();
		}
		neurone->setDummy(dummy);
		if(!dummy)
			neurone->setDummyNode(dummyNode);
		neurone->create(libraries, this);
		return neurone;
	}
	
	NeuroneDef::NeuroneCreateFunction NeuroneDef::getCreateFunction() const {
		return create_function;
	}
	
	void NeuroneDef::setCreateFunction(NeuroneDef::NeuroneCreateFunction func) {
		create_function = func;
	}
	
	Str getNameFromParam(const char *param) {
		Str result = param;
		result.replace(" ", "");
		int idx = result.find("=");
		if(idx > 0) result.setSize(idx);
		//qDebug() << "_getNameFromParam" << result.str();
		return result;
	}
	
	Str getValueFromParam(const char *param) {
		Str result = param;
		result.replace(" ", "");
		int idx = result.find("=");
		if(idx > 0 && idx < result.size() - 1) {
			result.del(0, idx + 1); 
			//qDebug() << "_getValueFromParam" << result.str();
			return result;
		}
		return "";
	}
	
	void NeuroneDef::addInput(const char *name, const char *value, const TypeInfo &type) {
		InputDef *item = new InputDef();
		item->name = getNameFromParam(name);
		
		item->value = value;
		if(item->value == "") {
			item->value = getValueFromParam(name);
		}
		
		item->type_id = type.id();
		inputs.append(item);
	}
	
	int NeuroneDef::getInputsCount() const {
		return inputs.count();
	}
	
	const NeuroneDef::InputDef &NeuroneDef::getInput(int i) const {
		return inputs[i];
	}
	
	void NeuroneDef::addOutput(const char *name, const char *value, const TypeInfo &type) {
		OutputDef *item = new OutputDef();
		item->name = name;
		item->value = value;
		item->type_id = type.id();
		outputs.append(item);
	}
	
	int NeuroneDef::getOutputsCount() const {
		return outputs.count();
	}
	
	const NeuroneDef::OutputDef &NeuroneDef::getOutput(int i) const {
		return outputs[i];
	}
	
	void NeuroneDef::addParameter(const char *name, const char *value, const TypeInfo &type) {
		ParameterDef *item = new ParameterDef();
		item->name = name;
		item->value = value;
		item->type_id = type.id();
		parameters.append(item);
	}
	
	int NeuroneDef::getParametersCount() const {
		return parameters.count();
	}
	
	const NeuroneDef::ParameterDef &NeuroneDef::getParameter(int i) const {
		return parameters[i];
	}
	
	const NeuroneDef::ParameterDef *NeuroneDef::getParameter(const char *name) {
		for(int i=0; i<parameters.count(); i++)
		{
			if(parameters[i].name == Str(name))
				return &parameters[i];
		}
		return NULL;
	}
	
	void NeuroneDef::addAction(const char *name) {
		ActionDef *item = new ActionDef();
		item->name = name;
		actions.append(item);
	}
	
	int NeuroneDef::getActionsCount() const {
		return actions.count();
	}

	const NeuroneDef::ActionDef &NeuroneDef::getAction(int i) const {
		return actions[i];
	}
	
	void NeuroneDef::addEvent(const char *name) {
		EventDef *item = new EventDef();
		item->name = name;
		events.append(item);
	}
	
	int NeuroneDef::getEventsCount() const {
		return events.count();
	}

	const NeuroneDef::EventDef &NeuroneDef::getEvent(int i) const {
		return events[i];
	}
	
	void NeuroneDef::setData(void *data) {
		this->data = data;
	}
	
	const void *NeuroneDef::getData() const {
		return data;
	}
	
	bool NeuroneDef::canCreateDummy() const {
		return can_create_dummy;
	}
	
	void NeuroneDef::setCreateDummy(bool value) {
		can_create_dummy = value;
	}
	
	const Library *NeuroneDef::getLibrary() const {
		return library;
	}
}

//------------------------------------------------------------------------------
