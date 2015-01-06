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

bool Trade(QList<SOrder>& listTryBuy, QList<SOrder>& listTrySell,  C_SubVector<S_Tick>& vecTicks, QList<SOrder>& listHitBuy, QList<SOrder>& listHitSell)
{
	bool hit=false;
	for(uint i=0; i<vecTicks.size; i++){
		S_Tick& tick=vecTicks[i];
		if (tick.isSell()){
			while(!listTrySell.isEmpty()){
				SOrder& my=listTrySell.first();
				if (tick.price<my.Price-0.001)
					break;
				my.datetime=tick.datetime;
				listHitSell<<my;
				listTrySell.removeFirst();
				hit=true;
			}
		}
		else {
			while(!listTryBuy.isEmpty()){
				SOrder& my=listTryBuy.first();
				if (my.Price<tick.price-0.001)
					break;
				listHitBuy<<my;
				listTryBuy.removeFirst();
				hit=true;
			}
		}
	}
	return hit;
}
void FilterSell(QList<SOrder>& listTrySell, float minThreshold, float maxThreshold, int& Stocks)
{
	while (!listTrySell.isEmpty()){
		SOrder& out=listTrySell.first();
		if (minThreshold<out.Price+0.0001)
			break;
		Stocks+=out.Volume;
		listTrySell.removeFirst();
	}
	while (!listTrySell.isEmpty()){
		SOrder& out=listTrySell.last();
		if (maxThreshold>out.Price-0.0001)
			break;
		Stocks+=out.Volume;
		listTrySell.removeLast();
	}
}

