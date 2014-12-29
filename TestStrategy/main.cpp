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
void FilterSell(QList<SOrder>& listSell, float nearThreshold, float farThreshold, int& Stocks)
{
	while (!listSell.isEmpty() && nearThreshold>listSell.first().Price){
		Stocks+=listSell.first().Volume;
		listSell.removeFirst();
	}
	while (!listSell.isEmpty() && farThreshold<listSell.last().Price){
		Stocks+=listSell.first().Volume;
		listSell.removeLast();
	}
}

void FilterBuy(QList<SOrder>& listBuy, float nearThreshold, float farThreshold, float &Cash)
{
	while (!listBuy.isEmpty() && nearThreshold<listBuy.first().Price){
		Cash+=listBuy.first().Volume*listBuy.first().Price;
		listBuy.removeFirst();
	}
	while (!listBuy.isEmpty() && farThreshold>listBuy.last().Price){
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
		C_Strategy()
		{
			Cash=0;
			Stocks=0;
			Commision=0.035/100;
		}
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
	T near;
	T far;
	S_NearFar(){
		near=0;
		far=0;
	}
	S_NearFar(T Near, T Far){
		near=Near;
		far=Far;
	}
	void init(T Near, T Far){
		near=Near;
		far=Far;
	}
};


class C_S1_Strategy:public C_Strategy{
public:
	QList<SOrder> listBuy;
	QList<SOrder> listSell;
	
	S_NearFar<float> BuyDelta;
	S_NearFar<float> SellDelta;

	C_S1_Strategy():C_Strategy(){
		SellDelta.far =(1+0.5/100);
		SellDelta.near=(1+0.1/100);
		BuyDelta.near =(1-0.1/100);
		BuyDelta.far  =(1-0.5/100);
	}

	virtual void operator << (C_SubVector<S_Tick>& vecTick){
		Trade(listBuy,listSell,vecTick,Cash,Stocks,Commision);
		
	}
	virtual void Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote)
	{
		if (listSellQuote.isEmpty() || listBuyQuote.isEmpty()) // if glass is empty
			return;
	
		S_MinMax<float> Spread;
		Spread.max=listSellQuote.first().price;
		Spread.min=listBuyQuote.first().price;
		_ASSERTE(Spread.max>Spread.min);
		FilterSell(listSell,Spread.max*SellDelta.near, Spread.max*SellDelta.far,Stocks);
		FilterBuy( listBuy, Spread.min*BuyDelta.near,  Spread.min*BuyDelta.far,Cash);
		if (Stocks==0){
			if (listBuy.isEmpty()){
				SOrder NewBuy;
				NewBuy.Price=(Spread.min*BuyDelta.near+Spread.min*BuyDelta.far)/2;
				NewBuy.Volume=1;
				listBuy << NewBuy;
				Cash-=NewBuy.Price*NewBuy.Volume;
			}
		}
		else {
			if (listSell.isEmpty()){
				SOrder NewSell;
				NewSell.Price=(Spread.max*SellDelta.near + Spread.max*SellDelta.far)/2;
				NewSell.Volume=1;
				listSell << NewSell;
				Stocks-=NewSell.Volume;
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
	C_S1_Strategy S1[10];
	C_S1_Strategy B1[10];

	S1[0].SellDelta.init(0,0);
	S1[0].BuyDelta .init(0.1/100,0.2/100);

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

	

	size_t idxBegin=Instrument.pData->Ticks.size;
	
	
	while(1){
		size_t idxEnd=Instrument.pData->Ticks.size;
		C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data,idxBegin,idxEnd);
		for(int i=0; i<TickPortion.size; i++){
			S_Tick& Tick=TickPortion[i];
			qDebug()<< Tick.toXML() ;//<< "\n";
			
		}
		idxBegin=idxEnd;
	
		QList<S_Quote> listBuyQuote;
		QList<S_Quote> listSellQuote;
		Instrument.pData->Quotes.UpdateCurrentQuotes(listBuyQuote,listSellQuote);
		//qDebug() << Instrument.pData->Quotes.toXML(6);
		//Sleep(1000);
	
		for(int i=0; i<10; i++){
			S1[i] << TickPortion;
			S1[i].Update(listBuyQuote,listSellQuote);
		}
		for(int i=0; i<10; i++){
			B1[i] << TickPortion;
			B1[i].Update(listBuyQuote,listSellQuote);
		}

		
		
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