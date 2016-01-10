//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// authors:		Alexey Egorov (FadeToBlack aka EvilSpirit)
// 				Zakhar Shelkovnikov
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

#include "AnandamideLibrary.h"

//------------------------------------------------------------------------------

#include "AnandamideLogic.h"
#include "AnandamideScript.h"

#include "AnandamideUi.h"

#include <QLibrary>
#include <QDebug>

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QImage>

//------------------------------------------------------------------------------


namespace Anandamide {
	
	FormApiLoadCallback locFormApiLoadCallback = 0;
	
	//--------------------------------------------------------------------------
	//
	// class Library
	//
	//--------------------------------------------------------------------------
	
	void Library::addTypeDefinition(TypeDef *def) {
		type_defs.append(new int(def->getTypeInfo().id()), def);
	}
	
	Library::Library() {
		external = false;
	}
	
	Library::~Library() {
		//qDebug() << __FUNCTION__ << getName();
	}
	
	unsigned long long Library::getSize() {
		unsigned long long res = 0;
		
		res += sizeof(Library);
		
		res += name.size();
		res += fileName.size();
		res += pluginName.size();
		res += sizeof(node_defs);
		res += sizeof(type_defs);
		
		for (int i=0; i<node_defs.count(); i++) {
			NeuroneDef* nd = &node_defs[i].getValue();
			if(nd) res += nd->getSize();
		}
		
		for (int i=0; i<type_defs.count(); i++) {
			TypeDef* nd = &type_defs[i].getValue();
			if(nd) res += nd->getSize();
		}
		
		return res;
	}
	
	const char *Library::getName() const {
		return name.str();
	}
	
	void Library::setName(const char *new_name) {
		name = Str(new_name);
	}
	
	const char *Library::getPluginName() const
	{
		return pluginName.str();
	}
	
	void Library::setPluginName(const char *new_plugin_name)
	{
		pluginName.set(new_plugin_name);
	}
	
	int Library::getDefsCount() const {
		return node_defs.count();
	}
	
	const NeuroneDef *Library::getDef(int i) const {
		return &node_defs[i].getValue();
	}
	
	const NeuroneDef *Library::getDef(const char *id) const {
		return node_defs.get(id);
	}
	
	bool Library::addDefinition(NeuroneDef *def) {
		if(Str(def->getName()) == "") {
			errorMessage(format("Can not add definition to the library \"%s\": definition has no name.", getName()));
			return false;
		}
		if(getDef(def->getName()) != NULL) {
			errorMessage(format("Can not add definition to the library \"%s\": definition of \"%s\" already exist.", getName(), def->getName()));
			return false;
		}
		def->library = this;
		node_defs.append(new Str(def->getName()), def);
		return true;
	}

	bool Library::addDefinition(const char *kind, NeuroneDef *def) {
		def->setKind(kind);
		return addDefinition(def);
	}
	
	bool Library::addDefinition(const char *kind, const char *header, NeuroneDef *def) {
		def->setKind(kind);
		def->setHeader(header);
		return addDefinition(def);
	}
	
	int Library::getTypeDefsCount() const {
		return type_defs.count();
	}
	
	const TypeDef *Library::getTypeDef(int i) const {
		return &type_defs[i].getValue();
	}
	
	const TypeDef *Library::getTypeDef(const TypeInfo &type) const {
		return type_defs.get(type.id());
	}
	
	const TypeDef *Library::getTypeDefById(int id) const {
		return type_defs.get(id);
	}
	
	const TypeDef *Library::getTypeDefByName(const char *name) const {
		for(int i=0; i<type_defs.count(); i++) {
			if(Str(type_defs[i].getValue().getTypeInfo().name()) == name) {
				return &type_defs[i].getValue();
			}	
		}
		return NULL;
	}
	
	const char *Library::getFileName() const {
		return "";
	}
	
	void Library::setFileName(const char *fileName) {}
	
	const char *Library::getLibFileName() const
	{
		return "";
	}
	
