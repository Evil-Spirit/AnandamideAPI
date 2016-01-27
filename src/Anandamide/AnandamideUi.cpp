//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// author:		Georgy Kostarev
// mailto:		anandamide@mail.ru
// 				anandamide.script@gmail.com
//				kostarevgi@gmail.com
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

#include "AnandamideUi.h"
#include "AnandamideLogic.h"
#include "AnandamideScript.h"

#include <QWidget>
#include <QUiLoader>
#include <QFile>
#include <QMetaMethod>
#include <QDebug>
#include <QVariant>
#include <QByteArray>

#include <QBuffer>
#include <QRegExp>

#include <QEvent>

#include <QMetaEnum>
#include <QUuid>

#include <QAbstractScrollArea>

#define msgassert(expr, ...) if (!(expr)) { msgwarning(__VA_ARGS__); } assert(expr)
#define msgwarning(...) Anandamide::errorMessage(format(__VA_ARGS__)); qDebug(__VA_ARGS__);
#define str_from_ba(byte_array) Str(byte_array.constData())
#define str_from_int(Num) Str(QString::number( Num ).toLocal8Bit().constData())

DEFINE_TYPEID_OBJECT(QObject*, 1100124)
DEFINE_TYPEID_OBJECT(QWidget*, 1000000)
DEFINE_TYPEID_OBJECT(QEvent*,	1100130)

namespace Anandamide {

	//----------------------------------------------------------------------------------------------------------------------------
	//	Common
	//----------------------------------------------------------------------------------------------------------------------------

	Str get_arg_name(QMetaMethod &method, Str suffix, int index) {
		auto ba = method.parameterNames().at(index);
		if (ba.isDetached() || ba.isEmpty() || ba.isNull()) {
			return suffix + str_from_int(index);
		}
		return ba.constData();
	}

	template <int I>
	QGenericArgument get_generic_arg(Neurone *neurone, QMetaMethod &method) {
		void *value = neurone->getInput(get_arg_name(method, "arg_", I))->getValue().getDataPtr();
		return  QGenericArgument(method.parameterTypes().at(I).constData(), value);
	}

	template <int... Is>
	bool invoke_method(QObject *object, Neurone *neurone, QMetaMethod &method, QGenericReturnArgument &v_return) {
		return method.invoke(object, v_return, get_generic_arg <Is>(neurone, method)...);
	}

	QString eval_kind(QObject *object) {
		QObject *it = object;
		QString kind = it->objectName();
		while (it->parent()) {
			kind = it->parent()->objectName() + "/Childs/" + kind;
			it = it->parent();
		}
		return kind;
	}

	QString eval_kind(const QMetaObject *metaobject) {
		const QMetaObject *it = metaobject;
		QString kind = it->className();
		while (it->superClass()) {
			kind = it->superClass()->className() + QString("/Childs/") + kind;
			it = it->superClass();
		}
		return kind;
	}

	QString eval_name(QObject *object) {
		QObject *it = object;
		QString name = it->objectName();
		while (it->parent()) {
			name = it->parent()->objectName() + "->" + name;
			it = it->parent();
		}
		return name;
	}

	///
	/// @brief Класс, связывающий сигнал объекта с событием нейрона
	///

	class Connection : public QObject
	{

		Neurone *event;
		QMetaMethod method;
		QVector <QVariant> argv;

	public:
		Connection(QObject *object, const QMetaMethod &method, Neurone *neurone)
			: QObject(object), method(method), event(neurone)
		{
			bool connection = initConnection(object);
			msgassert(connection, "Can not init connection between object %s, method %s, neurone %s",\
						   object->metaObject()->className(), method.methodSignature().constData(), neurone->getType());
		}
		~Connection() {

		}

		QVector <QVariant> &getArgv() { return argv; }

		int qt_metacall(QMetaObject::Call metacall, int method_index, void **argv) {
			method_index = QObject::qt_metacall(metacall, method_index, argv);

			if (method_index < 0)
				return method_index;

			if (metacall == QMetaObject::InvokeMetaMethod) {
				if (method_index == 0) {
					parseArgs(argv);
					redirect();
				}
				--method_index;
			}
			return method_index;
		}

