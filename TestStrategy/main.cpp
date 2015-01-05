#include "string.h"
#include <windows.h>
#include "MainWindow.h"
#include <QApplication>


#include "shared.h"
#include "TradeMaster.h"

#include <QTextCodec>
#include <QTDebug>
#include <QThread>
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

bool Trade(QList<SOrder>& listBuy, QList<SOrder>& listSell,  C_SubVector<S_Tick>& vecTicks, float &Cash, int& Stocks, float Commision )
{
	bool isTransactionHappened=false;
	for(uint i=0; i<vecTicks.size; i++){
		S_Tick& tick=vecTicks[i];
		if (tick.isSell()){
			while(!listSell.isEmpty() && tick.price>=listSell.first().Price){
				const SOrder& My=listSell.first();
				Cash+=My.Price*My.Volume*(1-Commision);
				listSell.removeFirst();
				isTransactionHappened=true;
			}
		}
		else {
			while(!listBuy.isEmpty() && tick.price<=listBuy.first().Price){
				const SOrder& My=listBuy.first();
				Cash-=My.Price*My.Volume*Commision;
				Stocks+=My.Volume;
				listBuy.removeFirst();
				isTransactionHappened=true;
			}
		}
	}
	return isTransactionHappened;
}
void FilterSell(QList<SOrder>& listSell, float NearThreshold, float FarThreshold, int& Stocks)
{
	while (!listSell.isEmpty() && NearThreshold>listSell.first().Price){
		Stocks+=listSell.first().Volume;
		listSell.removeFirst();
	}
	while (!listSell.isEmpty() && FarThreshold<listSell.last().Price){
		Stocks+=listSell.first().Volume;
		listSell.removeLast();
	}
}

void FilterBuy(QList<SOrder>& listBuy, float NearThreshold, float FarThreshold, float &Cash)
{
	while (!listBuy.isEmpty() && NearThreshold<listBuy.first().Price){
		Cash+=listBuy.first().Volume*listBuy.first().Price;
		listBuy.removeFirst();
	}
	while (!listBuy.isEmpty() && FarThreshold>listBuy.last().Price){
		Cash+=listBuy.first().Volume*listBuy.first().Price;
		listBuy.removeLast();
	}
}




void Close(QList<SOrder>& listBuy, QList<SOrder>& listSell, float &Cash, int& Stocks, float ClosePrise, float Commision)
{
	while (!listBuy.isEmpty()){
		Cash+=listBuy.first().Volume*listBuy.first().Price;
		listBuy.removeFirst();
	}
	while (!listSell.isEmpty()){
		Cash+=listSell.first().Volume*ClosePrise*(1-Commision);
		listSell.removeFirst();
	}
}

class C_Strategy{
	public:
		float Cash;
		int   Stocks;
		float Commision;
		C_XML_Logger* logger;

		C_Strategy()
		{
			Cash=0;
			Stocks=0;
			Commision=0.035/100;
			logger=0;
		}
		C_Strategy(QString logname)
		{
			Cash=0;
			Stocks=0;
			Commision=0.035/100;
			logger=new C_XML_Logger(logname);
		}
		//QTextStream* Stream(){
		//	return logger->logStream;
		//}
		//void Log(QString str){
		//	if (logger)
		//		*logger->logStream<< str;
		//}
		virtual void operator << (C_SubVector<S_Tick>& vecTick){

		}
		virtual void Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote){

		}
		virtual float Profit(float ClosePrice){
			return 0;			
		}
};

template <class T> struct S_MinMax {
	T min;
	T max;
	S_MinMax(){
		min=0;
		max=0;
	}
	S_MinMax(T Min, T Max){
		min=Min;
		max=Max;
	}
	void init(T Min, T Max){
		min=Min;
		max=Max;
	}
};

template <class T> struct S_NearFar {
	T Near;
	T Far;
	S_NearFar(){
		//Near=0;
		//Far=0;
	}
	S_NearFar(T _Near, T _Far){
		Near=_Near;
		Far=_Far;
	}
	void init(T _Near, T _Far){
		Near=_Near;
		Far=_Far;
	}
};


class C_S1_Strategy:public C_Strategy{
public:
	QList<SOrder> listBuy;
	QList<SOrder> listSell;
	
	S_NearFar<float> BuyDelta;
	S_NearFar<float> SellDelta;

