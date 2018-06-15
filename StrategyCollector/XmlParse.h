#ifndef XMLPARSE_DEFINED
#define XMLPARSE_DEFINED


#include <QXmlStreamReader>
#include <QMap>
#include "main.h"
#include "TransaqConnector.h"

void ParseTick(QXmlStreamReader& xml, S_XML_Tick& tick );
int ParseSecurity(QXmlStreamReader& xml, S_XML_SecInfo& SecInfo );
int ParseQuote(QXmlStreamReader& xml,S_XML_QuoteInfo& QuoteInfo);
int ParseOrder(QXmlStreamReader& xml,S_XML_OrderInfo& OrderInfo);
bool ParseResult(QString& xml, QString& success_value, QString att_key, QString& att_value, QString& text);
int ParseTrade(QXmlStreamReader& xml,S_XML_TradeInfo& trade);

#endif
