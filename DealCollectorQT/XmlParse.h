#ifndef XMLPARSE_DEFINED
#define XMLPARSE_DEFINED


#include <QXmlStreamReader>
#include <QMap>
#include "main.h"
#include "TransaqConnector.h"

void ParseTick(QXmlStreamReader& xml, C_Tick& tick );
int ParseSecurity(QXmlStreamReader& xml, S_SecInfo& SecInfo );

#endif
