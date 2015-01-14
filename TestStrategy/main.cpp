#include "shared.h"
#include "MainWindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTDebug>
#include <QThread>
#include <conio.h>

void Sleep( int millisecondsToWait )
{
	QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
}


bool Trade(QList<SOrder>& listTryBuy, QList<SOrder>& listTrySell,  C_SubVector<S_Tick>& vecTicks, QList<SOrder>& listHitBuy, QList<SOrder>& listHitSell)
{
	bool hit=false;
	for(uint i=0; i<vecTicks.size; i++){
		S_Tick& tick=vecTicks[i];
		if (tick.isSell()){
			while(!listTrySell.isEmpty()){
				SOrder& my=listTrySell.first();
				//if (tick.price<my.Price-0.00001)
				//	break;
				if (my.Price<tick.price+0.000001){
					my.datetime=tick.datetime;
					my.id=i+vecTicks.index;
					listHitSell<<my;
					listTrySell.removeFirst();
					hit=true;
				}
				else
					break;
			}
		}
		else {
			while(!listTryBuy.isEmpty()){
				SOrder& my=listTryBuy.first();
				//if (my.Price<tick.price-0.00001)
				//	break;
				if (my.Price>tick.price-0.000001){
					my.datetime=tick.datetime;
					my.id=i+vecTicks.index;		
					listHitBuy<<my;
					listTryBuy.removeFirst();
					hit=true;
				}
				else
					break;
			}
		}
	}
	return hit;
}
void FilterSell(QList<SOrder>& listTrySell, float minThreshold, float maxThreshold, int& Stocks)
{
	while (!listTrySell.isEmpty()){
		SOrder& out=listTrySell.first();
		if (minThreshold<out.Price+0.000001)
			break;
		Stocks+=out.Volume;
		listTrySell.removeFirst();
	}
	while (!listTrySell.isEmpty()){
		SOrder& out=listTrySell.last();
		if (maxThreshold>out.Price-0.000001)
			break;
		Stocks+=out.Volume;
		listTrySell.removeLast();
	}
}

void FilterBuy(QList<SOrder>& listTryBuy, float minThreshold, float maxThreshold, float &Cash)
{
	while (!listTryBuy.isEmpty()){
		SOrder& out=listTryBuy.first();
		if (maxThreshold>out.Price-0.000001)
			break;
		Cash+=out.Volume*out.Price;
		listTryBuy.removeFirst();
	}
	while (!listTryBuy.isEmpty()){
		SOrder& out=listTryBuy.last();
		if (minThreshold<out.Price+0.000001)
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
	Stocks=0;
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
			Commision=float(0.035/100);
			logger=0;
		}
		C_Strategy(QString logname)
		{
			Cash=0;
			Stocks=0;
			Commision=float(0.035/100);
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
			vecTick;
			return false;
		}
		virtual void Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote){
			listBuyQuote;
			listSellQuote;

		}
		virtual float Profit(float ClosePrice){
			ClosePrice;
			return 0;			
		}
};


