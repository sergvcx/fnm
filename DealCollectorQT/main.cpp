#include "string.h"
#include "MainWindow.h"
#include <QApplication>
#include "TransaqConnector.h"

#include "shared.h"
extern CThreadAllDeals* pThreadAllDeals;

#include <QTextCodec>
#include <QTDebug>
using namespace std;

C_TransaqConnector TransaqConnector;


void Ticks2Mysql( QSqlQuery& query, QString seccode, S_Tick* data, int size, bool echo){


		if (size==0)
			return;

		//int err=query.prepare("INSERT INTO " + seccode+ "_deal (trdate,trtime,volume,price,trtype)  VALUES (?, ?, ?, ?, ?)");

		QVariantList listDate;
		QVariantList listTime;
		QVariantList listVolume;
		QVariantList listPrice;
		QVariantList listType;

		if (echo)
			qDebug()<< "<" +seccode+ ">";
		for (int i=0; i<size; i++,data++){
			S_Tick& tick =*data;
			
			listDate	<< tick.DateTime().toString("yyyyMMdd");
			listTime	<< tick.DateTime().toString("hhmmss");
			listVolume	<< tick.quantity;
			listPrice	<< tick.price;
			listType	<< tick.type;

			if (echo)
				qDebug()<<"    " << tick.toXML() ;//<< "\n";
			
		}
		if (echo)
			qDebug()<< "</" +seccode+ ">";
		/*
		query.addBindValue(listDate);
		query.addBindValue(listTime);
		query.addBindValue(listVolume);
		query.addBindValue(listPrice);
		query.addBindValue(listType);
	
		printf("execBatch...");
		if (!query.execBatch()){
			printf(ASCII( query.lastQuery()));
			printf("\n");
			printf(ASCII( query.lastError().text()));
			printf("\n");
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		}
		*/
		//printf("ok! %d ticks inserted\n",size);

	}

 int main(int argc, char *argv[])
 {
	 setlocale(LC_ALL, "Russian");

	 uint dt=0;
	 QDateTime datetime;
	 datetime.setTime_t(0);//=QDateTime::
	 qDebug() << datetime.toString();
	 
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
	 TransaqConnector	<< "GMKN" <<"LKOH"  ;//"AFLT" ;//<< "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		 		//		<< "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
		 		//		<<"ROSN"  <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";


	if (TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
	}
	else 
		TransaqConnector.connect();
	
	pThreadAllDeals->ReadPortfolio();

	while(!TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
		Sleep(500);
	}
	
	qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
	//TransaqConnector.server_status();
	//TransaqConnector.isConnected();
	 //TransaqConnector.connect();
	 //TransaqConnector.isConnected();
	 //while(!TransaqConnector.isConnected()){
	//	 printf("Transaq not connected\n");
	//	 Sleep(1000);
	// }

	 
	QMap<QString,C_Instrument> mapInstrument;
	for(int i=0; i<TransaqConnector.listActive.size();i++){
		QString seccode=TransaqConnector.listActive.at(i);
		C_Instrument Instrument;
		bool ok=Instrument.Attach(seccode);
		if (ok)
			mapInstrument[seccode]=Instrument;
	}

	 

	// C_Instrument Instrument;
	 

	// int dec=Instrument.pData->Info.decimals;
	

	//Sleep(1000*10);
	//TransaqConnector.subscribe(QString("GMKN"));
	//TransaqConnector.server_status();
	//Sleep(1000*20);
	//TransaqConnector.server_status();
	//while (!isReadyToCommand)
	//	Sleep(1000);
	if (TransaqConnector.subscribe(TransaqConnector.listActive)){
		TransaqConnector.disconnect();
		return 1;
	}
	
	//if (TransaqConnector.subscribe_ticks(QString("GMKN"))){
	if (TransaqConnector.subscribe_ticks(TransaqConnector.listActive)){	 
		TransaqConnector.disconnect();
		return 1;
	 }
	//if (!TransaqConnector.subscribe_ticks(TransaqConnector.listActive))
	//	TransaqConnector.disconnect();

	//QMap<QString,C_Instrument> mapInstrument;


	QSqlQuery tick_query(pThreadAllDeals->db_trading);

	while (1){
		//Instrument.pData->Quotes.UpdateCurrentQuotes()
		
		//qDebug() << xml_glass;
		
		
		foreach(QString seccode , mapInstrument.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_EasyTicks& Ticks=Instrument.pData->Ticks;

			int count= Ticks.size-Instrument.tail;
			Ticks2Mysql( tick_query, seccode, Ticks.data+Instrument.tail,count, true);
			Instrument.tail+=count;

			QString xml_glass=Instrument.pData->Quotes.toXML();
			qDebug() << xml_glass;
			//}
		}
		
		
		Sleep(1000);
		pThreadAllDeals->db_trading.commit();
	}
	
	
	//Sleep(1000*60*60*8);

	TransaqConnector.disconnect();
	//Instrument.pSharedMemory->detach();
	//TransaqConnector.change_pass();
	
	CloseXML();
	 return 1;
	 
 }