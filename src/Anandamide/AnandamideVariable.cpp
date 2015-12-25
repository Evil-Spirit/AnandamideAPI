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

#include "AnandamideVariable.h"

//------------------------------------------------------------------------------

#include "MathCore.h"
#include "Anandamide.h"
#include "Action.h"

//------------------------------------------------------------------------------

namespace Anandamide {
	
	void Variable::clear() {
		i = 0;
		f = 0.0f;
		s = "";
		unify = NULL;
		on_change = NULL;
	}
	
	void Variable::reset() {
		delete unify;
		unify = NULL;
	}
	
	void Variable::notifyChanged() {
		if(on_change != NULL) on_change->run();
	}
	
	Variable::Variable() {
		clear();
		setInt(0);
	}
	
	Variable::Variable(int value) {
		clear();
		setInt(value);
	}
	
	Variable::Variable(bool value) {
		clear();
		setBool(value);
	}
	
	Variable::Variable(float value) {
		clear();
		setFloat(value);
	}
	
	Variable::Variable(double value) {
		clear();
		setDouble(value);
	}
	
	Variable::Variable(const char *value) {
		clear();
		setStr(value);
	}
	
	Variable::Variable(const Str &value) {
		clear();
		setStr(value);
	}
	
	Variable::Variable(const Variable &o) {
		clear();
		*this = o;
	}
	
	Variable &Variable::operator=(const Variable &o) {
		
		i = o.i;
		f = o.f;
		s = o.s;
		type = o.type;
		
		reset();
		
		if(o.unify != NULL) unify = o.unify->clone();
		
		notifyChanged();
		return *this;
	}
	
	Variable::~Variable() {
		delete unify;
	}
	
	const Variable &Variable::get() const {
		return *this;
	}
	
	void Variable::set(const Variable &v) {
		*this = v;
	}
	
	bool Variable::getBool() const {
		
		switch(type.id()) {
			case CASEID(const char *)	: return s == "true";
			
			case CASEID(float)			: 
			case CASEID(double)			: return f != 0.0;
			
			case CASEID(int)			: 
			case CASEID(bool)			: return i != 0;
		}
		
		if(isEnum()) return i != 0;
		if(unify != NULL) return *((int *)unify->getValue()) != 0;
		
		errorMessage(format("Getting bool from variable of type \"%s\".", type.name()));
		return false;
	}
	
	void Variable::setBool(bool value) {
		reset();
		i = (value) ? 1 : 0;
		type = TYPEID(bool);
		notifyChanged();
	}
	
	int Variable::getInt() const {
		switch(type.id()) {
			case CASEID(const char *)	: return Nutmeg::strToInt(s);
			
			case CASEID(float)			: 
			case CASEID(double)			: return int(f);
			
			case CASEID(int)			: 
			case CASEID(bool)			: return i;
			
			case CASEID(TypeInfo)		: return toType <TypeInfo>().id();
		}
		
		if(isEnum()) return i;
		if (unify != NULL) return *((int *)unify->getValue());
		
		errorMessage(format("Getting int from unify variable of type \"%s\".", type.name()));
		return 0;
	}
	
	void Variable::setInt(int value) {
		reset();
		i = value;
		type = TYPEID(int);
		notifyChanged();
	}
	
	float Variable::getFloat() const {
		switch(type.id()) {
			case CASEID(const char *)	: return Nutmeg::strToFloat(s);
			
			case CASEID(float)			: 
			case CASEID(double)			: return float(f);
			
			case CASEID(int)			: 
			case CASEID(bool)			: return float(i);
		}
		
		if(isEnum()) return float(i);
		
		errorMessage(format("Getting float from unify variable of type \"%s\".", type.name()));
		return 0;
	}
	
	void Variable::setFloat(float value) {
		reset();
		f = value;
		type = TYPEID(float);
		notifyChanged();
	}
	