	private:
		bool initConnection(QObject *object) {
			static const int mem_offs = QObject::staticMetaObject.methodCount();
			if (!object) {
				msgwarning("Connection: Can not connect on a null object.");
				return false;
			}
			if (!method.isValid()) {
				msgwarning("Connection: Null signal name is not valid.");
				return false;
			}
			const int index = method.methodIndex();
			if (index < 0) {
				msgwarning("Connection: No such signal: \"%s\".", method.methodSignature().constData());
				return false;
			}

			if (!QMetaObject::connect(object, index, this, mem_offs, Qt::DirectConnection, 0)) {
				msgwarning("Connection: QMetaObject::connect returned false. Unable to connect.");
				return false;
			}
			return true;
		}

		void parseArgs(void **argv) {
			this->argv.clear();
			this->argv.reserve(method.parameterCount());
			for (int i = 0; i < method.parameterCount(); ++i) {
				const QMetaType::Type type = static_cast<QMetaType::Type>(method.parameterType(i));
				if (type == QMetaType::QVariant)
					this->argv << *reinterpret_cast<QVariant*>(argv[i + 1]);
				else
					this->argv << QVariant(type, argv[i + 1]);
			}
		}

		void redirect() {
			event->getEvent("triggered")->run();
		}

	};

	///
	/// @brief Контейнер значения типа QVariant
	///

	class TypeContainerVariant : public TypeContainer
	{
			QVariant variant;
			void *value;
		public:

			TypeContainerVariant(const QVariant &v) : variant(v) {
				createVoid();
			}

			~TypeContainerVariant() {
				destroy();
			}

			virtual void *getValue() {
				return value;
			}

			virtual TypeContainer *clone() const {
				return new TypeContainerVariant(variant);
			}

			virtual bool isGreater(const TypeContainer *o) const {
				auto v = dynamic_cast <const decltype(this)>(o);
				if (!v) return false;
				return (variant > v->variant);
			}

			virtual bool isEqual(const TypeContainer *o) const {
				auto v = dynamic_cast <const decltype(this)>(o);
				if (!v) return false;
				return (variant == v->variant);
			}

			virtual void toString(Str &str) const {
				variant.toString();
				str.set(variant.toString().toLocal8Bit().constData());
			}

			virtual void fromString(const Str &str) {
				destroy();
				int type= variant.type();
				variant = QString(str.str());
				if (!variant.convert(type)) {
					Anandamide::warningMessage(format("Can not convert \"%s\" value to type <%s>.", str.str(), variant.typeName()));
				}
				createVoid();
			}

			QVariant prepare(const TypeContainer *o) const {
				Str o_value;
				o->toString(o_value);
				QVariant o_variant = QString(o_value.str());
				if (!o_variant.convert(variant.type())) return QVariant();
				return o_variant;
			}

			virtual TypeContainer *add(const TypeContainer *o) const {
				ANANDAMIDE_UNUSED(o);
				return NULL;
			}
			virtual TypeContainer *sub(const TypeContainer *o) const {
				ANANDAMIDE_UNUSED(o);
				return NULL;
			}
			virtual TypeContainer *mul(const TypeContainer *o) const {
				ANANDAMIDE_UNUSED(o);
				return NULL;
			}
			virtual TypeContainer *div(const TypeContainer *o) const {
				ANANDAMIDE_UNUSED(o);
				return NULL;
			}

			virtual void create() {
				value = NULL;
			}
			virtual void destroy() {
				QMetaType(variant.type()).destroy(value);
			}
		private:
			void createVoid() {
				value = QMetaType(variant.type()).create(variant.type(), variant.constData());
			}
	};

	///
	/// @brief Helper class to create "instance" input, if needed
	///

	template <class T>
	struct create_instance_input {
		inline create_instance_input(Libraries *libraries, T *neurone);
	};

	//----------------------------------------------------------------------------------------------------------------------------
	//	Neurones
	//----------------------------------------------------------------------------------------------------------------------------

	class ObjectDefBase;

	///
	/// @brief The ObjectNeuroneBase class
	///

	class ObjectNeuroneBase : public Neurone
	{
	public:
		ObjectNeuroneBase() : Neurone() {}
		virtual ~ObjectNeuroneBase() {}
		bool hasCaption() { return true; }
		Str onGetCaption() const;
		QObject *getObject() const;
		const ObjectDefBase *getDef() const;
	};

	///
	/// @brief The ObjectNeurone class
	///

	class ObjectNeurone : public ObjectNeuroneBase
    {

    public:

