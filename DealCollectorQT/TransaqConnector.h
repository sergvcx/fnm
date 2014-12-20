#ifndef TRAHNSAQCONNECTOR_DEFINED
#define TRAHNSAQCONNECTOR_DEFINED
#include <windows.h>

#include <iostream>
#include <fstream>
#include <qfile>
#define CONNECTOR_166PLUS // пример для версии библиотеки версии 1.66 (5.02) или выше

#define TACCESS_API  __declspec(dllimport)
typedef bool (WINAPI *tcallback)(BYTE* pData);
typedef BYTE* (WINAPI *typeSendCommand)(BYTE* pData);
typedef bool (WINAPI  *typeFreeMemory)(BYTE* pData);
typedef bool (WINAPI *typeSetCallback)(tcallback pCallback);

#ifdef CONNECTOR_166PLUS
typedef BYTE* (WINAPI *typeInitialize)(const BYTE* dir, int level);
typedef BYTE* (WINAPI *typeSetLogLevel)(int level);
typedef BYTE* (WINAPI *typeUninitialize)();
#endif

class S_Tick{
	//QString secid;		// 4
	QString board;		// TQBR
	QString seccode;	// LKOH
	QString tradeno;	// 2397108618
	QString tradetime;	// 08.08.2014 09:59:59
	QString price;		// 1928.3
	QString quantity;	// 2
	QString period;		// L
	QString buysell;	// S
};

struct S_SecInfo{
	QString seccode;
	QString shortname;
	QString decimails;
	QString active;
	QString secid;
	QString minstep;
	QString lotsize;
	QString board;
};

struct S_QuoteInfo {

	QString secid;	// "внутренний код">
	QString board;	// Идентификатор режима торгов по умолчанию 
	QString seccode; // Код инструмента </seccode>
	QString price;	// цена</price>
	QString source; // Источник котировки (маркетмейкер)</source>
	QString yield;	// доходность (актуально только для 	облигаций)</yield>
	QString buy;	// количество бумаг к покупке</buy>
	QString sell;	// количество бумаг к продаже</sell>
};

struct S_Quote {
	float price;
	float time_create;
	float time_update;
	int	  quantity;
};
struct  S_Security {
	//QFile file;
	S_SecInfo SecInfo;
	QList<S_Quote> listSellQuote;
	QList<S_Quote> listBuyQuote;
	QQueue<C_Tick> queueTick;
	QFile* xml_quotses;
};
	


class C_TransaqConnector {
	HMODULE hm;
	static const unsigned buffSize = 256;
	char error[buffSize];
	
	typeInitialize   Initialize;
	typeUninitialize UnInitialize;
	typeSetLogLevel  SetLogLevel;
	typeSetCallback  SetCallback;
	typeSendCommand  SendCommand;
	
public:

	QList<QString> listActive;
	C_TransaqConnector();
	~C_TransaqConnector();
	int connect();
	int disconnect();
	int server_status();
	int subscribe_ticks(QList<QString>& SeccodeList, int tradeno=1);
	int subscribe(QList<QString>& SeccodeList);
	int get_securities();
	int change_pass();

};

void OpenXML();
void CloseXML();

#endif