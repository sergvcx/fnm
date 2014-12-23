#ifndef TRAHNSAQCONNECTOR_DEFINED
#define TRAHNSAQCONNECTOR_DEFINED
#include <windows.h>

#include <iostream>
#include <fstream>
#include <qfile>
#include <QSharedMemory>
#include <qtdEBUG>
#include "main.h"
//#include "shared.h"
class C_SharedMemoryInstrument;
class C_Instrument;


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
	QString decimals;
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
	//S_XML_ServerStatus(){
	///	connected="false";
	//}
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
	int		decimals;
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
		decimals=xml_secinfo.decimals.toInt(&ok);		_ASSERTE(ok);
		strcpy(active,STR(xml_secinfo.active));
		strcpy(secid ,STR(xml_secinfo.secid));
		minstep	 =xml_secinfo.minstep.toDouble(&ok);	_ASSERTE(ok);
		lotsize	 =xml_secinfo.lotsize.toDouble(&ok);	_ASSERTE(ok);
		strcpy(board,STR(xml_secinfo.board));
	}
};

struct S_Tick{
	float		price;
	int			quantity;
	int			type;
	uint		datetime;

	S_Tick(){
		price=0;
		quantity=0;
		type=0;
		
	}
	void SetSellType(){
		type&=0xFF;
		type|=(-1)<<8;
	}
	// Продажа (более низкая цена). На бирже выставлена заявка на покупку. Спрос
	void SetBuyType(){
		type&=0xFF;
		type|=(1)<<8;
	}

	QString toXML(){
		QString str_price;		str_price.setNum(price); 
		QString str_quantity;	str_quantity.setNum(quantity); 
		QString str_date;		str_date="date";
		QString str_time;		str_time="time";
		QString str_type;		str_type="type";
		QString XML="	<tick price='"+str_price+"' volume='"+str_quantity+"' date='"+str_date+"' time='"+str_time+"' type='"+str_type+"' >";
		qDebug()<< str_price << " " << str_quantity;
		return XML;
	}
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

	C_TransaqConnector& operator << (QString seccode);
	QMap<QString,C_Instrument> mapInstrument;	

	//QQueue<S_XML_Tick> queueTick;
	//QQueue<S_XML_QuoteInfo> queueQuote;


	bool isConnected();
};

void OpenXML();
void CloseXML();

#endif