        void onGetObject() {
			if (qobject_cast <QWidget*>(getObject()))
				getOutput("this")->setValue(Variable(qobject_cast <QWidget*>(getObject())));
			else
				getOutput("this")->setValue(Variable(getObject()));
        }

        void onInit(Libraries *libraries) {
			ANANDAMIDE_UNUSED(libraries);
			if (qobject_cast <QWidget*>(getObject()))
				addOutput("this", new Output(this, Variable(qobject_cast <QWidget*>(getObject()))));
			else
				addOutput("this", new Output(this, Variable(getObject())));
        }

	};

	///
	/// @brief The MethodNeurone class
	///

	class MethodNeurone : public ObjectNeuroneBase
    {
        QMetaMethod method;

		friend struct create_instance_input <MethodNeurone>;

	public:
		MethodNeurone(QMetaMethod method) : ObjectNeuroneBase(), method(method) {
            addAction("in", new Action());
            addEvent("out", new Event());
        }

        void onInit(Libraries *libraries) {

			create_instance_input <MethodNeurone>(libraries, this);

			getAction("in")->setAction(Common::Action::create(this, &MethodNeurone::onRun));
            for (int i = 0; i < method.parameterCount(); ++i) {
				addInput(get_arg_name(method, "arg_", i), new Input(libraries->createVariableByTypeName(method.parameterTypes().at(i).constData())));
            }
			if (method.returnType() != qMetaTypeId <void>()) {
				addOutput("result", new Output(this, libraries->createVariableByTypeName(method.typeName())));
			}
        }

		void onRun() {
			if (getObject() == nullptr)
				return warningMessage(format("Input pointer is NULL. Script: %s, Logic: %s, Neurone: %s, Pos: (%d, %d).",\
									  getLogic()->getScript()->getName(), getLogic()->getName(), getType(), getPosX(), getPosY()));
            bool result = false;
			void *v_ret = QMetaType(method.returnType()).create();
			auto v_return = QGenericReturnArgument(method.typeName(), v_ret);
			switch (method.parameterCount()) {
				case 0: result = method.invoke(getObject(), v_return); break;
				case 1: result = invoke_method <0>(getObject(), this, method, v_return); break;
				case 2: result = invoke_method <0, 1>(getObject(), this, method, v_return); break;
				case 3: result = invoke_method <0, 1, 2>(getObject(), this, method, v_return); break;
				case 4: result = invoke_method <0, 1, 2, 3>(getObject(), this, method, v_return); break;
				case 5: result = invoke_method <0, 1, 2, 3, 4>(getObject(), this, method, v_return); break;
				case 6: result = invoke_method <0, 1, 2, 3, 4, 5>(getObject(), this, method, v_return); break;
				case 7: result = invoke_method <0, 1, 2, 3, 4, 5, 6>(getObject(), this, method, v_return); break;
				case 8: result = invoke_method <0, 1, 2, 3, 4, 5, 6, 7>(getObject(), this, method, v_return); break;
				case 9: result = invoke_method <0, 1, 2, 3, 4, 5, 6, 7, 8>(getObject(), this, method, v_return); break;
			}
			msgassert(result == true, "An error occured while trying to invoke method %s::%s", getObject()->metaObject()->className(), method.methodSignature().constData());
			if (method.returnType() != qMetaTypeId <void>()) {
				getOutput("result")->getVariable().setUnify(getOutput("result")->getVariable().getType(), new TypeContainerVariant(QVariant(method.returnType(), v_ret)));
				//make it clean!
				QMetaType(method.returnType()).destroy(v_ret);
			}
            getEvent("out")->run();
        }
    };

	///
	/// @brief The SignalNeurone class
	///

	class SignalNeurone : public ObjectNeuroneBase
    {
        Connection *connection;
        QMetaMethod signal;
    public:
		SignalNeurone(const QMetaMethod &signal) : ObjectNeuroneBase(), signal(signal), connection(nullptr) {
			addEvent("triggered", new Event());
        }
        ~SignalNeurone() {
				delete connection;
        }

        void afterCompile() {
			delete connection;
			connection = new Connection(getObject(), signal, this);
        }

        void onGetValue() {
			if (connection == nullptr) return;
            QVector <QVariant> &argv = connection->getArgv();
            for (int i = 0; i < argv.count(); ++i) {
				Output *output = getOutput(get_arg_name(signal, "ret_", i));
				output->getVariable().setUnify(output->getVariable().getType(), new TypeContainerVariant(argv.at(i)));
            }
        }

