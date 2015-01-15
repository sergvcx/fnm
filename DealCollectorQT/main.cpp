#include "TransaqConnector.h"
#include "shared.h"
#include "MainWindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTDebug>
#include <conio.h>
//using namespace std;

C_TransaqConnector TransaqConnector;
QSqlDatabase db_trading;

bool sql_switch_all_buysell(QSqlDatabase& db);

 int main(int argc, char *argv[])
 {
	 QApplication app(argc, argv);
	 setlocale(LC_ALL, "Russian");   // cout << "Русский текст в консоли" << endl;
	 OpenXML();
	 sql_open_database("trading",db_trading);
	 //sql_switch_all_buysell(db_trading);
	 //MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}

	//  mainWin->show();
	// return app.exec();
	 
	 TransaqConnector <<  "GMKN" 
 		<<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
 		<< "ODVA" <<"PLZL"<<"SVAV"<<"MGNT" <<"MSNG"   <<"MTSS"  <<"MTLRP"  <<"NLMK" <<"NMTP" <<"NVTK" <<"ROSN"
 		<<"RTKM" <<"RTKMP" <<"HYDR"  <<"CHMF" <<"URKA" <<"YNDX" <<"VTBR" ; 

	QMap<QString,C_Instrument> mapInstrument;

	for(int i=0; i<TransaqConnector.listActive.size();i++){
		QString seccode=TransaqConnector.listActive.at(i);
		C_Instrument Instrument;
		while (!Instrument.Attach(seccode)){
			qDebug() << seccode << "attach error!!!";
			Sleep(100);
		}
		mapInstrument[seccode]=Instrument; // MUST BE IN THE END !!!
		sql_create_seccode_deal (db_trading,seccode);
		sql_create_seccode_quote(db_trading,seccode);
		Instrument.TickInfo.lastDateTimeInDB=sql_get_last_datetime_from_seccode_deal(db_trading, seccode);
		//Instrument.TickInfo.tail=Instrument.pData->Ticks.FindAfter(lastDateTime);

		qDebug() << seccode << "attached!!!";
		//_getch();
	}

	qDebug() << "map Instrument is constructed";


	TransaqConnector.connect();
	while (!TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
		Sleep(1000);
		//TransaqConnector.connect();
	}
	
	qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";

	TransaqConnector.get_servtime_difference();
	//if (TransaqConnector.servtime_difference!=0) 
	//	return -1;

	Sleep(1000);
	
	if (TransaqConnector.subscribe(TransaqConnector.listActive)){
		TransaqConnector.disconnect();
		qDebug() << "Error in subscribe";
		return 1;
	}
	
	if (TransaqConnector.subscribe_ticks(TransaqConnector.listActive)){	 
		TransaqConnector.disconnect();
		qDebug() << "Error in subscribe_ticks";
		return 1;
	}



	
	QSqlQuery tick_query(db_trading);
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

	

	TransaqConnector.start();
	while (1){
		foreach(QString seccode , mapInstrument.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_EasyTicks&   Ticks=Instrument.pData->Ticks;
			S_EasyQuotes& Quotes=Instrument.pData->Quotes;
		
			int count= Ticks.size-Instrument.TickInfo.tail;
			Ticks2Mysql( tick_query, seccode, Ticks,Instrument.TickInfo.tail,count, Instrument.TickInfo.lastDateTimeInDB);
			Instrument.TickInfo.tail+=count;
		
			count = Quotes.size-Instrument.QuoteInfo.tail;
			Quotes2Mysql(tick_query,seccode,Quotes, Instrument.QuoteInfo.tail,count, false );
			Instrument.QuoteInfo.tail+=count;		
		}
		
		Sleep(1000);
		db_trading.commit();
	}
	


	TransaqConnector.disconnect();
	//Instrument.pSharedMemory->detach();
	//TransaqConnector.change_pass();
	sql_close_database(db_trading);
	CloseXML();
	 return 1;
	 
 }