void FilterBuy(QList<SOrder>& listTryBuy, float minThreshold, float maxThreshold, float &Cash)
{
	while (!listTryBuy.isEmpty()){
		SOrder& out=listTryBuy.first();
		if (maxThreshold>out.Price-0.0001)
			break;
		Cash+=out.Volume*out.Price;
		listTryBuy.removeFirst();
	}
	while (!listTryBuy.isEmpty()){
		SOrder& out=listTryBuy.last();
		if (minThreshold<out.Price+0.0001)
			break;
		Cash+=out.Volume*out.Price;
		listTryBuy.removeLast();
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
		virtual bool operator << (C_SubVector<S_Tick>& vecTick){

			return false;
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
// 
// template <class T> struct S_NearFar {
// 	T Near;
// 	T Far;
// 	S_NearFar(){
// 		//Near=0;
// 		//Far=0;
// 	}
// 	S_NearFar(T _Near, T _Far){
// 		Near=_Near;
// 		Far=_Far;
// 	}
// 	void init(T _Near, T _Far){
// 		Near=_Near;
// 		Far=_Far;
// 	}
// };


class C_S1_Strategy:public C_Strategy{
public:
	QList<SOrder> listBuy;
	QList<SOrder> listSell;
	C_XML_Logger* pLogger;
	S_MinMax<float> BuyDelta;
	S_MinMax<float> SellDelta;
	uint Count;

	C_S1_Strategy():C_Strategy(){
		pLogger=0;
		Count=0;
// 		SellDelta.Far =(1+0.5/100);
// 		SellDelta.Near=(1+0.1/100);
// 		BuyDelta.Near =(1-0.1/100);
// 		BuyDelta.Far  =(1-0.5/100);
	}
	C_S1_Strategy(QString logname, float sellmax, float sellmin, float buymax, float buymin){
		pLogger=new C_XML_Logger(logname,LOGGER_WRITE);
		pLogger->Header();
		*pLogger<< "<strategy>\n";
		*pLogger << QString::number(sellmax) <<"\n"<<  QString::number(sellmin) <<"\n"<< QString::number(buymax) <<"\n"<< QString::number(buymin)<<"\n";
		pLogger->flush();
		SellDelta.max=sellmax;
		SellDelta.min=sellmin;
		BuyDelta.max=buymax;
		BuyDelta.min=buymin;
		Count=0;
		
	}
	~C_S1_Strategy(){
		*pLogger<< "</strategy>\n";
		pLogger->flush();
		delete pLogger;
	}

	bool operator << (C_SubVector<S_Tick>& vecTick){
		QList<SOrder> listHitBuy;
		QList<SOrder> listHitSell;
		bool hit=Trade(listBuy,listSell,vecTick,listHitBuy,listHitSell);//Cash,Stocks,Commision);
		while (!listHitBuy.isEmpty()){
			SOrder& hit=listHitBuy.first();
			Cash-=hit.Price*hit.Volume*Commision;
			Stocks+=hit.Volume;
			Count++;
			if (pLogger){
				*pLogger << "<hit type='B' datetime='" <<hit.datetime << "' price='" <<hit.Price<< "' quantity='"<<hit.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks<< "' cnt='" <<Count<< "' />\n";
				pLogger->flush();
			}
			listHitBuy.removeFirst();
		}
		while (!listHitSell.isEmpty()){
			SOrder& hit=listHitSell.first();
			Cash+=hit.Price*hit.Volume*(1-Commision);
			Count++;
			if (pLogger){
				*pLogger << "<hit type='S' datetime='" <<hit.datetime << "' price='" <<hit.Price<< "' quantity='"<<hit.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks<< "' cnt='" <<Count<< "' />\n";
				pLogger->flush();
			}
			listHitSell.removeFirst();
		}
		return hit;
	}

	virtual void Update(float SpreadMin, float SpreadMax, uint datetime)
	{
		_ASSERTE(SpreadMax>SpreadMin);
		FilterSell(listSell,SpreadMax*SellDelta.min, SpreadMax*SellDelta.max,Stocks);
		FilterBuy( listBuy, SpreadMin*BuyDelta.min,  SpreadMin*BuyDelta.max, Cash);

		if (listBuy.isEmpty() && listSell.isEmpty()){
			if (Stocks){
				SOrder NewSell;
				NewSell.Price=SpreadMax*(SellDelta.min + SellDelta.max)/2;
				NewSell.Volume=1;
				NewSell.datetime=datetime;

				listSell << NewSell;
				Stocks-=NewSell.Volume;
				if (pLogger){
					*pLogger << "<try type='S' datetime='" <<NewSell.datetime << "' price='" <<NewSell.Price<< "' quantity='"<<NewSell.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks <<"'/>\n";
					pLogger->flush();
				}
			}
			else {
				SOrder NewBuy;
				NewBuy.Price=SpreadMin*(BuyDelta.min+BuyDelta.max)/2;
				NewBuy.Volume=1;
				NewBuy.datetime=datetime;
				listBuy << NewBuy;
				Cash-=NewBuy.Price*NewBuy.Volume;
				if (pLogger){
					*pLogger << "<try type='B' datetime='" <<NewBuy.datetime << "' price='" <<NewBuy.Price<< "' quantity='"<<NewBuy.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks <<"'/>\n";
					pLogger->flush();
				}
			}
		}
	}


	virtual void Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote, uint datetime)
	{
		if (listSellQuote.isEmpty() || listBuyQuote.isEmpty()) // if glass is empty
			return;
	
		Update(listBuyQuote.first().price, listSellQuote.first().price, datetime);
		
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


struct S_TestMatrix {
	C_S1_Strategy* st[10][10];
	S_TestMatrix(QString seccode){
		for(int i=0; i<10; i++){
			for(int j=0; j<10; j++){
				st[i][j]=new C_S1_Strategy(seccode+QString::number(i)+QString::number(j)+".xml",
					1+i*i*0.02/100,
					1+i*i*0.01/100,
					1-j*j*0.01/100,
					1-j*j*0.02/100);
			}
		}
	}
	~S_TestMatrix(){
		for(int i=0; i<10; i++){
			for(int j=0; j<10; j++){
				delete st[i][j];
			}
		}
	}
};
 int main(int argc, char *argv[])
 {
	
	 setlocale(LC_ALL, "Russian");
	
	 QApplication app(argc, argv);


	QString seccode="GMKN";
	C_Instrument Instrument;
	while(!Instrument.Attach(seccode)){
		qDebug()<< "No connection to " + seccode;
		Sleep(1000);
	}


	uint idxBegin=Instrument.pData->Ticks.FindAfter("2015-01-06 10:30:00");
	uint idxEnd  =Instrument.pData->Ticks.FindAfter("2015-01-06 18:45:00");
	//for()
	//..C_S1_Strategy S("my.xml");

	//C_TradeMaster TradeMaster;

	//TradeMaster.Init();
	//C_S1_Strategy S("my.xml");
	//S.pLogger->flush();
	//S_TestMatrix("my");
	//return 1;
	//C_S1_Strategy S1[10];
	
		//[10];
	//C_S1_Strategy B1[10];


	//S_TestMatrix *S[10];
	
	 QList<QString> listMy; 
	 
	 listMy <<  "GMKN"  <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" ;
		 			//	<< "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"   <<"MTSS"  
		 			//	<<"ROSN"  <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";


	QMap<QString,S_TestMatrix*> mapTest;
	QMap<QString,C_Instrument> mapInstrument;
	
	for(int i=0; i<listMy.size();i++){
		QString seccode=listMy[i];
		S_TestMatrix* pTest=new S_TestMatrix(seccode);
		mapTest[seccode]=pTest;

		C_Instrument Instrument;
		if (!Instrument.Attach(seccode)){
			qDebug()<< "No connection to " + seccode;
			return 1;
		}
		
		Instrument.TickInfo.tail=Instrument.pData->Ticks.size;
		mapInstrument[seccode]=Instrument;
	}

	
	
	while(1){

		foreach (QString seccode,mapTest.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_TestMatrix& Test=*mapTest[seccode];
		
			size_t idxBegin=Instrument.TickInfo.tail;
			size_t idxEnd=Instrument.pData->Ticks.size;
			C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data, idxBegin, idxEnd);
			Instrument.TickInfo.tail=idxEnd;
	
			QList<S_Quote> listBuyQuote;
			QList<S_Quote> listSellQuote;
			Instrument.pData->Quotes.UpdateCurrentQuotes(listBuyQuote,listSellQuote);
		
			qDebug() << Instrument.pData->Quotes.toXML(6);
			for(int i=0; i<10; i++){
				for(int j=0; j<10; j++){

					Test.st[i][j]->Update(listBuyQuote,listSellQuote,QDateTime::currentDateTime().toTime_t());
					if (TickPortion.size)
						*Test.st[i][j] << TickPortion;
					
				}
			}
		}
		Sleep(1000);
	}


	 return 1;
	 
 }


 
	
// 
// 	size_t idxBegin=0;//Instrument.pData->Ticks.size;
// 	
// 	uint fromIndex=0;
// 	while(idxBegin<Instrument.pData->Ticks.size){
// 		
// 		size_t idxEnd=Instrument.pData->Ticks.NextSecondIndex(idxBegin);
// 		C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data, idxBegin, idxEnd);
// 		idxBegin=idxEnd;
// 		S1[0] << TickPortion;
// 		//QList<S_Quote> listBuyQuote;
// 		//QList<S_Quote> listSellQuote;
// 
// 		S_Quote BuyQuote; 
// 		S_Quote SellQuote; 
// 		C_FixedGlass* pGlass = Instrument.pData->Glasses.FindBefore(TickPortion.data[0].datetime,30,fromIndex);
// 		/*if (pGlass){
// 			BuyQuote.price=pGlass->buy[0].price;
// 			BuyQuote.quantity=pGlass->buy[0].quantity;
// 			BuyQuote.datetime_create=QDateTime::fromTime_t(pGlass->datetime);
// 
// 			SellQuote.price=pGlass->sell[0].price;
// 			SellQuote.quantity=pGlass->sell[0].quantity;
// 			SellQuote.datetime_create=QDateTime::fromTime_t(pGlass->datetime);
// 
// 			qDebug()<< SellQuote.datetime_create;
// 			qDebug()<< BuyQuote.datetime_create;
// 			
// 
// 			listBuyQuote<< BuyQuote;
// 			listSellQuote<<SellQuote;
// 		}*/
// 		
// 		if (Instrument.)
// 			S1[0].Update(pGlass->buy[0].price,pGlass->sell[0].price);
// 		
// 
// 		
// 	}