        void onInit(Libraries *libraries) {
            for (int i = 0; i < signal.parameterCount(); ++i) {
				Str name = get_arg_name(signal, "ret_", i);
				Variable var =  libraries->createVariableByTypeName(signal.parameterTypes().at(i).constData());
                addOutput (name, new Output(this, var));
                getOutput(name)->setOnGetValueAction(Common::Action::create(this, &SignalNeurone::onGetValue));
            }

        }
    };

	///
	/// @brief The PropertyNeurone class
	///

	class PropertyNeurone : public ObjectNeuroneBase
	{
		QMetaProperty property;
		bool rw;

		friend struct create_instance_input <PropertyNeurone>;

	public:
		PropertyNeurone(const QMetaProperty &property, bool rw) : ObjectNeuroneBase(), property(property), rw(rw) {
			if (!rw) return;
			addAction("set", new Action());
			getAction("set")->setAction(Common::Action::create(this, &PropertyNeurone::onSetValue));
			addEvent("out", new Event());
		}
		~PropertyNeurone() {
		}

		void onGetValue() {
			QVariant v = getObject()->property(property.name());
			getOutput("get")->getVariable().setUnify(getOutput("get")->getVariable().getType(), new TypeContainerVariant(v));
		}

		void onSetValue() {
			QVariant v;
			//v.create(property.type(), getInput("set")->getVariable().getDataPtr());
			getObject()->setProperty(property.name(), v);
			getEvent("out")->run();
		}

		void onInit(Libraries *libraries) {

			create_instance_input <PropertyNeurone>(libraries, this);

			Variable in = libraries->createVariableByTypeName(Str(property.typeName()));
			Variable out = in;

			if (rw) { addInput("set", new Input(in)); }

			addOutput("get", new Output(this, out));
			getOutput("get")->setOnGetValueAction(Common::Action::create(this, &PropertyNeurone::onGetValue));

		}

	};

	//----------------------------------------------------------------------------------------------------------------------------
	//	Definitions
	//----------------------------------------------------------------------------------------------------------------------------

	///
	/// @brief The ObjectDefBase class
	///

	class ObjectDefBase : public NeuroneDef {

		QObject *instance;
		const QMetaObject *metaobject;

	public:

		~ObjectDefBase() {}

		QObject *getInstance() const { return instance; }
		const QMetaObject *getMetaObject() const { return metaobject; }

		bool hasInstance() const { return (instance != nullptr); }

	protected:

		ObjectDefBase() : NeuroneDef(), instance(nullptr), metaobject(nullptr) {}

		void fromInstance(QObject *instance) {
			this->instance = instance;
			this->metaobject = instance->metaObject();
			setup();
		}

		void fromMetaObject(const QMetaObject *metaobject) {
			this->metaobject = metaobject;
			setup();
		}

	private:

		void setup() {
			setName(name().toLocal8Bit().constData());
			setKind(kind().toLocal8Bit().constData());
			setHeader(header().toLocal8Bit().constData());
		}

		virtual QString name() const { return "#Undefined"; }
		virtual QString kind() const { return "#Undefined"; }
		virtual QString header() const { return "#Undefined"; }

	};

	Str ObjectNeuroneBase::onGetCaption() const
	{
		if (getDef()->hasInstance()) return getObject()->objectName().toLocal8Bit().constData();
		return getDef()->getMetaObject()->className();
	}

	QObject *ObjectNeuroneBase::getObject() const
	{
		if (getDef()->hasInstance()) return getDef()->getInstance();
		return getInput("instance")->getValue().toType <QObject*>();
	}

	const ObjectDefBase *ObjectNeuroneBase::getDef() const
	{
		return static_cast <const ObjectDefBase *>(Neurone::getDef());
	}

	template <class T>
	create_instance_input <T>::create_instance_input(Libraries *libraries, T *neurone)
	{
		if (neurone->getDef()->hasInstance()) return;
		QString name1 = QString(neurone->getDef()->getMetaObject()->className()) + "*";
		QString name2 = QString(neurone->getDef()->getMetaObject()->className()) + " *";
		auto v = libraries->createVariableByTypeName(name1.toLocal8Bit().constData());
		if (v.getType() == TYPEID(Undefined))
			v = libraries->createVariableByTypeName(name2.toLocal8Bit().constData());
		neurone->addInput("instance", new Input(v));
	}


