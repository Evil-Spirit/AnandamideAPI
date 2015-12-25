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

#include "AnandamideScript.h"
#include "AnandamideLogic.h"
#include "AnandamideLibrary.h"
#include "AnandamideStdLib.h"
#include "Xml.h"
#include <QDebug>
#include <QUuid>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QElapsedTimer>

namespace Anandamide {

	Script::Script() {
		user_data = NULL;
		main = NULL;
		loading = NULL;
		is_shutdown = false;
		shutdown_action = NULL;
		scriptCompileState = SCS_NOT_COMPILED;
		libraries = new Libraries();
		clear();
		generateId();
	}
	
	Script *Script::create() {
		return new Script();
	}
	
	Script *Script::clone() {
		Script* res = Script::create();
		res->setName(this->getName());
		Xml xml("script");
		writeXml(&xml, this->fileName.str());
		res->readXml(&xml, 1, this->fileName.str());
		return res;
	}
	
	void Script::destroy() {
		delete this;
	}
	
	Script::~Script() {
		logics.clear();
		delete main;
		main = NULL;
		delete libraries;
		libraries = NULL;
		delete shutdown_action;
		shutdown_action = NULL;
		//		TypeDef::clearDefImages();
	}
	
	void Script::generateId() {
		id.set(QUuid::createUuid().toString().toLocal8Bit().constData());
	}
	
	unsigned long long Script::getSize() {
		qDebug() << __FUNCTION__ << name;
		
		unsigned long long res = 0;
		res += sizeof(*this);
		
		for(int i=0; i<logics.count(); i++) {
			res += logics.getItem(i)->getSize();
		}
		if(main) {
			qDebug() << __FUNCTION__ << "*COMPILED MAIN";
			unsigned long long main_size = 0;
			main_size = main->getSize();
			qDebug() << __FUNCTION__ << "*COMPILED MAIN SIZE" << main_size;
			res += main_size;
		}
		
		res += name.size();
		res += id.size();
		res += fileName.size();
		
		unsigned long long libs_size = 0;
		if(libraries) libs_size += libraries->getSize();
		
		res += libs_size;
		
		qDebug() << __FUNCTION__ << name << "libs" << libs_size;
		
		qDebug() << __FUNCTION__ << name << "size" << res;

		//Libraries *libraries;
		
		return res;
	}
	
	const char *Script::getId() const {
		return id.str();
	}
	
	void Script::clear() {
		name = "";
		logics.clear();
		delete main;
		main = NULL;
		loading = NULL;
		libraries->clear();
		libraries->loadLibrary("AnandamideAPI");
	}
	
	void Script::newScript() {
		clear();
		Logic *main_def = new Logic(this);

		ActionNeurone *action = new ActionNeurone("entry", main_def);
		action->setPos(-240, -160);
		Neurone *shutdown = libraries->getLibrary("Std")->getDef("Shutdown")->createNeurone(libraries);
		shutdown->setPos(0, -144);
		action->getEvent("out")->addAction(shutdown->getAction("in"));

		main_def->addNode(action);
		main_def->addNode(shutdown);
		main_def->setName("Main");
		logics.append(main_def);
	}
	
	bool Script::compile() {
		Logic *base = findLogic("Main");
		if(base == NULL) {
			errorMessage("Compile failed : can not find \"Main\" node.");
			scriptCompileState = SCS_COMPILED_FAILED;
			return false;
		}
		delete main;
		main = (Logic *)base->clone(base);
		is_shutdown = false;
		main->afterCompile();
		scriptCompileState = SCS_COMPILED_OK;
		return true;
	}
	
	ScriptCompileState Script::getCompileState() const {return scriptCompileState;}
	
	bool Script::run() {
		if(main == NULL) {
			errorMessage("Run failed: script is not compiled.");
			return false;
		}

		Anandamide::Action *entry = main->getAction("entry");
		if(entry == NULL) {
			errorMessage("Run failed: script action \"entry\" not found.");
			return false;
		}

		entry->run();
		return true;
	}

	void Script::setShutdownAction(Action *action)
	{
		delete shutdown_action;
		shutdown_action = action;
	}
	
	void Script::shutdown() {
		warningMessage(format("Script \"%s\" is shutdown.", getName()));
		is_shutdown = true;
		if (shutdown_action) {
			shutdown_action->run();
		}
	}

