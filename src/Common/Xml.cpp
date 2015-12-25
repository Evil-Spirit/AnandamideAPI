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

#include <string.h>
#include "Xml.h"
#include "MathCore.h"

namespace Anandamide {

	//------------------------------------------------------------------------------
	
	static void strToFileEscape(const char *input, FILE *f) {
		while(*input != '\0') {
			switch(*input) {
				case '<':	fputs("&lt;", f); break;
				case '>':	fputs("&gt;", f); break;
				case '&':	fputs("&amp;", f); break;
				case '"':	fputs("&quot;", f); break;
				case '\'':	fputs("&apos;", f); break;
				default:	fputc(*input, f);
			};
			input++;
		}
	}
	
	static Str strToStrEscape(const char *input) {
		Str output;
		while(*input != '\0') {
			switch(*input) {
				case '<':	output = output + "&lt;"; break;
				case '>':	output = output + "&gt;"; break;
				case '&':	output = output + "&amp;"; break;
				case '"':	output = output + "&quot;"; break;
				case '\'':	output = output + "&apos;"; break;
				default:	output = output + *input;
			};
			input++;
		}
		return output;
	}
	
	/*
	static void strFromEscape(Str &str) {
		str.replace("&lt;", "<");
		str.replace("&gt;", ">");
		str.replace("&amp;", "&");
		str.replace("&quot;", "\"");
		str.replace("&apos;", "\'");
	}
	*/
	static void strFromEscape(const char *input, char *output) {
		#define SUBS_CHAR(STR, CHR) { static int l = strlen(STR); if(!strncmp(input, STR, l)) { *(output++) = CHR; input += l; continue; } }
		while(*input) {
			if(*input == '&') {
				SUBS_CHAR("&lt;", '<');
				SUBS_CHAR("&gt;", '>');
				SUBS_CHAR("&amp;", '&');
				SUBS_CHAR("&quot;", '\"');
				SUBS_CHAR("&apos;", '\'');
				SUBS_CHAR("&nbsp;", ' ');
				
				//SUBS_CHAR("&#34;", '"');
				//SUBS_CHAR("&#38;", '&');
				//SUBS_CHAR("&#39;", '\'');
				//SUBS_CHAR("&#60;", '<');
				//SUBS_CHAR("&#62;", '>');
				//SUBS_CHAR("&#160;", ' ');
			}
			*(output++) = *(input++);
		};
		/*
		#define SUBS_CHAR(STR, CHR) { static int l = strlen(STR); if(!strncmp(input, STR, l)) { *(output++) = CHR; input += l; continue; } }
		while(*input) {
			SUBS_CHAR("&lt;", '<');
			SUBS_CHAR("&gt;", '>');
			SUBS_CHAR("&amp;", '&');
			SUBS_CHAR("&quot;", '\"');
			SUBS_CHAR("&apos;", '\'');
			SUBS_CHAR("&nbsp;", ' ');
			
			//SUBS_CHAR("&#34;", '"');
			//SUBS_CHAR("&#38;", '&');
			//SUBS_CHAR("&#39;", '\'');
			//SUBS_CHAR("&#60;", '<');
			//SUBS_CHAR("&#62;", '>');
			//SUBS_CHAR("&#160;", ' ');
			
			*(output++) = *(input++);
		};
		*/
		*output = *input;
		#undef SUBS_CHAR
	}
	
	//------------------------------------------------------------------------------
	
	Xml::Xml(const char *name) {
		setName(name);
	}
	
	//------------------------------------------------------------------------------
	
	Xml::~Xml() {
	
	}
	
	//------------------------------------------------------------------------------
	