	///
	/// @brief The ObjectDef class
	///

	class ObjectDef : public ObjectDefBase
    {

    public:

		static ObjectDef *create(QObject *object) {
			auto self = new ObjectDef;
			self->fromInstance(object);
			return self;
		}

		Neurone *createNeurone(Libraries *libraries, bool dummy = false, Neurone* dummyNode = NULL) const override {
			ANANDAMIDE_UNUSED(dummy);
			ANANDAMIDE_UNUSED(dummyNode);
			auto neurone = new ObjectNeurone;
            neurone->create(libraries, this);
            return neurone;
        }

	private:

		virtual QString name() const { return eval_name(getInstance()); }
		virtual QString kind() const { return eval_kind(getInstance()); }
		virtual QString header() const { return getMetaObject()->className(); }

    };

	///
	/// @brief The MethodDef class
	///

	class MethodDef : public ObjectDefBase
    {
        QMetaMethod method;

		MethodDef(const QMetaMethod &method) : ObjectDefBase(), method(method) {}

    public:

		static MethodDef *create(QObject *object, const QMetaMethod &method) {
			auto self = new MethodDef(method);
			self->fromInstance(object);
			return self;
        }

		static MethodDef *create(const QMetaObject *metaobject, const QMetaMethod &method) {
			auto self = new MethodDef(method);
			self->fromMetaObject(metaobject);
			return self;
		}

		Neurone *createNeurone(Libraries *libraries, bool dummy = false, Neurone* dummyNode = NULL) const override {

			ANANDAMIDE_UNUSED(dummy);
			ANANDAMIDE_UNUSED(dummyNode);

            Neurone *neurone = nullptr;

            switch (method.methodType()) {
				case QMetaMethod::Method:       neurone = new MethodNeurone(method);    break;
				case QMetaMethod::Signal:       neurone = new SignalNeurone(method);    break;
				case QMetaMethod::Slot:         neurone = new MethodNeurone(method);    break;
				case QMetaMethod::Constructor:  neurone = new MethodNeurone(method);    break;
            }

			if (!neurone) return neurone;

            neurone->create(libraries, this);
            return neurone;
        }

	private:

		virtual QString name() const override {
			if (hasInstance()) return eval_name(getInstance()) + "->" + QString(method.methodSignature());
			return getMetaObject()->className() + QString("::") + method.methodSignature();
		}

		virtual QString kind() const override {
			QString k;
			switch (method.methodType()) {
				case QMetaMethod::Method:       k = "/Methods";      break;
				case QMetaMethod::Signal:       k = "/Signals";      break;
				case QMetaMethod::Slot:         k = "/Slots";        break;
				case QMetaMethod::Constructor:  k = "/Constructors"; break;
			}
			if (hasInstance()) return eval_kind(getInstance()) + k;
			return "InheritedClasses/" + (getMetaObject()->className() + k);
		}

		virtual QString header() const override {
			return method.name();
		}

    };


	///
	/// @brief The PropertyDef class
	///

	class PropertyDef : public ObjectDefBase
	{
		QMetaProperty property;
		bool rw;

		PropertyDef(const QMetaProperty &property, bool rw) : ObjectDefBase(), property(property), rw(rw) {}

	public:
		static PropertyDef *create(QObject *object, const QMetaProperty &property, bool rw) {
			auto self = new PropertyDef(property, rw);
			self->fromInstance(object);
			return self;
		}

		static PropertyDef *create(const QMetaObject *metaobject, const QMetaProperty &property, bool rw) {
			auto self = new PropertyDef(property, rw);
			self->fromMetaObject(metaobject);
			return self;
		}

		Neurone *createNeurone(Libraries *libraries, bool dummy = false, Neurone* dummyNode = NULL) const override {

			ANANDAMIDE_UNUSED(dummy);
			ANANDAMIDE_UNUSED(dummyNode);

			Neurone *neurone = new PropertyNeurone(property, rw);
			neurone->create(libraries, this);
			return neurone;
		}

	private:

		virtual QString name() const override {
			if (hasInstance()) return eval_name(getInstance()) + QString("->") + property.name();
			return QString(getMetaObject()->className()) + "::" + QString(property.name());
		}

		virtual QString kind() const override {
			if (hasInstance()) return eval_kind(getInstance()) + "/Properties";
			return "InheritedClasses/" + (getMetaObject()->className() + QString("/Properties"));
		}