	//--------------------------------------------------------------------------
	//
	// class DynamicLibrary
	//
	//--------------------------------------------------------------------------
	
	class DynamicLibrary : public Library {
			
			typedef void (*DefineLibraryFunc)(Library *);
			
			QLibrary *dll;
			Str file_name;
			Str lib_file_name;
			
			DynamicLibrary() { }
			
		public:
			
			static DynamicLibrary *create(const char *file_name) {
				//qDebug() << __FUNCTION__ << file_name;
				
				QElapsedTimer timer;
				timer.start();
				QFileInfo file_info = QString::fromLocal8Bit(file_name);
				QString base_name = file_info.baseName();
				
				//qDebug() << "base_name" << base_name;
				
				/*QRegExp x86("*x86.dll"), x86d("*x86d.dll"), x64("*x64.dll"), x64d("*x64d.dll");
				x86.setPatternSyntax(QRegExp::Wildcard);
				x86d.setPatternSyntax(QRegExp::Wildcard);
				x64.setPatternSyntax(QRegExp::Wildcard);
				x64d.setPatternSyntax(QRegExp::Wildcard);*/
				
				base_name.replace("_x86d", "");
				base_name.replace("_x86", "");
				base_name.replace("_x64d", "");
				base_name.replace("_x64", "");
				QString new_name = base_name;
				
					#ifdef DEBUG
						//qDebug() << "THE x86d";
						#ifdef _WIN32
							new_name += "_x86d.dll";
						#else
							new_name += "_x86d.so";
						#endif
					#else
						#ifdef _WIN32
							new_name += "_x86.dll";
						#else
							new_name += "_x86.so";
						#endif
					#endif
						
				//qDebug() << "new_name" << new_name;
				
				//messageMessage(format("Loading \"%s\" library.", new_name.toLocal8Bit().constData()));
					
				if(!QFileInfo::exists(new_name)) {
					errorMessage(format("Can not load library from \"%s\" file : file not found.", new_name.toLocal8Bit().constData()));
					return NULL;
				}
				
				QLibrary *dll = new QLibrary(new_name);
				if(!dll->load()) {
					errorMessage(format("Error while loading library from \"%s\" : \"%s\".", new_name.toLocal8Bit().constData(), dll->errorString().toLocal8Bit().constData()));
					delete dll;
					return NULL;
				}
				
				DefineLibraryFunc defineLibrary = (DefineLibraryFunc) dll->resolve("anandamideDefineLibrary");
				
				if(defineLibrary == NULL) {
					errorMessage(format("Can not load library from \"%s\" file : function \"anandamideDefineLibrary(Anandamide::Library *)\" is missing.", new_name.toLocal8Bit().constData()));
					delete dll;
					return NULL;
				}
				
				DynamicLibrary *lib = new DynamicLibrary();
				
				lib->dll = dll;
				lib->file_name = base_name.toLocal8Bit().constData();
				
				/// \bug Что это за хрень?
				QString fn = QString::fromLocal8Bit(file_name);
				if(fn.indexOf(".dll", Qt::CaseInsensitive) != -1) {
					fn.remove(".dll", Qt::CaseInsensitive);
					int idx = fn.lastIndexOf('_');
					if(idx != -1) {
						fn = fn.left(idx);
					}
				}
				lib->lib_file_name.set(fn.toLocal8Bit().constData());

				defineLibrary(lib);
				messageMessage(format("Library \"%s\" loaded in %.2lf ms.", new_name.toLocal8Bit().constData(), double(timer.nsecsElapsed()) / 1e6));
				
				return lib;
			}
			
			~DynamicLibrary() {
				//qDebug() << __FUNCTION__ << getName();
				//qDebug() << __FUNCTION__ << dll->fileName();
				dll->unload();
				delete dll;
			}
			
			virtual const char *getFileName() const {
				return file_name;
			}
			
			virtual const char *getLibFileName() const {
				return lib_file_name;
			}
	};
	