	double Variable::getDouble() const {
		
		switch(type.id()) {
			case CASEID(const char *)	: return Nutmeg::strToFloat(s);
			
			case CASEID(float)			: 
			case CASEID(double)			: return f;
			
			case CASEID(int)			: 
			case CASEID(bool)			: return i;
		}
		
		if(isEnum()) return i;
		
		errorMessage(format("Getting double from unify variable of type \"%s\".", type.name()));
		return 0;
	}
	
	void Variable::setDouble(double value) {
		reset();
		f = value;
		type = TYPEID(double);
		notifyChanged();
	}
	
	const char *Variable::getStr() const {
		
		switch(type.id()) {
			case CASEID(const char *)	: return s;
			case CASEID(int)			: return Nutmeg::intToStr(i);
			case CASEID(float)			: return Nutmeg::floatToStr(float(f));
			case CASEID(double)			: return Nutmeg::doubleToStr(float(f));
			case CASEID(bool)			: return (i != 0) ? "true" : "false";
		}
		
		if (isEnum()) {
			const char *str = type.getEnum()->getItemNameById(i);
			if(str == NULL) return "{X}";
			return str;
		}
		
		if (unify != NULL) {
			static Str str;
			unify->toString(str);
			return str;
		}
		
		return "{0}";
	}
	
	void Variable::setStr(const char *value) {
		reset();
		s = value;
		type = TYPEID(const char *);
		notifyChanged();
	}
	
	void Variable::setFromStr(const char *value) {
		
		switch(type.id()) {
			case CASEID(const char *)	: setStr(value);						return;
			case CASEID(int)			: setInt(Nutmeg::strToInt(value));		return;
			case CASEID(float)			: setFloat(Nutmeg::strToFloat(value));	return;
			case CASEID(double)			: setDouble(Nutmeg::strToDouble(value));return;
			case CASEID(bool)			: setBool(Str(value) == "true");		return;
		}
		
		if (isEnum()) {
			const int *id = type.getEnum()->getItemIdByName(value);
			if(id == NULL) {
				i = type.getEnum()->getItemId(0);
			} else {
				i = *id;
			}
			notifyChanged();
		} else 
		if(unify != NULL) {
			unify->fromString(value);
			notifyChanged();
		}
		
	}
	
	void *Variable::getDataPtr() const {
	
		switch(type.id()) {
			case CASEID(const char *)	: return (void *)s.str();
			
			case CASEID(float)			:
			case CASEID(double)			: return (void *)&f;
			
			case CASEID(int)			:
			case CASEID(bool)			: return (void *)&i;
		}
	
		if(isEnum()) return (void *)&i;
		if(unify != NULL) return unify->getValue();
		
		errorMessage(format("getDataPtr(): Can not get pointer from variable of type \"%s\".", type.name()));
		return NULL;
	}
	
	void Variable::setEnum(TypeInfo ti, int value) {
		reset();
		type = ti;
		i = value;
		notifyChanged();
	}
	
	void Variable::setEnum(int value) {
		if(type.getEnum() == NULL) {
			errorMessage(format("%s: Can not set enum to variable of type \"%s\".", __FUNCTION__, type.name()));
			return;
		}
		i = value;
		if(i < 0) i = 0;
		if(i >= type.getEnum()->getItemsCount()) i = type.getEnum()->getItemsCount() - 1;
		notifyChanged();
	}
	
	bool Variable::isEnum() const {
		return type.getEnum() != NULL;
	}

	void Variable::setUnify(const TypeInfo &type_info, TypeContainer *unify) {
		reset();
		type = type_info;
		if (type.getEnum()) {
			setEnum(type, *static_cast <int*>(unify->getValue()));
			delete unify;
			return;
		}
		switch (type.id()) {
			case CASEID(bool):			setBool(*static_cast <bool*>(unify->getValue()));		delete unify; return;
			case CASEID(int):			setInt(*static_cast <int*>(unify->getValue()));			delete unify; return;
			case CASEID(float):			setFloat(*static_cast <float*>(unify->getValue()));		delete unify; return;
			case CASEID(double):		setDouble(*static_cast <double*>(unify->getValue()));	delete unify; return;
			case CASEID(const char *):	Str ss; unify->toString(ss); setStr(ss.str());			delete unify; return;
		}

		this->unify = unify;
	}