		virtual QString header() const override {
			return "@" + QString(property.name());
		}

	};

	//----------------------------------------------------------------------------------------------------------------------------
	//	Event system
	//----------------------------------------------------------------------------------------------------------------------------

	namespace EventSystem {

		class Event;

		///
		/// @brief The Gazer class
		///

		class Gazer : public QObject {

			Event *neurone;
			QEvent::Type type;
			bool removed;

		public:
			Gazer(QObject *object, Event *neurone, QEvent::Type type) : neurone(neurone), type(type) {
				object->installEventFilter(this);
				removed = false;
				setParent(object);
			}

			void notify() {
				removed = true;
				parent()->removeEventFilter(this);
			}

		private:

			bool eventFilter(QObject *, QEvent *ev);

		};

		///
		/// @brief The Event class
		///

		class Event : public ObjectNeuroneBase {

			Gazer *gazer;
			QObject *viewport;
			QEvent::Type type;

		public:
			Event(QObject *viewport, QEvent::Type type) : ObjectNeuroneBase(), type(type), viewport(viewport), gazer(nullptr) {}

			~Event() {
				if (gazer) {
					gazer->notify();
					gazer->deleteLater();
				}
			}

			void afterCompile() {
				if (gazer) delete gazer;
				gazer = new Gazer(getViewport(), this, type);
			}

			virtual void setEvent(QEvent *ev) {
				getOutput("event")->setValue(Variable(ev));
			}

			virtual void trigger() {
				getEvent("triggered")->run();
			}

			virtual bool accepted() {
				return getInput("accepted")->getValue().getBool();
			}

		private:

			QObject *getViewport() {
				if (!viewport)
					return getObject();
				return viewport;
			}

		};

		bool Gazer::eventFilter(QObject *, QEvent *ev)
		{
			if (removed) return true;
			if (ev->type() != type) return false;
			neurone->setEvent(ev);
			neurone->trigger();
			if (removed) return true;
			return neurone->accepted();
		}

		#define EVENT_ENUM QEvent::staticMetaObject.enumerator(QEvent::staticMetaObject.indexOfEnumerator("Type"))

		///
		/// @brief The EventDef class
		///

		class EventDef : public ObjectDefBase {

			QObject *viewport;
			QEvent::Type type;

			EventDef(QObject *viewport, const QEvent::Type &type) : viewport(viewport), type(type), ObjectDefBase()
			{
				addEvent("triggered");
				addInput <bool>("accepted", "false");
				addOutput <QEvent*>("event", "0");
			}

		public:

			static EventDef *create(QObject *object, QObject *viewport, const QEvent::Type &type) {
				auto self = new EventDef(viewport, type);
				self->fromInstance(object);
				return self;
			}

			Neurone *createNeurone(Libraries *libraries, bool dummy = false, Neurone *dummyNode = NULL) const override {

				ANANDAMIDE_UNUSED(dummy);
				ANANDAMIDE_UNUSED(dummyNode);

				auto neurone = new Event(viewport, type);
				neurone->create(libraries, this);
				return neurone;
			}

		private:

			QString name() const override {
				return eval_name(getInstance()) + "->" + EVENT_ENUM.valueToKey(type);
			}

			QString kind() const override {
				return eval_kind(getInstance()) + "/Events";
			}

			QString header() const override {
				return "@" + QString(EVENT_ENUM.valueToKey(type));
			}

		};

	}

	//----------------------------------------------------------------------------------------------------------------------------
	//	Enums
	//----------------------------------------------------------------------------------------------------------------------------

	///
	/// @brief The Enumerator class
	///

	class Enumerator : public Enum {

		QMetaEnum enumerator;

	public:

		Enumerator(const QMetaEnum &enumerator) : Enum(), enumerator(enumerator) {}
		virtual ~Enumerator() {}

		virtual int getItemsCount() const {
			return enumerator.keyCount();
		}

		virtual const char *getItemName(int i) const {
			return enumerator.key(i);
		}

		virtual int getItemId(int i) const {
			return enumerator.value(i);
		}

		virtual const char *getItemNameById(int id) const {
			return enumerator.valueToKey(id);
		}

		virtual const int *getItemIdByName(const char *name) const {
			return new int(enumerator.keyToValue(name));
		}
	};

	TypeContainer *dummyCreator() { return nullptr; }