	//--------------------------------------------------------------------------
	//
	// class ScriptLibrary
	//
	//--------------------------------------------------------------------------
	
	ScriptLibrary::ScriptLibrary() : Library() {
		script = Script::create();
	}
	
	ScriptLibrary::~ScriptLibrary() {
		delete script;
		//				script->destroy();
	}
	
	ScriptLibrary *ScriptLibrary::create(const char *file_name) {
		
		//messageMessage(format("Loading \"%s\" library.", file_name));
		//QElapsedTimer timer;
		//timer.start();
		
		ScriptLibrary *lib = new ScriptLibrary();
		if(!lib->script->loadScriptName(file_name)) {
			delete lib;
			return NULL;
		}
		
		if(!lib->script->load(file_name)) {
			delete lib;
			return NULL;
		}
		
		lib->setName(lib->script->getName());
		lib->file_name = file_name;

		for(int i=0; i<lib->script->getLogicsCount(); i++) {
			lib->addDefinition(ScriptLogicDef::create(lib->script->getLogic(i), lib->script->getName()));
		}
		
		//messageMessage(format("Library \"%s\" loaded in %.2lf ms.", file_name, double(timer.nsecsElapsed()) / 1e6));
		return lib;
	}

	ScriptLibrary *ScriptLibrary::create(Script *script)
	{
		ScriptLibrary *lib = new ScriptLibrary;
		lib->script = script;
		lib->setName(lib->script->getName());
		lib->setFileName("");
		for(int i=0; i<lib->script->getLogicsCount(); i++) {
			lib->addDefinition(ScriptLogicDef::create(lib->script->getLogic(i), lib->script->getName()));
		}

		return lib;
	}
	
	const char *ScriptLibrary::getFileName() const {
		return file_name;
	}
	
	void ScriptLibrary::setFileName(const char *fileName)
	{
		file_name.set(fileName);
	}
	
	const char *ScriptLibrary::getLibFileName() const
	{
		return file_name;
	}
	
	Script *ScriptLibrary::getScript()
	{
		return script;
	}
	
	ScriptLibrary::ScriptLogicDef::ScriptLogicDef(Logic *logic) {
		this->logic = logic;
		setName(logic->getName());
	}
	
	ScriptLibrary::ScriptLogicDef *ScriptLibrary::ScriptLogicDef::create(Logic *logic, const char *libName) {
		ScriptLogicDef* res = new ScriptLogicDef(logic);
		res->libName.set(libName);
		res->setKind(logic->getKind());
		return res;
	}
	
	Neurone *ScriptLibrary::ScriptLogicDef::createNeurone(Libraries *libraries, bool dummy, Neurone *dummyNode) const {
		if (dummy) {
			LogicDummy *res = new LogicDummy(*logic, this);
			//res->setLibNameForDummy(libName.str());
			return res;
		}
		Logic *result = logic->cloneLogic();
		result->setDummyNode(dummyNode);
		//result->setLibNameForDummy(libName.str());
		result->setDef(this);
		return result;
	}
	
	//--------------------------------------------------------------------------
	//
	// class Libraries
	//
	//--------------------------------------------------------------------------
	
	Libraries::~Libraries() {
		
	}
	
	unsigned long long Libraries::getSize() {
		unsigned long long res = 0;
		
		for(int i=0; i<getLibrariesCount(); i++) {
			Library *library = (Library*)getLibrary(i);
			res += library->getSize();
		}
		res += sizeof(libraries);
		
		return res;
	}
	
	int Libraries::getLibrariesCount() const {
		return libraries.count();
	}
	
	const Library *Libraries::getLibrary(int i) const {
		return &libraries[i].getValue().get();
	}

	const Library *Libraries::getLibraryByName(const char *name) const {
		const LibraryRef *res = libraries.get(name);
		if(res != NULL) return &res->get();
		return NULL;
	}

	const Library *Libraries::getLibraryByFileName(const char *file_name) const
	{
		for (int i = 0; i < getLibrariesCount(); ++ i) {
			if (Str(getLibrary(i)->getFileName()) == Str(file_name)) return getLibrary(i);
		}
		return NULL;
	}
	
