#include "string.h"
#include <windows.h>
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

void Trade(QList<SOrder>& listSell, QList<SOrder>& listBuy, C_SubVector& vecDeals ){
	QQueue<SDeal>	qSecondDeals;
	QQueue<SDeal>	qBuyDeals;
	QQueue<SDeal>	qSellDeals;

	while (!qDeals.isEmpty()){
		ExtractInSecondTicks(qDeals,qSecondDeals);
		if (qSecondDeals[0].nTime<103000){
			qSecondDeals.clear();
			continue;
		}
		if (qSecondDeals[0].nTime>184000){
			Close();
			qSecondDeals.clear();
			continue;
		}
		SplitBuySell(qSecondDeals,qBuyDeals,qSellDeals);
		if (!qBuyDeals.isEmpty()){
			LastBuyDeal=qBuyDeals.dequeue();
			while (!qBuyDeals.isEmpty()){
				if (LastBuyDeal.Price>qBuyDeals.last().Price)
					LastBuyDeal=qBuyDeals.dequeue();
				else 
					qBuyDeals.dequeue();
			}

			while (!TrapNet.listBuy.isEmpty() && LastBuyDeal.Price<=TrapNet.listBuy.first().Price){
				CompleteFirstOrder(TrapNet.listBuy,BUY);
			}
			while (!HotNet.listBuy.isEmpty() && LastBuyDeal.Price<=HotNet.listBuy.first().Price){
				CompleteFirstOrder(HotNet.listBuy,BUY);
			}
			//qBuyDeals.clear();
		}
		if (!qSellDeals.isEmpty()){
			LastSellDeal=qSellDeals.dequeue();
			while (!qSellDeals.isEmpty()){
				if (LastSellDeal.Price<qSellDeals.last().Price)
					LastSellDeal=qSellDeals.dequeue();
				else 
					qSellDeals.dequeue();
			}


			while (!TrapNet.listSell.isEmpty() && LastSellDeal.Price>=TrapNet.listSell.first().Price){
				CompleteFirstOrder(TrapNet.listSell,SELL);
			}
			while (!HotNet.listSell.isEmpty() && LastSellDeal.Price>=HotNet.listSell.first().Price){
				CompleteFirstOrder(HotNet.listSell,SELL);
			}
			//qSellDeals.clear();
		}
		if (Counter==120)
			int g=1;
		if (Cash>100000)
			int g=1;
		//if (!qBuyDeals.isEmpty()){
		//	LastBuyDeal=qBuyDeals.dequeue();
		GetBuyQuote(qDeals,BuyQuote);
		GetSellQuote(qDeals,SellQuote);
		MakeOrders();
	}
}

 int main(int argc, char *argv[])
 {
	 setlocale(LC_ALL, "Russian");



	
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
		Sleep(1000);
	
		
		
		QList<SOrder> MySell;
		QList<SOrder> MyBuy;


		//int   Stocks=Sell(MySell,TickPortion);
		//SOrder 
		//float Cash  =Buy (MyBuy,TickPortion);




		//TradeMaster<<TickPortion;
		

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