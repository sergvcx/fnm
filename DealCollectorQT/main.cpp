#include "TransaqConnector.h"
#include "shared.h"
#include "MainWindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTDebug>
#include <conio.h>
//using namespace std;

QMap<QString,C_Instrument> mapInstrument;
C_TransaqConnector TransaqConnector;
QSqlDatabase db_trading;

bool sql_switch_all_buysell(QSqlDatabase& db);

 int main(int argc, char *argv[])
 {

	 

	QApplication app(argc, argv);
	setlocale(LC_ALL, "Russian");   // cout << "������� ����� � �������" << endl;
	OpenXML();

	QList<QString> listSecurity;
	listSecurity <<  "GMKN" 
		 <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		 << "ODVA" <<"PLZL"<<"SVAV"<<"MGNT" <<"MSNG"   <<"MTSS"  <<"MTLRP"  <<"NLMK" <<"NMTP" <<"NVTK" <<"ROSN"
		<<"RTKM" <<"RTKMP" <<"HYDR"  <<"CHMF" <<"URKA" <<"YNDX" <<"VTBR" ; 

	//----------- Create instruments in shared memory -------------------
	for(int i=0; i<listSecurity.size();i++){
		QString seccode=listSecurity.at(i);
		C_Instrument Instrument;
		bool ok=Instrument.Create(seccode);
		_ASSERTE(ok);
		qDebug()<< "Instrument" << seccode << "created";
		mapInstrument[seccode]=Instrument;
	 }

	//----------- Start mysql ----------------------------------------
restart:	 
	sql_open_database("trading",db_trading);
	for(int i=0; i<listSecurity.size();i++){
		QString seccode=listSecurity.at(i);
		C_Instrument& Instrument=mapInstrument[seccode];
		sql_create_seccode_deal (db_trading,seccode);
		sql_create_seccode_quote(db_trading,seccode);
		Instrument.TickInfo.lastDateTimeInDB=sql_get_last_datetime_from_seccode_deal(db_trading, seccode);
	}
	qDebug() << "map of instruments is constructed";

reconnect:

	while (QTime::currentTime()<Text2Time("09:55:00") || QTime::currentTime()>Text2Time("21:00:00")){
		printf("Zzzz...");
		Sleep(1000);
	}

	// ----------- Start transaq ---------------------------------------
	TransaqConnector.connect();
	while (!TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
		Sleep(1000);
	}
	qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";

	TransaqConnector.get_servtime_difference();
	if (TransaqConnector.servtime_difference!=0) {
		qDebug() << "Disconnecting...";
		TransaqConnector.disconnect();
		Sleep(1000);
		goto reconnect;
	}

	Sleep(1000);
	
	if (TransaqConnector.subscribe(listSecurity)){
		TransaqConnector.disconnect();
		qDebug() << "Error in subscribe";
		return 1;
	}
	
	if (TransaqConnector.subscribe_ticks(listSecurity)){	 
		TransaqConnector.disconnect();
		qDebug() << "Error in subscribe_ticks";
		return 1;
	}

	// -------------- parse incoming deals 2 mysql ---------------

	QSqlQuery tick_query(db_trading);

	while (Text2Time("09:55:00")<QTime::currentTime() && QTime::currentTime()<Text2Time("21:00:00")){
		foreach(QString seccode , mapInstrument.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_RingEasyTicks&   Ticks=Instrument.pData->Ticks;
			S_RingEasyQuotes& Quotes=Instrument.pData->Quotes;
		
			Ticks2Mysql( tick_query, seccode, Ticks,  Instrument.TickInfo.lastDateTimeInDB);
			Quotes2Mysql(tick_query, seccode, Quotes, false );  
		}
		
		Sleep(1000);
		db_trading.commit();
	}
	


	TransaqConnector.disconnect();
	sql_close_database(db_trading);

goto restart;

	
	CloseXML();
	return 1;
	 
 }












 //	TransaqConnector.start();
 // 	C_Instrument& Instrument=mapInstrument["VTBR"];
 // 
 // 	float price=0.065;
 // 	Instrument.pData->Orders.NewOrders.Insert(price,1,'S');
 // 	Sleep(5000);
 // 	while (Instrument.pData->Trades.head==3){
 // 		
 // 		uint& id=Instrument.pData->Orders.NewOrders.Last().transaq.transactionid;
 // 		qDebug() << "cancel" << price;
 // 		bool ok=Instrument.pData->Orders.CancelOrders.Insert(id);
 // 		_ASSERTE(ok);
 // 		qDebug() << "cancel ok";
 // 		price-=0.001;
 // 		qDebug() << "insert" << price;
 // 		Sleep(100);
 // 		Instrument.pData->Orders.NewOrders.Insert(price,1,'S');
 // 		Sleep(5000);
 // 		
 // 	}
 // 	qDebug()<< "hit";
 //274672
 // 	
 // 	
 // 
 // 
 // 
 // 	while (1){
 // 
 // 		Sleep(1000);
 // 	}