	bool Script::isShutdown() const
	{
		return is_shutdown;
	}
	
	void Script::update(float dt) {
		if(main == NULL) {
			errorMessage("Run failed: script is not compiled.");
			return;
		}
		main->update(dt);
	}
	
	int Script::getLogicsCount() {
		return logics.count();
	}

	Logic *Script::getMain() {
		return main;
	}
	
	Logic *Script::getLogic(int i) {
		return &logics[i];
	}
	
	Logic *Script::addLogic(const char *name) {
		if(findLogic(name) != NULL) return NULL;
		Logic *logic = new Logic(this);
		logic->setName(name);
		logics.append(logic);
		return logic;
	}
	
	bool Script::addLogic(Logic *logic) {
		if(findLogic(logic->getName()) != NULL) return false;
		logics.append(logic);
		return true;
	}
	
	Logic *Script::findLogic(const char *name_str) {
		Str name = name_str;
		for(int i=0; i<logics.count(); i++) {
			if(name == logics[i].getName()) {
				return &logics[i];
			}
		}
		return NULL;
	}
	
	int Script::indexOfLogic(const char *name_str) {
		Str name = name_str;
		for(int i=0; i<logics.count(); i++) {
			if(name == logics[i].getName()) {
				return i;
			}
		}
		return -1;
	}
	
	void Script::removeLogic(Logic *logic) {
		logics.remove(logics.seek(logic));
	}
	
	Neurone *Script::createNeurone(const char *name, const char *library, bool dummy, Neurone *dummyNode) {
		Str lib = library;
		if(lib == "" || lib == "#script") {
			for(int i=0; i<logics.count(); i++) {
				if(Str(logics[i].getName()) == name) {
					if(dummy) return new LogicDummy(logics[i], NULL);
					return logics[i].clone(dummyNode);
				}
			}
		}
		return libraries->create(library, name, dummy, dummyNode);
	}
	
	static Str prepareFileName(const char *lib_file, const char *script_file) {
		QString sName = QString::fromLocal8Bit(lib_file);
		if(!sName.endsWith(".and")) return lib_file;
		if(QFileInfo(sName).isRelative()) {
			QDir scriptDir = QFileInfo(QString::fromLocal8Bit(script_file)).dir();
			QString absLibFile = scriptDir.absoluteFilePath(sName);
			QDir dir("./");
			QString new_name = dir.relativeFilePath(absLibFile);
			if(!QFileInfo(new_name).exists()) {
				absLibFile = dir.absoluteFilePath(sName);
				new_name = dir.relativeFilePath(absLibFile);
			}
			return new_name.toLocal8Bit().constData();
		}
		return lib_file;
	}
		
	bool Script::readXml(const Xml *xml, int version, const char *scriptFileName) {
		clear();
		
		Str newId;
		if(xml->getArg("id", newId)) {
			this->id = newId;
		}
		
		if(newId == Str()) {
			generateId();
		}
		
		const Xml *xml_libs = xml->getChild("libraries");
		if(xml_libs) {
			for(int i=0; i<xml_libs->getChildrenCount(); i++) {
				const Xml *xml_lib = xml_libs->getChild(i);
				Str name;
				xml_lib->getArg("name", name);
				if(name == "Std") continue;
				if(xml_lib->isArg("plugin")) {
					Str plugin;
					xml_lib->getArg("plugin", plugin);
					if(!libraries->loadLibraryFormApi(plugin)) {
						return false;
					}
				}
				else
				if(xml_lib->isArg("file")) {
					Str file;
					xml_lib->getArg("file", file);
					if(!libraries->loadLibrary(prepareFileName(file, scriptFileName))) {
						errorMessage(format("Can not load library from file \"%s\"", file.str()));
						return false;
					}
				}
			}
		}
		
		QElapsedTimer timer;
		timer.start();
				
		const Xml *xml_logics = xml->getChild("logics");
		
		if(xml_logics == NULL && version == 0) {
			xml_logics = xml;
		}
		
		if(xml_logics) {
			loading = xml_logics;
			for(int i=0; i<xml_logics->getChildrenCount(); i++) {
				const Xml *child = xml_logics->getChild(i);
				if(Str(child->getName()) != "logic") continue;
				Str name;
				child->getArg("name", name);
				if(findLogic(name) != NULL) {
					//warningMessage(format("Logic \"%s\" already exist.", name.str()));
					continue;
				}
				Logic *logic = new Logic(this);
				if(!logic->readXml(child, true)) {
					//return false;
				}
				logics.append(logic);
			}
			// resort
			for(int i=0; i<xml_logics->getChildrenCount(); i++) {
				const Xml *child = xml_logics->getChild(i);
				if(Str(child->getName()) != "logic") continue;
				Str name;
				child->getArg("name", name);
				int index = indexOfLogic(name);
				logics.exchange(i, index);
			}
		}
		
		loading = NULL;
		
		messageMessage(format("Script \"%s\" loaded in %.2lf ms.", scriptFileName, double(timer.nsecsElapsed()) / 1e6));
		return true;
	}
	
