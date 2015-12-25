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

#include "Anandamide.h"

#include "AnandamideScript.h"

//------------------------------------------------------------------------------

#include "QDebug.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	Messager default_messager;
	Messager *messager = &default_messager;
	
	Messager::~Messager() {
		
	}
	
	void Messager::message(const char *text) {
		qDebug() << text;
	}
	
	void Messager::warning(const char *text) {
		qDebug() << "Warning: " << text;
	}
	
	void Messager::error(const char *text) {
		qDebug() << "Error: " << text;
	}
	
	void errorMessage(const char *text) {
		qDebug() << text;
		if(messager != NULL) messager->error(text);
	}

	void warningMessage(const char *text) {
		qDebug() << text;
		if(messager != NULL) messager->warning(text);
	}
	
	void messageMessage(const char *text) {
		if(messager != NULL) messager->message(text);
	}
	
	void setMessager(Anandamide::Messager *m) {
		messager = m;
		if(messager == NULL) messager = &default_messager;
	}
	
	void errorPointerMessage(Input *input)
	{
		errorMessage(format("ERROR: input pointer is NULL (Script \"%s\", Logic \"%s\", Block \"%s\", Input \"%s\")", 
							input->getNeurone()->getLogic()->getScript()->getName(), 
							input->getNeurone()->getLogic()->getName(), 
							input->getNeurone()->getType(), 
							input->getName()));
	}
	
	
}
