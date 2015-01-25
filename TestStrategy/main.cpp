#include "shared.h"
#include "MainWindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTDebug>
#include <QThread>
#include <conio.h>
#include "strategy.h"
#include "testconnector.h"

void Sleep( int millisecondsToWait )
{
	QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
}



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
uint MinMaxInSecond(C_SubVector<S_Tick>& vec, uint idx, S_MinMax<float>& price, char buysell){
	S_Tick* pTick=&vec[idx];
	if (!pTick->isInSecond()){
		return false;
	}
	bool isSell=(buysell=='S');
	price.max=0;
	price.min=999999;
	uint dt=pTick->datetime;
	for( ;pTick->datetime==dt && idx<vec.size; idx++,pTick++){
		if (pTick->isSell()==isSell){
			price.min=MIN(price.min,pTick->price);
			price.max=MAX(price.max,pTick->price);
		}
	}
	if (price.max==0)
		return 0;
	else return idx;
}

uint  FindAfter(C_SubVector<S_Tick>& vec, uint idxFrom, uint after_timeout, char buysell){
	uint datetimeout = vec[idxFrom].datetime+after_timeout;
	bool isSell=(buysell=='S');
	for(uint i=idxFrom; i<vec.size; i++){
		S_Tick& tick=vec[i];
		if (tick.isSell()!=isSell)
			continue;
		if (tick.datetime>=datetimeout)
			return i;
	}
	return 0;
}


uint  FindGtPrice(C_SubVector<S_Tick>& vec, uint idxFrom, float price, uint timeout, char buysell){
	uint datetimeout = vec[idxFrom].datetime+timeout;
	bool isSell=(buysell=='S');
	for(uint i=idxFrom; i<vec.size; i++){
		S_Tick& tick=vec[i];
		if (tick.datetime>datetimeout)
			return 0;
		if (tick.isSell()!=isSell)
			continue;
		if (tick.price>=price)
			return i;
	}
	return 0;
}


uint  FindLtPrice(C_SubVector<S_Tick>& vec, uint idxFrom, float price, uint timeout, char buysell){
	uint datetimeout = vec[idxFrom].datetime+timeout;
	bool isSell=(buysell=='S');
	for(uint i=idxFrom; i<vec.size; i++){
		S_Tick& tick=vec[i];
		if (tick.datetime>datetimeout)
			return 0;
		if (tick.isSell()!=isSell)
			continue;
		if (tick.price<=price)
			return i;
	}
	return 0;
}

struct S_FlashStatistics
{
	QTime minTime;
	QTime maxTime;
	QDate minDate;
	QDate maxDate;

	float enterDeviation;
	float returnDeviation;
	uint  countSell;
	uint  countBuy;

	uint countGoodSell;
	uint countGoodBuy;
	//char buysell;
	float profitSell;
	float profitBuy;
	float lossSell;
	float lossBuy;
	float lossComission;
	float Commision;

	S_FlashStatistics(){
		lossComission=0;
		Commision=float(0.04/100);
		countSell=0;
		countBuy=0;

		countGoodSell=0;
		countGoodBuy=0;

		profitSell=0;
		profitBuy=0;
		lossSell=0;
		lossBuy=0;

		minTime=Text2Time("10:10:00");
		maxTime=Text2Time("18:30:00");
		
	}
	void operator << (C_SubVector<S_Tick>& vec){
		//S_Tick* baseTick=0;
		for(uint i=0; i<vec.size; i++){
			S_Tick& tick=vec.data[i];
			QDateTime dt; dt=QDateTime::fromTime_t(tick.datetime);
			//if (dt.date()<minDate || dt.date()>maxDate)
			//	continue;
			//qDebug() << dt.time().toString();
			//qDebug() << minTime.toString();
			if (dt.time()<minTime || dt.time()>maxTime)
				continue;
			if (tick.isFirstInSecond()){
				S_MinMax<float> price;
				uint idx=MinMaxInSecond(vec,i,price,'B');
				if (idx){
					if (price.max*(1-enterDeviation)>price.min){	// значит произошeл Enter-Buy
						countBuy++;
						
						if (FindGtPrice(vec,idx,price.max*(1-returnDeviation),60,'S')){ // Return-Sell is occurred
							countGoodBuy++;
							profitBuy+= -price.max*(1-enterDeviation)+price.max*(1-returnDeviation);
						}
						else{
							idx=FindAfter(vec,idx,60,'S');
							if (idx)
								lossBuy+= -price.max*(1-enterDeviation)+ vec[idx].price;
						}
						lossComission+=	Commision*price.min*2;

					}
				}
				idx=MinMaxInSecond(vec,i,price,'S');
				if (idx){
					if (price.min*(1+enterDeviation)<price.max){	// значит произошeл Enter-Sell
						countSell++;
						if (FindLtPrice(vec,idx,price.min*(1+returnDeviation),60,'B')){	// если произошел return-Buy
							countGoodSell++;
							profitSell+= price.min*(1+enterDeviation) - price.min*(1+returnDeviation);
						}
						else {
							idx=FindAfter(vec,idx,60,'B');
							lossSell+=  price.min*(1+enterDeviation)-vec[idx].price;
						}
						lossComission+=	Commision*price.min*2;
					}
				}
			}
		}
	}

};

