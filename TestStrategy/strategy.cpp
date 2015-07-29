#include  "strategy.h"

bool Trade(QList<SOrder>& listTryBuy, QList<SOrder>& listTrySell,  C_SubVector<S_Tick>& vecTicks, QList<SOrder>& listHitBuy, QList<SOrder>& listHitSell)
{
	bool hit=false;
	for(uint i=0; i<vecTicks.size; i++){
		S_Tick& tick=vecTicks[i];
		if (tick.isSell()){
			while(!listTrySell.isEmpty()){
				SOrder& my=listTrySell.first();
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
		if (minThreshold<=out.Price)
			break;
		Stocks+=out.Volume;
		listTrySell.removeFirst();
	}
	while (!listTrySell.isEmpty()){
		SOrder& out=listTrySell.last();
		if (out.Price<=maxThreshold)
			break;
		Stocks+=out.Volume;
		listTrySell.removeLast();
	}
}

void FilterBuy(QList<SOrder>& listTryBuy, float minThreshold, float maxThreshold, float &Cash)
{
	while (!listTryBuy.isEmpty()){
		SOrder& out=listTryBuy.first();
		if (out.Price<=maxThreshold)
			break;
		Cash+=out.Volume*out.Price;
		listTryBuy.removeFirst();
	}
	while (!listTryBuy.isEmpty()){
		SOrder& out=listTryBuy.last();
		if (minThreshold<=out.Price)
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

//=====================================================================

C_S1_Strategy::C_S1_Strategy():C_Strategy(){
	pLogger=0;
	Count=0;
	// 		SellDelta.Far =(1+0.5/100);
	// 		SellDelta.Near=(1+0.1/100);
	// 		BuyDelta.Near =(1-0.1/100);
	// 		BuyDelta.Far  =(1-0.5/100);
}

C_S1_Strategy::C_S1_Strategy(QString logname, float sellmax, float sellmin, float buymax, float buymin)
{
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
	Commision=float(0.11/100);
}

C_S1_Strategy::~C_S1_Strategy()
{
	*pLogger<< "</strategy>\n";
	pLogger->flush();
	delete pLogger;
}

bool C_S1_Strategy::operator << (C_SubVector<S_Tick>& vecTick)
{
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

void C_S1_Strategy::Update(float SpreadMin, float SpreadMax, uint datetime)
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


void C_S1_Strategy::Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote, uint datetime)
{
	if (listSellQuote.isEmpty() || listBuyQuote.isEmpty()) // if glass is empty
		return;

	Update(listBuyQuote.first().price, listSellQuote.first().price, datetime);

}
float C_S1_Strategy::Profit(float ClosePrice){
	QList<SOrder> listB=listBuy;
	QList<SOrder> listS=listSell;
	float C=Cash;
	int S=Stocks;
	Close(listB,listS,C,S,ClosePrice,Commision);
	return C/ClosePrice;
}
// class C_SuperOrder {
// 	float price;
// 	uint  quantity;
// 	uint  quantity_initial;
// 	char  buysell;
// 	uint  datetime_initial;
// 	S_MinMax<float> tunel;
// 	//void Validate(S_MinMax<float> spread){
// //
// 	//}
// 
// };

//=================================================
// class C_S2_Strategy: public C_Strategy{
// 
// 	QList<SOrder> listBuy;
// 	QList<SOrder> listSell;
// 
// 	QList<SOrder> listFastBuy;
// 	QList<SOrder> listFastSell;
// 
// 	S_MinMax<float> BuyDelta;
// 	S_MinMax<float> SellDelta;
// 	S_MinMax<float> FastBuyDelta;
// 	S_MinMax<float> FastSellDelta;
// }
// 
// void C_S2_Strategy::Update(S_MinMax spread, uint datetime){
// 	FilterSell(listSell, spread.max*SellDelta.min, spread.max*SellDelta.max,Stocks);
// 	FilterBuy( listBuy,  spread.min*BuyDelta.min,  spread.min*BuyDelta.max, Cash);
// 
// 	S_MinMax<float> FastSell;
// 	FastSell.max=spread.max*SellDelta.min;
// 	FastSell.min=spread.max*
// 	y1*timeout+(y1-y0)*(t-timeout))/timeout;
// 
// 	FilterSell(listFastSell, spread.max*SellDelta.min (t-timeout)/time (), spread.max*SellDelta.max,Stocks);
// 	FilterBuy( listFastBuy,  spread.min*BuyDelta.min,  spread.min*BuyDelta.max, Cash);
// 
// }


//	pLogger=0;
//	Count=0;
	// 		SellDelta.Far =(1+0.5/100);
	// 		SellDelta.Near=(1+0.1/100);
	// 		BuyDelta.Near =(1-0.1/100);
	// 		BuyDelta.Far  =(1-0.5/100);
//};