	const char *Libraries::getLibraryName(int i) const {
		if(i < 0 || i >= getLibrariesCount()) return NULL;
		return libraries[i].getKey();
	}
	
	const Library *Libraries::getLibrary(const char *id) {
		return getLibraryByName(id);
	}
	
	bool Libraries::addLibrary(LibraryRef library) {
		if(!library.is()) return false;
		if(Str(library->getName()) == "") return false;
		if(libraries.get(library->getName()) != NULL) return false;
		libraries.append(new Str(library->getName()), new LibraryRef(library));
		return true;
	}
	
	Neurone *Libraries::create(const char *lib_name, const char *id, bool dummy, Neurone *dummyNode) {
		if(Str(lib_name) == "") return create(id, dummy, dummyNode);
		const Library *library = getLibraryByName(lib_name);
		if(library == NULL) {
			errorMessage(format("Can not find library \"%s\" with definition of \"%s\".", lib_name, id));
			return NULL;
		}
		
		const NeuroneDef *def = library->getDef(id);
		if(def == NULL) {
			errorMessage(format("Can not find definition of \"%s\" in library \"%s\".", id, lib_name));
			return NULL;
		}
		return def->createNeurone(this, dummy, dummyNode);
	}
	
	Neurone *Libraries::create(const char *id, bool dummy, Neurone *dummyNode) {
		for(int i=0; i<libraries.count(); i++) {
			const Library *library = getLibrary(i);
			const NeuroneDef *def = library->getDef(id);
			if(def == NULL) continue;
			return def->createNeurone(this, dummy, dummyNode);
		}
		errorMessage(format("Can not find definition of \"%s\".", id));
		return NULL;
	}
	
	const TypeDef *Libraries::getTypeDef(const TypeInfo &type) const {
		for(int i=0; i<getLibrariesCount(); i++) {
			const Library *library = getLibrary(i);
			const TypeDef *def = library->getTypeDef(type);
			if(def == NULL) continue;
			return def;
		}
		return NULL;
	}
	
	const TypeDef *Libraries::getTypeDefById(int id) const {
		for(int i=0; i<getLibrariesCount(); i++) {
			const Library *library = getLibrary(i);
			const TypeDef *def = library->getTypeDefById(id);
			if(def == NULL) continue;
			return def;
		}
		return NULL;
	}
	
	const TypeDef *Libraries::getTypeDefByName(const char *name) const {
		for(int i=0; i<getLibrariesCount(); i++) {
			const Library *library = getLibrary(i);
			const TypeDef *def = library->getTypeDefByName(name);
			if(def == NULL) continue;
			return def;
		}
		return NULL;
	}
	
	Variable Libraries::createVariableByType(int type_id) {	
		Variable var;
		const TypeDef *type_def = getTypeDefById(type_id);
		if(type_def != NULL) {
			var.setType(type_def->getTypeInfo());
		} else {
			var.setType(TYPEID(Undefined));
			errorMessage(format("Can not find type with id [%d].", type_id));
		}
		return var;
	}
	
	Variable Libraries::createVariableByTypeName(const char *name) {
		Variable var;
		const TypeDef *type_def = getTypeDefByName(name);
		if(type_def != NULL) {
			var.setType(type_def->getTypeInfo());
		} else {
			var.setType(TYPEID(Undefined));
			errorMessage(format("Can not find type with name [%s].", name));
		}
		return var;
	}
	
	//--------------------------------------------------------------------------
	//
	// class Libraries
	//
	//--------------------------------------------------------------------------
	
	bool Libraries::loadLibrary(const char *filename) {
		
		LibraryRef library;
		library.load(filename);
		
		if(!library.is()) return false;
		
		if(!addLibrary(library)) {
			errorMessage(format("Can not load library \"%s\" from file \"%s\".", library->getName(), library->getFileName()));
		}
		
		return true;
	}
	