	void Variable::setUnify(void *value, const TypeInfo &type_info, Deleter *deleter) {

		reset();
		type = type_info;

		switch (type_info.id()) {
		case CASEID(bool):			i = *static_cast <bool*>(value);					break;
		case CASEID(int):			i = *static_cast <int*>(value);						break;
		case CASEID(float):			f = *static_cast <float*>(value);					break;
		case CASEID(double):		f = *static_cast <double*>(value);					break;
		case CASEID(const char*):	s = static_cast <const char*>(value);				break;
		default:					unify = new TypeContainerVoid(value, deleter);		break;
		}
		notifyChanged();
	}

	TypeInfo Variable::getType() const {
		return type;
	}

	const char *Variable::getTypeName() const {
		return type.name();
	}

	void Variable::setType(const TypeInfo &new_type) {
		if(new_type == type) return;
		reset();
		type = new_type;
		unify = type.newDefaultValue();
		if(type.getEnum() != NULL) i = type.getEnum()->getItemId(0);
		notifyChanged();
	}

	Common::Action *Variable::getOnChangeAction() {
		return on_change;
	}

	void Variable::setOnChangeAction(Common::Action *action) {
		delete on_change;
		on_change = action;
	}

	bool Variable::operator==(const Variable &o) const {
		switch(type.id()) {
		case CASEID(const char *)	: return s == o.getStr();
		case CASEID(int)			: return i == o.getInt();
			case CASEID(float)			: return float(f) == o.getFloat();
			case CASEID(double)			: return f == o.getDouble();
			case CASEID(bool)			: return (i != 0) == o.getBool();
		}
		
		if (isEnum()) return i == o.getInt();
		
		if(unify == NULL) {
			errorMessage("Variable operator == : value is not assigned.");
			return false;
		}
		
		if(o.unify != NULL) return unify->isEqual(o.unify);
		return *((int *)unify->getValue()) == o.getInt();
	}
	
	bool Variable::operator>(const Variable &o) const {
		
		switch(type.id()) {
			case CASEID(const char *)	: return s > o.getStr();
			case CASEID(int)			: return i > o.getInt();
			case CASEID(float)			: return float(f) > o.getFloat();
			case CASEID(double)			: return f > o.getDouble();
			case CASEID(bool)			: return (i != 0) && !o.getBool();
		}
		
		if (isEnum()) return i > o.getInt();
		if(unify != NULL && o.unify != NULL) return unify->isGreater(o.unify);
		
		errorMessage("Variable operator > : value is not assigned.");
		return false;
	}
	
