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


#define CONNECTOR_166PLUS // ������ ��� ������ ���������� ������ 1.66 (5.02) ��� ����

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
	QString value; // ����� ������ � ��������
	QString accruedint;
	QString settlecode;
	QString balance; //����������������� ������� ������ ������ � ����� (����������)
	QString price;
	QString quantity;
	QString result; //��������� ����� � ������ ������ ���������  ������

};

struct S_XML_TradeInfo
{
	QString secid;		//������������� ������
	QString tradeno;	//����� ������ �� �����QString
	QString orderno;	//����� ������ �� �����QString
	QString board;		//������������� �����QString /board; //
	QString seccode;	// ��� ����������� QString /seccode; //
	QString client;	//������������� �������QString /client; //
	QString buysell;	//B - �������, S - �������QString /buysell; //
	QString time;		//����� ������QString /time; //
	QString brokerref; //����������QString /brokerref; //
	QString value;		//����� ������QString /value; //
	QString comission; //��������QString /comission; //
	QString price;		//����QString /price; //
	QString quantity; //���������� �����QString /quantity; //
	QString yield;		//����������QString /yield; //
	QString accruedint; //���QString /accruedint; //
	QString tradetype; //��� ������: �T� � ������� �N� � ��� �R� � ���� 	�P� � ����������QString /tradetype; //
	QString settlecode; //��� ��������QString /settlecode; //
	QString currentpos; //������� ������� �� �����������uint /currentpos; //
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
	int servtime_difference;	// ����� ������� - ����� �� ���������� (+ �� ��� �������� ����� �������������� )
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