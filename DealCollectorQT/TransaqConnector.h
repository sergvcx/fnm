#ifndef TRAHNSAQCONNECTOR_DEFINED
#define TRAHNSAQCONNECTOR_DEFINED
#include <windows.h>

#include <iostream>
#include <fstream>
#include <qfile>
#include <QSharedMemory>
#include <qtdEBUG>
#include "main.h"
#include "shared.h"
//class C_SharedMemoryInstrument;
//class C_Instrument;


#define CONNECTOR_166PLUS // пример дл€ версии библиотеки версии 1.66 (5.02) или выше

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

struct S_XML_OrderInfo{
	QString transactionid;
	QString orderno;
	QString secid;
	QString board;
	QString seccode;
	QString client;
	QString status;
	QString buysell;
	QString brokerref;
	QString time;
	QString value; // объем за€вки в копейках
	QString accruedint;
	QString settlecode;
	QString balance; //Ќеудовлетворенный остаток объема за€вки в лотах (контрактах)
	QString price;
	QString quantity;
	QString result; //сообщение биржи в случае отказа выставить  за€вку

};

struct S_XML_TradeInfo
{
	QString secid;		//»дентификатор бумаги
	QString tradeno;	//Ќомер сделки на биржеQString
	QString orderno;	//Ќомер за€вки на биржеQString
	QString board;		//»дентификатор бордаQString /board; //
	QString seccode;	//  од инструмента QString /seccode; //
	QString client;	//»дентификатор клиентаQString /client; //
	QString buysell;	//B - покупка, S - продажаQString /buysell; //
	QString time;		//врем€ сделкиQString /time; //
	QString brokerref; //примечаниеQString /brokerref; //
	QString value;		//объем сделкиQString /value; //
	QString comission; //комисси€QString /comission; //
	QString price;		//ценаQString /price; //
	QString quantity; //количество лотовQString /quantity; //
	QString yield;		//доходностьQString /yield; //
	QString accruedint; //Ќ ƒQString /accruedint; //
	QString tradetype; //тип сделки: СTТ Ц обычна€ СNТ Ц –ѕ— СRТ Ц –≈ѕќ 	СPТ Ц размещениеQString /tradetype; //
	QString settlecode; //код поставкиQString /settlecode; //
	QString currentpos; //“екуща€ позици€ по инструментуuint /currentpos; //
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




struct  S_Security {
	//QFile file;
	S_XML_SecInfo SecInfo;
	QList<S_Quote> listSellQuote;
	QList<S_Quote> listBuyQuote;
	QQueue<S_XML_Tick> queueTick;
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
	int servtime_difference;	// врем€ сервера - врем€ на компьютере (+ на эту величину нужно корректировать )
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
	int get_servtime_difference();
	int neworder(QString seccode,float price, uint quantity, QString buysell, QString brokerref);

	C_TransaqConnector& operator << (QString seccode);
	//C_TransaqConnector& operator << (QList<QString>& list);
	QMap<QString,C_Instrument> mapInstrument;	

	//QQueue<S_XML_Tick> queueTick;
	//QQueue<S_XML_QuoteInfo> queueQuote;


	bool isConnected();
};

void OpenXML();
void CloseXML();

#endif