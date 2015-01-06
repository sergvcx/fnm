#include "string.h"
#include "MainWindow.h"
#include <QApplication>
#include "TransaqConnector.h"

#include "shared.h"
//extern CThreadAllDeals* pThreadAllDeals;

#include <QTextCodec>
#include <QTDebug>
using namespace std;

C_TransaqConnector TransaqConnector;
QSqlDatabase db_trading;



 int main(int argc, char *argv[])
 {
	 setlocale(LC_ALL, "Russian");

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

	 MainWindow* mainWin=new MainWindow;
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
		 				<< "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"   <<"MTSS"  
		 				<<"ROSN"  <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";

	sql_open_database("trading",db_trading);
	//sql_drop_tables(db_trading,TransaqConnector.listActive,"_quote");
	//sql_drop_tables(db_trading,TransaqConnector.listActive,"_trd");

	if (TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
	}
	else 
		TransaqConnector.connect();
	
	

//	pThreadAllDeals->ReadPortfolio();

	while(!TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
		Sleep(500);
	}
	
	qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";

	TransaqConnector.get_servtime_difference();
	if (TransaqConnector.servtime_difference!=0) 
		return -1;
	Sleep(1000);

	 
	QMap<QString,C_Instrument> mapInstrument;
	for(int i=0; i<TransaqConnector.listActive.size();i++){
		QString seccode=TransaqConnector.listActive.at(i);

		sql_create_seccode_deal(db_trading,seccode);
		sql_create_seccode_quote(db_trading,seccode);
		C_Instrument Instrument;
		//Instrument.pQuoteLog=new C_XML_Logger(seccode+"_glass.xml",LOGGER_APPEND);
		//Instrument.pQuoteLog->Header();
		//Instrument.pQuoteLog->flush();

		
		
		Instrument.TickInfo.lastDateTimeInDB=sql_get_last_datetime_from_seccode_deal(db_trading, seccode);
		//Instrument.TickInfo.tail=Instrument.pData->Ticks.FindAfter(lastDateTime);

		bool ok=Instrument.Attach(seccode);
		if (ok){

			//Instrument.tail=Instrument.WhichDateTime(min_datetime_filter);
			//if (Instrument.tail==-1)
			//	Instrument.tail=0;

			mapInstrument[seccode]=Instrument; // MUST BE IN THE END !!!
			
			

		}
		
	}
	qDebug() << "map Instrumente is constructed";

	
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
			
			
			//bool isUpdated=Instrument.pData->Quotes.UpdateCurrentQuotes(Instrument.listBuyQuote,Instrument.listSellQuote);
// 			QString new_glass=Instrument.pData->Quotes.toXML(6);
// 			if (Instrument.pQuoteLog)
// 			if (Instrument.strLastGlass!=new_glass){
// 				Instrument.strLastGlass= new_glass;
// 				qDebug() << new_glass;
// 				*Instrument.pQuoteLog<<new_glass << "\n";
// 				Instrument.pQuoteLog->flush();
// 			}
// 
// 			

		}
		
		
		Sleep(1000);
		db_trading.commit();
	}
	
	
	//Sleep(1000*60*60*8);

	TransaqConnector.disconnect();
	//Instrument.pSharedMemory->detach();
	//TransaqConnector.change_pass();
	sql_close_database(db_trading);
	CloseXML();
	 return 1;
	 
 }