	bool Script::readXmlLibraries(const Xml *xml, int version, const char *scriptFileName)
	{
		clear();
		
		Str newId;
		if(xml->getArg("id", newId))
			this->id = newId;
		if(newId == Str())
			generateId();
		
		const Xml *xml_libs = xml->getChild("libraries");
		if(xml_libs) {
			for(int i=0; i<xml_libs->getChildrenCount(); i++) {
				const Xml *xml_lib = xml_libs->getChild(i);
				Str name;
				xml_lib->getArg("name", name);
				if(xml_lib->isArg("plugin")) {
					Str plugin;
					xml_lib->getArg("plugin", plugin);
					if(!libraries->loadLibraryFormApi(plugin)) {
						return false;
					}
				}
				else
				if(xml_lib->isArg("file")) {
					Str file;
					xml_lib->getArg("file", file);
					if(!libraries->loadLibrary(prepareFileName(file, scriptFileName))) {
						return false;
					}
				}
			}
		}
		
		return true;
	}
	
	bool Script::readXmlLogics(const Xml *xml, int version)
	{				
		const Xml *xml_logics = xml->getChild("logics");
		
		if(xml_logics == NULL && version == 0) {
			xml_logics = xml;
		}
		
		if(xml_logics) {
			loading = xml_logics;
			for(int i=0; i<xml_logics->getChildrenCount(); i++) {
				const Xml *child = xml_logics->getChild(i);
				if(Str(child->getName()) != "logic") continue;
				Str name;
				child->getArg("name", name);
				if(findLogic(name) != NULL) {
					//warningMessage(format("Logic \"%s\" already exist.", name.str()));
					continue;
				}
				Logic *logic = new Logic(this);
				if(!logic->readXml(child, true)) {
					//return false;
				}
				logics.append(logic);
			}
			// resort
			for(int i=0; i<xml_logics->getChildrenCount(); i++) {
				const Xml *child = xml_logics->getChild(i);
				if(Str(child->getName()) != "logic") continue;
				Str name;
				child->getArg("name", name);
				int index = indexOfLogic(name);
				logics.exchange(i, index);
			}
		}
		
		loading = NULL;
		return true;
	}
	
	void Script::preLoadLogic(const char *name_str) {
		if(loading == NULL) return;
		if(findLogic(name_str) != NULL) return;
		
		Str name = name_str;
		
		for(int i=0; i<loading->getChildrenCount(); i++) {
			const Xml *child = loading->getChild(i);
			if(Str(child->getName()) != "logic") continue;
			if(name != child->getArg("name")) continue;
			Logic *logic = new Logic(this);
			logic->readXml(child, true);
			logics.append(logic);
		}
	}
	
	void Script::writeXml(Xml *xml, const char *script_file) const {
		xml->setArg("id", id.str());
		Xml *xml_libs = xml->addChild("libraries");
		for(int i=0; i<libraries->getLibrariesCount(); i++) {
			const Library *library = libraries->getLibrary(i);
			Xml *xml_lib = xml_libs->addChild("library");
			xml_lib->setArg("name", library->getName());
			QString lib_file = QString::fromLocal8Bit(library->getFileName());
			if(script_file != NULL && lib_file.endsWith(".and")) {
				QFileInfo fi(lib_file);
				QDir script_dir(QFileInfo(QString::fromLocal8Bit(script_file)).dir());
				lib_file = script_dir.relativeFilePath(fi.absoluteFilePath());
			}
			xml_lib->setArg("file", lib_file.toLocal8Bit().constData());
			if(Str(library->getPluginName()) != Str()) {
				xml_lib->setArg("plugin", library->getPluginName());
			}
		}
		Xml *xml_logics = xml->addChild("logics");
		for(int i=0; i<logics.count(); i++) {
			Xml *child = xml_logics->addChild("logic");
			logics[i].writeXml(child);
		}
	}
	
