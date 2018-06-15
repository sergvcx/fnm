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
QSqlDatabase db_traders;

bool sql_switch_all_buysell(QSqlDatabase& db);
QString strategy="_atlant";
 int main(int argc, char *argv[])
 {

	 
	 system("ping tr1.finam.ru");

	QApplication app(argc, argv);
	setlocale(LC_ALL, "Russian");   // cout << "Русский текст в консоли" << endl;
	OpenXML();

	QList<QString> listSecurity;
	listSecurity << "GMKN"	<< "LKOH"  ;
	/*
	//----------- Create instruments in shared memory -------------------
	for(int i=0; i<listSecurity.size();i++){
		QString seccode=listSecurity.at(i);
		C_Instrument Instrument;
		bool ok=Instrument.Create(seccode,"_usafinance");
		_ASSERTE(ok);
		qDebug()<< "Instrument" << seccode << "created";
		mapInstrument[seccode]=Instrument;
	 }*/

	//----------- Start mysql ----------------------------------------
restart:	 
	if (!sql_open_database("trader_atlant", db_traders))
		return -1;
/*	for(int i=0; i<listSecurity.size();i++){
		QString seccode=listSecurity.at(i);
		C_Instrument& Instrument=mapInstrument[seccode];
		sql_create_seccode_deal (db_trading,seccode);
		sql_create_seccode_quote(db_trading,seccode);
		Instrument.TickInfo.lastDateTimeInDB=sql_get_last_datetime_from_seccode_deal(db_trading, seccode);
	}
	qDebug() << "map of instruments is constructed";
	*/
reconnect:
	
	while (QTime::currentTime()<Text2Time("09:55:00") || QTime::currentTime()>Text2Time("21:00:00")){
		printf("Zzzz...");
		Sleep(1000); 
	}
	

	// ----------- Start transaq ---------------------------------------
	TransaqConnector.connect();
	for(int i=0; i<100; i++){
		if (!TransaqConnector.isConnected()){
			qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
			Sleep(1000);
		}
		else break;
	}
	qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
	if (!TransaqConnector.isConnected())
		goto reconnect;

	TransaqConnector.get_servtime_difference();
	if (TransaqConnector.servtime_difference!=0) {
		qDebug() << "Disconnecting...";
		TransaqConnector.disconnect();
		Sleep(1000);
		goto reconnect;
	}

/*	if (TransaqConnector.get_portfolio()!=0) {
		qDebug() << "Disconnecting...";
		TransaqConnector.disconnect();
		Sleep(1000);
		return 1;
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
*/
	QSqlQuery trade_query(db_traders);
	while (Text2Time("09:55:00")<QTime::currentTime() && QTime::currentTime()<Text2Time("21:00:00")){
	//while (1) {
		foreach(QString seccode , mapInstrument.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			//S_RingEasyTicks&   Ticks=Instrument.pData->ringEasyTicks;
			//S_RingEasyQuotes& Quotes=Instrument.pData->ringEasyQuotes;
			S_RingEasyTrades& Trades = Instrument.pData->ringEasyTrades;
			//sql_wirite_ticks( tick_query, seccode, Ticks,  Instrument.TickInfo.lastDateTimeInDB);
			//sql_wirite_quotes(tick_query, seccode, Quotes, false );  
			
			if (!Trades.isEmpty()) {

				sql_create_seccode_deal(db_traders, seccode);
				int last_datetime = sql_get_last_datetime_from_seccode_deal(db_traders, seccode);
				sql_write_trades(trade_query, seccode, Trades, last_datetime);
			}
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

