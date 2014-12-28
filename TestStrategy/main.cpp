#include "string.h"
#include "MainWindow.h"
#include <QApplication>


#include "shared.h"
#include "TradeMaster.h"

#include <QTextCodec>
#include <QTDebug>
using namespace std;



 int main(int argc, char *argv[])
 {
	 setlocale(LC_ALL, "Russian");


/*
	 C_Instrument Instrument;
	 QString seccode="GMKN";

	 bool ok=Instrument.Attach(seccode);
	 if (!ok)
		 return;
		 
	C_TradeMaster TradeMaster;
	while(!Instrument.pData->Ticks.size){
		Sleep(100);
	}
	TradeMaster.Init(Instrument.pData->Ticks.Last(),Instrument.pData->Ticks.Last()*2,0,0))
	 while(){

		Instrument.
		
	 }
*/


	
	 QApplication app(argc, argv);

	 MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}

	//  mainWin->show();
	// return app.exec();

	// OpenXML();



	 

	 //TransaqConnector.disconnect();
	 //Sleep(1000);	
	 QList<QString> listActive ; 
	 listActive  <<  "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		 				<< "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
		 				<<"ROSN"  <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";

	

	 
	QMap<QString,C_Instrument> mapInstrument;
	for(int i=0; i<listActive.size();i++){
		QString seccode=listActive.at(i);
		C_Instrument Instrument;
	//	Instrument.pQuoteLog=new C_XML_Logger(seccode+"_glass.xml",LOGGER_WRITE);
	//	Instrument.pQuoteLog->Header();
	//	Instrument.pQuoteLog->flush();

		// MUST BE IN THE END !!!
		bool ok=Instrument.Attach(seccode);
		if (ok)
			mapInstrument[seccode]=Instrument;
		
	}

	


	while (1){
		//Instrument.pData->Quotes.UpdateCurrentQuotes()
		
		//qDebug() << xml_glass;
		
		
		foreach(QString seccode , mapInstrument.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_EasyTicks& Ticks=Instrument.pData->Ticks;

			int count= Ticks.size-Instrument.tail;
			Instrument.tail+=count;

			//bool isUpdated=Instrument.pData->Quotes.UpdateCurrentQuotes(Instrument.listBuyQuote,Instrument.listSellQuote);
// 			QString new_glass=Instrument.pData->Quotes.toXML(6);
// 			if (Instrument.strLastGlass!=new_glass){
// 				Instrument.strLastGlass= new_glass;
// 				qDebug() << new_glass;
// 				*Instrument.pQuoteLog<<new_glass << "\n";
// 				Instrument.pQuoteLog->flush();
// 			}

			

		}
		
		
		//Sleep(1000);

	}
	
	
	//Sleep(1000*60*60*8);

	 return 1;
	 
 }