	void Script::save(const char *filename) {
		Xml xml("script");
		if(Str(getName()) == Str()) {
			setName(QFileInfo(QString::fromLocal8Bit(filename)).baseName().toLocal8Bit().constData());
		}
		xml.setArg("name", this->name);
		xml.setArg("version", 1);
		writeXml(&xml, filename);
		xml.save(filename);
	}
	
	bool Script::load(const char *filename) {
		Xml xml("script");
		if(!xml.load(filename)) {
			errorMessage(format("Can not load script file \"%s\".", filename));
			return false;
		}
		this->fileName = Str(filename);
		int version = 0;
		xml.getArg("version", version);
		if(!readXml(&xml, version, filename)) return false;
		xml.getArg("name", this->name);
		if(name == "") {
			QFileInfo file_info(QString::fromLocal8Bit(filename));
			name = file_info.baseName().toLocal8Bit().constData();
			warningMessage(format("Don`t give up while loading name from \"%s\" file. New name will be \"%s\".", filename, name.str()));
		}
		return true;
	}
	
	bool Script::loadLibraries(const char *filename)
	{
		Xml xml("script");
		if(!xml.load(filename)) {
			errorMessage(format("Can not load script file \"%s\".", filename));
			return false;
		}
		int version = 0;
		xml.getArg("version", version);
		if(!readXmlLibraries(&xml, version, filename)) return false;
		xml.getArg("name", this->name);
		if(name == "") {
			QFileInfo file_info(QString::fromLocal8Bit(filename));
			name = file_info.baseName().toLocal8Bit().constData();
			warningMessage(format("Don`t give up while loading name from \"%s\" file. New name will be \"%s\".", filename, name.str()));
		}
		return true;
	}
	
	bool Script::loadLogics(const char *filename)
	{
		Xml xml("script");
		if(!xml.load(filename)) {
			errorMessage(format("Can not load script file \"%s\".", filename));
			return false;
		}
		int version = 0;
		xml.getArg("version", version);
		if(!readXmlLogics(&xml, version)) return false;
		xml.getArg("name", this->name);
		if(name == "") {
			QFileInfo file_info(QString::fromLocal8Bit(filename));
			name = file_info.baseName().toLocal8Bit().constData();
			warningMessage(format("Don`t give up while loading name from \"%s\" file. New name will be \"%s\".", filename, name.str()));
		}
		return true;
	}
	
	bool Script::loadScriptName(const char *filename)
	{
		Xml xml("script");
		if(!xml.load(filename)) {
			errorMessage(format("Can not load script file \"%s\".", filename));
			return false;
		}
		int version = 0;
		xml.getArg("version", version);
		xml.getArg("name", name);
		if(name == "") {
			QFileInfo file_info(QString::fromLocal8Bit(filename));
			name = file_info.baseName().toLocal8Bit().constData();
			warningMessage(format("Don`t give up while loading name from \"%s\" file. New name will be \"%s\".", filename, name.str()));
		}
		return true;
	}
	
	Libraries *Script::getLibraries() {
		return libraries;
	}
	
	const char *Script::getName() const {
		return name;
	}
	
	void Script::setName(const char *name) {
		this->name = name;
	}
	
	void Script::setUserData(void *data) {
		user_data = data;
	}
	
	void *Script::getUserData() const {
		return user_data;
	}
	
	void Script::renameNodes(const char *oldName, const char *newName) {
		for(int i = 0; i < logics.count(); ++i) {
			logics[i].renameNodes(oldName, newName);
		}
	}
	
	void Script::removeLibraryNodes(Library *lib) {
		for(int i = 0; i < logics.count(); ++i) {
			for(int j = 0; j < logics.getItem(i)->getNodesCount(); ++j) {
				Neurone* node = &logics.getItem(i)->getNode(j);
				if(node->getLibrary() != lib) continue;
				logics.getItem(i)->removeNode(j--);
			}
			for(int j = 0; j < logics.getItem(i)->getNodeGroupCount(); ++j) {
				NeuroneGroup* group = &logics.getItem(i)->getNodeGroup(j);
				if(group->getNodesCount() != 0) continue;
				logics.getItem(i)->removeGroup(j--);
			}
		}
	}

}
