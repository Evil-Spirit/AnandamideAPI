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

#include "AnandamideAction.h"

#include "AnandamideEvent.h"
#include "AnandamideNeurone.h"

namespace Anandamide {

	Action::~Action() {
		while (events.count() > 0) {
			events[0].removeAction(this);
		}
		delete action;
	}
	
	Neurone *Action::getNeurone() {
		return neurone;
	}
	
	const Neurone *Action::getNeurone() const {
		return neurone;
	}
	
	void Action::run() {
		if(action == NULL) return;
		action->run();
	}
	
	void Action::setAction(Common::Action *action) {
		delete this->action;
		this->action = action;
	}
	
	const char *Action::getName() const {
		if(neurone == NULL) return "";
		int i = neurone->indexOf(this);
		if(i < 0) return "";
		return neurone->getActionName(i);
	}
	
	void Action::addEvent(Event *event) {
		if (events.seek(event) >= 0) return;
		events.append(event);
	}
	
	void Action::removeEvent(Event *event) {
		events.remove(event);
	}
	
	void Action::setNeurone(Neurone *neurone) {
		this->neurone = neurone;
	}
	
	Action::Action() : events(false) {
		action = NULL;
		neurone = NULL;
	}
	
}