	Variable Variable::operator+(const Variable &o) const {
		if (type == TYPEID(const char *)) {
			return Variable(s + o.getStr());
		} else
			if (type == TYPEID(int)) {
				if (o.type == TYPEID(const char *)) {
					return Variable(i + o.getInt());
				} else
					if (o.type  == TYPEID(int)) {
						return Variable(i + o.getInt());
					} else
						if (o.type  == TYPEID(float)) {
							return Variable(i + o.getFloat());
						} else
							if (o.type  == TYPEID(double)) {
								return Variable(i + o.getDouble());
							} else
								if (o.type  == TYPEID(bool)) {
									return Variable((i != 0) || o.getBool());
								} else
									if (o.isEnum()) {
										return Variable(i + o.getInt());
									}
			} else
				if (type == TYPEID(float)) {
					if (o.type == TYPEID(const char *)) {
						return Variable(float(f) + o.getFloat());
					} else
						if (o.type  == TYPEID(int)) {
							return Variable(float(f) + o.getInt());
						} else
							if (o.type  == TYPEID(float)) {
								return Variable(float(f) + o.getFloat());
							} else
								if (o.type  == TYPEID(double)) {
									return Variable(float(f) + o.getDouble());
								} else
									if (o.type  == TYPEID(bool)) {
										return Variable((f != 0.0) || o.getBool());
									} else
										if (o.isEnum()) {
											return Variable(float(f) + o.getInt());
										}
				} else
					if (type == TYPEID(double)) {
						if (o.type == TYPEID(const char *)) {
							return Variable(f + o.getDouble());
						} else
							if (o.type  == TYPEID(int)) {
								return Variable(f + o.getInt());
							} else
								if (o.type  == TYPEID(float)) {
									return Variable(f + o.getFloat());
								} else
									if (o.type  == TYPEID(double)) {
										return Variable(f + o.getDouble());
									} else
										if (o.type  == TYPEID(bool)) {
											return Variable((f != 0.0) || o.getBool());
										} else
											if (o.isEnum()) {
												return Variable(f + o.getInt());
											}
					} else
						if (type == TYPEID(bool)) {
							if (o.type == TYPEID(const char *)) {
								return Variable(i != 0 || Str(o.getStr()) == "true");
							} else
								if (o.type  == TYPEID(int)) {
									return Variable(i != 0 || o.getInt() != 0);
								} else
									if (o.type  == TYPEID(float)) {
										return Variable(i != 0 || o.getFloat() != 0.0f);
									} else
										if (o.type  == TYPEID(double)) {
											return Variable(i != 0 || o.getDouble() != 0.0);
										} else
											if (o.type  == TYPEID(bool)) {
												return Variable(i != 0 || o.getBool());
											} else
												if (o.isEnum()) {
													return Variable(i != 0 || o.getInt() != 0);
												}
						} else
							if (isEnum()) {
								if (o.type == TYPEID(const char *)) {
									return Variable(i + o.getInt());
								} else
									if (o.type  == TYPEID(int)) {
										return Variable(i + o.getInt());
									} else
										if (o.type  == TYPEID(float)) {
											return Variable(i + o.getFloat());
										} else
											if (o.type  == TYPEID(double)) {
												return Variable(i + o.getDouble());
											} else
												if (o.type  == TYPEID(bool)) {
													return Variable(i != 0.0 || o.getBool());
												} else
													if (o.isEnum()) {
														return Variable(i + o.getInt());
													}
							} else if(unify != NULL && o.unify != NULL) {
								Variable v;
								v.setType(type);
								v.unify = unify->add(o.unify);
								return v;
							}
		return Variable();
	}
	
	Variable Variable::operator-(const Variable &o) const {
		if (type == TYPEID(const char *)) {
			errorMessage(format("Operator \"%s\" - \"%s\" is not defined.", type.name(), o.type.name()));
		} else
			if (type == TYPEID(int)) {
				if (o.type == TYPEID(const char *)) {
					errorMessage(format("Operator \"%s\" - \"%s\" is not defined.", type.name(), o.type.name()));
				} else
					if (o.type  == TYPEID(int)) {
						return Variable(i - o.getInt());
					} else
						if (o.type  == TYPEID(float)) {
							return Variable(i - o.getFloat());
						} else
							if (o.type  == TYPEID(double)) {
								return Variable(i - o.getDouble());
							} else
								if (o.isEnum()) {
									return Variable(i - o.getInt());
								}
			} else
				if (type == TYPEID(float)) {
					if (o.type == TYPEID(const char *)) {
						errorMessage(format("Operator \"%s\" - \"%s\" is not defined.", type.name(), o.type.name()));
					} else
						if (o.type  == TYPEID(int)) {
							return Variable(float(f) - o.getInt());
						} else
							if (o.type  == TYPEID(float)) {
								return Variable(float(f) - o.getFloat());
							} else
								if (o.type  == TYPEID(double)) {
									return Variable(float(f) - o.getDouble());
								} else
									if (o.isEnum()) {
										return Variable(float(f) - o.getInt());
									}
				} else
					if (type == TYPEID(double)) {
						if (o.type == TYPEID(const char *)) {
							errorMessage(format("Operator \"%s\" - \"%s\" is not defined.", type.name(), o.type.name()));
						} else
							if (o.type  == TYPEID(int)) {
								return Variable(f - o.getInt());
							} else
								if (o.type  == TYPEID(float)) {
									return Variable(f - o.getFloat());
								} else
									if (o.type  == TYPEID(double)) {
										return Variable(f - o.getDouble());
									} else
										if (o.isEnum()) {
											return Variable(f - o.getInt());
										}
					} else
						if (isEnum()) {
							if (o.type == TYPEID(const char *)) {
								errorMessage(format("Operator \"%s\" - \"%s\" is not defined.", type.name(), o.type.name()));
							} else
								if (o.type  == TYPEID(int)) {
									return Variable(i - o.getInt());
								} else
									if (o.type  == TYPEID(float)) {
										return Variable(i - o.getFloat());
									} else
										if (o.type  == TYPEID(double)) {
											return Variable(i - o.getDouble());
										} else
											if (o.isEnum()) {
												return Variable(i - o.getInt());
											}
						} else if(unify != NULL && o.unify != NULL) {
							Variable v;
							v.setType(type);
							v.unify = unify->sub(o.unify);
							return v;
						}
		return Variable();
	}
	
