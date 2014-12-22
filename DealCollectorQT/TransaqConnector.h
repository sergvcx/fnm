#ifndef TRAHNSAQCONNECTOR_DEFINED
#define TRAHNSAQCONNECTOR_DEFINED
#include <windows.h>

#include <iostream>
#include <fstream>
#include <qfile>
#include <QSharedMemory>

#include "main.h"
//#include "shared.h"
class C_SharedMemoryInstrument;
//class C_Instrument;

class C_Instrument {
public:
	/*
	C_Instrument(const C_Insrument& Inst){

	}
	C_Instrument(){

	}*/
	//C_Insrument& operator = (const C_Insrument& Inst){
	//	this->pData=Inst.pData;
	//	this->pSharedMemory=Inst.pSharedMemory;
	//	return *this;
	//}
	C_SharedMemoryInstrument* pData;
	QSharedMemory* pSharedMemory;
	void Lock(){
		pSharedMemory->lock();
	}
	void Unlock(){
		pSharedMemory->unlock();
	}

};
	

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

struct S_XML_Tick{
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

struct S_XML_SecInfo{
	QString seccode;
	QString shortname;
	QString decimails;
	QString active;
	QString secid;
	QString minstep;
	QString lotsize;
	QString board;
};

struct S_XML_QuoteInfo {

	QString secid;	// "внутренний код">
	QString board;	// Идентификатор режима торгов по умолчанию 
	QString seccode; // Код инструмента </seccode>
	QString price;	// цена</price>
	QString source; // Источник котировки (маркетмейкер)</source>
	QString yield;	// доходность (актуально только для 	облигаций)</yield>
	QString buy;	// количество бумаг к покупке</buy>
	QString sell;	// количество бумаг к продаже</sell>

	
};

struct S_XML_ServerStatus{
	QString id ;
	QString connected;
	QString recover;
	QString server_tz;
	QString status;
};

struct S_Quote {
	int	  quantity;
	float price;
	QDateTime datetime_create;
	QDateTime datetime_update;
};
struct  S_Security {
	//QFile file;
	S_XML_SecInfo SecInfo;
	QList<S_Quote> listSellQuote;
	QList<S_Quote> listBuyQuote;
	QQueue<S_XML_Tick> queueTick;
	QFile* xml_quotses;
};
	
struct S_InstrumentInfo{
	char	seccode[16];
	char	shortname[16];
	int		decimails;
	char	active[16];
	char	secid[16];
	int		market;
	double	minstep;
	double	lotsize;
	char	board[16];
	
	void operator = (S_XML_SecInfo& xml_secinfo){
		bool ok;
		strcpy(seccode,STR(xml_secinfo.seccode));
		strcpy(shortname,STR(xml_secinfo.shortname));
		decimails=xml_secinfo.decimails.toInt(&ok);		_ASSERTE(ok);
		strcpy(active,STR(xml_secinfo.active));
		strcpy(secid ,STR(xml_secinfo.secid));
		minstep	 =xml_secinfo.minstep.toDouble(&ok);	_ASSERTE(ok);
		lotsize	 =xml_secinfo.lotsize.toDouble(&ok);	_ASSERTE(ok);
		strcpy(board,STR(xml_secinfo.board));
	}
};

struct S_Tick{
	float		price;
	int			quntity;
	int			type;
	QDateTime	datetime;
};


struct C_EasyQuote{
	float		price;
	int			buy;
	int			sell;
	QDateTime	datetime;
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
	bool isBusy;
	S_XML_ServerStatus ServerStatus;

	QList<QString> listActive;
	C_TransaqConnector();
	~C_TransaqConnector();
	int connect();
	int disconnect();
	int server_status();
	int subscribe_ticks(QList<QString>& SeccodeList, int tradeno=1);
	int subscribe_ticks(QString& seccode);
	int subscribe(QList<QString>& SeccodeList);
	int get_securities();
	int change_pass();

	C_TransaqConnector& operator << (char * seccode);
	QMap<QString,C_Instrument> mapInstrument;	

	//QQueue<S_XML_Tick> queueTick;
	//QQueue<S_XML_QuoteInfo> queueQuote;


	bool isConnected();
};

void OpenXML();
void CloseXML();

#endif