	C_S1_Strategy():C_Strategy(){
		SellDelta.Far =(1+0.5/100);
		SellDelta.Near=(1+0.1/100);
		BuyDelta.Near =(1-0.1/100);
		BuyDelta.Far  =(1-0.5/100);
	}

	void operator << (C_SubVector<S_Tick>& vecTick){
		if (Trade(listBuy,listSell,vecTick,Cash,Stocks,Commision));
		//	*logger<< "<cash='" << Cash <<"' stocks= '"<< Stocks <<"'>");
		
	}
	virtual void Update(float SpreadMin, float SpreadMax)
	{
		_ASSERTE(SpreadMax>SpreadMin);
		FilterSell(listSell,SpreadMax*(1+SellDelta.Near), SpreadMax*(1+SellDelta.Far),Stocks);
		FilterBuy( listBuy, SpreadMin*(1-BuyDelta.Near),  SpreadMin*(1-BuyDelta.Far),Cash);

		if (listBuy.isEmpty() && listSell.isEmpty()){
			if (Stocks){
				SOrder NewSell;
				NewSell.Price=SpreadMax*(1+SellDelta.Near + 1+SellDelta.Far)/2;
				NewSell.Volume=1;
				listSell << NewSell;
				Stocks-=NewSell.Volume;

			}
			else {
				SOrder NewBuy;
				NewBuy.Price=SpreadMin*(1-BuyDelta.Near+1-BuyDelta.Far)/2;
				NewBuy.Volume=1;
				listBuy << NewBuy;
				Cash-=NewBuy.Price*NewBuy.Volume;
			}
		}
	}


	virtual void Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote)
	{
		if (listSellQuote.isEmpty() || listBuyQuote.isEmpty()) // if glass is empty
			return;
	
		S_MinMax<float> Spread;
		Spread.max=listSellQuote.first().price;
		Spread.min=listBuyQuote.first().price;
		_ASSERTE(Spread.max>Spread.min);
		FilterSell(listSell,Spread.max*(1+SellDelta.Near), Spread.max*(1+SellDelta.Far),Stocks);
		FilterBuy( listBuy, Spread.min*(1-BuyDelta.Near),  Spread.min*(1-BuyDelta.Far),Cash);
		
		if (listBuy.isEmpty() && listSell.isEmpty()){
			if (Stocks){
				SOrder NewSell;
				NewSell.Price=Spread.max*(1+SellDelta.Near + 1+SellDelta.Far)/2;
				NewSell.Volume=1;
				listSell << NewSell;
				Stocks-=NewSell.Volume;

			}
			else {
				SOrder NewBuy;
				NewBuy.Price=Spread.min*(1-BuyDelta.Near+1-BuyDelta.Far)/2;
				NewBuy.Volume=1;
				listBuy << NewBuy;
				Cash-=NewBuy.Price*NewBuy.Volume;
			}
		}
		
	}
	float Profit(float ClosePrice){
		QList<SOrder> listB=listBuy;
		QList<SOrder> listS=listSell;
		float C=Cash;
		int S=Stocks;
		Close(listB,listS,C,S,ClosePrice,Commision);
		return C/ClosePrice;
	}
};


