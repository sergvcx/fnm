#include "string.h"
#include "MainWindow.h"
#include <QApplication>


#include "shared.h"
#include "TradeMaster.h"

#include <QTextCodec>
#include <QTDebug>
using namespace std;


template<class T> class C_SubVector
{
public:
	T*		data;
	size_t	size;
	C_SubVector(T* vec, size_t idxBegin,  size_t idxEnd){
		data=vec+idxBegin;
		size=idxEnd-idxBegin;
	}
	C_SubVector(QVector<T>& vec, size_t idxBegin,  size_t idxEnd)
	{
		data=&vec[idxBegin];
		size=idxEnd-idxBegin;
	}
	T& operator[] (size_t idx){
		return data[idx];
	}
};
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

	// MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}

	//  mainWin->show();
	// return app.exec();

	// OpenXML();

	QString seccode="GMKN";
	C_Instrument Instrument;
	while(!Instrument.Attach(seccode)){
		qDebug()<< "No connection to " + seccode;
		//Sleep(1000);
	}


	//C_TradeMaster TradeMaster;

	//TradeMaster.Init();


	size_t idxBegin=Instrument.pData->Ticks.size;
	while(1){
		size_t idxEnd=Instrument.pData->Ticks.size;
		C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data,idxBegin,idxEnd);
		for(int i=0; i<TickPortion.size; i++){
			S_Tick& Tick=TickPortion[i];
			qDebug()<< Tick.toXML() ;//<< "\n";
		}
		idxBegin=idxEnd;

	
		qDebug() << Instrument.pData->Quotes.toXML(6);
		/*
		QList<SOrder> MySell;
		QList<SOrder> MyBuy;


		int   Stocks=Sell(MySell,TickPortion);
		SOrd
		float Cash  =Buy (MyBuy,TickPortion);




		TradeMaster<<TickPortion;
		*/

	}
	//TradeMaster<<Deal;


	/* 

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
	*/
	
	//Sleep(1000*60*60*8);

	 return 1;
	 
 }