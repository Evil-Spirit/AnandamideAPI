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

#include "AnandamideInput.h"

#include "AnandamideOutput.h"
#include "AnandamideNeurone.h"
#include "AnandamideLibrary.h"
#include "AnandamideLogic.h"
#include "AnandamideScript.h"

namespace Anandamide {
	
	void Input::setNeurone(Neurone *neurone) {
		this->neurone = neurone;
	}
	
	Input::Input(const Variable &default_value_) {
		default_value = default_value_;
		source = NULL;
	}
	
	const Variable &Input::getValue() const {
		if (source != NULL) return source->getValue();
		return default_value;
	}
	
	void Input::setValue(const Variable &default_value_) {
		default_value = default_value_;
	}
	
	const Output *Input::getSource() const {
		return source;
	}
	
	Output *Input::getSource() {
		return source;
	}
	
	void Input::setSource(Output *source_) {
		source = source_;
	}
	
	const char *Input::getName() const {
		if(neurone == NULL) return "";
		int i = neurone->indexOf(this);
		if(i < 0) return "";
		return neurone->getInputName(i);
	}
	
	Neurone *Input::getNeurone() {
		return neurone;
	}
	
	const Neurone *Input::getNeurone() const {
		return neurone;
	}
	
	const TypeDef *Input::getTypeDef() const {
		return neurone->getLogic()->getScript()->getLibraries()->getTypeDef(default_value.getType());
	}
	
	float Input::getPosX() const {
		return pos_x;
	}
	
	float Input::getPosY() const {
		return pos_y;
	}
	
	void Input::setPos(float x, float y) {
		pos_x = x;
		pos_y = y;
	}
	
	float Input::getDirX() const {
		return pos_x;
	}
	
	float Input::getDirY() const {
		return pos_y;
	}
	
	void Input::setDir(float x, float y) {
		dir_x = x;
		dir_y = y;
	}
	
	Variable &Input::getVariable() {
		return default_value;
	}
	
	Variable &Input::getDefaultValue() {
		return default_value;
	}
	
	const Variable &Input::getDefaultValue() const {
		return default_value;
	}
	
}
