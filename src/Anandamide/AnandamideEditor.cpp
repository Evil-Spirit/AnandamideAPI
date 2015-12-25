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

#include "AnandamideEditor.h"

//------------------------------------------------------------------------------

#include "AnandamideScript.h"
#include "AnandamideInput.h"
#include "AnandamideOutput.h"
#include "AnandamideParameter.h"
#include "AnandamideEvent.h"
#include "AnandamideAction.h"
#include "AnandamideLibrary.h"

#include "Camera2d.h"
#include "Grid.h"
#include "Xml.h"

#include <QDebug>

#define CUSTOM_NEURONE
//------------------------------------------------------------------------------

namespace Anandamide {
	
	void LogicEditor::onRemoveNode(Neurone *node) {
		
	}
	
	void LogicEditor::onSelectNode(Neurone *node) {
		
	}
	
	void LogicEditor::onLinkCreateBlock(float x, float y) {
		
	}
	
	LogicEditor::LogicEditor() {
		on_selection_changed = NULL;
		on_link_create_block = NULL;
		blockGroupSelection = false;
		setGroupFlag = false;
		editableFlag = true;
		pasteShift = 0;
		logic = NULL;
		camera = new Camera2d();
		grid = new Grid();
		hovered = NULL;
		selected = NULL;
		reset();
	}
	
	LogicEditor::~LogicEditor() {
		delete on_selection_changed;
		delete on_link_create_block;
		delete camera;
		delete grid;
	}
	
	void LogicEditor::reset() {
		selectNode(NULL);
		hoverNode(NULL);
		edit_state = SELECT;
		link_event = NULL;
		link_action = NULL;
		link_output = NULL;
		link_input = NULL;
		link_input_index = -1;
		link_output_index = -1;
		link_event_index = -1;
		link_action_index = -1;
		picked_link = Link();
		selected_link = Link();
		grid->step = 8.0;
		camera->clear();
	}
	
	//--------------------------------------------------------------------------
	
	void LogicEditor::clearSelection() {
		if (logic == NULL) return;
		for (int i=0; i<logic->getNodesCount(); i++) {
			logic->getNode(i).setSelected(false);
		}
		selectNode(NULL);
		hoverNode(NULL);
		selected_link = Link();
		if(!blockGroupSelection)
			for (int i=0; i<logic->getNodeGroupCount(); i++) {
				logic->getNodeGroup(i).setSelected(false);
			}
	}
	
	void LogicEditor::focusNode(Neurone *node) {
		camera->setTarget(vec3(node->getPosX() + node->getWidth() / 2.0f, node->getPosY() + node->getHeight() / 2.0f, 0.0f));
	}
	
	void LogicEditor::render(Renderer *renderer, int w, int h, bool flag) {
		if(logic == NULL) return;
		if(!flag) {
			camera->setup(renderer, w, h);
			grid->render(renderer, camera->getFrustum());
			return;
		}
		if (selected != NULL) {
			renderer->color(1.0f, 0.3f, 0.3f);
			//renderer->rect(selected->getPosX(), selected->getPosY(), selected->getWidth(), selected->getHeight());
			renderer->rectLine(selected->getPosX() - 1, selected->getPosY() - 1, selected->getWidth() + 2, selected->getHeight() + 2);
				//renderer->rect(selected->getPosX() - 2, selected->getPosY() - 2, selected->getWidth() + 4, selected->getHeight() + 4);
		}
		
		if (logic != NULL) {
			logic->renderContent(renderer);
		}
		
		if (edit_state == SELECT_RECT) {
			renderer->color(0.3f, 0.5f, 1.0f, 0.3f);
			renderer->rectFill(rect.min.x, rect.min.y, rect.max.x - rect.min.x, rect.max.y - rect.min.y);
		} else if (edit_state == LINK_EVENT_ACTION) {
			
			vec3 p0;
			vec3 p1;
			
#ifdef CUSTOM_NEURONE
			float actionWireAngle = 0.0f;
			float eventWireAngle = 360.0f;
#endif
			
			if(link_action != NULL) {
				
				p1 = link_action->getActionPos(link_action_index);
				p0 = free_point;
				
#ifdef CUSTOM_NEURONE
				actionWireAngle = link_action->getActionWireAngle(link_action_index);
#endif
				
			} else if(link_event != NULL) {
				
				p0 = link_event->getEventPos(link_event_index);
				p1 = free_point;
				
#ifdef CUSTOM_NEURONE
				eventWireAngle = link_event->getEventWireAngle(link_event_index);
#endif
				
			}
			
			if (link_action != NULL || link_event != NULL) {
				
#ifndef CUSTOM_NEURONE
				renderer->linkSpline(p0, p1, vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f));
#endif
				
#ifdef CUSTOM_NEURONE
				renderer->linkSpline(p0, p1,\
									 eventWireAngle,\
									 actionWireAngle,\
									 vec3(1.0f, 1.0f, 1.0f),\
									 vec3(1.0f, 1.0f, 1.0f),\
									 false, -1);
#endif
				
			}
		} else if (edit_state == LINK_OUTPUT_INPUT) {
			
			vec3 p0;
			vec3 p1;
			
#ifdef CUSTOM_NEURONE
			float inputWireAngle = 360.0f;
			float outputWireAngle = 0.0f;
#endif
			
			if(link_input != NULL) {
				p1 = link_input->getInputPos(link_input_index);
				p0 = free_point;
				
#ifdef CUSTOM_NEURONE
				inputWireAngle = link_input->getInputWireAngle(link_input_index);
#endif
				
			} else if(link_output != NULL) {
				p0 = link_output->getOutputPos(link_output_index);
				p1 = free_point;
				
#ifdef CUSTOM_NEURONE
				outputWireAngle = link_output->getOutputWireAngle(link_output_index);
#endif
			}
			
			if (link_input != NULL || link_output != NULL) {
				
				vec3 color = vec3(0.3f, 1.0, 1.0f);
				if (link_input != NULL) {
					const TypeDef *td = link_input->getInput(link_input_index).getTypeDef();
					if(td) color = vec3(td->getR(), td->getG(), td->getB());
				}
				if (link_output != NULL) {
					const TypeDef *td = link_output->getOutput(link_output_index).getTypeDef();
					if(td) color = vec3(td->getR(), td->getG(), td->getB());
				}
				
#ifndef CUSTOM_NEURONE
				renderer->linkSpline(p0, p1, color, color);
#endif
				
#ifdef CUSTOM_NEURONE
				renderer->linkSpline(p0, p1, outputWireAngle, inputWireAngle, color, color, false, -1);
#endif
			}
		}
		
