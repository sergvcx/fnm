#include "XmlParse.h"
#include <iostream>

void ParseTick(QXmlStreamReader& xml, C_Tick& tick ){
	if(!(xml.isStartElement() && xml.name() == "tick")) {
		return;
	}

	xml.readNext();
	//printf(STR(xml.name().toString()));
	while(!(xml.isEndElement() && xml.name() == "tick")) {
		if(xml.isStartElement()) {
			if(xml.name() == "secid") {
				tick.secid= xml.readElementText();
				continue;
			}
			if(xml.name() == "board") {
				tick.board= xml.readElementText();
				continue;
			}
			if(xml.name() == "tradeno"){
				tick.tradeno= xml.readElementText();
				continue;
			}
			if(xml.name() == "tradetime"){
				tick.tradetime= xml.readElementText();
				continue;
			}
			if(xml.name() == "price"){
				tick.price= xml.readElementText();
				continue;
			}
			if(xml.name() == "quantity"){
				tick.quantity= xml.readElementText();
				continue;
			}
			if(xml.name() == "period"){
				tick.period= xml.readElementText();
				continue;
			}
			if(xml.name() == "buysell"){
				tick.buysell= xml.readElementText();
				continue;
			}
		}
		// ...and next... 
		xml.readNext();
		//printf(STR(xml.name().toString()));
	}
	return;
}


int ParseSecurity(QXmlStreamReader& xml,S_SecInfo& SecInfo){
	if(!(xml.isStartElement() && xml.name() == "security")) {
		return 1;
	}

	QXmlStreamAttributes attributes = xml.attributes();
	if (attributes.hasAttribute("secid"))
		SecInfo.secid = attributes.value("secid").toString();
	
	if (attributes.hasAttribute("active"))
		SecInfo.active = attributes.value("active").toString();

	xml.readNext();
	while(!(xml.isEndElement() &&	xml.name() == "security")) {
		if(xml.isStartElement()) {
			if(xml.name() == "shortname"){
				SecInfo.shortname= xml.readElementText();
				continue;
			}
			if(xml.name() == "decimails"){
				SecInfo.decimails= xml.readElementText();
				continue;
			}
			if(xml.name() == "minstep"){
				SecInfo.minstep= xml.readElementText();
				continue;
			}
			if(xml.name() == "lotsize"){
				SecInfo.lotsize= xml.readElementText();
				continue;
			}
			if(xml.name() == "board"){
				SecInfo.board= xml.readElementText();
				continue;
			}
			if(xml.name() == "seccode"){
				SecInfo.seccode= xml.readElementText();
				continue;
			}

		}
		// ...and next... 
		xml.readNext();
	}
	
	
	return 0;
}