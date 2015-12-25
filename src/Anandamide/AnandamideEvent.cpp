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

#include "AnandamideEvent.h"

#include "AnandamideAction.h"
#include "AnandamideNeurone.h"

namespace Anandamide {
	
	void Event::setNeurone(Neurone *neurone) {
		this->neurone = neurone;
	}
	
	Event::Event() : actions(false) {
		neurone = NULL;
	}
	
	Event::~Event() {
		for (int i=0; i<actions.count(); i++) {
			actions[i].removeEvent(this);
		}
	}
	
	void Event::run() {
		for (int i=0; i<actions.count(); i++) {
			actions[i].run();
		}
	}
	
	void Event::addAction(Action *action) {
		
		if(actions.seek(action) >= 0) return;
		
		action->addEvent(this);
		actions.append(action);
	}
	
	void Event::removeAction(int index) {
		actions[index].removeEvent(this);
		actions.remove(index);
	}
	
	void Event::removeAction(Action *action) {
		int index = actions.seek(action);
		if (index < 0) return;
		removeAction(index);
	}
	
	int Event::getActionsCount() const {
		return actions.count();
	}
	
	Action &Event::getAction(int i) {
		return actions[i];
	}
	
	const Action &Event::getAction(int i) const {
		return actions[i];
	}
	
	Neurone *Event::getNeurone() {
		return neurone;
	}
	
	const Neurone *Event::getNeurone() const {
		return neurone;
	}
	
	const char *Event::getName() const {
		if(neurone == NULL) return "";
		int i = neurone->indexOf(this);
		if(i < 0) return "";
		return neurone->getEventName(i);
	}
	
}
