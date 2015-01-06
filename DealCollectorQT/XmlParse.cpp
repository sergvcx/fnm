#include "XmlParse.h"
#include <iostream>
#include "main.h"
#include "shared.h"

bool ParseResult(QString& result, QString& success_value, QString att_key, QString& att_value)
{
	QXmlStreamReader xml(result);
	while (!xml.atEnd() && !xml.hasError()){
		xml.readNext();
	
		if (xml.isStartDocument())
			continue;

		if (xml.isStartElement() && xml.name() == "result"){

			QXmlStreamAttributes attributes = xml.attributes();
			if (attributes.hasAttribute("success"))
				success_value = attributes.value("success").toString();

			if (attributes.hasAttribute(att_key))
				att_value = attributes.value(att_key).toString();
		}
	}
	_ASSERTE(!xml.hasError());
	return (success_value=="true");
}

int ParseOrder(QXmlStreamReader& xml,S_XML_OrderInfo& order){
	
	if(!(xml.isStartElement() && xml.name() == "order")) {
		return 0;
	}

	xml.readNext();
	//printf(STR(xml.name().toString()));
	while(!(xml.isEndElement() && xml.name() == "order")) {
		if(xml.isStartElement()) {

			QXmlStreamAttributes attributes = xml.attributes();
			if (attributes.hasAttribute("transactionid"))
				order.transactionid = attributes.value("transactionid").toString();

			if(xml.name() == "orderno") {
				order.orderno= xml.readElementText();
				continue;
			}

			if(xml.name() == "secid") {
				order.secid= xml.readElementText();
				continue;
			}

			if(xml.name() == "board") {
				order.board= xml.readElementText();
				continue;
			}

			if(xml.name() == "seccode") {
				order.seccode= xml.readElementText();
				continue;
			}

			if(xml.name() == "client") {
				order.client= xml.readElementText();
				continue;
			}

			if(xml.name() == "status") {
				order.status= xml.readElementText();
				continue;
			}
		
			if(xml.name() == "buysell") {
				order.buysell= xml.readElementText();
				continue;
			}

			if(xml.name() == "brokerref") {
				order.brokerref= xml.readElementText();
				continue;
			}
			if(xml.name() == "status") {
				order.status= xml.readElementText();
				continue;
			}
			if(xml.name() == "time") {
				order.time= xml.readElementText();
				continue;
			}
			if(xml.name() == "value") {
				order.value= xml.readElementText();
				continue;
			}
			if(xml.name() == "accruedint") {
				order.accruedint= xml.readElementText();
				continue;
			}
			if(xml.name() == "settlecode") {
				order.settlecode= xml.readElementText();
				continue;
			}
			if(xml.name() == "balance") {
				order.balance= xml.readElementText();
				continue;
			}
			if(xml.name() == "price") {
				order.price= xml.readElementText();
				continue;
			}
			if(xml.name() == "quantity") {
				order.quantity= xml.readElementText();
				continue;
			}
			if(xml.name() == "result") {
				order.result= xml.readElementText();
				continue;
			}
			if(xml.name() == "status") {
				order.status= xml.readElementText();
				continue;
			}
		}
		// ...and next... 
		xml.readNext();
		//printf(STR(xml.name().toString()));
	}
	

	return 1;
}
void ParseTick(QXmlStreamReader& xml, S_XML_Tick& tick ){
	if(!(xml.isStartElement() && xml.name() == "tick")) {
		return;
	}

	xml.readNext();
	//printf(STR(xml.name().toString()));
	while(!(xml.isEndElement() && xml.name() == "tick")) {
		if(xml.isStartElement()) {
			if(xml.name() == "seccode") {
				tick.seccode= xml.readElementText();
				continue;
			}
			//if(xml.name() == "secid") {
			//	tick.secid= xml.readElementText();
			//	continue;
			//}
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


int ParseSecurity(QXmlStreamReader& xml,S_XML_SecInfo& SecInfo){
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
			if(xml.name() == "decimals"){
				SecInfo.decimals= xml.readElementText();
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

int ParseQuote(QXmlStreamReader& xml,S_XML_QuoteInfo& QuoteInfo){
	if(!(xml.isStartElement() && xml.name() == "quote")) {
		return 1;
	}

	
	QXmlStreamAttributes attributes = xml.attributes();
	if (attributes.hasAttribute("secid"))
		QuoteInfo.secid = attributes.value("secid").toString();


	xml.readNext();
	while(!(xml.isEndElement() &&	xml.name() == "quote")) {
		if(xml.isStartElement()) {
			if(xml.name() == "board"){
				QuoteInfo.board= xml.readElementText();
				continue;
			}
			if(xml.name() == "seccode"){
				QuoteInfo.seccode= xml.readElementText();
				continue;
			}
			if(xml.name() == "price"){
				QuoteInfo.price= xml.readElementText();
				continue;
			}
			if(xml.name() == "source"){
				QuoteInfo.source= xml.readElementText();
				continue;
			}
			if(xml.name() == "yield"){
				QuoteInfo.yield= xml.readElementText();
				continue;
			}
			if(xml.name() == "buy"){
				QuoteInfo.buy= xml.readElementText();
				continue;
			}
			if(xml.name() == "sell"){
				QuoteInfo.sell= xml.readElementText();
				continue;
			}

		}
		// ...and next... 
		xml.readNext();
	}


	return 0;
}