		picked_link.render(renderer, true);
		selected_link.render(renderer, true);
		
	}
	
	//событие изменения логики сценария
	void LogicEditor::onChangeLogic()
	{
		qDebug() << "LogicEditor::onChangeLogic()";
	}
	
	//событие изменения месторасположения блоков
	void LogicEditor::onChangeLocation()
	{
		qDebug() << "LogicEditor::onChangeLocation()";
	}
	
	void LogicEditor::clear() {
		if (logic != NULL) {
			logic->clear();
		}
		reset();
	}
	
	Str LogicEditor::getInfo(int x, int y) {
		if(logic == NULL) return "";
		vec3 pos = camera->fromMouse(x, y);
		for(int i=logic->getNodesCount() - 1; i>=0; i--) {
			Neurone *node = &logic->getNode(i);
			if(node->isSelected(pos.x, pos.y)) {
				Str result = "Node";
				result = result + format("Type: %s", node->getType());
				result = result + format("\nName: %s", node->getName());
				result = result + format("\nLibrary: %s", node->getLibraryName());
				result = result + format("\nDummy: %s", (node->isDummy()) ? "true" : "false");
				result = result + format("\ntypeid: %s", typeid(*node).name());
				result = result + format("\nPosX: %d", node->getPosX());
				result = result + format("\nPosY: %d", node->getPosY());
				
				if(node->getLibrary() != NULL) {
					result = result + format("\nLibrary: %s", node->getLibrary()->getName());
				}
				
				return result;
			}
			int index = node->isInputSelected(pos.x, pos.y);
			if(index >= 0) {
				Str result = "Input";
				result = result + format("\nName: %s", node->getInputName(index));
				result = result + format("\nType: %s", node->getInput(index).getVariable().getTypeName());
				result = result + format("\nTypeID: %d", node->getInput(index).getVariable().getType().id());
				result = result + format("\nValue: %d", node->getInput(index).getValue().getStr());
				if(node->getLibrary() != NULL) {
					const TypeDef *def = node->getLibrary()->getTypeDef(node->getInput(index).getVariable().getType());
					if(def != NULL) {
						result = result + format("\nColor: %f %f %f", def->getR(), def->getG(), def->getB());
					}
				}
				return result;
			}
			index = node->isOutputSelected(pos.x, pos.y);
			if(index >= 0) {
				Str result = "Output";
				result = result + format("\nName: %s", node->getOutputName(index));
				result = result + format("\nType: %s", node->getOutput(index).getVariable().getTypeName());
				result = result + format("\nTypeID: %d", node->getOutput(index).getVariable().getType().id());
				if(node->getLibrary() != NULL) {
					const TypeDef *def = node->getLibrary()->getTypeDef(node->getOutput(index).getVariable().getType());
					if(def != NULL) {
						result = result + format("\nColor: %f %f %f", def->getR(), def->getG(), def->getB());
					}
				}
				
				return result;
			}
			index = node->isActionSelected(pos.x, pos.y);
			if(index >= 0) {
				return format("Action name: %s",node->getActionName(index));
			}
			index = node->isEventSelected(pos.x, pos.y);
			if(index >= 0) {
				Event *event = &node->getEvent(index);
				Str result = format("Event name: %s",node->getEventName(index));
				for(int j=0; j<event->getActionsCount(); j++) {
					result = result + format("\n%s :: %s", event->getAction(j).getNeurone()->getType(), event->getAction(j).getName());
				}
				return result;
			}
		}
		return "";
	}
	
	Link LogicEditor::selectLink(const vec3 &point) {
		if(logic == NULL) return Link();

		for (int i=0; i<logic->getNodesCount(); i++) {
			Neurone *node = &logic->getNode(i);
			
			// select events actions
			for (int j=0; j<node->getEventsCount(); j++) {
				Event *event = &node->getEvent(j);
				for (int k=0; k<event->getActionsCount(); k++) {
					Neurone *action_neurone = event->getAction(k).getNeurone();
					int action_index = action_neurone->indexOf(&event->getAction(k));
					Link link(node, j, action_neurone, action_index, true);
					if(!link.select(point)) continue;
					return link;
				}
			}

			// select inputs outputs
			for (int j=0; j<node->getInputsCount(); j++) {
				Input *input = &node->getInput(j);
				if (input->getSource() == NULL) continue;
				int l = input->getSource()->getInstance()->indexOf(input->getSource());
				Link link(input->getSource()->getInstance(), l, node, j, false);
				if(!link.select(point)) continue;
				return link;
			}
		}
		return Link();
	}
	
	Link &LogicEditor::getPickedLink() {
		return picked_link;
	}
	
	void LogicEditor::clearLinks() {
		picked_link = Link();
		selected_link = Link();
	}
	
	Neurone *LogicEditor::getNodeByPos(int X, int Y)
	{
		if(logic == NULL) return NULL;
		vec3 pos = camera->fromMouse(X, Y);
		return logic->getNodeByPos(pos.x, pos.y);
	}
	
	Action *LogicEditor::getActionByPos(int X, int Y)
	{
		if(logic == NULL) return NULL;
		vec3 pos = camera->fromMouse(X, Y);
		return logic->getActionByPos(pos.x, pos.y);
	}
	
	bool LogicEditor::canCreateNewGroup()
	{
		if(logic == NULL) return false;
		return logic->canCreateNewGroup();
	}
	
	bool LogicEditor::createNewGroup()
	{
		if(!canCreateNewGroup())
			return false;
		logic->addNodesGroupBySelection();
		return true;
	}
	
	NeuroneGroup *LogicEditor::getSelectedGroup()
	{
		if(logic == NULL) return NULL;
		return logic->getSelectedGroup();
	}
	
	NeuroneGroup *LogicEditor::getGroupOfSelectedNodes()
	{
		if(logic == NULL) return NULL;
		return logic->getGroupOfSelectedNodes();
	}
	
	bool LogicEditor::removeSelectedFromGroup()
	{
		if(logic == NULL) return false;
		return logic->removeSelectedFromGroup();
	}
	
	NeuroneGroup *LogicEditor::getGroupByPos(int X, int Y)
	{
		if(logic == NULL) return NULL;
		vec3 pos = camera->fromMouse(X, Y);
		return logic->getGroupByPos(pos.x, pos.y);
	}
	
	bool LogicEditor::removeGroup(NeuroneGroup *group)
	{
		if(logic == NULL) return false;
		return logic->removeGroup(group);
	}
	
	bool LogicEditor::removeGroupWithNodes(NeuroneGroup *group)
	{
		if(logic == NULL) return false;
		return logic->removeGroupWithNodes(group);
	}
	
	int LogicEditor::getGroupsCount()
	{
		if(logic == NULL) return 0;
		return logic->getNodeGroupCount();
	}
	
	bool LogicEditor::addToGroupPrepare()
	{
		if(!canCreateNewGroup() || getGroupsCount() == 0)
			return false;
		setGroupFlag = true;
		return true;
	}
	
	void LogicEditor::addToGroupCancel()
	{
		setGroupFlag = false;
	}
	
	bool LogicEditor::isAddToGroupMode()
	{
		return setGroupFlag;
	}
	
	bool LogicEditor::addSelectedToGroup(NeuroneGroup *group)
	{
		if(logic == NULL) return false;
		return logic->addSelectedToGroup(group);
	}
	
	bool LogicEditor::addSelectedToGroupByPos(int X, int Y)
	{
		if(!setGroupFlag)
			return false;
		setGroupFlag = false;
		NeuroneGroup* group = getGroupByPos(X, Y);
		if(group == NULL)
			return false;
		return addSelectedToGroup(group);
	}
	
	bool LogicEditor::canPopupMenu()
	{
		return edit_state != LINK_OUTPUT_INPUT && edit_state != LINK_EVENT_ACTION;
	}
	
	bool LogicEditor::isSomethingSelected()
	{
		if(logic == NULL) return false;
		return logic->isSomethingSelected();
	}
	
	int LogicEditor::getSelectedGroupsCount()
	{
		if(logic == NULL) return 0;
		return logic->getSelectedGroupsCount();
	}
	
	void LogicEditor::makeLogicFromGroup(NeuroneGroup *group)
	{
		if(group == NULL || logic == NULL)
			return;
		if(group->getLogic() != logic)
			return;
		Script* script = logic->getScript();
		if(script == NULL)
			return;
		int index = 0;
		Logic* newLogic;
		do {
			newLogic = script->addLogic(format("Group_logic_%d", index));
			index++;
		} while(newLogic == NULL);
		newLogic->setKind("Group Logics");
		newLogic->setComment(group->getComment());
		copyGroupContentsToLogic(group, newLogic);
	}
	
	bool LogicEditor::canMakeGroupFromLogic()
	{
		if(logic == NULL)
			return false;
		Neurone* node = NULL;
		for(int i = 0; i < logic->getNodesCount(); ++i) {
			if(logic->getNode(i).isSelected()) {
				if(node == NULL)
					node = &logic->getNode(i);
				else 
					return false;
			}
		}
		if(node == NULL)
			return false;
		return Str(typeid(*node).name()) == "class Anandamide::LogicDummy";
	}
	
	void LogicEditor::makeGroupFromLogic(Neurone *node)
	{
		if(node == NULL || logic == NULL)
			return;
		if(node->getLogic() != logic)
			return;
		if(Str(typeid(*node).name()) != "class Anandamide::LogicDummy")
			return;
		createGroupFromLogic(node);
	}
	
	vec3 LogicEditor::getCameraPos() const
	{
		return camera->getPos();
	}
	
	void LogicEditor::setCameraPos(vec3 pos)
	{
		camera->setPos(pos);
	}
	
	void LogicEditor::createEventLink(Neurone *node, int index)
	{
		link_event = node;
		link_event_index = index;
		edit_state = LINK_EVENT_ACTION;
		createEventActionLinks();
		blockGroupSelection = true;
	}
	
	void LogicEditor::createActionLink(Neurone *node, int index)
	{
		link_action = node;
		link_action_index = index;
		edit_state = LINK_EVENT_ACTION;
		createEventActionLinks();
		blockGroupSelection = true;
	}
	
	void LogicEditor::createOutputLink(Neurone *node, int index)
	{
		link_output = node;
		link_output_index = index;
		edit_state = LINK_OUTPUT_INPUT;
		createEventActionLinks();
		blockGroupSelection = true;
	}
	
	void LogicEditor::createInputLink(Neurone *node, int index)
	{
		link_input = node;
		link_input_index = index;
		edit_state = LINK_OUTPUT_INPUT;
		createEventActionLinks();
		blockGroupSelection = true;
	}
	
	void LogicEditor::setEditable(bool editable)
	{
		editableFlag = editable;
	}
	
	void LogicEditor::mouseDown(int X_, int Y_, int b, bool ctrl, bool blockLink) {
		
		if(logic == NULL) return;
		float x = camera->getCursorPos().x;
		float y = camera->getCursorPos().y;
		
		if(b == 0) {
			
			if(picked_link.is()) {
				clearSelection();
				selected_link = picked_link;
				return;
			}
			
			bool try_get_link = edit_state == LINK_OUTPUT_INPUT && (link_input != NULL || link_output != NULL);
			
			bool clear = true;
			
			bool nodeSelected = false;
			bool groupSelected = false;
			selectedGroup = NULL;
			Neurone *new_selection = NULL;
			for (int i=logic->getNodesCount() - 1; i>=0; i--) {
				
				if(edit_state == SELECT) {
					if (logic->getNode(i).isSelected(x, y)) {
						
						nodeSelected = true;
						
						if (ctrl == false) {
							if (logic->getNode(i).isSelected() == false) clearSelection();
							logic->getNode(i).setSelected(true);
						} else {
							logic->getNode(i).setSelected(!logic->getNode(i).isSelected());
						}
						
						
						if (logic->getNode(i).isSelected()) {
							new_selection = &logic->getNode(i);
						}
						
						break;
					}
				}
				
				if (edit_state != LINK_OUTPUT_INPUT && editableFlag) {
					
					int index = -1;
					if(!blockLink)
						index = logic->getNode(i).isActionSelected(x, y);
//					int index = logic->getNode(i).isActionSelected(x, y);
					
					if (index >= 0) {
						link_action = &logic->getNode(i);
						link_action_index = index;
						edit_state = LINK_EVENT_ACTION;
						createEventActionLinks();
						blockGroupSelection = true;
						break;
					}
					
					index = -1;
					if(!blockLink)
						index = logic->getNode(i).isEventSelected(x, y);
//					index = logic->getNode(i).isEventSelected(x, y);
					
					if (index >= 0) {
						link_event = &logic->getNode(i);
						link_event_index = index;
						edit_state = LINK_EVENT_ACTION;
						createEventActionLinks();
						blockGroupSelection = true;
						break;
					}
				}
				
				if (edit_state != LINK_EVENT_ACTION && editableFlag) {
					
					int index = -1;
					if(!blockLink)
						index = logic->getNode(i).isInputSelected(x, y);
//					int index = logic->getNode(i).isInputSelected(x, y);
					
					if (index >= 0) {
						link_input = &logic->getNode(i);
						link_input_index = index;
						edit_state = LINK_OUTPUT_INPUT;
						createOutputInputLinks();
						blockGroupSelection = true;
						break;
					}
					
					index = -1;
					if(!blockLink)
						index = logic->getNode(i).isOutputSelected(x, y);
//					index = logic->getNode(i).isOutputSelected(x, y);
					
					if (index >= 0) {
						link_output = &logic->getNode(i);
						link_output_index = index;
						edit_state = LINK_OUTPUT_INPUT;
						createOutputInputLinks();
						blockGroupSelection = true;
						break;
					}
				}
				
			}
			
			if(edit_state == SELECT && !nodeSelected && !blockGroupSelection) {
				for(int i = 0; i < logic->getNodeGroupCount(); ++i) 
					if (logic->getNodeGroup(i).isSelected(x, y)) {
						groupSelected = true;
						
						if (ctrl == false) {
							if (logic->getNodeGroup(i).isSelected() == false) clearSelection();
							logic->getNodeGroup(i).setSelected(true);
						} else {
							logic->getNodeGroup(i).setSelected(!logic->getNodeGroup(i).isSelected());
						}
						
						
						if (logic->getNodeGroup(i).isSelected()) {
							selectedGroup = &logic->getNodeGroup(i);
						}
					}
			}
			
			
			selectNode(new_selection);
			
			if (selected != NULL || selectedGroup != NULL) {
				edit_state = MOVE;
				clear = false;
				if(selected == NULL)
					selected = &selectedGroup->getNode(0);
				move.x = x - selected->getPosX();
				move.y = y - selected->getPosY();
			}
			
			if (edit_state == SELECT && clear && !nodeSelected && !groupSelected && !blockGroupSelection) {
				edit_state = SELECT_RECT;
				rect.min.x = x;
				rect.min.y = y;
				rect.min.z = -1.0f;
				rect.max.x = x;
				rect.max.y = y;
				rect.max.z = 1.0f;
			}
			
			if(try_get_link && edit_state == LINK_OUTPUT_INPUT) {
				onLinkCreateBlock(x, y);
			}
			
		} else if(b == 1) {
			undoLinks();
		} else if(b == 2) {
			camera->mouseDown(x, y);
		}
		
	}
	
	void LogicEditor::undoLinks() {
		link_action = NULL;
		link_event = NULL;
		link_input = NULL;
		link_output = NULL;
		edit_state = SELECT;
	}
	
	void LogicEditor::createEventActionLinks() {
		
		if (link_action == NULL) return;
		if (link_event == NULL) return;
		
		link_event->getEvent(link_event_index).addAction(&link_action->getAction(link_action_index));
		
		undoLinks();
	}
	
	void LogicEditor::createOutputInputLinks() {
		
		if (link_input == NULL) return;
		if (link_output == NULL) return;
		
		link_input->getInput(link_input_index).setSource(&link_output->getOutput(link_output_index));
		
		undoLinks();
	}
	
	//--------------------------------------------------------------------------
	
	void LogicEditor::removeNode(int index) {
		
		if (logic == NULL) return;
		
		onRemoveNode(&logic->getNode(index));
		
		if(selected == &logic->getNode(index)) selectNode(NULL);
		if(hovered == &logic->getNode(index)) hoverNode(NULL);
		
		logic->removeNode(index);
	}
	
	void LogicEditor::removeNode(Neurone *node) {
		if (logic == NULL) return;
		if(node == selected) clearSelection();
		logic->removeNode(node);
	}
	
	bool LogicEditor::removeSelected() {
		if (logic == NULL) return false;
		bool res = false;
		undoLinks();
		selected_link.remove();
		picked_link = Link();
		for (int i=0; i<logic->getNodesCount(); i++) {
			if (logic->getNode(i).isSelected() == false) continue;
			Str type = logic->getNode(i).getType();
			if(type == "Action") continue;
			if(type == "Event") continue;
			if(type == "Input") continue;
			if(type == "Output") continue;
			removeNode(i--);
			res = true;
		}
		for (int i=0; i<logic->getNodeGroupCount(); i++) {
			if (logic->getNodeGroup(i).isSelected() == false) continue;
			logic->removeGroup(&logic->getNodeGroup(i--));
			res = true;
		}
		return res;
	}
	
	bool LogicEditor::cloneSelected() {
		
		Array <Neurone> nodes_to_clone(false);
		
		for (int i=0; i<logic->getNodesCount(); i++) {
			Neurone *node = &logic->getNode(i);
			if (node->isSelected() == false) continue;
			Str type = node->getType();
			if(type == "Action") continue;
			if(type == "Event") continue;
			if(type == "Input") continue;
			if(type == "Output") continue;
			nodes_to_clone.append(node);
		}
		
		Array <NeuroneGroup> groups_to_clone(false);
		
		for (int i=0; i<logic->getNodeGroupCount(); i++) {
			NeuroneGroup *group = &logic->getNodeGroup(i);
			if (group->isSelected() == false) continue;
			groups_to_clone.append(group);
		}
		
		clearSelection();
		
		logic->cloneNodesAndGroups(nodes_to_clone, groups_to_clone, logic, true, true);
		
		// shift selection for visual effect
		for (int i=0; i<logic->getNodesCount(); i++) {
			Neurone *node = &logic->getNode(i);
			if (node->isSelected() == false) continue;
			node->setPos(node->getPosX() + 16, node->getPosY() + 16);
		}
		
		return nodes_to_clone.count() != 0;
	}
	
	void LogicEditor::copyAsXml(Str &xml_str) {
		Logic save_logic(logic->getScript());
		
		Array <Neurone> nodes_to_clone(false);
		
		for (int i=0; i<logic->getNodesCount(); i++) {
			Neurone *node = &logic->getNode(i);
			if (node->isSelected() == false) continue;
			Str type = node->getType();
			if(type == "Action") continue;
			if(type == "Event") continue;
			if(type == "Input") continue;
			if(type == "Output") continue;
			if(type == "GetVariable") {
				if(save_logic.getVariableByName(node->getInstance()) == NULL) {
					Variable *var = save_logic.addVariable(node->getInstance());
					GetVariableNeurone *get_neurone = dynamic_cast <GetVariableNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "GetVariablePtr") {
				if(save_logic.getVariableByName(node->getInstance()) == NULL) {
					Variable *var = save_logic.addVariable(node->getInstance());
					GetVariablePtrNeurone *get_neurone = dynamic_cast <GetVariablePtrNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "SetVariable") {
				if(save_logic.getVariableByName(node->getInstance()) == NULL) {
					Variable *var = save_logic.addVariable(node->getInstance());
					SetVariableNeurone *set_neurone = dynamic_cast <SetVariableNeurone *>(node);
					if(set_neurone != NULL) {
						*var = *set_neurone->getLogicVariable();
					}
				}
			}
			
			//Че это была за херня?
//			const LogicDummy *node_as_logic = dynamic_cast <const LogicDummy *>(node);
//			if(node_as_logic != NULL && node_as_logic->getSource()->getScript() == logic->getScript()) continue;
			
			nodes_to_clone.append(node);
		}
		
		Array <NeuroneGroup> groups_to_clone(false);
		
		for (int i=0; i<logic->getNodeGroupCount(); i++) {
			NeuroneGroup *group = &logic->getNodeGroup(i);
			if (group->isSelected() == false) continue;
			groups_to_clone.append(group);
		}
		
		logic->cloneNodesAndGroups(nodes_to_clone, groups_to_clone, &save_logic, true, false);
		Xml xml("logic");
		save_logic.writeXml(&xml);
		xml.saveToStr(xml_str);
		pasteShift = 0;
	}
	
	bool LogicEditor::pasteAsXml(const Str &xml_str, int x, int y) {
		
		if(xml_str.find("<logic name=") == -1) return false;
		clearSelection();
		Logic load_logic(logic->getScript());
		Xml xml("logic");
		xml.parse(xml_str);
		load_logic.readXml(&xml, true);
		
		Array <Neurone> nodes_to_clone(false);
		
		for (int i=0; i<load_logic.getNodesCount(); i++) {
			Neurone *node = &load_logic.getNode(i);
			Str type = node->getType();
			if(type == "Action") continue;
			if(type == "Event") continue;
			if(type == "Input") continue;
			if(type == "Output") continue;
			if(type == "GetVariable") {
				if(logic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = logic->addVariable(node->getInstance());
					GetVariableNeurone *get_neurone = dynamic_cast <GetVariableNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "GetVariablePtr") {
				if(logic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = logic->addVariable(node->getInstance());
					GetVariablePtrNeurone *get_neurone = dynamic_cast <GetVariablePtrNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "SetVariable") {
				if(logic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = logic->addVariable(node->getInstance());
					SetVariableNeurone *set_neurone = dynamic_cast <SetVariableNeurone *>(node);
					if(set_neurone != NULL) {
						*var = *set_neurone->getLogicVariable();
					}
				}
			}
			nodes_to_clone.append(node);
		}
		
		Array <NeuroneGroup> groups_to_clone(false);
		
		for (int i=0; i<load_logic.getNodeGroupCount(); i++) {
			NeuroneGroup *group = &load_logic.getNodeGroup(i);
			groups_to_clone.append(group);
		}
		
		load_logic.cloneNodesAndGroups(nodes_to_clone, groups_to_clone, logic, true, true);
		// shift selection for visual effect
		Neurone *first = NULL;
		for (int i=0; i<logic->getNodesCount(); i++) {
			Neurone *node = &logic->getNode(i);
			if (node->isSelected() == false) continue;
			if(first == NULL) {
				first = node;
				continue;
			}
			node->setPos(node->getPosX() - first->getPosX() + x + pasteShift, node->getPosY() - first->getPosY() + y + pasteShift);
		}
		if(first)
			first->setPos(x + pasteShift, y + pasteShift);
		pasteShift += 16;
		
		return nodes_to_clone.count() != 0;
	}
	
	void LogicEditor::copyGroupContentsToLogic(NeuroneGroup *srcGroup, Logic *destLogic)
	{
		if(!srcGroup || !destLogic)
			return;
		if(srcGroup->getLogic() == destLogic)
			return;
		//Create list of group nodes
		Array <Neurone> nodes_to_clone(false);
		for(int i = 0; i < srcGroup->getNodesCount(); ++i) {
			Neurone *node = &srcGroup->getNode(i);
			Str type = node->getType();
			if(type == "Action") continue;
			if(type == "Event") continue;
			if(type == "Input") continue;
			if(type == "Output") continue;
			if(type == "GetVariable") {
				if(destLogic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = destLogic->addVariable(node->getInstance());
					GetVariableNeurone *get_neurone = dynamic_cast <GetVariableNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "GetVariablePtr") {
				if(destLogic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = destLogic->addVariable(node->getInstance());
					GetVariablePtrNeurone *get_neurone = dynamic_cast <GetVariablePtrNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "SetVariable") {
				if(destLogic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = destLogic->addVariable(node->getInstance());
					SetVariableNeurone *set_neurone = dynamic_cast <SetVariableNeurone *>(node);
					if(set_neurone != NULL) {
						*var = *set_neurone->getLogicVariable();
					}
				}
			}
			
			nodes_to_clone.append(node);
		}
		//Clone group nodes into new logic
		srcGroup->getLogic()->cloneNodes(nodes_to_clone, destLogic, true, false);
		// shift all nodes in new logic
		int xx = srcGroup->getPosX() + Neurone::head_h;
		int yy = srcGroup->getPosY() + Neurone::head_h;
		int ww = srcGroup->getWidth() - 2*Neurone::head_h;
		int hh = srcGroup->getHeight() - 2*Neurone::head_h;
		for (int i=0; i<destLogic->getNodesCount(); i++) {
			Neurone *node = &destLogic->getNode(i);
			node->setPos(node->getPosX() - xx - ww/2, node->getPosY() - yy - hh/2);
		}
		
		//Find external links
		Array<Action> groupExtActions(false);
		Array<Str> groupExtActionNames;
		Array< Array<Event> > srcEvents;
		
		Array<Event> groupExtEvents(false);
		Array<Str> groupExtEventNames;
		Array< Array<Action> > srcActions;
		
		Array<Input> groupExtInputs(false);
		Array<Str> groupExtInputNames;
		Array<Output> srcOutputs(false);
		
		Array<Output> groupExtOutputs(false);
		Array<Str> groupExtOutputNames;
		Array< Array<Input> > srcInputs;
	
		for(int i = 0; i < srcGroup->getNodesCount(); ++i) {
			Neurone *node = &srcGroup->getNode(i);
			//Find external actions of group
			for(int j = 0; j < node->getActionsCount(); ++j) {
				Action* act = &node->getAction(j);
				for(int k = 0; k < act->events.count(); ++k) {
					Event* ev = act->events.getItem(k);
					if(!srcGroup->isNodeContains(ev->getNeurone())) {
						Action* newAct = &destLogic->getNode(i).getAction(j);
						int arrIdx = groupExtActions.seek(newAct);
						if(arrIdx < 0) {
							arrIdx = groupExtActions.count();
							groupExtActions.append(newAct);
							srcEvents.append(new Array<Event>(false) );
							groupExtActionNames.append(new Str(node->getActionName(j)));
						}
						srcEvents.getItem(arrIdx)->append(ev);
					}
				}
			}
			//Find external events of group
			for(int j = 0; j < node->getEventsCount(); ++j) {
				Event* ev = &node->getEvent(j);
				for(int k = 0; k < ev->getActionsCount(); ++k) {
					Action* act = &ev->getAction(k);
					if(!srcGroup->isNodeContains(act->getNeurone())) {
						Event* newEv = &destLogic->getNode(i).getEvent(j);
						int arrIdx = groupExtEvents.seek(newEv);
						if(arrIdx < 0) {
							arrIdx = groupExtEvents.count();
							groupExtEvents.append(newEv);
							srcActions.append(new Array<Action>(false) );
							groupExtEventNames.append(new Str(node->getEventName(j)));
						}
						srcActions.getItem(arrIdx)->append(act);
					}
				}
			}
			//Find external inputs of group
			for(int j = 0; j < node->getInputsCount(); ++j) {
				Input* inp = &node->getInput(j);
				Output* out = inp->getSource();
				if(out != nullptr) {
					if(!srcGroup->isNodeContains(out->getInstance())) {
						Input* newInp = &destLogic->getNode(i).getInput(j); 
						if(groupExtInputs.seek(newInp) < 0) {
							groupExtInputs.append(newInp);
							srcOutputs.append(out);
							groupExtInputNames.append(new Str(node->getInputName(j)));
						}
					}
				}
			}
		}
		//Find external outputs of group
		for(int i = 0; i < srcGroup->getLogic()->getNodesCount(); ++i) {
			Neurone* node = &srcGroup->getLogic()->getNode(i);
			if(srcGroup->isNodeContains(node)) continue;
			for(int j = 0; j < node->getInputsCount(); ++j) {
				Input* inp = &node->getInput(j);
				Output* out = inp->getSource();
				
				if(out != nullptr) {
					if(srcGroup->isNodeContains(out->getInstance())) {
						int idx = srcGroup->getNodeIndex(out->getInstance());
						int outIdx = out->getInstance()->indexOf(out);
						Output* newOut = &destLogic->getNode(idx).getOutput(outIdx);
						int arrIdx = groupExtOutputs.seek(newOut);
						if(arrIdx < 0) {
							arrIdx = groupExtOutputs.count();
							groupExtOutputs.append(newOut);
							srcInputs.append(new Array<Input>(false) );
							groupExtOutputNames.append(new Str(out->getInstance()->getOutputName(out->getInstance()->indexOf(out))));
						}
						srcInputs.getItem(arrIdx)->append(inp);
					}
				}
			}
		}
		
		//Create logic actions
		int topShift = 0;
		for(int i = 0; i < groupExtActions.count(); ++i) {
			Str actName = groupExtActionNames[i];
			if(destLogic->getAction(actName.str())) {
				int index = 2;
				while(destLogic->getAction(format("%s_%d", actName.str(), index)) != NULL) index++;
				actName = format("%s_%d", actName.str(), index);
			}
			ActionNeurone* new_node = new Anandamide::ActionNeurone(actName.str(), destLogic);
			new_node->setPosX(-ww/2 - Neurone::width * 2);
			new_node->setPosY(-hh/2 + topShift);
			topShift += new_node->getRealHeight() + Neurone::head_h * 2;
			new_node->getEvent("out")->addAction(groupExtActions.getItem(i));
		}
		
		//Create logic inputs
		if(topShift)
			topShift += Neurone::width;
		for(int i = 0; i < groupExtInputs.count(); ++i) {
			Str inpName = groupExtInputNames[i];
			if(destLogic->getInput(inpName.str())) {
				int index = 2;
				while(destLogic->getInput(format("%s_%d", inpName.str(), index)) != NULL) index++;
				inpName = format("%s_%d", inpName.str(), index);
			}
			InputNeurone* new_node = new Anandamide::InputNeurone(inpName.str(), destLogic);
			new_node->setPosX(-ww/2 - Neurone::width * 2);
			new_node->setPosY(-hh/2 + topShift);
			topShift += new_node->getRealHeight() + Neurone::head_h * 2;
			groupExtInputs[i].setSource(new_node->getOutput("value"));
		}
		
		//Create logic events
		topShift = 0;
		for(int i = 0; i < groupExtEvents.count(); ++i) {
			Str evName = groupExtEventNames[i];
			if(destLogic->getEvent(evName.str())) {
				int index = 2;
				while(destLogic->getEvent(format("%s_%d", evName.str(), index)) != NULL) index++;
				evName = format("%s_%d", evName.str(), index);
			}
			EventNeurone* new_node = new Anandamide::EventNeurone(evName.str(), destLogic);
			new_node->setPosX(ww/2 + Neurone::width);
			new_node->setPosY(-hh/2 + topShift);
			topShift += new_node->getRealHeight() + Neurone::head_h * 2;
			groupExtEvents.getItem(i)->addAction(new_node->getAction("in"));
		}
		
		//Create logic outputs
		if(topShift)
			topShift += Neurone::width;
		for(int i = 0; i < groupExtOutputs.count(); ++i) {
			Str outName = groupExtOutputNames[i];
			if(destLogic->getOutput(outName.str())) {
				int index = 2;
				while(destLogic->getOutput(format("%s_%d", outName.str(), index)) != NULL) index++;
				outName = format("%s_%d", outName.str(), index);
			}
			OutputNeurone* new_node = new Anandamide::OutputNeurone(outName.str(), destLogic);
			new_node->setPosX(ww/2 + Neurone::width);
			new_node->setPosY(-hh/2 + topShift);
			topShift += new_node->getRealHeight() + Neurone::head_h * 2;
			new_node->getInput("value")->setSource(groupExtOutputs.getItem(i));
		}
		
		//Create logic node
		Neurone* newLogicNode = srcGroup->getLogic()->addNode(destLogic->getType(), destLogic->getLibraryName(), destLogic->getInstance(), true);
		newLogicNode->setComment(destLogic->getComment());
		newLogicNode->setPos(xx + (ww - newLogicNode->getWidth())/2, yy + (hh - newLogicNode->getHeight())/2);
		
		//Connect logic node to external events
		for(int i = 0; i < srcEvents.count(); ++i) {
			Array<Event> *arr = srcEvents.getItem(i);
			for(int j = 0; j < arr->count(); ++j) {
				arr->getItem(j)->addAction(&newLogicNode->getAction(i));
			}
		}
		
		//Connect logic node to external outputs
		for(int i = 0; i < srcOutputs.count(); ++i) {
			newLogicNode->getInput(i).setSource(srcOutputs.getItem(i));
		}
		
		//Connect logic node to external actions
		for(int i = 0; i < srcActions.count(); ++i) {
			Array<Action> *arr = srcActions.getItem(i);
			for(int j = 0; j < arr->count(); ++j) {
				newLogicNode->getEvent(i).addAction(arr->getItem(j));
			}
		}
		
		//Connect logic node to external inputs
		for(int i = 0; i < srcInputs.count(); ++i) {
			Array<Input> *arr = srcInputs.getItem(i);
			for(int j = 0; j < arr->count(); ++j) {
				arr->getItem(j)->setSource(&newLogicNode->getOutput(i));
			}
		}
		
		//Remove src group
		srcGroup->getLogic()->removeGroupWithNodes(srcGroup);
		selected = NULL;
		selectedGroup = NULL;
		
		postPrepareLogic(destLogic);
	}
	
	void LogicEditor::postPrepareLogic(Logic *destLogic)
	{
		Array<Neurone> deletingNodes(false);
		for(int i = 0; i < destLogic->getNodesCount(); ++i) {
			Neurone* node = &destLogic->getNode(i);
			Str type = node->getType();
			if(type == "Action") {
				Event* ev = &node->getEvent(0);
				for(int j = 0; j < ev->getActionsCount(); ++j) {
					Neurone* nextnode = ev->getAction(j).getNeurone();
					if(Str(nextnode->getType()) != "FlowConnector" || Str(nextnode->getLibraryName()) != "Std") continue;
					deletingNodes.append(nextnode);
					Event* connEv = &nextnode->getEvent(0);
					for(int k = 0; k < connEv->getActionsCount(); ++k)
						ev->addAction(&connEv->getAction(k));
				}
			}
			else if(type == "Event") {
				Action* act = &node->getAction(0);
				for(int j = 0; j < act->events.count(); ++j) {
					Neurone* nextnode = act->events.getItem(j)->getNeurone();
					if(Str(nextnode->getType()) != "FlowConnector" || Str(nextnode->getLibraryName()) != "Std") continue;
					deletingNodes.append(nextnode);
					Action* connAct = &nextnode->getAction(0);
					for(int k = 0; k < connAct->events.count(); ++k)
						connAct->events.getItem(k)->addAction(act);
				}
			}
			else if(type == "Input") {
				Output* out = &node->getOutput(0);
				for(int j = 0; j < destLogic->getNodesCount(); ++j) {
					Neurone* nextnode = &destLogic->getNode(j);
					Input* inp = nextnode->getInput("in");
					if(inp)
						if(inp->getSource() == out) {
							if(Str(nextnode->getType()) != "ValueConnector" || Str(nextnode->getLibraryName()) != "Std") continue;
							deletingNodes.append(nextnode);
							Output* connOut = &nextnode->getOutput(0);
							for(int k = 0; k < destLogic->getNodesCount(); ++k) {
								Neurone* neu = &destLogic->getNode(k);
								for(int n = 0; n < neu->getInputsCount(); ++n) {
									Input* inp2 = &neu->getInput(n);
									if(inp2->getSource() == connOut)
										inp2->setSource(out);
								}
							}
						}
				}
			}
			else if(type == "Output") {
				Input* inp = &node->getInput(0);
				Output* out = inp->getSource();
				if(out) {
					Neurone* nextnode = out->getInstance();
					if(Str(nextnode->getType()) == "ValueConnector" && Str(nextnode->getLibraryName()) == "Std") {
						deletingNodes.append(nextnode);
						Input* connInp = &nextnode->getInput(0);
						inp->setSource(connInp->getSource());
					}
				}
			}
		}
		
		for(int i = 0; i < deletingNodes.count(); ++i) {
			destLogic->removeNode(deletingNodes.getItem(i));
		}
	}
	
	void LogicEditor::createGroupFromLogic(Neurone *logicNode)
	{
		if(logic == NULL)
			return;
		Script* script = logic->getScript();
		if(script == NULL)
			return;
		Logic* srcLogic = logic->getScript()->findLogic(logicNode->getType());
		if(srcLogic == NULL)
			return;
		
		//Create list of logics nodes
		Array <Neurone> nodes_to_clone(false);
		for(int i = 0; i < srcLogic->getNodesCount(); ++i) {
			Neurone *node = &srcLogic->getNode(i);
			Str type = node->getType();
			if(type == "Action") continue;
			if(type == "Event") continue;
			if(type == "Input") continue;
			if(type == "Output") continue;
			if(type == "GetVariable") {
				if(logic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = logic->addVariable(node->getInstance());
					GetVariableNeurone *get_neurone = dynamic_cast <GetVariableNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "GetVariablePtr") {
				if(logic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = logic->addVariable(node->getInstance());
					GetVariablePtrNeurone *get_neurone = dynamic_cast <GetVariablePtrNeurone *>(node);
					if(get_neurone != NULL) {
						*var = *get_neurone->getLogicVariable();
					}
				}
			}
			if(type == "SetVariable") {
				if(logic->getVariableByName(node->getInstance()) == NULL) {
					Variable *var = logic->addVariable(node->getInstance());
					SetVariableNeurone *set_neurone = dynamic_cast <SetVariableNeurone *>(node);
					if(set_neurone != NULL) {
						*var = *set_neurone->getLogicVariable();
					}
				}
			}
			nodes_to_clone.append(node);
		}
		
		//Clone nodes into current logic
		for(int i = 0; i < logic->getNodesCount(); ++i)
			logic->getNode(i).setSelected(false);
		for(int i = 0; i < logic->getNodeGroupCount(); ++i)
			logic->getNodeGroup(i).setSelected(false);
		srcLogic->cloneNodes(nodes_to_clone, logic, true, true);
		
		// shift all nodes
		int xx = logicNode->getPosX();
		int yy = logicNode->getPosY();
		int ww = logicNode->getWidth();
		int hh = logicNode->getHeight();
		for (int i=0; i<logic->getNodesCount(); i++) {
			Neurone *node = &logic->getNode(i);
			if(node->isSelected())
				node->setPos(node->getPosX() + xx + ww/2, node->getPosY() + yy + hh/2);
		}
		
		// create new group
		int gcnt = logic->getNodeGroupCount();
		if(!createNewGroup())
			return;
		if(gcnt == logic->getNodeGroupCount())
			return;
		NeuroneGroup* group = &logic->getNodeGroup(gcnt);
		group->setComment(logicNode->getComment());
		
		//Create external group flow connectors
		Array<Neurone> groupExtraActionNodes(false);
		for(int i = 0; i < logicNode->getActionsCount(); ++i) {
			Neurone* conn = logic->addNode("FlowConnector", "Std", "", true);
			if(!conn)
				return;
			conn->setPos(group->getPosX() - conn->getWidth() * 3 / 2, group->getPosY() + i * conn->getHeight() * 2);
			groupExtraActionNodes.append(conn);
		}
		Array<Neurone> groupExtraEventNodes(false);
		for(int i = 0; i < logicNode->getEventsCount(); ++i) {
			Neurone* conn = logic->addNode("FlowConnector", "Std", "", true);
			if(!conn)
				return;
			conn->setPos(group->getPosX() + group->getWidth() + conn->getWidth() / 2, group->getPosY() + i * conn->getHeight() * 2);
			groupExtraEventNodes.append(conn);
		}
		
		//Find & connect external links to group actions
		int cntr = 0;
		for(int i = 0; i < srcLogic->getNodesCount(); ++i) {
			ActionNeurone *node = dynamic_cast <ActionNeurone *> (&srcLogic->getNode(i));
			if(node == NULL) continue;
			Event* ev = node->getEvent("out");
			if(ev == NULL) continue;
//			if(!ev->getActionsCount()) continue; //no internal connection to action
			Array<Action> actOfNodes(false);
			for(int j = 0; j < ev->getActionsCount(); ++j) {
				Action* act = &ev->getAction(j);
				int idx = -1;
				for(int k = 0; k < nodes_to_clone.count(); ++k)
					if(nodes_to_clone.getItem(k) == act->getNeurone()) {
						idx = k;
						break;
					}
				if(idx >= 0) 
					actOfNodes.append(&group->getNode(idx).getAction( act->getNeurone()->indexOf(act) ));
				else {
					int evCntr = 0;
					for(int k = 0; k < srcLogic->getNodesCount(); ++k) {
						bool isEventNode = Str(srcLogic->getNode(k).getType()) == "Event";
						if(!isEventNode) continue;
						if(act->getNeurone() == &srcLogic->getNode(k)) {
							Action* a = groupExtraEventNodes.getItem(evCntr)->getAction("in");
							if(a)
								actOfNodes.append( a );
						}
						evCntr++;
					}
				}
			}
			//internal connection to "FlowConnector" block
			Event* ev2 = groupExtraActionNodes.getItem(cntr)->getEvent("out");
			if(ev2) {
				for(int j = 0; j < actOfNodes.count(); ++j) {
					ev2->addAction(actOfNodes.getItem(j));
				}
			}
			//external connection to "FlowConnector" block
			Action* act2 = groupExtraActionNodes.getItem(cntr)->getAction("in");
			if(act2) {
				for(int j = 0; j < logicNode->getAction(cntr).events.count(); ++j) {
					Event* eee = logicNode->getAction(cntr).events.getItem(j);
					if(eee->getNeurone() == logicNode) {//перехлест
						int evIdx = logicNode->indexOf(eee);
						groupExtraEventNodes.getItem(evIdx)->getEvent(0).addAction(act2);
					}
					else
						eee->addAction(act2);
				}
			}
			cntr++;
		}
		
		//Find & connect external links to group events
		cntr = 0;
		for(int i = 0; i < srcLogic->getNodesCount(); ++i) {
			EventNeurone *node = dynamic_cast <EventNeurone *> (&srcLogic->getNode(i));
			if(node == NULL) continue;
			Action* act = node->getAction("in");
			if(act == NULL) continue;
//			if(!act->events.count()) continue; //no internal connection to event
			Array<Event> evOfNodes(false);
			for(int j = 0; j < act->events.count(); ++j) {
				Event* ev = act->events.getItem(j);
				int idx = -1;
				for(int k = 0; k < nodes_to_clone.count(); ++k)
					if(nodes_to_clone.getItem(k) == ev->getNeurone()) {
						idx = k;
						break;
					}
				if(idx >= 0)
					evOfNodes.append(&group->getNode(idx).getEvent( ev->getNeurone()->indexOf(ev) ));
			}
			//internal connection to "FlowConnector" block
			Action* act2 = groupExtraEventNodes.getItem(cntr)->getAction("in");
			if(act2) {
				for(int j = 0; j < evOfNodes.count(); ++j) {
					evOfNodes.getItem(j)->addAction(act2);
				}
			}
			//external connection to "FlowConnector" block
			Event* ev2 = groupExtraEventNodes.getItem(cntr)->getEvent("out");
			if(ev2) {
				for(int j = 0; j < logicNode->getEvent(cntr).getActionsCount(); ++j) {
					Action* aaa = &logicNode->getEvent(cntr).getAction(j);
					if(aaa->getNeurone() != logicNode) {
						ev2->addAction( aaa );
					}
				}
			}
			cntr++;
		}
		
		//Create external group value connectors
		Array<Neurone> groupExtraInputNodes(false);
		for(int i = 0; i < logicNode->getInputsCount(); ++i) {
			Neurone* conn = logic->addNode("ValueConnector", "Std", "", true);
			if(!conn) {
				return;
			}
			conn->setPos(group->getPosX() - conn->getWidth() * 3 / 2, group->getPosY() + (i + logicNode->getActionsCount()) * conn->getHeight() * 2);
			groupExtraInputNodes.append(conn);
		}
		Array<Neurone> groupExtraOutputNodes(false);
		for(int i = 0; i < logicNode->getOutputsCount(); ++i) {
			Neurone* conn = logic->addNode("ValueConnector", "Std", "", true);
			if(!conn)
				return;
			conn->setPos(group->getPosX() + group->getWidth() + conn->getWidth() / 2, group->getPosY() + (i + logicNode->getEventsCount()) * conn->getHeight() * 2);
			groupExtraOutputNodes.append(conn);
		}
		
		//Find & connect external links to group inputs
		for(int i = 0; i < logicNode->getInputsCount(); ++i) {
			//external link
			Output* out = logicNode->getInput(i).getSource();
			if(out) {
				if(out->getInstance() == logicNode) //перехлест
					groupExtraInputNodes.getItem(i)->getInput(0).setSource( &groupExtraOutputNodes.getItem( logicNode->indexOf(out) )->getOutput(0) );
				else
					groupExtraInputNodes.getItem(i)->getInput(0).setSource(out);
			}
			//internal links
			int in_cntr = 0;
			InputNeurone *in_node = NULL;
			for(int j = 0; j < srcLogic->getNodesCount(); ++j) {
				InputNeurone *node = dynamic_cast <InputNeurone *> (&srcLogic->getNode(j));
				if(node == NULL) continue;
				if(in_cntr == i) {
					in_node = node;
					break;
				}
				in_cntr++;
			}
			if(in_node) {
				Output* out = &in_node->getOutput(0);
				for(int j = 0; j < nodes_to_clone.count(); ++j) {
					Neurone* nnn = nodes_to_clone.getItem(j);
					for(int k = 0; k < nnn->getInputsCount(); ++k) {
						Input* iii = &nnn->getInput(k);
						if(iii->getSource() == out) {
							group->getNode(j).getInput(k).setSource( &groupExtraInputNodes.getItem(i)->getOutput(0) );
						}
					}
				}
				int out_cntr = 0;
				for(int j = 0; j < srcLogic->getNodesCount(); ++j) {
					OutputNeurone *node = dynamic_cast <OutputNeurone *> (&srcLogic->getNode(j));
					if(node == NULL) continue;
					Input* iii = &node->getInput(0);
					if(iii->getSource() == out) {
						groupExtraOutputNodes.getItem(out_cntr)->getInput(0).setSource( &groupExtraInputNodes.getItem(i)->getOutput(0) );
					}
					out_cntr++;
				}
				
			}
		}
		
		//Find & connect external links to group outputs
		for(int i = 0; i < logicNode->getOutputsCount(); ++i) {
			//external link
			Output* out = &logicNode->getOutput(i);
			for(int j = 0; j < logic->getNodesCount(); ++j) {
				Neurone* nnn = &logic->getNode(j);
				if(nnn == logicNode) continue;
				for(int k = 0; k < nnn->getInputsCount(); ++k) {
					Input* iii = &nnn->getInput(k);
					if(iii->getSource() == out) {
						iii->setSource( &groupExtraOutputNodes.getItem(i)->getOutput(0) );
					}
				}
			}
//			if(out)
//				groupExtraInputNodes.getItem(i)->getInput(0).setSource(out);
			//internal links
			int out_cntr = 0;
			OutputNeurone *out_node = NULL;
			for(int j = 0; j < srcLogic->getNodesCount(); ++j) {
				OutputNeurone *node = dynamic_cast <OutputNeurone *> (&srcLogic->getNode(j));
				if(node == NULL) continue;
				if(out_cntr == i) {
					out_node = node;
					break;
				}
				out_cntr++;
			}
			if(out_node) {
				Input* inp = &out_node->getInput(0);
				Output* out = inp->getSource();
				if(out)
					for(int j = 0; j < nodes_to_clone.count(); ++j) {
						if(nodes_to_clone.getItem(j) == out->getInstance()) {
							int idx = out->getInstance()->indexOf(out);
							groupExtraOutputNodes.getItem(i)->getInput(0).setSource( &group->getNode(j).getOutput(idx) );
							break;
						}
					}
			}
			//connection
			
		}
		
		//Add extra nodes to group
		for(int i = 0; i < groupExtraActionNodes.count(); ++i) {
			group->addNode(groupExtraActionNodes.getItem(i));
		}
		for(int i = 0; i < groupExtraEventNodes.count(); ++i) {
			group->addNode(groupExtraEventNodes.getItem(i));
		}
		for(int i = 0; i < groupExtraInputNodes.count(); ++i) {
			group->addNode(groupExtraInputNodes.getItem(i));
		}
		for(int i = 0; i < groupExtraOutputNodes.count(); ++i) {
			group->addNode(groupExtraOutputNodes.getItem(i));
		}
		
		//remove logic node
		logic->removeNode(logicNode);
		selected = NULL;
	}
	
	void LogicEditor::mouseUp(int X, int Y, int b, bool ctrl) {
		if(logic == NULL) return;
		
		float x = camera->getCursorPos().x;
		float y = camera->getCursorPos().y;
		
		if (b == 2) {
			camera->mouseUp(x, y);
		}
		
		if(b != 0) return;
		
		if (edit_state == SELECT_RECT) {
			
			if (ctrl == false) {
				clearSelection();
			}
			
			if (logic != NULL) {
				for (int i=0; i<logic->getNodesCount(); i++) {
					BBox node_rect = BBox(
								vec3(logic->getNode(i).getPosX(), logic->getNode(i).getPosY(), -1.0f),
								vec3(logic->getNode(i).getPosX(), logic->getNode(i).getPosY(), +1.0f) +
								vec3(logic->getNode(i).getWidth(), logic->getNode(i).getHeight(), 0.0f)
								);
					
					BBox rect_(rect.min, rect.max);
					
					if (overlaps(rect_, node_rect)) {
						if (ctrl) {
							logic->getNode(i).setSelected(!logic->getNode(i).isSelected());
						} else {
							logic->getNode(i).setSelected(true);
							//if(selected == NULL) selectNode(&logic->getNode(i))
						}
					}
				}
				for (int i=0; i<logic->getNodeGroupCount(); i++) {
					BBox node_rect = BBox(
								vec3(logic->getNodeGroup(i).getPosX(), logic->getNodeGroup(i).getPosY(), -1.0f),
								vec3(logic->getNodeGroup(i).getPosX(), logic->getNodeGroup(i).getPosY(), +1.0f) +
								vec3(logic->getNodeGroup(i).getWidth(), logic->getNodeGroup(i).getHeight(), 0.0f)
								);
					
					BBox rect_(rect.min, rect.max);
					
					if (overlaps(rect_, node_rect)) {
						if (ctrl) {
							logic->getNodeGroup(i).setSelected(!logic->getNodeGroup(i).isSelected());
						} else {
							logic->getNodeGroup(i).setSelected(true);
						}
					}
				}
			}
			edit_state = SELECT;
		} else if (edit_state == MOVE) {
			edit_state = SELECT;
		}
		blockGroupSelection = false;
	}
	
	void LogicEditor::mouseMove(int X, int Y) {
		
		if(logic == NULL) return;
		
		if(camera->getScroll()) {
			pasteShift = 0;
		}
		
		camera->mouseMove(X, Y);
		
		float x = camera->getCursorPos().x;
		float y = camera->getCursorPos().y;
		
		free_point.x = x;
		free_point.y = y;
		
		picked_link = Link();
		
		if (edit_state == MOVE) {
			
			if (selected != NULL) {
				float pos_x = selected->getPosX();
				float pos_y = selected->getPosY();
				
				float dx = grid->snap(x - move.x) - pos_x;
				float dy = grid->snap(y - move.y) - pos_y;
				
				for (int i=0; i<logic->getNodesCount(); i++) {
					if (logic->getNode(i).isSelected() == false) continue;
					logic->getNode(i).setPosX(grid->snap(logic->getNode(i).getPosX() + dx));
					logic->getNode(i).setPosY(grid->snap(logic->getNode(i).getPosY() + dy));
				};
				/*
					for (int i=0; i<logic->getNodesCount(); i++) {
						if (logic->getNode(i).isSelected() == false) continue;
						
						
						logic->getNode(i).setPosX(grid->snap(logic->getNode(i).getPosX() + x - move.x));
						logic->getNode(i).setPosY(grid->snap(logic->getNode(i).getPosY() + y - move.y));
					}
					
					move.x += selected->getPosX() - pos_x;
					move.y += selected->getPosY() - pos_y;
					*/
			}
			
		} else if (edit_state == SELECT_RECT) {
			rect.max.x = x;
			rect.max.y = y;
			rect.max.z = 1.0f;
		} else {
			picked_link = selectLink(free_point);
			Neurone *hovered = NULL;
			for(int i=logic->getNodesCount() - 1; i>=0; i--) {
				Neurone *node = &logic->getNode(i);
				if(node->isSelected(x, y)) {
					hovered = node;
					break;
				}
			}
			hoverNode(hovered);
		}
		
	}
	
	void LogicEditor::mouseWheel(float delta, bool shift) {
		camera->zoomIn(0.2f * delta, shift);
	}
	
	//--------------------------------------------------------------------------
	
	void LogicEditor::selectNode(Neurone *node) {
		
		if (selected == node) return;
		//logic->bringToFront(node);
		//onSelectNode(node);
		selected = node;
		if(on_selection_changed != NULL) on_selection_changed->run();
	}
	
	void LogicEditor::hoverNode(Neurone *node) {
		if(hovered == node)	return;
		if(hovered != NULL) hovered->setHovered(false);
		hovered = node;
		if(hovered != NULL) hovered->setHovered(true);
	}
	
	void LogicEditor::runOnSelection()
	{
		if(on_selection_changed != NULL) on_selection_changed->run();
	}
	
	Neurone *LogicEditor::getSelectedNode() {
		if (logic == NULL) return NULL;
		if(logic->getSelectedGroup()) {
			return NULL;
		}
		return selected;
	}
	
	Neurone *LogicEditor::create(const char *name, const char *library, const char *instance) {
		if(logic == NULL) return NULL;
		//Neurone *neurone = logic->addNode(name);
		Neurone *neurone = logic->addNode(name, library, instance, true);
		place(neurone);
		return neurone;
	}
	
	void LogicEditor::place(Neurone *node) {
		
		if (logic == NULL) {
			qDebug() << __FUNCTION__ << "no logic";
			return;
		}
		if (node == NULL) {
			qDebug() << __FUNCTION__ << "no node";
			return;
		}
		
		node->setLogic(logic);
		
		clearSelection();
		selectNode(node);
		node->setSelected(true);
		edit_state = MOVE;
		move.x = node->getWidth() / 2.0f;
		move.y = node->getHeaderHeight() / 2.0f;

		float dx = grid->snap(camera->getCursorPos().x - move.x);
		float dy = grid->snap(camera->getCursorPos().y - move.y);
		
		node->setPosX(dx);
		node->setPosY(dy);
	}
	
	void LogicEditor::setLogic(Logic *logic_) {
		logic = logic_;
		reset();
	}
	
	Logic *LogicEditor::getLogic() {
		return logic;
	}
	
	void LogicEditor::nullLogic() {
		logic = NULL;
	}
	
	vec3 LogicEditor::getCursorPos()
	{
		return camera->getCursorPos();
	}
	
	float LogicEditor::getCameraPosX() {
		return camera->getPos().x;
	}
	
	float LogicEditor::getCameraPosY() {
		return camera->getPos().y;
	}
	
	float LogicEditor::getCameraZoom() {
		return camera->getZoom();
	}
	
	void LogicEditor::setupCamera(Renderer *renderer, float w, float h) {
		camera->setup(renderer, w, h)	;
	}
	
	void LogicEditor::setOnSelectionChanged(Common::Action *action) {
		delete on_selection_changed;
		on_selection_changed = action;
	}
	
	Common::Action *LogicEditor::getOnSelectionChanged() {
		return on_selection_changed;
	}
	
	//--------------------------------------------------------------------------
	
}
