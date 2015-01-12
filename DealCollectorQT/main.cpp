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
	 setlocale(LC_ALL, "Russian");

	 sql_switch_all_buysell(db_trading);
// 	 uint dt=0;
// 	 QDateTime datetime;
// 	 datetime.setTime_t(0);//=QDateTime::
// 	 qDebug() << datetime.toString();
// 	 


// 	 C_XML_Logger Logger("logger.xml");
// 	 Logger<< "<root>";
// 	 Logger << "text";
// 	 Logger<< "</root>";
// 	 Logger.close();

	 //return XML;

	// cout << "Русский текст в консоли" << endl;


	// C_SharedPortfolio SharedPortfolio;

 //SharedPortfolio <<  "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
	//	 << "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
	//	 <<"ROSN" <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";

	
	 QApplication app(argc, argv);

	 //MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}

	//  mainWin->show();
	// return app.exec();

	 OpenXML();


	
	 

	 

	 //TransaqConnector.disconnect();
	 //Sleep(1000);
	 
	 TransaqConnector <<  "GMKN" 
 		<<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
 		<< "ODVA" <<"PLZL"<<"SVAV"<<"MGNT" <<"MSNG"   <<"MTSS"  <<"MTLRP"  <<"NLMK" <<"NMTP" <<"NVTK" <<"ROSN"
 		<<"RTKM" <<"RTKMP" <<"HYDR"  <<"CHMF" <<"URKA" <<"YNDX" <<"VTBR" ; 


	sql_open_database("trading",db_trading);
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
		//Instrument.TickInfo.lastDateTimeInDB=sql_get_last_datetime_from_seccode_deal(db_trading, seccode);
		//Instrument.TickInfo.tail=Instrument.pData->Ticks.FindAfter(lastDateTime);


		qDebug() << seccode << "attached!!!";
		//_getch();

	}

	qDebug() << "map Instrumente is constructed";


	//sql_drop_tables(db_trading,TransaqConnector.listActive,"_quote");
	//sql_drop_tables(db_trading,TransaqConnector.listActive,"_trd");


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

	//TransaqConnector.neworder("SBER",58.40,1,"S","");
	//while (1)
	Sleep(1000);

	 
	
	
	if (TransaqConnector.subscribe(TransaqConnector.listActive)){
		TransaqConnector.disconnect();
		qDebug() << "Error in subscribe";
		return 1;
	}
	
	//if (TransaqConnector.subscribe_ticks(QString("GMKN"))){
	if (TransaqConnector.subscribe_ticks(TransaqConnector.listActive)){	 
		TransaqConnector.disconnect();
		qDebug() << "Error in subscribe_ticks";
		return 1;
	}
	//if (!TransaqConnector.subscribe_ticks(TransaqConnector.listActive))
	//	TransaqConnector.disconnect();

	//QMap<QString,C_Instrument> mapInstrument;


	QSqlQuery tick_query(db_trading);

	while (1){
		//Instrument.pData->Quotes.UpdateCurrentQuotes()
		
		//qDebug() << xml_glass;
		
		
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