class C_S1_Strategy:public C_Strategy{
public:
	QList<SOrder> listBuy;
	QList<SOrder> listSell;
	C_XML_Logger* pLogger;
	S_MinMax<float> BuyDelta;
	S_MinMax<float> SellDelta;
	uint Count;
	float CurrentProfit;
	S_Tick* pLastSell;
	S_Tick* pLastBuy;

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
		CurrentProfit=0;
		
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
				*pLogger << "<hit type='B' datetime='" << DateTime2Text(hit.datetime) << "' price='" <<hit.Price<< "' quantity='"<<hit.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks<< "' cnt='" <<Count<< "' tick='"<<hit.id <<"'/>\n";
				pLogger->flush();
			}
			listHitBuy.removeFirst();
		}
		while (!listHitSell.isEmpty()){
			SOrder& hit=listHitSell.first();
			Cash+=hit.Price*hit.Volume*(1-Commision);
			Count++;
			CurrentProfit=Cash/hit.Price*100;
			if (pLogger){
				*pLogger << "<hit type='S' datetime='" <<DateTime2Text(hit.datetime) << "' price='" <<hit.Price<< "' quantity='"<<hit.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks<< "' cnt='" <<Count<< "' tick='"<<hit.id <<"'/>\n";
				pLogger->flush();
			}
			listHitSell.removeFirst();
		}
		return hit;
	}

	virtual void Update(float SpreadMin, float SpreadMax, uint datetime)
	{
		if (SpreadMax<SpreadMin)
			return ;
		_ASSERTE(SpreadMax>=SpreadMin);

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
					*pLogger << "<try type='S' datetime='" <<DateTime2Text(NewSell.datetime) << "' price='" <<NewSell.Price<< "' quantity='"<<NewSell.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks <<"'/>\n";
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
					*pLogger << "<try type='B' datetime='" <<DateTime2Text(NewBuy.datetime) << "' price='" <<NewBuy.Price<< "' quantity='"<<NewBuy.Volume<< "' cash='" <<Cash<< "' stocks='" <<Stocks <<"'/>\n";
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

	//============================================== offline mode =======================================
// 	//QString seccode="VTBR";
// 	//QString seccode="gazp";
// 	//QString seccode="aflt";
// 	 QString seccode="gmkn";
// 	C_Instrument Instrument;
// 	while(!Instrument.Attach(seccode)){
// 		qDebug()<< "No connection to " + seccode;
// 		Sleep(1000);
// 	}
// 
// 	if (Instrument.pData->Quotes.size==0){
// 		qDebug()<< "no quotes!!! press Enter...";
// 		_getch();
// 
// 	}
// 
// for(int s=1; s<10; s++)
// for(int b=1; b<10; b++){
// 	S_MinMax<float> mySell;
// 	S_MinMax<float> myBuy;
// 	mySell.max=1+s*s*0.02/100;
// 	mySell.min=1+s*s*0.01/100;
// 	myBuy.max =1-b*b*0.01/100;
// 	myBuy.min =1-b*b*0.02/100;
// 	C_S1_Strategy Test(	seccode+QString::number(b)+QString::number(s)+".xml",
// 						mySell.max,
// 						mySell.min, 
// 						myBuy.max, 
// 						myBuy.min);
// 
// 	
// 	uint idxStart=Instrument.pData->Ticks.FindAfter("2015-01-08 10:30:00");
// 	uint idxFinal=Instrument.pData->Ticks.FindAfter("2015-01-08 18:30:00");
// 
// 	uint idxBegin=idxStart;
// 	uint idxEnd=0;
// 	uint idxFind;
// 	S_Glass& Glass=Instrument.Glass;
// 	S_EasyTicks& Ticks=Instrument.pData->Ticks;
// 	Glass.toIndex=0;
// 
// 	while (idxEnd<idxFinal){
// 		idxEnd=Instrument.pData->Ticks.NextSecondIndex(idxBegin);
// 		C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data, idxBegin, idxEnd);
// 		
// 
// 		if (Instrument.pData->Quotes.FindBefore(Ticks.data[idxBegin].datetime, Glass.toIndex, idxFind)){
// 			Instrument.pData->Quotes.UpdateGlass(Glass,idxFind,200);			
// 		}
// 
// 		if (!Glass.listBuy.isEmpty() && !Glass.listSell.isEmpty()){
// 			float maxSpread=Glass.listSell.first().price;
// 			float minSpread=Glass.listBuy.first().price;
// 			Test.Update(minSpread, maxSpread,Glass.datetime);
// 		}
// 		Test << TickPortion;
// 
// 		idxBegin=idxEnd;
// 	}
// 	qDebug()<< s << b << Test.Profit(Ticks.data[1000].price)*100 << "%";
// 
// }
// 	qDebug() << "Press to continue...";
// 	_getch();
// 	return 1;
	//==================================================== real time ======================================================
	 
	 
	 QList<QString> listMy; 
	 
	 listMy <<  "GMKN"  <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" <<"ROSN"  <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"   <<"MTSS";

		 			//	<< "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"    
		 			//	 <<"CHMF" <<"URKA";


	QMap<QString,S_TestMatrix*> mapTest;
	QMap<QString,C_Instrument> mapInstrument;
	
	for(int i=0; i<listMy.size();i++){
		QString seccode=listMy[i];
		S_TestMatrix* pTest=new S_TestMatrix(seccode);
		mapTest[seccode]=pTest;

		C_Instrument Instrument;
		while (!Instrument.Attach(seccode)){
			qDebug()<< "No connection to " + seccode;
			Sleep(100);
		}
		
		Instrument.TickInfo.tail=Instrument.pData->Ticks.size;
		mapInstrument[seccode]=Instrument;
	}

	while(mapInstrument["SBER"].pData->Ticks.size==0){
		qDebug()<< "Zzzz...";
		Sleep(1000);
	}
	
	while(1){


		foreach (QString seccode,mapTest.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_TestMatrix& Test=*mapTest[seccode];
		
			size_t idxBegin=Instrument.TickInfo.tail;
			size_t idxEnd=Instrument.pData->Ticks.size;
			C_SubVector<S_Tick> TickPortion(Instrument.pData->Ticks.data, idxBegin, idxEnd);
			Instrument.TickInfo.tail=idxEnd;
	
			//QList<S_Quote> listBuyQuote;
			//QList<S_Quote> listSellQuote;
			//Instrument.pData->Quotes.UpdateCurrentQuotes(listBuyQuote,listSellQuote);
			Instrument.UpdateGlass();
		
			//qDebug() << Instrument.pData->Quotes.toXML(6);
			float MaxProfit=-100;
			float Profit=-100;
			int ij=0;
			for(int i=0; i<10; i++){
				for(int j=0; j<10; j++){
					if (!Instrument.Glass.listBuy.isEmpty() && !Instrument.Glass.listSell.isEmpty()){
						float maxSpread=Instrument.Glass.listSell.first().price;
			 			float minSpread=Instrument.Glass.listBuy.first().price;
						Test.st[i][j]->Update(minSpread, maxSpread, Instrument.Glass.datetime);
						if (TickPortion.size)
							*Test.st[i][j] << TickPortion;
					}
					Profit = Test.st[i][j]->Profit(Instrument.pData->Ticks.Last().price);
					if (Profit>MaxProfit){
						MaxProfit=Profit;
						ij=i*10+j;
					}
					//MaxProfit=MAX(MaxProfit,);
				}
			}
			qDebug()<< seccode << MaxProfit <<ij;
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