	Variable Variable::operator*(const Variable &o) const {
		if (type == TYPEID(const char *)) {
			errorMessage(format("Operator \"%s\" * \"%s\" is not defined.", type.name(), o.type.name()));
		} else
			if (type == TYPEID(int)) {
				if (o.type == TYPEID(const char *)) {
					errorMessage(format("Operator \"%s\" * \"%s\" is not defined.", type.name(), o.type.name()));
				} else
					if (o.type  == TYPEID(int)) {
						return Variable(i * o.getInt());
					} else
						if (o.type  == TYPEID(float)) {
							return Variable(i * o.getFloat());
						} else
							if (o.type  == TYPEID(double)) {
								return Variable(i * o.getDouble());
							} else
								if (o.isEnum()) {
									return Variable(i * o.getInt());
								}
			} else
				if (type == TYPEID(float)) {
					if (o.type == TYPEID(const char *)) {
						errorMessage(format("Operator \"%s\" * \"%s\" is not defined.", type.name(), o.type.name()));
					} else
						if (o.type  == TYPEID(int)) {
							return Variable(float(f) * o.getInt());
						} else
							if (o.type  == TYPEID(float)) {
								return Variable(float(f) * o.getFloat());
							} else
								if (o.type  == TYPEID(double)) {
									return Variable(float(f) * o.getDouble());
								} else
									if (o.isEnum()) {
										return Variable(float(f) * o.getInt());
									}
				} else
					if (type == TYPEID(double)) {
						if (o.type == TYPEID(const char *)) {
							errorMessage(format("Operator \"%s\" * \"%s\" is not defined.", type.name(), o.type.name()));
						} else
							if (o.type  == TYPEID(int)) {
								return Variable(f * o.getInt());
							} else
								if (o.type  == TYPEID(float)) {
									return Variable(f * o.getFloat());
								} else
									if (o.type  == TYPEID(double)) {
										return Variable(f * o.getDouble());
									} else
										if (o.isEnum()) {
											return Variable(f * o.getInt());
										}
					} else
						if (isEnum()) {
							if (o.type == TYPEID(const char *)) {
								errorMessage(format("Operator \"%s\" * \"%s\" is not defined.", type.name(), o.type.name()));
							} else
								if (o.type  == TYPEID(int)) {
									return Variable(i * o.getInt());
								} else
									if (o.type  == TYPEID(float)) {
										return Variable(i * o.getFloat());
									} else
										if (o.type  == TYPEID(double)) {
											return Variable(i * o.getDouble());
										} else
											if (o.isEnum()) {
												return Variable(i * o.getInt());
											}
						} else if(unify != NULL && o.unify != NULL) {
							Variable v;
							v.setType(type);
							v.unify = unify->mul(o.unify);
							return v;
						}
		return Variable();
	}
	