uint test_datetime;
 int main(int argc, char *argv[])
 {
	
	 

	 setlocale(LC_ALL, "Russian");
	
	 QApplication app(argc, argv);

	 
	 

	 C_Instrument Instrument;
	 while (!Instrument.Attach("gmkn")){
		 Sleep(1);
	 }
	 
	 int counter=0;
	for (float fast=0.01/100; fast< 0.1/100; fast+=0.01/100)
		for(float slow=0.01/100; slow< 0.5/100; slow+=0.05/100){
			counter++;
			 S_MinMax<float> FastDeviation(float(fast),float(2*fast));
			 S_MinMax<float> SlowDeviation(float(slow),float(2*slow));
			 C_YoStrategy Strategy(Instrument,SlowDeviation,FastDeviation,0,0);
			 C_TestConnector TestConnector(Instrument);
			 TestConnector.Trade(Strategy);
			 TestConnector.Close(Strategy);
			 qDebug() << fast << slow << Strategy.cash << Strategy.commission << Strategy.cash - Strategy.commission << Instrument.pData->Trades.head;
		}
 	 return 1;
	 
// 	 C_Instrument Instrument;
// 	 QString seccode="gmkn";
// 	 while (!Instrument.Attach(seccode)){
// 		 qDebug()<< "No connection to " + seccode;
// 		 Sleep(100);
// 	 }
// 
// 	 
// 	 //size_t idxBegin=Instrument.TickInfo.tail;
// 	 //size_t idxEnd  =Instrument.pData->Ticks.size;
// 	 C_SubVector<S_Tick> TickData=Instrument.TickSubVector();
// 	
// 	 for(float dev=0.01; dev <0.5; dev+=0.01){
// 		S_FlashStatistics stat;
// 		stat.enterDeviation=dev/100;
// 		stat.returnDeviation=stat.enterDeviation*0.0;
// 		stat<< TickData;
// 		float profit = stat.profitBuy + stat.lossBuy + stat.profitSell + stat.lossSell ;
// 		
// 		qDebug()<< dev << "|B="<< stat.countBuy << "/" << stat.countGoodBuy << "|S=" << stat.countSell <<"/" << stat.countGoodSell << "|B" << stat.profitBuy << stat.lossBuy << "|S" << stat.profitSell << stat.lossSell  << "||" << profit << stat.lossComission << "=" << profit - stat.lossComission ;// dev*(stat.countBuy+stat.countSell);
// 	//	printf();
// 	 }
	 

	 return 1;

	//============================================== offline mode =======================================
// 	//QString seccode="VTBR";
// 	//QString seccode="gazp";
// 	//QString seccode="aflt";
// 	//QString seccode="gmkn";
// 	 QString seccode="sber";
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
// 	mySell.max=1+s*s*0.02/100/2;
// 	mySell.min=1+s*s*0.01/100/2;
// 	myBuy.max =1-b*b*0.01/100/2;
// 	myBuy.min =1-b*b*0.02/100/2;
// 	C_S1_Strategy Test(	seccode+QString::number(b)+QString::number(s)+".xml",
// 						mySell.max,
// 						mySell.min, 
// 						myBuy.max, 
// 						myBuy.min);
// 
// 	
// 	uint idxStart=Instrument.pData->Ticks.FindAfter("2015-01-15 10:30:00");
// 	uint idxFinal=Instrument.pData->Ticks.FindAfter("2015-01-15 18:30:00");
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
// 		if (Instrument.pData->Quotes.FindBefore(Ticks.data[idxBegin].datetime+2, Glass.toIndex, idxFind)){
// 			Instrument.pData->Quotes.UpdateGlass(Glass,idxFind,200);			
// 		}
// 
// 		if (!Glass.listBuy.isEmpty() && !Glass.listSell.isEmpty()){
// 			float maxSpread=Glass.listSell.first().price;
// 			float minSpread=Glass.listBuy.first().price;
// 			Test.Update(minSpread, maxSpread,Ticks.data[idxBegin].datetime);
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
		
		Instrument.TickInfo.tail=Instrument.pData->Ticks.head;
		mapInstrument[seccode]=Instrument;
	}

	while(mapInstrument["SBER"].pData->Ticks.head==0){
		qDebug()<< "Zzzz...";
		Sleep(1000);
	}
	
	while(1){


		foreach (QString seccode,mapTest.keys()){
			C_Instrument& Instrument=mapInstrument[seccode];
			S_TestMatrix& Test=*mapTest[seccode];
		
			size_t idxBegin=Instrument.TickInfo.tail;
			size_t idxEnd=Instrument.pData->Ticks.head;
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
						Test.st[i][j]->Update(minSpread, maxSpread, TickPortion.data[TickPortion.size-1].datetime);
						if (TickPortion.size)
							*Test.st[i][j] << TickPortion;
					}
					Profit = Test.st[i][j]->Profit(Instrument.pData->Ticks.refLast().price);
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