	bool Libraries::loadLibraryFormApi(const char *pluginName) {
		if(locFormApiLoadCallback == 0) return false;
		locFormApiLoadCallback(QString::fromLocal8Bit(pluginName), this);
		return true;
	}
	
	bool Libraries::unloadLibrary(const char *name) {
		if(libraries.get(name) != NULL) {
			libraries.remove(name);
			return true;
		}
		return false;
	}
	
	bool Libraries::unloadLibrary(int index) {
		if(index < 0 || index >= getLibrariesCount()) return false;
		libraries.remove(index);
		return true;
	}
	
	void Libraries::clear() {
		libraries.clear();
	}
	
	bool Libraries::isLibraryLoaded(const char *fileName) {
		QString fn = QString::fromLocal8Bit(fileName);
		if(fn.indexOf(".dll", Qt::CaseInsensitive) != -1) {
			fn.remove(".dll", Qt::CaseInsensitive);
			int idx = fn.lastIndexOf('_');
			if(idx != -1) {
				fn = fn.left(idx);
			}
		}
		else return false;
		for(int i = 0; i < getLibrariesCount(); ++i) {
			if( Str(getLibrary(i)->getLibFileName()) == Str(fn.toLocal8Bit().constData()) ) return true;
		}
		return false;
	}
	
	void Libraries::setFormApiCallback(FormApiLoadCallback formApiLoadCallback) {
		locFormApiLoadCallback = formApiLoadCallback;
	}
	
	//--------------------------------------------------------------------------
	//
	// class TypeDef
	//
	//--------------------------------------------------------------------------
	
	TypeDef::~TypeDef() {
//		if(image)
		//			delete image;
	}
	
	unsigned long long TypeDef::getSize() {
		unsigned long long res = 0;
		
		if(image) res += image->byteCount();
		res += sizeof(*this);
		res += sizeof(def_images);
		
		foreach (QString key, def_images.keys()) {
			QImage* image = def_images.take(key);
			if(image) res += image->byteCount();
		}
				
		return res;
	}
	
	TypeDef *TypeDef::create() {
		return new TypeDef();	
	}
	
	void TypeDef::setColor(float r, float g, float b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
	
	void TypeDef::setImage(QImage *image, const QString &image_key) {
		if(image_key == QString() || image == NULL)
			return;
		TypeDef::addDefImage(image_key, image);
		this->image = TypeDef::getDefImage(image_key);
	}
	
	void TypeDef::setTypeInfo(const TypeInfo &type) {
		this->type = type;
	}
	
	float TypeDef::getR() const { return r; }
	float TypeDef::getG() const { return g; }
	float TypeDef::getB() const { return b; }
	const TypeInfo &TypeDef::getTypeInfo() const { return type; }
	const QImage *TypeDef::getImage() const { return image; }
	
	QMap <QString, QImage*> TypeDef::def_images;
	
	QImage *TypeDef::getDefImage(const QString &key)
	{
		if(!def_images.contains(key))
			return NULL;
		return def_images[key];
	}
	
	void TypeDef::addDefImage(const QString &key, QImage *image)
	{
		if(!def_images.contains(key))		//UBERFIX for compression
			def_images.insert(key, new QImage(*image));
		delete image;
	}
	
	void TypeDef::clearDefImages()
	{
		foreach (QString key, def_images.keys()) {
			QImage* image = def_images.take(key);
			if(image)
				delete image;
		}
	}
	
	
	Library *Library::loadResource(const char *file_name) {
		Str name_str = file_name;
		name_str.toLowerCase();
		name_str.del(0, name_str.size() - 4);

		Str name_ui = file_name;
		name_ui.toLowerCase();
		name_ui.del(0, name_ui.size() - 3);

		if(name_str == ".and") {
			return ScriptLibrary::create(file_name);
		} else if(name_ui == ".ui") {
			return QObjectLibrary::create(file_name);
		} else {
			return DynamicLibrary::create(file_name);
		}
	}
	
}

//------------------------------------------------------------------------------
