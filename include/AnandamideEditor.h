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

#ifndef AnandamideEditorH
#define AnandamideEditorH

//------------------------------------------------------------------------------

#include "AnandamideLibAPI.h"
#include "AnandamideLogic.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	//--------------------------------------------------------------------------

	class Camera2d;
	class Grid;
	class Renderer;
	
	//--------------------------------------------------------------------------
	//
	// class LogicEditor
	//
	//--------------------------------------------------------------------------

	class ANANDAMIDE_API LogicEditor {

		enum {
			SELECT,
			MOVE,
			SELECT_RECT,
			LINK_EVENT_ACTION,
			LINK_OUTPUT_INPUT
		};

		Logic *logic;

		int edit_state;
		Vector2f move;
		vec3 free_point;
		BBox rect;

		Grid *grid;
		Camera2d *camera;
		Common::Action *on_selection_changed;
		Common::Action *on_link_create_block;
		bool blockGroupSelection;
		int pasteShift;
		bool setGroupFlag;
		bool editableFlag;

	protected:

		Neurone *link_event;
		Neurone *link_action;

		int link_event_index;
		int link_action_index;

		Neurone *link_input;
		Neurone *link_output;

		int link_input_index;
		int link_output_index;
		
		Link picked_link;
		Link selected_link;
		
		Neurone *selected;
		Neurone *hovered;
		NeuroneGroup *selectedGroup;

		virtual void onRemoveNode(Neurone *node);
		virtual void onSelectNode(Neurone *node);
		virtual void onLinkCreateBlock(float x, float y);

	public:
	
		LogicEditor();
		~LogicEditor();
		
		void reset();
		void render(Renderer *renderer, int w, int h, bool flag);
		
		//событие изменения логики сценария
		void onChangeLogic();
		//событие изменения месторасположения блоков
		void onChangeLocation();
		
		virtual void clear();
		void undoLinks();
		void createEventActionLinks();
		void createOutputInputLinks();
		
		virtual void removeNode(int index);
		virtual void removeNode(Neurone *node);
		bool removeSelected();
		bool cloneSelected();
		void copyAsXml(Str &xml_str);
		bool pasteAsXml(const Str &xml_str, int x = 0, int y = 0);
		void copyGroupContentsToLogic(NeuroneGroup* srcGroup, Logic* destLogic);
		void postPrepareLogic(Logic* destLogic);
		void createGroupFromLogic(Neurone* logicNode);
		
		void mouseDown(int X_, int Y_, int b, bool ctrl = false, bool blockLink = false);
		void mouseUp(int X, int Y, int b, bool ctrl = false);
		void mouseMove(int X, int Y);
		void mouseWheel(float delta, bool shift = false);
		
		void selectNode(Neurone *node);
		void hoverNode(Neurone *node);
		
		void runOnSelection();
		Neurone *getSelectedNode();
		void clearSelection();
		void focusNode(Neurone *node);
		
		Neurone *create(const char *name, const char *library = "", const char *instance = "");
		void place(Neurone *node);
		void setLogic(Logic *logic_);
		Logic *getLogic();
		void nullLogic();
		
		vec3 getCursorPos();
		float getCameraPosX();
		float getCameraPosY();
		float getCameraZoom();
		void setupCamera(Renderer *renderer, float width, float h);
		
		void setOnSelectionChanged(Common::Action *action);
		Common::Action *getOnSelectionChanged();
		
		Str getInfo(int x, int y);
		Link selectLink(const vec3 &point);
		Link& getPickedLink();
		void clearLinks();
		
		Neurone* getNodeByPos(int X, int Y);
		Action* getActionByPos(int X, int Y);
		
		bool canCreateNewGroup();
		bool createNewGroup();
		NeuroneGroup* getSelectedGroup();
		NeuroneGroup* getGroupOfSelectedNodes();
		bool removeSelectedFromGroup();
		
		NeuroneGroup* getGroupByPos(int X, int Y);
		bool removeGroup(NeuroneGroup* group);
		bool removeGroupWithNodes(NeuroneGroup* group);
		int getGroupsCount();
		bool addToGroupPrepare();
		void addToGroupCancel();
		bool isAddToGroupMode();
		bool addSelectedToGroup(NeuroneGroup* group);
		bool addSelectedToGroupByPos(int X, int Y);
		
		bool canPopupMenu();
		bool isSomethingSelected();
		int getSelectedGroupsCount();
		void makeLogicFromGroup(NeuroneGroup* group);
		bool canMakeGroupFromLogic();
		void makeGroupFromLogic(Neurone* node);
		
		vec3 getCameraPos() const;
		void setCameraPos(vec3 pos);
		
		void createEventLink(Neurone* node, int index);
		void createActionLink(Neurone* node, int index);
		void createOutputLink(Neurone* node, int index);
		void createInputLink(Neurone* node, int index);
		
		void setEditable(bool editable);
	};
}

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
