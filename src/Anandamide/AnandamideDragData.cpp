//------------------------------------------------------------------------------
//
// This file is part of AnandamideAPI Script
//
// copyright:	(c) 2010 - 2016
// author:		Sergey Almighty
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

#include "AnandamideDragData.h"
#include "AnandamideLibrary.h"

//------------------------------------------------------------------------------

#include <QDebug>
//------------------------------------------------------------------------------

namespace Anandamide {
	
	BlockDragData::BlockDragData() {}
	
	BlockDragData::~BlockDragData() {}
	
	void BlockDragData::addToMimeData(QMimeData *data)
	{
		if(!data->hasFormat("AnandamideBlock")) {
			QByteArray bytes2;
			data->setData("AnandamideBlock", bytes2);
		}
		
		QByteArray bytes = data->data("AnandamideBlock");
		
		QByteArray bytes3;
		QDataStream out(&bytes3, QIODevice::WriteOnly);
		out << content.type;
		out << content.library;
		out << content.instance;
		out << content.params;
		out << content.inputs;
		
		bytes += bytes3;
		
		data->setData("AnandamideBlock", bytes);
	}
	
	bool BlockDragData::fillFromMimedata(const QMimeData *data)
	{
		bool res = false;
		
		if(data->hasFormat("AnandamideBlock") && data->hasFormat("AnandamideBlockNum")) {
			QByteArray bytes2 = data->data("AnandamideBlockNum");
			QDataStream in2(&bytes2, QIODevice::ReadOnly);
			int num = 0;
			in2 >> num;
			
			QByteArray bytes = data->data("AnandamideBlock");
			QDataStream in(&bytes, QIODevice::ReadOnly);
			
			contents.clear();
			for(int i=0; i<num; i++) {
				in >> content.type;
				in >> content.library;
				in >> content.instance;
				in >> content.params;
				in >> content.inputs;

				contents.append(content);
			}
		}
		
		return res;
	}
	
	QMimeData *BlockDragData::toMimeData()
	{
		QMimeData *data = new QMimeData;
		
		data->setText("This is a Drug & Drop");
		
		QByteArray bytes;
		QDataStream out(&bytes, QIODevice::WriteOnly);
		out << content.type;
		out << content.library;
		out << content.instance;
		out << content.params;
		out << content.inputs;
		
		data->setData("AnandamideBlock", bytes);
		
		return data;
	}
	
	bool BlockDragData::fromMimeData(const QMimeData *data)
	{
		bool res = false;
		
		if(data->hasFormat("AnandamideBlock")) {
			QByteArray bytes = data->data("AnandamideBlock");
			
			QDataStream in(&bytes, QIODevice::ReadOnly);
			in >> content.type;
			in >> content.library;
			in >> content.instance;
			in >> content.params;
			in >> content.inputs;
		}
		
		return res;
	}
	
	void BlockDragData::fillNeurone(Neurone *neurone) {
		if(content.type == "ObjectBlock") {
			neurone->getParameter("name")->setValue(Anandamide::Variable(content.instance.toLocal8Bit().constData()));
			neurone->getParameter("ownerName")->setValue(Anandamide::Variable(content.ownerName.toLocal8Bit().constData()));
		} else if(content.type == "SignalBlock" || content.metaType == "Signals") {
			neurone->getParameter("name")->setValue(Anandamide::Variable(content.instance.toLocal8Bit().constData()));
			neurone->getParameter("signal")->setValue(Anandamide::Variable(content.itemValue.toLocal8Bit().constData()));
			neurone->getParameter("ownerName")->setValue(Anandamide::Variable(content.ownerName.toLocal8Bit().constData()));
		} else if(content.metaType == "Slots") {
			neurone->getParameter("name")->setValue(Anandamide::Variable(content.instance.toLocal8Bit().constData()));
			neurone->getParameter("slot")->setValue(Anandamide::Variable(content.itemValue.toLocal8Bit().constData()));
			neurone->getParameter("ownerName")->setValue(Anandamide::Variable(content.ownerName.toLocal8Bit().constData()));
		} else if(content.metaType == "Methods") {
			neurone->getParameter("name")->setValue(Anandamide::Variable(content.instance.toLocal8Bit().constData()));
			neurone->getParameter("slot")->setValue(Anandamide::Variable(content.itemValue.toLocal8Bit().constData()));
			neurone->getParameter("ownerName")->setValue(Anandamide::Variable(content.ownerName.toLocal8Bit().constData()));
		} else if(content.metaType == "Properties") {
			neurone->getParameter("name")->setValue(Anandamide::Variable(content.instance.toLocal8Bit().constData()));
			neurone->getParameter("property")->setValue(Anandamide::Variable(content.itemValue.toLocal8Bit().constData()));
			neurone->getParameter("ownerName")->setValue(Anandamide::Variable(content.ownerName.toLocal8Bit().constData()));
		} else {
			foreach(auto key, content.params.keys()) {
				Parameter *param = neurone->getParameter(key.toLocal8Bit().constData());
				if(param) param->setValue(Anandamide::Variable(content.params[key].toLocal8Bit().constData()));
			}
			foreach(auto key, content.inputs.keys()) {
				Input *input = neurone->getInput(key.toLocal8Bit().constData());
				if(input) input->setValue(Anandamide::Variable(content.inputs[key].toLocal8Bit().constData()));
			}
			neurone->invalidate();
		}
	}
	
	void BlockDragData::fillBlocksNum(QMimeData *data, int num)
	{
		data->setText("I`m Sexy Guy");
		QByteArray bytes;
		QDataStream out(&bytes, QIODevice::WriteOnly);
		out << num;
		data->setData("AnandamideBlockNum", bytes);
	}
	
	void BlockDragData::print()
	{
		qDebug() << __FUNCTION__ << "type=" << content.type;
		qDebug() << __FUNCTION__ << "library=" << content.library;
		qDebug() << __FUNCTION__ << "instance=" << content.instance;
		qDebug() << __FUNCTION__ << "====params";
		foreach(auto key, content.params.keys()) {
			qDebug() << key << content.params[key];
		}
		qDebug() << __FUNCTION__ << "====inputs";
		foreach(auto key, content.inputs.keys()) {
			qDebug() << key << content.inputs[key];
		}
	}
}