	const char *Xml::getName() const {
		return name;
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::setName(const char *name_) {
		name = name_;
	}
	
	//------------------------------------------------------------------------------
	
	int Xml::getChildrenCount() const {
		return children.count();
	}
	
	//------------------------------------------------------------------------------
	
	Xml *Xml::getChild(int i) {
		return &children[i];
	}
	
	//------------------------------------------------------------------------------
	
	Xml *Xml::getChild(const char *child) {
		for (int i=0; i<children.count(); i++) {
			if (children[i].name == child) {
				return &children[i];
			}
		}
		return NULL;
	}
	
	//------------------------------------------------------------------------------
	
	const Xml *Xml::getChild(int i) const {
		return &children[i];
	}
	
	//------------------------------------------------------------------------------
	
	const Xml *Xml::getChild(const char *child) const {
		for (int i=0; i<children.count(); i++) {
			if (children[i].name == child) {
				return &children[i];
			}
		}
		return NULL;
	}
	
	//------------------------------------------------------------------------------
	
	Xml *Xml::addChild(Xml *xml) {
		children.append(xml);
		return xml;
	}
	
	//------------------------------------------------------------------------------
	
	Xml *Xml::addChild(const char *name) {
		Xml *child = new Xml(name);
		children.append(child);
		return child;
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::removeChild(int i) {
		children.remove(i);
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::removeChild(Xml *xml) {
		children.remove(xml);
	}
	
	//------------------------------------------------------------------------------
	
	int Xml::getArgsCount() const {
		return args.count();
	}
	
	//------------------------------------------------------------------------------
	
	const char *Xml::getArg(int i) const {
		return args[i].data;
	}
	
	//------------------------------------------------------------------------------
	
	const char *Xml::getArg(const char *arg) const {
		int arg_index = findArg(arg);
		if(arg_index < 0) return NULL;
		return args[arg_index].data;
	}
	
	//------------------------------------------------------------------------------
	
	bool Xml::isArg(const char *name) const {
		return getArg(name) != NULL;
	}
	
	//------------------------------------------------------------------------------
	
	int Xml::findArg(const char *name) const {
		for(int i = 0; i<args.count(); i++) {
			if (args[i].name == name) {
				return i;
			}
		}
		return -1;
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::setArg(int i, const char *name, const char *data) {
		args[i].name = name;
		args[i].data = data;
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::setArg(const char *name, const char *data) {
	
		int arg_index = findArg(name);
		if (arg_index != -1) {
			setArg(arg_index, name, data);
		} else {
			args.append(new Arg(name, data));
		}
	
	}
	
	//------------------------------------------------------------------------------
	
	const char *Xml::getData() const { return data; }
	
	//------------------------------------------------------------------------------
	
	void Xml::setData(const char *data_) { data = data_; }
	
	//------------------------------------------------------------------------------
	
	const char *Xml::parseXml(const char *str) {
	
		#define IS_SPACE(chr)				(chr == ' ' || chr == '\t')
		#define IS_EOL(chr)					(chr == '\n' || chr == '\r')
		#define IS_EOS(chr)					(chr == '\0')
		#define IS_LETTER(chr)				(chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z' || chr == '_')
		#define IS_DIGIT(chr)				(chr >= '0' && chr <= '9')
	
		#define SKIP_SPACES_NOCHECK(str)   	while (IS_SPACE(*str) || IS_EOL(*str)) str ++;
	
		#define SKIP_SPACES(str)		   	SKIP_SPACES_NOCHECK(str);								\
											if (IS_EOS(*str)) throw "Xml::parseXml(): unexpected end of string while skipping spaces.";
	
		#define SKIP_CHAR(str, chr)			{ if (*str != chr) throw "Xml::parseXml(): expected character "#chr; str ++; }
	
		#define GET_NAME(str, name)			{														\
												if (!IS_LETTER(*str)) throw "Xml::parseXml(): name has wrong characters.";		\
												*name = '\0';										\
												char *ptr = name;									\
												while (IS_LETTER(*str) || IS_DIGIT(*str)) {			\
													*ptr = *str;									\
													str ++;											\
													ptr ++;											\
												}													\
												*ptr = '\0';										\
												if (IS_EOS(*str)) throw "Xml::parseXml(): unexpected end of string while getting name.";\
											}
	
		#define GET_DATA(str, data, term)	{														\
												*data = '\0';										\
												char *ptr = data;									\
												while (!IS_EOS(*str) && *str != term) {				\
													*ptr = *str;									\
													str ++;											\
													ptr ++;											\
												}													\
												*ptr = '\0';										\
												if (IS_EOS(*str)) throw "Xml::parseXml(): unexpected end of string wiled getting data.";			\
											}
	
		char name[128];
		char close_name[128];
		static char arg_name[128];
		static char data[16384];
		static char cdata[16384];
	
		SKIP_SPACES_NOCHECK(str);
		if (IS_EOS(*str)) return str;
	
		SKIP_CHAR(str, '<');
	
		// comment T
		// TODO : add comment reading
		if (*str == '!') {
	
			SKIP_CHAR(str, '!');
			SKIP_CHAR(str, '-');
			SKIP_CHAR(str, '-');
			// *data = '\0';
			// char *ptr = data;
			while (!IS_EOS(*str)) {
				// *ptr = *str;
				str ++;
				// ptr ++;
	
				if (str[0] == '-' && str[1] != '\0' && str[1] == '-' && str[2] != '\0' && str[2] == '>') {
					SKIP_CHAR(str, '-');
					SKIP_CHAR(str, '-');
					SKIP_CHAR(str, '>');
					setComment("comment");
					return str;
					//break;
				}
	
			}
			//*ptr = '\0';
			//if (IS_EOS(*str)) throw "Xml::parseXml():";
	
			//setComment(data);
			//SKIP_SPACES(str);
			//SKIP_CHAR(str, '<');
		}
	
		SKIP_SPACES(str);
	
		GET_NAME(str, name);
	
		setName(name);
	
		SKIP_SPACES(str);
	
		// get args
		while (IS_LETTER(*str)) {
	
			GET_NAME(str, arg_name);
	
			SKIP_SPACES(str);
	
			SKIP_CHAR(str, '=');
	
			SKIP_SPACES(str);
	
			SKIP_CHAR(str, '"');
	
			GET_DATA(str, data, '"');
	
			SKIP_CHAR(str, '"');
	
			strFromEscape(data, cdata);
			setArg(arg_name, cdata);
	
			SKIP_SPACES(str);
		}
	
		// check no data node
		if (str[0] == '/' && str[1] == '>') {
			str += 2;
			return str;
		}
	
		SKIP_CHAR(str, '>');
	
		const char *old_str = str;
		SKIP_SPACES(str);
	
		bool data_node = false;
	
		if (*str != '<') {
	
			str = old_str;
			GET_DATA(str, data, '<');
			strFromEscape(data, cdata);
			setData(cdata);
	
			data_node = true;
	
		}
	
		do {
	
			SKIP_SPACES(str);
	
			// parse end tag
			if (str[0] == '<' && str[1] == '/') {
	
				str += 2;
	
				SKIP_SPACES(str);
	
				GET_NAME(str, close_name);
	
				SKIP_SPACES(str);
	
				SKIP_CHAR(str, '>');
	
				if(strcmp(close_name, name) != 0) throw "Xml::parseXml(): open and close tags does not match.";
	
				return str;
			} else if (data_node) {
				throw "Xml::parseXml(): close tag not found.";
			}
	
			Xml *child = new Xml("child");
			str = child->parseXml(str);
	
			// if comment
			if (child->comment != "") {
				delete child;
				continue;
			}
	
			children.append(child);
	
		} while (!IS_EOS(*str));
	
		throw "Xml::parseXml(): unexpected end of file.";
	
		#undef IS_SPACE
		#undef IS_EOL
		#undef IS_EOS
		#undef IS_LETTER
		#undef IS_DIGIT
		#undef SKIP_SPACES_NOCHECK
		#undef SKIP_SPACES
		#undef SKIP_CHAR
		#undef GET_NAME
		#undef GET_DATA
		
	}
	
	//------------------------------------------------------------------------------
	
	size_t fileSize(FILE *handle) {
		if (!handle) return 0;
		size_t pos, sz;
		pos = ftell(handle);
		fseek(handle, 0, SEEK_END);
		sz = ftell(handle);
		fseek(handle, pos, SEEK_SET);
		return sz;
	}
	
	bool Xml::load(const char *name) {
	
		FILE *file = fopen(name, "rb");
	
		if (file == NULL) {
			return false;
		}
	
		char *buffer = NULL;
		size_t size = fileSize(file);
		buffer = new char[size + 1];
		fread(buffer, size, 1, file);
		buffer[size] = '\0';
		const char *str = buffer;
	
		do {
			comment = "";
			str = parseXml(str);
		} while (comment != "");
	
		delete [] buffer;
		fclose(file);
		return true;
	
	}
	
	void Xml::saveToFile(FILE *f, int depth) const {
	
		if (comment != "") {
			for(int i=0; i<depth; i++) fputs("\t", f);
			fputs("<!--", f);
			fputs(comment.str(), f);
			fputs("-->\n", f);
		}
	
		for(int i=0; i<depth; i++) fputs("\t", f);
	
		fputs("<", f);
		fputs(name.str(), f);
	
		if (args.count() > 0) {
			for (int i=0; i<args.count(); i++) {
				fputs(" ", f);
				fputs(args[i].name.str(), f);
				fputs("=\"", f);
				strToFileEscape(args[i].data.str(), f);
				fputs("\"", f);
			}
		}
	
		if (children.count() > 0) {
			fputs(">\n", f);
			
			for(int i=0; i<children.count(); i++) {
				children[i].saveToFile(f, depth + 1);
			}
			
			for(int i=0; i<depth; i++) fputs("\t", f);
			fputs("</", f);
			fputs(name.str(), f);
			fputs(">\n", f);
			
		} else if(data != "") {
			fputs(">", f);
			strToFileEscape(data.str(), f);
			fputs("</", f);
			fputs(name.str(), f);
			fputs(">\n", f);
		} else {
			fputs("/>\n", f);
		}
	
	}
	
	void Xml::saveToStr(Str &s) const {
		s = s + format("<%s", name.str());
	
		if (args.count() > 0) {
			for (int i=0; i<args.count(); i++) {
				s = s + format(" %s=\"%s\"", args[i].name.str(), strToStrEscape(args[i].data.str()).str());
			}
		}
	
		if (children.count() > 0) {
			s = s + ">";
			for(int i=0; i<children.count(); i++) {
				children[i].saveToStr(s);
			}
			s = s + format("</%s>", name.str());
	
		} else if(data != "") {
			s = s + format(">%s</%s>", strToStrEscape(data.str()).str(), name.str());
		} else {
			s = s + format("/>");
		}
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::save(const char *name) const {
		FILE *f = fopen(name, "wb");
		if (f == NULL) throw("Xml::save(): cant open file to write.");
		saveToFile(f);
		fclose(f);
	}
	
	//------------------------------------------------------------------------------
	
	bool Xml::getArg(const char *name, int &r) const { const char *arg = getArg(name); if (arg) Nutmeg::fromString(arg, r); return arg != NULL; }
	void Xml::setArg(const char *name, int d) { setArg(name, Nutmeg::intToStr(d)); }
	
	bool Xml::getArg(const char *name, bool &r) const {
		const char *arg = getArg(name); 
		if (arg) {
			int i;
			Nutmeg::fromString(arg, i);
			r = i != 0;
			return true;
		}
		return false;
	}
	
	void Xml::setArg(const char *name, bool d) { setArg(name, Nutmeg::intToStr(d)); }
	
	bool Xml::getArg(const char *name, float &r) const { const char *arg = getArg(name); if (arg) Nutmeg::fromString(arg, r); return arg != NULL; }
	void Xml::setArg(const char *name, float d) { setArg(name, Nutmeg::floatToStr(d)); }
	
	bool Xml::getArg(const char *name, Str &r) const { r = getArg(name); return true; }
	
	//------------------------------------------------------------------------------
	
	void Xml::getData(int &r) const { Nutmeg::fromString(data, r); }
	void Xml::setData(int d) { data = Nutmeg::intToStr(d); }
	
	void Xml::getData(bool &r) const { int i; Nutmeg::fromString(data, i); r = i != 0; }
	void Xml::setData(bool d) { data = Nutmeg::intToStr(d); }
	
	void Xml::getData(float &r) const { Nutmeg::fromString(data, r); }
	void Xml::setData(float d) { data = Nutmeg::floatToStr(d); }
	
	void Xml::getData(Str &r) const { r = data; }
	
	//------------------------------------------------------------------------------
	
	bool Xml::getChildData(const char *name, int &r) const { const Xml *child = getChild(name); if (child != NULL) child->getData(r); return child != NULL; }
	void Xml::setChildData(const char *name, int d) { addChild(name)->setData(d); }
	
	bool Xml::getChildData(const char *name, bool &r) const { const Xml *child = getChild(name); if (child != NULL) child->getData(r); return child != NULL; }
	void Xml::setChildData(const char *name, bool d) { addChild(name)->setData(d); }
	
	bool Xml::getChildData(const char *name, float &r) const { const Xml *child = getChild(name); if (child != NULL) child->getData(r); return child != NULL; }
	void Xml::setChildData(const char *name, float d) { addChild(name)->setData(d); }
	
	bool Xml::getChildData(const char *name, Str &r) const { const Xml *child = getChild(name); if (child != NULL) child->getData(r); return child != NULL; }
	void Xml::setChildData(const char *name, const char *d) { addChild(name)->setData(d); }
	
	//------------------------------------------------------------------------------
	
	void Xml::setComment(const char *comment_) {
		comment = comment_;
	}
	
	//------------------------------------------------------------------------------
	
	const char *Xml::getComment() const {
		return comment;
	}
	
	//------------------------------------------------------------------------------
	
	Xml &Xml::operator= (const Xml &o) {
	
		name = o.name;
		data = o.data;
		comment = o.comment;
	
		args.clear();
		for (int i=0; i<o.args.count(); i++) {
			setArg(o.args[i].name, o.args[i].data);
		}
	
		children.clear();
		for (int i=0; i<o.children.count(); i++) {
			Xml *child = addChild(o.children[i].getName());
			*child = o.children[i];
		}
	
		return *this;
	
	}
	
	//------------------------------------------------------------------------------
	
	void Xml::clear() {
		data = "";
		comment = "";
		args.clear();
		children.clear();
	}
	
	void Xml::parse(const char *xml) {
		const char *str = xml;
		do {
			comment = "";
			str = parseXml(str);
		} while (comment != "");
	}
	
}	

//------------------------------------------------------------------------------
