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

#include "AnandamideLogic.h"

//------------------------------------------------------------------------------

#include "AnandamideScript.h"
#include "Renderer.h"
#include "AnandamideLibrary.h"
#include "Xml.h"

#define CUSTOM_NEURONE

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//--------------------------------------------------------------------------

	Neurone *Logic::createNeurone(const char *type_, const char *instance, const char *library, bool dummy, Neurone* dummyNode) {
		Str type = type_;
		
		if (type == "Action") return new ActionNeurone(instance, this, dummyNode);
		if (type == "Event") return new EventNeurone(instance, this, dummyNode);
		if (type == "GetVariable") return new GetVariableNeurone(instance, this, dummyNode);
		if (type == "GetVariablePtr") return new GetVariablePtrNeurone(instance, this, dummyNode);
		if (type == "SetVariable") return new SetVariableNeurone(instance, this, dummyNode);
		if (type == "Input") return new InputNeurone(instance, this, dummyNode);
		if (type == "Output") return new OutputNeurone(instance, this, dummyNode);
		
		if(script != NULL) return script->createNeurone(type, library, dummy, dummyNode);
		return NULL;
	}
	
	Logic::Logic(Script *script) : nodes(false) {
		this->script = script;

	}
	
	Logic::Logic() : nodes(false) {
		
	}
	
	Logic::~Logic() {
		//qDebug() << __FUNCTION__ << getName();
		while (nodes.count() > 0) {
			removeNode(0);
		}
		while (nodeGroups.count() > 0) {
			removeNodesGroup(0);
		}
	}
	
	unsigned long long Logic::getSize() {
		unsigned long long res = Neurone::getSize();
		
		/*for(int i=0; i<nodes.count(); i++) {
			res += nodes.getItem(i)->getSize();
		}
		for(int i=0; i<nodeGroups.count(); i++) {
			res += nodeGroups.getItem(i)->getSize();
		}
		res += sizeof(nodeGroups);
		res += sizeof(nodes);
		
		res += sizeof(variables) + variables.count() * sizeof(Variable);
		
		qDebug() << "Logic" << getName() << "size" << res << "neurones=" << nodes.count();
		//qDebug() << "Logic" << name << "variables" << sizeof(variables) << "nodes" << sizeof(nodes) << "nodeGroups" << sizeof(nodeGroups);*/
		
		for(int i=0; i<nodes.count(); i++) {
			Logic *nl = dynamic_cast<Logic*>(&nodes[i]);
			if(nl) res += nl->getSize();
			else res++;
		}
		
		return res;
	}
	
	const char *Logic::getType() const {
		return getName();
	}
	
	void Logic::update(float dt) {
		for (int i=0; i<nodes.count(); i++) {
			nodes[i].update(dt);
		}
	}
	
	//--------------------------------------------------------------------------
	
	void Logic::renderContent(Renderer *renderer) {
		
		for (int i=0; i<nodeGroups.count(); i++) {
			nodeGroups[i].render(renderer);
		}
		
		for (int i=0; i<nodes.count(); i++) {
			nodes[i].render(renderer);
		}
		
		bool drawStackLink = false;
		vec3 stackLinkP0, stackLinkP1;
		
		for (int i=0; i<nodes.count(); i++) {
			
			for (int j=0; j<nodes[i].getEventsCount(); j++) {
				Event *event = &nodes[i].getEvent(j);
				vec3 p0 = nodes[i].getEventPos(j);
				int evActCnt = event->getActionsCount();
				if(evActCnt > 0)
					p0 = nodes[i].getEventPos(j);
				bool singleFlag = (evActCnt == 1);
				for (int k=0; k < evActCnt; k++) {
					int l = event->getAction(k).getNeurone()->indexOf(&event->getAction(k));
					vec3 p1 = event->getAction(k).getNeurone()->getActionPos(l);
					#ifndef CUSTOM_NEURONE
						renderer->linkSpline(p0, p1, vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), false, singleFlag ? -1 : k+1);
					#else
						renderer->linkSpline(p0, p1,\
							 event->getNeurone()->getEventWireAngle(event->getNeurone()->indexOf(event)),\
							 event->getAction(k).getNeurone()->getActionWireAngle(event->getAction(k).getNeurone()->indexOf(&event->getAction(k))),\
							vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), false, singleFlag ? -1 : k+1);
					#endif
				}
			}

		}

		for (int i=0; i<nodes.count(); i++) {

			for (int j=0; j<nodes[i].getInputsCount(); j++) {
				Input *input = &nodes[i].getInput(j);
				if (input->getSource() == NULL) continue;
				int l = input->getSource()->getInstance()->indexOf(input->getSource());
				vec3 p0 = input->getSource()->getInstance()->getOutputPos(l);
				vec3 p1 = nodes[i].getInputPos(j);
				
				vec3 c0(0.3f, 1.0, 1.0f);
				const TypeDef *td0 = input->getSource()->getInstance()->getOutput(l).getTypeDef();
				if(td0) c0 = vec3(td0->getR(), td0->getG(), td0->getB());
				
				
				vec3 c1(0.3f, 1.0, 1.0f);
				const TypeDef *td1 = input->getTypeDef();
				if(td1) c1 = vec3(td1->getR(), td1->getG(), td1->getB());
				
				#ifndef CUSTOM_NEURONE
					renderer->linkSpline(p0, p1, c0, c1);
				#else
					renderer->linkSpline(p0, p1,\
									 input->getSource()\
									 ->getInstance()\
									 ->getOutputWireAngle(input->getSource()->getInstance()->indexOf(input->getSource())),\
									 nodes[i].getInputWireAngle(j),\
									 c0, c1, false, -1);
				#endif
			}

		}
		
		if(drawStackLink) {
			renderer->linkSpline(stackLinkP0, stackLinkP1, vec3(0.047f, 0.416f, 1.0f), vec3(0.047f, 0.416f, 1.0f), true, -1);
		}
		
	}
	
	Logic *Logic::cloneLogic() {
		return (Logic*)clone();
	}
	
	void Logic::cloneNodes(Array<Neurone> &nodes, Logic *target_logic, bool dummy, bool set_selected) {
		
		Array <Neurone> clone_nodes(false);
		
		// clone nodes
		for(int i=0; i<nodes.count(); i++) {
			Neurone *src = &nodes[i];
			Neurone *dst = target_logic->addNode(src->getType(), src->getLibraryName(), src->getInstance(), dummy);
			if(dst != NULL) {
				dst->setPos(nodes[i].getPosX(), nodes[i].getPosY());
				dst->setComment(src->getComment());
				dst->setSelected(set_selected);
			}
			clone_nodes.append(dst);
		}
		
		// clone all
		for(int i=0; i<nodes.count(); i++) {
			Neurone *src_node = &nodes[i];
			Neurone *dst_node = &clone_nodes[i];
			if(dst_node == NULL) continue;
			
			// clone parameters
			for (int j=0; j<src_node->getParametersCount(); j++) {
				Parameter *src = &src_node->getParameter(j);
				Parameter *dst = &dst_node->getParameter(j);
				dst->setValue(src->getValue());
			}
			
			// invalidate
			dst_node->invalidate();
			
			// clone event-action links
			for (int j=0; j<src_node->getEventsCount(); j++) {
				
				Event *src_event = &src_node->getEvent(j);
				Event *dst_event = &dst_node->getEvent(j);
				
				for (int k=0; k<src_event->getActionsCount(); k++) {
					
					Action *src_action = &src_event->getAction(k);
					int instance_index = nodes.seek(src_action->getNeurone());
					
					// if nodes has not action node, skip (link with not in nodes list node)
					if(instance_index < 0) continue;
					
					Neurone *dst_instance = &clone_nodes[instance_index];
					if(dst_instance == NULL) continue;
					Action *dst_action = dst_instance->getAction(src_action->getName());
					dst_event->addAction(dst_action);
				}
			}
			
			// clone input-output links
			for (int j=0; j<src_node->getInputsCount(); j++) {
				
				Input *src_input = &src_node->getInput(j);
				Input *dst_input = dst_node->getInput(src_input->getName());
				dst_input->getVariable() = src_input->getVariable();
				
				Output *src_output = src_input->getSource();
				if(src_output == NULL) continue;
				
				int instance_index = nodes.seek(src_output->getInstance());
				
				// if nodes has not output node, skip (link with not in nodes list node)
				if(instance_index < 0) continue;
				
				Neurone *dst_instance = &clone_nodes[instance_index];
				Output *dst_output = dst_instance->getOutput(src_output->getName());
				dst_input->setSource(dst_output);				
			}
			
		}
		
	}
	
	void Logic::cloneNodesAndGroups(Array<Neurone> &nodes, Array<NeuroneGroup> &groups, Logic *target_logic, bool dummy, bool set_selected)
	{
		Array <NeuroneGroup> clone_groups(false);
		Array <Neurone> clone_nodes(false);
		
		// clone groups
		for(int i=0; i<groups.count(); i++) {
			NeuroneGroup *src = &groups[i];
			NeuroneGroup *dst = new NeuroneGroup();
			target_logic->addNodesGroup(dst);
			dst->setComment(src->getComment());
			float r,g,b;
			src->getColor(r,g,b);
			dst->setColor(r,g,b);
			dst->setSelected(set_selected);
			clone_groups.append(dst);
		}
		// clone nodes
		for(int i=0; i<nodes.count(); i++) {
			Neurone *src = &nodes[i];
			Neurone *dst = target_logic->addNode(src->getType(), src->getLibraryName(), src->getInstance(), dummy);
			if(dst != NULL) {
				dst->setComment(src->getComment());
				dst->setPos(nodes[i].getPosX(), nodes[i].getPosY());
				dst->setSelected(set_selected);
			}
			clone_nodes.append(dst);
			for(int j = 0; j < groups.count(); ++j) {
				NeuroneGroup *srcGroup = &groups[j];
				NeuroneGroup *dstGroup = &clone_groups[j];
				if(srcGroup->isNodeContains(src))
					dstGroup->addNode(dst);
			}
		}
		
		// clone all
		for(int i=0; i<nodes.count(); i++) {
			Neurone *src_node = &nodes[i];
			Neurone *dst_node = &clone_nodes[i];
			if(dst_node == NULL) continue;
			
			// clone parameters
			for (int j=0; j<src_node->getParametersCount(); j++) {
				Parameter *src = &src_node->getParameter(j);
				Parameter *dst = &dst_node->getParameter(j);
				dst->setValue(src->getValue());
			}
			
			// invalidate
			dst_node->invalidate();
			
			// clone event-action links
			for (int j=0; j<src_node->getEventsCount(); j++) {
				
				Event *src_event = &src_node->getEvent(j);
				Event *dst_event = &dst_node->getEvent(j);
				
				for (int k=0; k<src_event->getActionsCount(); k++) {
					
					Action *src_action = &src_event->getAction(k);
					int instance_index = nodes.seek(src_action->getNeurone());
					
					// if nodes has not action node, skip (link with not in nodes list node)
					if(instance_index < 0) continue;
					
					Neurone *dst_instance = &clone_nodes[instance_index];
					if(dst_instance == NULL) continue;
					Action *dst_action = dst_instance->getAction(src_action->getName());
					dst_event->addAction(dst_action);
				}
			}
			
			// clone input-output links
			for (int j=0; j<src_node->getInputsCount(); j++) {
				
				Input *src_input = &src_node->getInput(j);
				Input *dst_input = dst_node->getInput(src_input->getName());
				dst_input->getVariable() = src_input->getVariable();
				
				Output *src_output = src_input->getSource();
				if(src_output == NULL) continue;
				
				int instance_index = nodes.seek(src_output->getInstance());
				
				// if nodes has not output node, skip (link with not in nodes list node)
				if(instance_index < 0) continue;
				
				Neurone *dst_instance = &clone_nodes[instance_index];
				Output *dst_output = dst_instance->getOutput(src_output->getName());
				dst_input->setSource(dst_output);				
			}
			
		}
	}
	
	/// \todo make using cloneNodesAndGroups() 
	Neurone *Logic::clone(Neurone *dummyNode) {
		Logic *logic_clone = new Logic(this->script);
		logic_clone->setName(getName());
		logic_clone->setDummyNode(dummyNode);
	
		// clone variables
		for (int i=0; i<getVariablesCount(); i++) {
			logic_clone->addVariable(getVariableName(i));
			logic_clone->getVariable(i).set(getVariable(i));
		}
	
		// clone groups
		for(int i=0; i<nodeGroups.count(); i++) {
			NeuroneGroup *src = &nodeGroups[i];
			NeuroneGroup *dst = new NeuroneGroup();
			logic_clone->addNodesGroup(dst);
			dst->setComment(src->getComment());
			float r,g,b;
			src->getColor(r,g,b);
			dst->setColor(r,g,b);
		}
		
		// clone nodes
		for(int i=0; i<nodes.count(); i++) {
			
			Neurone *src = &nodes[i];
			Neurone *dst = logic_clone->addNode(src->getType(), src->getLibraryName(), src->getInstance(), false, src);
			if(dst == NULL) {
				errorMessage("Can`t clone()");
				return logic_clone;
			}
			dst->setComment(src->getComment());
			dst->setPos(nodes[i].getPosX(), nodes[i].getPosY());
			
			for(int j = 0; j < logic_clone->getNodeGroupCount(); ++j) {
				NeuroneGroup *srcGroup = &nodeGroups[j];
				NeuroneGroup *dstGroup = &logic_clone->getNodeGroup(j);
				if(srcGroup->isNodeContains(src))
					dstGroup->addNode(dst);
			}
		}
		
		// clone all
		for(int i=0; i<nodes.count(); i++) {
			Neurone *src_node = &nodes[i];
			Neurone *dst_node = &logic_clone->getNode(i);
			
			// clone parameters
			for (int j=0; j<src_node->getParametersCount(); j++) {
				Parameter *src = &src_node->getParameter(j);
				Parameter *dst = &dst_node->getParameter(j);
				dst->setValue(src->getValue());
			}
			
			// invalidate
			dst_node->invalidate();
			
			// clone event-action links
			for (int j=0; j<src_node->getEventsCount(); j++) {
				
				Event *src_event = &src_node->getEvent(j);
				Event *dst_event = &dst_node->getEvent(j);
				
				for (int k=0; k<src_event->getActionsCount(); k++) {
					
					Action *src_action = &src_event->getAction(k);
					int instance_index = indexOf(src_action->getNeurone());
					
					Neurone *dst_instance = &logic_clone->getNode(instance_index);
					Action *dst_action = dst_instance->getAction(src_action->getName());
					if(dst_action == NULL) {
						errorMessage(format("Can`f find dst action %s", src_action->getName()));
						return logic_clone;
					}
					dst_event->addAction(dst_action);
				}
			}
			
			// clone input-output links
			for (int j=0; j<src_node->getInputsCount(); j++) {
				
				Input *src_input = &src_node->getInput(j);
				Input *dst_input = dst_node->getInput(src_input->getName());
				if(dst_input == NULL) continue;
				dst_input->getVariable() = src_input->getVariable();
				
				Output *src_output = src_input->getSource();
				if(src_output == NULL) continue;
				
				int instance_index = indexOf(src_output->getInstance());
				
				Neurone *dst_instance = &logic_clone->getNode(instance_index);
				Output *dst_output = dst_instance->getOutput(src_output->getName());
				dst_input->setSource(dst_output);				
				
			}
		}
	
		return logic_clone;
	}
	
	void Logic::afterCompile()
	{
		for(int i=0; i<nodes.count(); i++)
			nodes[i].afterCompile();
	}
	
	void Logic::addNode(Neurone *node) {
		if (node == NULL) return;
		node->setLogic(this);
	}
	
	int Logic::indexOf(const Neurone *node) const {
		return nodes.seek(node);
	}
	
	void Logic::bringToFront(Neurone *node) {
		if(node == NULL) return;
		nodes.remove(node);
		nodes.append(node);
	}
	
	void Logic::removeNode(int index) {
		//qDebug() << __FUNCTION__ << index;
		Neurone *rem = &nodes[index];
		
		if(rem) rem->onShutdown();
		
		for (int j=0; j<nodeGroups.count(); j++) 
			nodeGroups[j].removeNode(rem);
		
		for (int i=0; i<nodes.count(); i++) {
			
			Neurone *node = &nodes[i];
			for (int j=0; j<node->getEventsCount(); j++) {
				
				Event *event = &node->getEvent(j);
				for (int k=0; k<event->getActionsCount(); k++) {
					
					Action *action = &event->getAction(k);
					if (action->getNeurone() == rem) {
						event->removeAction(k--);
					}
					
				}
			}
			
			for (int j=0; j<node->getInputsCount(); j++) {
				
				Input *input = &node->getInput(j);
				
				if (input->getSource() != NULL && input->getSource()->getInstance() == rem) {
					input->setSource(NULL);
				}
			}
			
		}
		
		nodes.remove(index);
		
		if (rem->isNeedDelete()) {
			delete rem;
		} else {
			rem->logic = NULL;
		}
		
	}
	
	void Logic::removeNodesGroup(int index)
	{
		//NeuroneGroup *rem = nodeGroups.getItem(index);
		nodeGroups.remove(index);
	}
	
	void Logic::removeNode(Neurone *rem) {
		int index = nodes.seek(rem);
		if (index < 0) return;
		removeNode(index);
	}
	
	int Logic::getNodesCount() const {
		return nodes.count();
	}
	
	Neurone &Logic::getNode(int i) {
		return nodes[i];
	}
	
	const Neurone &Logic::getNode(int i) const {
		return nodes[i];
	}
	
	Neurone *Logic::addNode(const char *type, const char *library, const char *instance, bool dummy, Neurone *dummyNode) {
		Neurone *node = createNeurone(type, instance, library, dummy, dummy ? NULL : dummyNode);
		if(node == NULL) {
			return NULL;
		}
		addNode(node);
		return node;
	}
	
	Neurone *Logic::getNodeByPos(int X, int Y)
	{
		for(int i = 0; i < nodes.count(); ++i) {
			if(nodes[i].isSelected(X, Y))
				return nodes.getItem(i);
		}
		return NULL;
	}
	
	Action *Logic::getActionByPos(int X, int Y)
	{
		for(int i = 0; i < nodes.count(); ++i) {
			Action* act = nodes[i].getSelectedAction(X, Y);
			if(act != NULL)
				return act;
		}
		return NULL;
	}
	
	void Logic::addNodesGroup(NeuroneGroup *group)
	{
		if(group == NULL)
			return;
		group->setLogic(this);
	}
	
	bool Logic::canCreateNewGroup()
	{
		bool emptyGlag = true;
		bool canCreate = true;
		for(int i = 0; i < getNodesCount(); ++i) {
			if(getNode(i).isSelected()) {
				emptyGlag = false;
				Str type = getNode(i).getType();
				if(type == "Action" ||
						type == "Event" ||
						type == "Input" ||
						type == "Output") {
					canCreate = false;
					break;
				}
				if(getNodeGroup(nodes.getItem(i)) >= 0) {
					canCreate = false;
					break;
				}
			}
		}
		return canCreate && !emptyGlag;
	}
	
	void Logic::addNodesGroupBySelection()
	{
		if(!canCreateNewGroup())
			return;
		NeuroneGroup* group = new NeuroneGroup();
		for(int i = 0; i < getNodesCount(); ++i) {
			if(getNode(i).isSelected())
				group->addNode(&getNode(i));
		}
		if(group->isNeedDelete())
			delete group;
		else
			addNodesGroup(group);
	}
	
	int Logic::getNodeGroup(Neurone *node)
	{
		for(int i = 0; i < nodeGroups.count(); ++i) 
			if(nodeGroups[i].isNodeContains(node)) 
				return i;
		return -1;
	}
	
	int Logic::getNodeGroupCount() const
	{
		return nodeGroups.count();
	}
	
	NeuroneGroup &Logic::getNodeGroup(int index)
	{
		return nodeGroups[index];
	}
	
	NeuroneGroup *Logic::getSelectedGroup()
	{
		for(int i = 0; i < nodeGroups.count(); ++i) {
			if(nodeGroups[i].isSelected())
				return nodeGroups.getItem(i);
		}
		return NULL;
	}
	
	NeuroneGroup *Logic::getGroupOfSelectedNodes()
	{
		int group_index = -1;
		for(int i = 0; i < nodes.count(); ++i) {
			if(nodes[i].isSelected()) {
				int gi = getNodeGroup(nodes.getItem(i));
				if(gi < 0)
					return NULL;
				if(group_index < 0)
					group_index = gi;
				else if(group_index != gi)
					return NULL;
			}
		}
		if(group_index < 0)
			return NULL;
		return nodeGroups.getItem(group_index);
	}
	
	bool Logic::removeSelectedFromGroup()
	{
		NeuroneGroup* group = getGroupOfSelectedNodes();
		if(group == NULL)
			return false;
		for(int i = 0; i < group->getNodesCount(); ++i) {
			if(!group->getNode(i).isSelected()) continue;
			group->removeNode(i--);
		}
		if(group->getNodesCount() == 0)
			removeGroup(group);
		return true;
	}
	
	NeuroneGroup *Logic::getGroupByPos(int X, int Y)
	{
		for(int i = 0; i < nodeGroups.count(); ++i) {
			if(nodeGroups[i].isSelected(X, Y))
				return nodeGroups.getItem(i);
		}
		return NULL;
	}
	
	bool Logic::removeGroup(NeuroneGroup *group)
	{
		nodeGroups.remove(group);
		return true;
	}
	
	void Logic::removeGroup(int i) {
		nodeGroups.remove(i);
	}
	
	bool Logic::removeGroupWithNodes(NeuroneGroup *group)
	{
		while(group->getNodesCount())
			removeNode(&group->getNode(0));
		removeGroup(group);
		return true;
	}
	
	bool Logic::addSelectedToGroup(NeuroneGroup *group)
	{
		if(group->getLogic() != this)
			return false;
		if(!this->canCreateNewGroup())
			return false;
		for(int i = 0; i < nodes.count(); ++i) {
			if(nodes[i].getLogic() != this)
				return false;
			if(!nodes[i].isSelected()) continue;
			group->addNode(nodes.getItem(i));
		}
		return true;
	}
	
	bool Logic::isSomethingSelected()
	{
		for(int i = 0; i < nodeGroups.count(); ++i) {
			if(nodeGroups[i].isSelected())
				return true;
		}
		for(int i = 0; i < nodes.count(); ++i) {
			if(nodes[i].isSelected())
				return true;
		}
		return false;
	}
	
	int Logic::getSelectedGroupsCount()
	{
		int res = 0;
		for(int i = 0; i < nodeGroups.count(); ++i) 
			if(nodeGroups[i].isSelected())
				res++;
		return res;
	}
	
	void Logic::renameNodes(const char *oldName, const char *newName)
	{
		for(int i = 0; i < nodes.count(); ++i) 
			if(Str(nodes[i].getName()) == Str(oldName))
				nodes[i].setName(newName);
	}
	
	void Logic::clear() {
		variables.clear();
		actions.clear();
		events.clear();
		inputs.clear();
		outputs.clear();
		nodes.clear();
	}
	
	//--------------------------------------------------------------------------
	//
	// ActionNeurone
	//
	//--------------------------------------------------------------------------
	
	ActionNeurone::ActionNeurone(const char *inst, Logic *logic_, Neurone *dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;
		
		event = new Event();
		addEvent("out", event);
		
		action = new Action();
		action->setAction(Common::Action::create(event, &Event::run));
		getLogic()->addAction(inst, action);
	}

	ActionNeurone::~ActionNeurone() {
		getLogic()->removeActionInternal((Action *)action);
	}
	
	const char *ActionNeurone::getType() const {
		return "Action";
	}
	
	const char *ActionNeurone::getInstance() const {
		return action->getName();
	}
	
	const char *ActionNeurone::getCaption() {
		return action->getName();
	}
	
	bool ActionNeurone::hasCaption() {
		return true;
	}
	
	Action *ActionNeurone::getLogicAction() {
		return action;
	}
	
	//--------------------------------------------------------------------------
	//
	// EventNeurone
	//
	//--------------------------------------------------------------------------

	EventNeurone::EventNeurone(const char *inst, Logic *logic_, Neurone *dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;

		event = new Event();
		getLogic()->addEvent(inst, event);
		
		action = addAction("in", new Action());
		action->setAction(Common::Action::create(event, &Event::run));
	}

	EventNeurone::~EventNeurone() {
		getLogic()->removeEventInternal((Event *)event);
	}
	
	const char *EventNeurone::getType() const {
		return "Event";
	}
	
	const char *EventNeurone::getInstance() const {
		return event->getName();
	}
	
	const char *EventNeurone::getCaption() {
		return event->getName();
	}
	
	bool EventNeurone::hasCaption() {
		return true;
	}
	
	Event *EventNeurone::getLogicEvent() {
		return event;
	}
	
	//--------------------------------------------------------------------------
	//
	// Logic
	//
	//--------------------------------------------------------------------------
	
	void Logic::writeXml(Xml *xml) const {
		xml->setArg("name", getName());
		xml->setArg("kind", getKind().str());
		Xml *xml_vars = xml->addChild("variables");
		for(int i=0; i<getVariablesCount(); i++) {
			Xml *xml_var = xml_vars->addChild("variable");
			xml_var->setArg("name", getVariableName(i));
			xml_var->setArg("type_id", getVariable(i).getType().id());
		}
		
		Xml *xml_nodes = xml->addChild("nodes");
		for(int i=0; i<nodes.count(); i++) {
			Neurone *node = const_cast<Neurone*>(nodes.getItem(i));
			node->blockIndex = i;
			Xml *xml_node = xml_nodes->addChild("node");
			node->write(xml_node);
			const LogicDummy *node_as_logic = dynamic_cast <const LogicDummy *>(node);
			if(node_as_logic == NULL) continue;
			if(node_as_logic->getSource()->getScript() != getScript()) continue;
			xml_node->setArg("library", "#script");
		}
		
		if(nodeGroups.count() > 0) {
			Xml *xml_nodeGroups = xml->addChild("groups");
			for(int i=0; i<nodeGroups.count(); i++) {
				const NeuroneGroup *group = nodeGroups.getItem(i);
				Xml *xml_group = xml_nodeGroups->addChild("group");
				group->write(xml_group);
			}
		}
	}
	
	//--------------------------------------------------------------------------
	
	
	///
	/// Класс нейрона, который не смог загрузиться из библиотеки
	///	
	class MrCorpse : public Neurone
	{
		Str library;
	public:
		MrCorpse() : Neurone() {}
		MrCorpse(const char *type, const char *library) : Neurone() {
			setName(type);
			this->library.set(library);
			setError(1);
		}
		const char *getLibraryName() const {
			return library.str();
		}
	};
	
	
	//--------------------------------------------------------------------------

	bool Logic::readXml(const Xml *xml, bool dummy) {
		
		if (xml == NULL) return false;
		clear();
		
		bool result = true;
		
		xml->getArg("name", di()->name);
		xml->getArg("kind", m_kind);

		for(int i=0; i<xml->getChildrenCount(); i++) {
			
			if(Str(xml->getChild(i)->getName()) == "variables") {
				const Xml *xml_vars = xml->getChild(i);
				for(int j=0; j<xml_vars->getChildrenCount(); j++) {
					const Xml *xml_var = xml_vars->getChild(j);
					Str var_name;
					int type_id = TYPEID(int).id();
					xml_var->getArg("name", var_name);
					xml_var->getArg("type_id", type_id);
					Variable *var = addVariable(var_name);
					const TypeDef *type_def = script->getLibraries()->getTypeDefById(type_id);
					if(type_def == NULL) {
						errorMessage(format("Error while loading logic \"%s\" : type \"%d\" not found for \"%s\" variable.", getName(), type_id, var_name.str()));
						result = false;
						continue;
						//return false;
					}
					var->setType(type_def->getTypeInfo());
				}
			}
			
			if(Str(xml->getChild(i)->getName()) == "nodes") {
				const Xml *xml_nodes = xml->getChild(i);
				for(int j=0; j<xml_nodes->getChildrenCount(); j++) {
					const Xml *child = xml_nodes->getChild(j);
					Str type;
					Str instance;
					Str name;
					Str lib;
					child->getArg("type", type);
					child->getArg("instance", instance);
					child->getArg("name", name);
					child->getArg("library", lib);
					if(lib == "#script") {
						script->preLoadLogic(type);
					}
					
					Neurone *node = createNeurone(type, instance, lib, dummy);
					
					if (node == NULL) {
						errorMessage(format("Error while loading logic \"%s\" : can not create \"%s\" neurone.", getName(), type.str()));
						node = new MrCorpse(type, lib);
						result = false;
						//return false;
					}
					node->readParameters(child);
					addNode(node);
				}
				
				for(int i=0; i<nodes.count(); i++) {
					const Xml *child = xml_nodes->getChild(i);
					nodes[i].readEvents(child);
				}
			}
			
			if(Str(xml->getChild(i)->getName()) == "groups" || Str(xml->getChild(i)->getName()) == "nodeGroups" ) {
				const Xml *xml_nodeGroups = xml->getChild(i);
				for(int j=0; j<xml_nodeGroups->getChildrenCount(); j++) {
					const Xml *child = xml_nodeGroups->getChild(j);
					NeuroneGroup* group = new NeuroneGroup();
					this->addNodesGroup(group);
					group->read(child);
				}
			}
		}
		
		return result;
	}
	
	Script *Logic::getScript() const {
		return script;	
	}
	
	//--------------------------------------------------------------------------
	// actions
	//--------------------------------------------------------------------------
	
	bool Logic::renameAction(const char *old_name, const char *new_name) {
		Str old_buff(old_name);
		bool res = renameActionInternal(old_name, new_name);
		if(res && script) {
			for(int i = 0; i < script->getLogicsCount(); ++i)
				for(int j = 0; j < script->getLogic(i)->getNodesCount(); ++j) {
					LogicDummy* neuLogic = dynamic_cast<LogicDummy*>(&script->getLogic(i)->getNode(j));
					if(neuLogic == NULL) continue;
					if(Str(neuLogic->getLibraryName()) != Str()) continue;
					if(Str(neuLogic->getName()) != Str(getName())) continue;
					neuLogic->renameActionInternal(old_buff.str(), new_name);
				}
		} 
		return res;
	}
	
	void Logic::removeAction(Action *action) {
		for(int i=0; i<nodes.count(); i++) {
			ActionNeurone *node = dynamic_cast <ActionNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicAction() != action) continue;
			removeNode(i--);
		}
		removeActionInternal(action);
	}
	
	ActionNeurone *Logic::getActionNode(Action *action) {
		for(int i=0; i<nodes.count(); i++) {
			ActionNeurone *node = dynamic_cast <ActionNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicAction() != action) continue;
			return node;
		}
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	// events
	//--------------------------------------------------------------------------
	
	bool Logic::renameEvent(const char *old_name, const char *new_name) {
		Str old_buff(old_name);
		bool res = renameEventInternal(old_name, new_name);
		if(res && script) {
			for(int i = 0; i < script->getLogicsCount(); ++i)
				for(int j = 0; j < script->getLogic(i)->getNodesCount(); ++j) {
					LogicDummy* neuLogic = dynamic_cast<LogicDummy*>(&script->getLogic(i)->getNode(j));
					if(neuLogic == NULL) continue;
					if(Str(neuLogic->getLibraryName()) != Str()) continue;
					if(Str(neuLogic->getName()) != Str(getName())) continue;
					neuLogic->renameEventInternal(old_buff.str(), new_name);
				}
		} 
		return res;
	}
	
	void Logic::removeEvent(Event *event) {
		for(int i=0; i<nodes.count(); i++) {
			EventNeurone *node = dynamic_cast <EventNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicEvent() != event) continue;
			removeNode(i--);
		}
		removeEventInternal(event);
	}
	
	EventNeurone *Logic::getEventNode(Event *event) {
		for(int i=0; i<nodes.count(); i++) {
			EventNeurone *node = dynamic_cast <EventNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicEvent() != event) continue;
			return node;
		}
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	// inputs
	//--------------------------------------------------------------------------
	
	bool Logic::renameInput(const char *old_name, const char *new_name) {
		Str old_buff(old_name);
		bool res = renameInputInternal(old_name, new_name);
		if(res && script) {
			for(int i = 0; i < script->getLogicsCount(); ++i)
				for(int j = 0; j < script->getLogic(i)->getNodesCount(); ++j) {
					LogicDummy* neuLogic = dynamic_cast<LogicDummy*>(&script->getLogic(i)->getNode(j));
					if(neuLogic == NULL) continue;
					if(Str(neuLogic->getLibraryName()) != Str()) continue;
					if(Str(neuLogic->getName()) != Str(getName())) continue;
					neuLogic->renameInputInternal(old_buff.str(), new_name);
				}
		} 
		return res;
	}
	
	void Logic::removeInput(Input *input) {
		for(int i=0; i<nodes.count(); i++) {
			InputNeurone *node = dynamic_cast <InputNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicInput() != input) continue;
			removeNode(i--);
		}
		removeInputInternal(input);
	}
	
	InputNeurone *Logic::getInputNode(Input *input) {
		for(int i=0; i<nodes.count(); i++) {
			InputNeurone *node = dynamic_cast <InputNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicInput() != input) continue;
			return node;
		}
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	// outputs
	//--------------------------------------------------------------------------
	
	bool Logic::renameOutput(const char *old_name, const char *new_name) {
		Str old_buff(old_name);
		bool res = renameOutputInternal(old_name, new_name);
		if(res && script) {
			for(int i = 0; i < script->getLogicsCount(); ++i)
				for(int j = 0; j < script->getLogic(i)->getNodesCount(); ++j) {
					LogicDummy* neuLogic = dynamic_cast<LogicDummy*>(&script->getLogic(i)->getNode(j));
					if(neuLogic == NULL) continue;
					if(Str(neuLogic->getLibraryName()) != Str()) continue;
					if(Str(neuLogic->getName()) != Str(getName())) continue;
					neuLogic->renameOutputInternal(old_buff.str(), new_name);
				}
		} 
		return res;
	}
	
	void Logic::removeOutput(Output *output) {
		for(int i=0; i<nodes.count(); i++) {
			OutputNeurone *node = dynamic_cast <OutputNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicOutput() != output) continue;
			removeNode(i--);
		}
		removeOutputInternal(output);
	}
	
	OutputNeurone *Logic::getOutputNode(Output *output) {
		for(int i=0; i<nodes.count(); i++) {
			OutputNeurone *node = dynamic_cast <OutputNeurone *> (&nodes[i]);
			if(node == NULL) continue;
			if(node->getLogicOutput() != output) continue;
			return node;
		}
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	// variables
	//--------------------------------------------------------------------------
	
	int Logic::getVariablesCount() const {
		return variables.count();
	}
	
	const Variable &Logic::getVariable(int i) const {
		return variables[i].getValue();
	}
	
	Variable &Logic::getVariable(int i) {
		return variables[i].getValue();
	}
	
	const Variable *Logic::getVariable(const char *id) const {
		return variables.get(id);
	}
	
	Variable *Logic::getVariable(const char *id) {
		return variables.get(id);
	}
	
	Variable *Logic::getVariableByName(const char *id) {
		return getVariable(id);
	}
	
	void Logic::onSetLogic(Logic *logic) {
		for(int i=0; i<nodes.count(); i++) {
			nodes[i].setLogic(this);
		}
	}

	void Logic::setKind(const Str &kind)
	{
		m_kind = kind;
	}

	Str Logic::getKind() const
	{
		return m_kind;
	}
	
	void Logic::setDef(const NeuroneDef *def) {
		this->def = def;
	}
	
	const char *Logic::getVariableName(int i) const {
		return variables[i].getKey();
	}
	
	const char *Logic::getVariableName(const Variable *variable) const {
		int index = indexOf(variable);
		if(index < 0) return "";
		return getVariableName(index);
	}
	
	bool Logic::renameVariable(const char *old_name, const char *new_name) {
		return variables.rename(old_name, new_name);
	}
	
	int Logic::indexOf(const Variable *variable) const {
		return variables.seek(variable);
	}
	
	Variable *Logic::addVariable(const char *id) {
		if(variables.get(id) != NULL) return NULL;
		Variable *item = new Variable();
		variables.append(new Str(id), item);
		return item;
	}
	
	void Logic::removeVariable(Variable *variable) {
		for(int i=0; i<nodes.count(); i++) {
			SetVariableNeurone *set_node = dynamic_cast <SetVariableNeurone *> (&nodes[i]);
			if(set_node != NULL && set_node->getLogicVariable() == variable) {
				removeNode(i--);
				continue;
			}
			GetVariableNeurone *get_node = dynamic_cast <GetVariableNeurone *> (&nodes[i]);
			if(get_node != NULL && get_node->getLogicVariable() == variable) {
				removeNode(i--);
				continue;
			}
			GetVariablePtrNeurone *ptr_node = dynamic_cast <GetVariablePtrNeurone *> (&nodes[i]);
			if(ptr_node != NULL && ptr_node->getLogicVariable() == variable) {
				removeNode(i--);
				continue;
			}
		}
		int index = variables.seek(variable);
		if (index == -1) return;
		variables.remove(index);
	}
	
	void Logic::setVariableType(Variable *variable, const TypeInfo &type) {
		int index = variables.seek(variable);
		if (index == -1) return;
		variable->setType(type);
		for(int i=0; i<nodes.count(); i++) {
			SetVariableNeurone *set_node = dynamic_cast <SetVariableNeurone *> (&nodes[i]);
			if(set_node != NULL && set_node->getLogicVariable() == variable) {
				set_node->updateType();
			}
			GetVariableNeurone *get_node = dynamic_cast <GetVariableNeurone *> (&nodes[i]);
			if(get_node != NULL && get_node->getLogicVariable() == variable) {
				get_node->updateType();
			}
		}
	}
	
	//--------------------------------------------------------------------------
	//
	// GetVariableNeurone
	//
	//--------------------------------------------------------------------------
	
	void GetVariableNeurone::onValueGet() {
		getOutput(0).setValue(*variable);
	}
	
	GetVariableNeurone::GetVariableNeurone(const char *inst, Logic *logic_, Neurone *dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;
		variable = getLogic()->getVariable(inst);
		if(variable == NULL) {
			errorMessage(format("Variable \"%s\" not found.", inst));
			return;
		}
		output = addOutput("value", new Output(this));
		output->getVariable().setType(variable->getType());
		output->setOnGetValueAction(Common::Action::create(this, &GetVariableNeurone::onValueGet));
	}
	
	GetVariableNeurone::~GetVariableNeurone() {
		
	}
	
	const char *GetVariableNeurone::getType() const {
		return "GetVariable";
	}
	
	const char *GetVariableNeurone::getInstance() const {
		return getLogic()->getVariableName(variable);
	}
	
	const char *GetVariableNeurone::getCaption() {
		return getLogic()->getVariableName(variable);
	}
	
	bool GetVariableNeurone::hasCaption() {
		return true;
	}
	
	Variable *GetVariableNeurone::getLogicVariable() {
		return variable;
	}
	
	void GetVariableNeurone::updateType() {
		if(variable->getType() != output->getVariable().getType()) {
			output->getVariable().setType(variable->getType());
		}
	}
	
	//--------------------------------------------------------------------------
	//
	// SetVariableNeurone
	//
	//--------------------------------------------------------------------------
	
	void SetVariableNeurone::actionSet() {
		variable->set(input->getValue());
		out->run();
	}
	
	SetVariableNeurone::SetVariableNeurone(const char *inst, Logic *logic_, Neurone* dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;
		variable = getLogic()->getVariable(inst);
		
		if(variable == NULL) {
			errorMessage(format("Variable \"%s\" not found.", inst));
			return;
		}
		
		input = addInput("value", new Input(*variable));
		out = addEvent("out", new Event());
		Action *action = addAction("set", new Action());
		action->setAction(Common::Action::create(this, &SetVariableNeurone::actionSet));
	}
	
	SetVariableNeurone::~SetVariableNeurone() {
		
	}
	
	const char *SetVariableNeurone::getType() const {
		return "SetVariable";
	}
	
	const char *SetVariableNeurone::getInstance() const {
		return getLogic()->getVariableName(variable);
	}
	
	const char *SetVariableNeurone::getCaption() {
		return getLogic()->getVariableName(variable);
	}
	
	bool SetVariableNeurone::hasCaption() {
		return true;
	}
	
	Variable *SetVariableNeurone::getLogicVariable() {
		return variable;
	}
	
	void SetVariableNeurone::updateType() {
		if(variable->getType() != input->getVariable().getType()) {
			input->getVariable().setType(variable->getType());
		}
	}
	
	//--------------------------------------------------------------------------
	//
	// InputNeurone
	//
	//--------------------------------------------------------------------------
	
	void InputNeurone::onValueGet() {
		output->setValue(input->getValue());
	}
	
	void InputNeurone::onSetDefault() {
		input->getDefaultValue() = default_value->getValue();
	}
	
	InputNeurone::InputNeurone(const char *instance, Logic *logic_, Neurone *dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;
		
		input = getLogic()->addInput(instance, new Input(Variable()));
		output = addOutput("value", new Output(this));
		type = addParameter("type", new Parameter(Variable(TYPEID(int))));
		type->getVariable().setOnChangeAction(Common::Action::create(this, &InputNeurone::onSetType));
		default_value = addParameter("default", new Parameter(Variable(0)));
		default_value->getVariable().setOnChangeAction(Common::Action::create(this, &InputNeurone::onSetDefault));
		
		output->setOnGetValueAction(Common::Action::create(this, &InputNeurone::onValueGet));
	}
	
	InputNeurone::~InputNeurone() {
		getLogic()->removeInputInternal(input);
	}
	
	const char *InputNeurone::getType() const {
		return "Input";
	}
	
	const char *InputNeurone::getInstance() const {
		return input->getName();
	}
	
	const char *InputNeurone::getCaption() {
		return input->getName();
	}
	
	bool InputNeurone::hasCaption() {
		return true;
	}
	
	Input *InputNeurone::getLogicInput() {
		return input;
	}
	
	void InputNeurone::invalidate() {
		onSetType();
		onSetDefault();
	}
	
	void InputNeurone::onSetType() {
		int type_id = type->getValue().getInt();
		const TypeDef *type_def = logic->getScript()->getLibraries()->getTypeDefById(type_id);
		TypeInfo type_info = TYPEID(int);
		if(type_def != NULL) {
			type_info = type_def->getTypeInfo();
		}
		output->getVariable().setType(type_info);
		default_value->getVariable().setType(type_info);
		if(input->getDefaultValue().getType() != type_info) input->getDefaultValue().setType(type_info);
	}
	
	//--------------------------------------------------------------------------
	//
	// OutputNeurone
	//
	//--------------------------------------------------------------------------
	
	void OutputNeurone::onValueGet() {
		output->setValue(input->getValue());
		get->run();
	}
	
	void OutputNeurone::onValueSet() {
		output->setValue(input->getValue());
	}
	
	void OutputNeurone::onSetType() {
		int type_id = type->getValue().getInt();
		const TypeDef *type_def = logic->getScript()->getLibraries()->getTypeDefById(type_id);
		TypeInfo type_info = TYPEID(int);
		if(type_def != NULL) {
			type_info = type_def->getTypeInfo();
		}
		output->getVariable().setType(type_info);
		if(input->getDefaultValue().getType() != type_info) input->getDefaultValue().setType(type_info);
	}
	
	OutputNeurone::OutputNeurone(const char *instance, Logic *logic_, Neurone *dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;
		
		output = getLogic()->addOutput(instance, new Output(getLogic()));
		output->setOnGetValueAction(Common::Action::create(this, &OutputNeurone::onValueGet));
		
		input = addInput("value", new Input(Variable()));
		type = addParameter("type", new Parameter(Variable(TYPEID(int))));
		type->getVariable().setOnChangeAction(Common::Action::create(this, &OutputNeurone::onSetType));
		get = addEvent("on get", new Event());
		Action *action = addAction("set", new Action());
		action->setAction(Common::Action::create(this, &OutputNeurone::onValueSet));
	}
	
	OutputNeurone::~OutputNeurone() {
		getLogic()->removeOutputInternal(output);
	}
	
	const char *OutputNeurone::getType() const {
		return "Output";
	}
	
	const char *OutputNeurone::getInstance() const {
		return output->getName();
	}
	
	const char *OutputNeurone::getCaption() {
		return output->getName();
	}
	
	bool OutputNeurone::hasCaption() {
		return true;
	}
	
	Output *OutputNeurone::getLogicOutput() {
		return output;
	}
	
	void OutputNeurone::invalidate() {
		onSetType();
	}
	
	//--------------------------------------------------------------------------
	//
	// class LogicDummy
	//
	//--------------------------------------------------------------------------
	
	LogicDummy::LogicDummy(const Logic &logic, const NeuroneDef *def) : NeuroneDummy(logic) {
		source = &logic;
		this->def = def;
	}
	
	void LogicDummy::invalidate() {
		
		// actions remove
		for(int i=0; i<getActionsCount(); i++) {
			if(source->getAction(getActionName(i)) != NULL) continue;
			removeActionInternal(i--);
		}
		
		// actions add
		for(int i=0; i<source->getActionsCount(); i++) {
			if(getAction(source->getActionName(i)) != NULL) continue;
			addAction(source->getActionName(i), new Action());
		}
		
		// events remove
		for(int i=0; i<getEventsCount(); i++) {
			if(source->getEvent(getEventName(i)) != NULL) continue;
			removeEventInternal(i--);
		}
		
		// events add
		for(int i=0; i<source->getEventsCount(); i++) {
			if(getEvent(source->getEventName(i)) != NULL) continue;
			addEvent(source->getEventName(i), new Event());
		}
		
		// inputs remove
		for(int i=0; i<getInputsCount(); i++) {
			if(source->getInput(getInputName(i)) != NULL) continue;
			removeInputInternal(i--);
		}
		
		// inputs add
		for(int i=0; i<source->getInputsCount(); i++) {
			if(getInput(source->getInputName(i)) != NULL) continue;
			addInput(source->getInputName(i), new Input(source->getInput(i).getDefaultValue()));
		}
		
		// inputs validate
		for(int i=0; i<source->getInputsCount(); i++) {
			const Input *src_input = &source->getInput(i);
			Input *dst_input = getInput(source->getInputName(i));
			if(src_input->getDefaultValue().getType() == dst_input->getDefaultValue().getType()) continue;
			dst_input->setValue(src_input->getValue());
			dst_input->getDefaultValue() = src_input->getDefaultValue();
		}
		
		// outputs remove
		for(int i=0; i<getOutputsCount(); i++) {
			if(source->getOutput(getOutputName(i)) != NULL) continue;
			removeOutputInternal(i--);
		}
		
		// outputs add
		for(int i=0; i<source->getOutputsCount(); i++) {
			if(getOutput(source->getOutputName(i)) != NULL) continue;
			addOutput(source->getOutputName(i), new Output(this, source->getOutput(i).getValue()));
		}
		
		// outputs validate
		for(int i=0; i<source->getOutputsCount(); i++) {
			const Output *src_output = &source->getOutput(i);
			Output *dst_output = getOutput(source->getOutputName(i));
			if(src_output->getValue().getType() == dst_output->getValue().getType()) continue;
			dst_output->setValue(src_output->getValue());
		}
		

	}
	
	const Logic *LogicDummy::getSource() const {
		return source;
	}
	
	Link::Link() {
		src = NULL;
		dst = NULL;
		src_index = -1;
		dst_index = -1;
		event_action = true;
	}
	
	Link::Link(Neurone *src, int src_index, Neurone *dst, int dst_index, bool event_action) {
		this->src = src;
		this->dst = dst;
		this->src_index = src_index;
		this->dst_index = dst_index;
		this->event_action = event_action;
	}
	
	void Link::render(Renderer *renderer, bool selected) {
		if(!is()) return;
		
		vec3 p0;
		vec3 p1;
		vec3 c0;
		vec3 c1;
		
		if(event_action) {
			p0 = src->getEventPos(src_index);
			p1 = dst->getActionPos(dst_index);
			c0 = vec3(1.0f, 1.0f, 1.0f);
			c1 = vec3(1.0f, 1.0f, 1.0f);
			
#ifdef CUSTOM_NEURONE
			float actionWireAngle = dst->getActionWireAngle(dst_index);
			float eventWireAngle = src->getEventWireAngle(src_index);
			renderer->linkSpline(p0, p1,\
								 eventWireAngle,\
								 actionWireAngle,\
								 c0,\
								 c1,\
								 selected, -1);
#endif
		} else {
			p0 = src->getOutputPos(src_index);
			p1 = dst->getInputPos(dst_index);
			
			c0 = vec3(0.3f, 1.0, 1.0f);
			const TypeDef *td0 = src->getOutput(src_index).getTypeDef();
			if(td0) c0 = vec3(td0->getR(), td0->getG(), td0->getB());
			
			
			c1 = vec3(0.3f, 1.0, 1.0f);
			const TypeDef *td1 = dst->getInput(dst_index).getTypeDef();
			if(td1) c1 = vec3(td1->getR(), td1->getG(), td1->getB());
			
#ifdef CUSTOM_NEURONE
			float inputWireAngle = dst->getInputWireAngle(dst_index);
			float outputWireAngle = src->getOutputWireAngle(src_index);
			renderer->linkSpline(p0, p1, outputWireAngle, inputWireAngle, c0, c1, selected, -1);
#endif
		}
		
#ifndef CUSTOM_NEURONE
		renderer->linkSpline(p0, p1, c0, c1, selected);
#endif
	}
	
	bool Link::select(const vec3 &point) {
		if(!is()) return false;
		
		vec3 p0;
		vec3 p1;
		
		
		
		if(event_action) {
			p0 = src->getEventPos(src_index);
			p1 = dst->getActionPos(dst_index);
			
#ifdef CUSTOM_NEURONE
			float actionWireAngle = dst->getActionWireAngle(dst_index);
			float eventWireAngle = src->getEventWireAngle(src_index);
			return Renderer::selectSpline(p0, p1, eventWireAngle, actionWireAngle, point, 6.0f);
#endif
		} else {
			
			p0 = src->getOutputPos(src_index);
			p1 = dst->getInputPos(dst_index);
			
#ifdef CUSTOM_NEURONE
			float inputWireAngle = dst->getInputWireAngle(dst_index);
			float outputWireAngle = src->getOutputWireAngle(src_index);
			return Renderer::selectSpline(p0, p1, outputWireAngle, inputWireAngle, point, 6.0f);
#endif
		}
		
#ifndef CUSTOM_NEURONE
		return Renderer::selectSpline(p0, p1, point, 6.0f);
#endif
	}
	
	void Link::remove() {
		if(!is()) return;
		if(event_action) {
			src->getEvent(src_index).removeAction(&dst->getAction(dst_index));
		} else {
			dst->getInput(dst_index).setSource(NULL);
		}
		*this = Link();
	}
	
	bool Link::is() {
		return src != NULL && dst != NULL && src_index >= 0 && dst_index >= 0;
	}
	
	void GetVariablePtrNeurone::onValueGet() {
		TypeToVariable <Variable *> (variable, getOutput(0).getVariable());
	}
	
	GetVariablePtrNeurone::GetVariablePtrNeurone(const char *inst, Logic *logic_, Neurone *dummyNode) {
		assert(logic_ != NULL);
		setLogic(logic_);
		this->dummyNode = dummyNode;
		variable = getLogic()->getVariable(inst);
		if(variable == NULL) {
			errorMessage(format("Variable \"%s\" not found.", inst));
			return;
		}
		output = addOutput("ptr", new Output(this));
		output->getVariable().setType(TYPEID(Variable*));
		output->setOnGetValueAction(Common::Action::create(this, &GetVariablePtrNeurone::onValueGet));
	}
	
	GetVariablePtrNeurone::~GetVariablePtrNeurone() {
		
	}
	
	const char *GetVariablePtrNeurone::getType() const {
		return "GetVariablePtr";
	}
	
	const char *GetVariablePtrNeurone::getInstance() const {
		return getLogic()->getVariableName(variable);
	}
	
	const char *GetVariablePtrNeurone::getCaption() {
		return getLogic()->getVariableName(variable);
	}
	
	bool GetVariablePtrNeurone::hasCaption() {
		return true;
	}
	
	Variable *GetVariablePtrNeurone::getLogicVariable() {
		return variable;
	}
	
}

//------------------------------------------------------------------------------