	TypeInfo createEnumTypeInfo(const QMetaEnum &en, const QString &name, const int &type_index) {
		char *name_v = new char[name.size() + 1];
		strcpy(name_v, name.toLocal8Bit().constData());
		return TypeInfo(type_index, name_v, new Enumerator(en), &dummyCreator);
	}

	//----------------------------------------------------------------------------------------------------------------------------
	//	Parser
	//----------------------------------------------------------------------------------------------------------------------------

	#define CREATE_DEF(CREATE_FUNCTION) \
		auto definition = CREATE_FUNCTION; \
		if (!library->getDef(definition->getName())) \
			library->addDefinition(definition); \
		else \
			delete definition

	void parseObject(Library *library, QObject *object) {

		library->addDefinition(ObjectDef::create(object));

		QWidget *w = qobject_cast <QWidget*>(object);

		if (w) {
			w->setMouseTracking(true);
			//QTreeWidget, QListWidget, and other scroll areas
			QObject *viewport = nullptr;
			auto sa = qobject_cast <QAbstractScrollArea*>(w);
			if (sa) viewport = sa->viewport();

			for (int i = 0; i < EVENT_ENUM.keyCount(); ++ i) {
				CREATE_DEF(EventSystem::EventDef::create(object, viewport, QEvent::Type(EVENT_ENUM.value(i))));
			}
		}

		for (int j = 0; j < object->metaObject()->methodCount(); ++j) {
			CREATE_DEF(MethodDef::create(object, object->metaObject()->method(j)));
		}

		for (int j = 0; j < object->metaObject()->propertyCount(); ++j) {
			CREATE_DEF(PropertyDef::create(object, object->metaObject()->property(j), object->metaObject()->property(j).isWritable()));
		}

		const QObjectList &objects = object->children();
		for (QObject *object : objects) {
			if (object->objectName().isEmpty()) continue;
			parseObject(library, object);
		}
	}

	void parseMetaObject(Library *library, const QMetaObject *metaobject) {
		for (int i = 0; i < metaobject->methodCount(); ++ i) {
			if (metaobject->method(i).methodType() == QMetaMethod::Signal) continue;
			CREATE_DEF(MethodDef::create(metaobject, metaobject->method(i)));
		}
		for (int i = 0; i < metaobject->propertyCount(); ++ i) {
			CREATE_DEF(PropertyDef::create(metaobject, metaobject->property(i), metaobject->property(i).isWritable()));
		}
	}

	#undef CREATE_DEF
	#undef EVENT_ENUM

	//----------------------------------------------------------------------------------------------------------------------------
	//	Library class
	//----------------------------------------------------------------------------------------------------------------------------

	QObjectLibrary::QObjectLibrary(QObject *object, const Str &filename) : object(object), filename(filename)
    {
		Str name = object->objectName().toLocal8Bit().constData();
		setName(name);
		parseObject(this, object);
	}

	QObjectLibrary *QObjectLibrary::create(const char *file_name)
    {
		QIODevice *device = nullptr;
		QUiLoader loader;
		QObject *object = nullptr;

		QFile uiform_file(file_name);
		bool opened = uiform_file.open(QIODevice::ReadOnly);
		if (!opened) {
			msgwarning("An error occured while trying to load Ui library from file: %s (Cannot open file)", file_name);
			return nullptr;
		}
		device = &uiform_file;
		object = loader.load(device);
		device->close();

		if (!object) {
			msgwarning("An error occured while trying to load widget from .ui file: %s (QUiLoader can not load ui)", file_name);
            return nullptr;
        }

		return new QObjectLibrary(object, file_name);
	}

	QObjectLibrary *QObjectLibrary::create(QObject *object)
	{
		return new QObjectLibrary(object, Str());
	}

	void QObjectLibrary::parseDefs(Library *library, QObject *object)
	{
		parseObject(library, object);
	}

	QObjectLibrary::~QObjectLibrary()
    {
		delete object;
    }

	const char *QObjectLibrary::getFileName() const
    {
		return filename;
    }

	void QObjectLibrary::setFileName(const char *filename)
    {
		this->filename = filename;
    }

	const char *QObjectLibrary::getLibFileName() const
    {
		return filename;
	}

	void QObjectLibrary::parseDefs(Library *library, const QMetaObject *metaobject)
	{
		parseMetaObject(library, metaobject);
	}

}

#undef msgassert
#undef msgwarning
#undef str_from_ba
#undef str_from_int