	Variable Variable::operator/(const Variable &o) const {
		if (type == TYPEID(const char *)) {
			errorMessage(format("Operator \"%s\" / \"%s\" is not defined.", type.name(), o.type.name()));
		} else
			if (type == TYPEID(int)) {
				if (o.type == TYPEID(const char *)) {
					errorMessage(format("Operator \"%s\" / \"%s\" is not defined.", type.name(), o.type.name()));
				} else
					if (o.type  == TYPEID(int)) {
						return Variable(i / o.getInt());
					} else
						if (o.type  == TYPEID(float)) {
							return Variable(i / o.getFloat());
						} else
							if (o.type  == TYPEID(double)) {
								return Variable(i / o.getDouble());
							} else
								if (o.isEnum()) {
									return Variable(i / o.getInt());
								}
			} else
				if (type == TYPEID(float)) {
					if (o.type == TYPEID(const char *)) {
						errorMessage(format("Operator \"%s\" / \"%s\" is not defined.", type.name(), o.type.name()));
					} else
						if (o.type  == TYPEID(int)) {
							return Variable(float(f) / o.getInt());
						} else
							if (o.type  == TYPEID(float)) {
								return Variable(float(f) / o.getFloat());
							} else
								if (o.type  == TYPEID(double)) {
									return Variable(float(f) / o.getDouble());
								} else
									if (o.isEnum()) {
										return Variable(float(f) / o.getInt());
									}
				} else
					if (type == TYPEID(double)) {
						if (o.type == TYPEID(const char *)) {
							errorMessage(format("Operator \"%s\" / \"%s\" is not defined.", type.name(), o.type.name()));
						} else
							if (o.type  == TYPEID(int)) {
								return Variable(f / o.getInt());
							} else
								if (o.type  == TYPEID(float)) {
									return Variable(f / o.getFloat());
								} else
									if (o.type  == TYPEID(double)) {
										return Variable(f / o.getDouble());
									} else
										if (o.isEnum()) {
											return Variable(f / o.getInt());
										}
					} else
						if (isEnum()) {
							if (o.type == TYPEID(const char *)) {
								errorMessage(format("Operator \"%s\" / \"%s\" is not defined.", type.name(), o.type.name()));
							} else
								if (o.type  == TYPEID(int)) {
									return Variable(i / o.getInt());
								} else
									if (o.type  == TYPEID(float)) {
										return Variable(i / o.getFloat());
									} else
										if (o.type  == TYPEID(double)) {
											return Variable(i / o.getDouble());
										} else
											if (o.isEnum()) {
												return Variable(i / o.getInt());
											}
						} else if(unify != NULL && o.unify != NULL) {
							Variable v;
							v.setType(type);
							v.unify = unify->div(o.unify);
							return v;
						}
		return Variable();
	}
	
	Variable Variable::operator%(const Variable &o) const
	{
		return [this, o]()
		{
			auto result = Variable(*this);
			while(result >= o) { result = result - o; }
			return result;
		}();
	}
	
	bool Variable::operator<(const Variable &o) const
	{
		if ((*this != o) && !(*this > o)) return true;
		return false;
	}
	
	bool Variable::operator>=(const Variable &o) const
	{
		return ((*this > o) || (*this == o));
	}
	
	bool Variable::operator<=(const Variable &o) const
	{
		return ((*this < o) || (*this == o));
	}
	
	bool Variable::operator!=(const Variable &o) const
	{
		return !(*this == o);
	}
	
	void Variable::create() {
		if(unify == NULL) {
			errorMessage(format("Can`t create variable of type \"%s\".", type.name()));
			return;
		}
		unify->create();
	}
	
	void Variable::destroy() {
		if(unify == NULL) {
			errorMessage(format("Can`t destroy variable of type \"%s\".", type.name()));
			return;
		}
		unify->destroy();
	}
	

	
	
}

Undefined::Undefined() {
	this->value.set("Undefined");
}

Undefined::Undefined(const char *value) {
	this->value.set(value);
}

Anandamide::Str &Undefined::getValue() {
	return value;
}

namespace Anandamide {
	
	void operator>>(Undefined t, Str &str) {
		str.set(t.getValue().str());
	}
	
	void operator<<(Undefined &t, const Str &str) {
		t = Undefined(str.str());
	}
	
}
