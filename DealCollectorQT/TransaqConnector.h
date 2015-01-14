#ifndef TRAHNSAQCONNECTOR_DEFINED
#define TRAHNSAQCONNECTOR_DEFINED

#include <windows.h>
#include "main.h"
#include "shared.h"

#include <iostream>
#include <fstream>
#include <qfile>
#include <QSharedMemory>
#include <QThread>
#include <qtdEBUG>

//class C_SharedMemoryInstrument;
//class C_Instrument;


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
	



class C_TransaqConnector:  public QThread {
	HMODULE hm;
	static const unsigned buffSize = 256;
	char error[buffSize];
	
	typeInitialize   Initialize;
	typeUninitialize UnInitialize;
	typeSetLogLevel  SetLogLevel;
	typeSetCallback  SetCallback;
	typeSendCommand  SendCommand;
	Q_OBJECT
public:
	int servtime_difference;	// время сервера - время на компьютере (+ на эту величину нужно корректировать )
	bool isBusy;
	S_XML_ServerStatus ServerStatus;

	QList<QString> listActive;
	C_TransaqConnector();
	~C_TransaqConnector();
	int connect();
	int disconnect();
	int server_status();
	int subscribe_ticks(QList<QString>& SeccodeList, unsigned long long tradeno=1);
	int subscribe_ticks(QString& seccode);
	int subscribe(QList<QString>& SeccodeList);
	int get_securities();
	int change_pass();
	int get_servtime_difference();
	//int neworder(QString seccode,float price, uint quantity, QString buysell);
	int neworder(QString seccode,S_NewOrder& order);
	int cancelorder(S_CancelOrder& order);

	C_TransaqConnector& operator << (QString seccode);
	//C_TransaqConnector& operator << (QList<QString>& list);
	QMap<QString,C_Instrument> mapInstrument;	

	//QQueue<S_XML_Tick> queueTick;
	//QQueue<S_XML_QuoteInfo> queueQuote;


	bool isConnected();
protected:	
	void run();
	void stop(){

	}
};

void OpenXML();
void CloseXML();

#endif