class sqlimport
{
	
public:
	void run(); //The main thread process
	QString outputFile;

////private:
	QFile outfile;
	QTextStream outStream;
};

 int main(int argc, char *argv[])
 {
	//sqlimport ttt;
	//ttt.

	 int jj=sizeof(C_FixedGlass);
	 C_XML_Logger2 loger("sssss");
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
		Sleep(1000);
	}


	//C_TradeMaster TradeMaster;

	//TradeMaster.Init();
	C_S1_Strategy S1[10];
	C_S1_Strategy B1[10];

	S1[0].SellDelta.init(0,0);
	S1[0].BuyDelta .init(0.01/100,0.012/100);

	S1[1].SellDelta.init(0,0);
	S1[1].BuyDelta .init(0.2/100,0.4/100);

	S1[2].SellDelta.init(0,0);
	S1[2].BuyDelta .init(0.3/100,0.6/100);

	S1[3].SellDelta.init(0,0);
	S1[3].BuyDelta .init(0.4/100,0.8/100);

	S1[4].SellDelta.init(0,0);
	S1[4].BuyDelta .init(0.5/100,1.0/100);

	S1[5].SellDelta.init(0,0.1/100);
	S1[5].BuyDelta .init(0.1/100,0.2/100);

	S1[6].SellDelta.init(0,0.1/100);
	S1[6].BuyDelta .init(0.2/100,0.4/100);

	S1[7].SellDelta.init(0,0.1/100);
	S1[7].BuyDelta .init(0.3/100,0.6/100);

	S1[8].SellDelta.init(0,0.1/100);
	S1[8].BuyDelta .init(0.4/100,0.8/100);

	S1[9].SellDelta.init(0,0.1/100);
	S1[9].BuyDelta .init(0.5/100,1.0/100);

	

	size_t idxBegin=0;//Instrument.pData->Ticks.size;
	
	uint fromIndex=0;
	while(idxBegin<Instrument.pData->Ticks.size){
		size_t idxEnd=Instrument.pData->Ticks.NextSecondIndex(idxBegin);
		C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data, idxBegin, idxEnd);
		idxBegin=idxEnd;
		S1[0] << TickPortion;
		//QList<S_Quote> listBuyQuote;
		//QList<S_Quote> listSellQuote;

		S_Quote BuyQuote; 
		S_Quote SellQuote; 
		C_FixedGlass* pGlass = Instrument.pData->Glasses.FindBefore(TickPortion.data[0].datetime,30,fromIndex);
		/*if (pGlass){
			BuyQuote.price=pGlass->buy[0].price;
			BuyQuote.quantity=pGlass->buy[0].quantity;
			BuyQuote.datetime_create=QDateTime::fromTime_t(pGlass->datetime);

			SellQuote.price=pGlass->sell[0].price;
			SellQuote.quantity=pGlass->sell[0].quantity;
			SellQuote.datetime_create=QDateTime::fromTime_t(pGlass->datetime);

			qDebug()<< SellQuote.datetime_create;
			qDebug()<< BuyQuote.datetime_create;
			

			listBuyQuote<< BuyQuote;
			listSellQuote<<SellQuote;
		}*/
		if (pGlass)
			S1[0].Update(pGlass->buy[0].price,pGlass->sell[0].price);
		

		
	}
	
	while(1){
		size_t idxEnd=Instrument.pData->Ticks.size;
		C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data,idxBegin,idxEnd);
		
		idxBegin=idxEnd;
	
		Sleep(1000);

		QList<S_Quote> listBuyQuote;
		QList<S_Quote> listSellQuote;
		Instrument.pData->Quotes.UpdateCurrentQuotes(listBuyQuote,listSellQuote);
		//qDebug() << Instrument.pData->Quotes.toXML(6);
		
	
		for(int i=0; i<1; i++){
			
			S1[i] << TickPortion;
			//qDebug()<< "<glass buy='"  + QString::number(listBuyQuote.first().price) +  "' sell= '" + QString::number(listSellQuote.first().price) + "'>" ;
			/*
			for(int i=0; i<TickPortion.size; i++){
				S_Tick& Tick=TickPortion[i];
				qDebug()<< Tick.toXML() ;//<< "\n";

			}*/

			float price=0;
			
			if (!S1[i].listBuy.isEmpty()){
				price=S1[i].listBuy.first().Price;
				qDebug()<< "<buy cash='"  + QString::number(S1[i].Cash) +  "' stocks= '" + QString::number(S1[i].Stocks) + "' price='" + QString::number(price) +  "'>" ;
			}
			else if (!S1[i].listSell.isEmpty()){
				price=S1[i].listSell.first().Price;
				qDebug()<< "<sell cash='"  + QString::number(S1[i].Cash) +  "' stocks= '" + QString::number(S1[i].Stocks) + "' price='" + QString::number(price) +  "'>" ;
			}
			else 
				qDebug()<< "<idle cash='"  + QString::number(S1[i].Cash) +  "' stocks= '" + QString::number(S1[i].Stocks)  +  "'>" ;


			

			S1[i].Update(listBuyQuote,listSellQuote);

			//QString C=QString::number(S1[i].Cash) ;
			
			
			
			//*S1[i].logger S1[i].logger->flush();
		}
		//for(int i=0; i<10; i++){
		//	B1[i] << TickPortion;
		//	B1[i].Update(listBuyQuote,listSellQuote);
		//}

		
		
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