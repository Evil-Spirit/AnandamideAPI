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

#include "AnandamideOutput.h"
#include "AnandamideNeurone.h"
#include "AnandamideLibrary.h"
#include "AnandamideLogic.h"
#include "AnandamideScript.h"

namespace Anandamide {
	
	Output::Output(Neurone *instance_) {
		instance = instance_;
		on_get_value = NULL;
	}
	
	Output::Output(Neurone *instance_, const Variable &value_) {
		instance = instance_;
		value = value_;
		on_get_value = NULL;
	}
	
	Output::~Output() {
		delete on_get_value;
	}
	
	const Variable &Output::getValue() const {
		if (on_get_value != NULL) on_get_value->run();
		return value;
	}
	
	void Output::setValue(const Variable &value_) {
		value = value_;
	}
	
	Neurone *Output::getInstance() {
		return instance;
	}
	
	const Neurone *Output::getInstance() const {
		return instance;
	}
	
	Variable &Output::getVariable() {
		return value;
	}
	
	void Output::setOnGetValueAction(Common::Action *action) {
		on_get_value = action;
	}
	
	Common::Action *Output::getOnGetValueAction() {
		return on_get_value;
	}
	
	const char *Output::getName() const {
		if(instance == NULL) return "";
		int i = instance->indexOf(this);
		if(i < 0) return "";
		return instance->getOutputName(i);
	}
	
	const TypeDef *Output::getTypeDef() const {
		return instance->getLogic()->getScript()->getLibraries()->getTypeDef(value.getType());
	}
	
}
