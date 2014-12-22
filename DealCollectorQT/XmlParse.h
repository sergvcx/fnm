#ifndef XMLPARSE_DEFINED
#define XMLPARSE_DEFINED


#include <QXmlStreamReader>
#include <QMap>
#include "main.h"
#include "TransaqConnector.h"

void ParseTick(QXmlStreamReader& xml, S_XML_Tick& tick );
int ParseSecurity(QXmlStreamReader& xml, S_XML_SecInfo& SecInfo );
int ParseQuote(QXmlStreamReader& xml,S_XML_QuoteInfo& QuoteInfo);

#endif
