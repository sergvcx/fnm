#include "TradeMaster.h"


void  SplitBuySell(QQueue<SDeal>& qDeals, QQueue<SDeal>& qBuyDeals,QQueue<SDeal>& qSellDeals ){
	SDeal Deal;
	while (!qDeals.isEmpty()){
		Deal=qDeals.dequeue();	
		if (Deal.IsSell()){
			qSellDeals<<Deal;
		}
		else 
			qBuyDeals<<Deal;
	}
}
void FirstSecond(QQueue<SDeal>& qDeals, QQueue<SDeal>& qInSecDeals ){
	SDeal Deal;
	if (qDeals.isEmpty())
		return;
	int nTime=qDeals.first().nTime;
	while (!qDeals.isEmpty() && nTime==qDeals.first().nTime){
		Deal=qDeals.dequeue();
		qInSecDeals.enqueue(Deal);
	}
}


C_TradeMaster::C_TradeMaster(){
	Cash=0;
	Stocks=0;
	LockedCash=0;
	LockedStocks=0;
	minDelta=0.2/100;
	maxDelta=1.0/100;
	Commission=0;//0.01/100;
}
void C_TradeMaster::Init(float StartPrice, float MinDelta, float MaxDelta, int nTraps, float fCash, int nStocks){
	numTraps = nTraps;
	minDelta = MinDelta;
	maxDelta = MaxDelta;
	Cash     = fCash;
	Stocks   = nStocks;
	
	GapVolume= (Stocks)/numTraps;
	GapPrice = StartPrice*(maxDelta-minDelta)/numTraps;

	//-minDela
	//
	// -Trap0
	//
	//
	// -Trap1
	//
	//
	// -Trap2
	//
	//-maxDelta

}
//--------------- complete ---------------------

void C_TradeMaster::CompleteFirstOrder(QList<SOrder>& listOrder, int Type){
	if (Type==BUY){
		SOrder& Order=listOrder.first();
		LockedCash-=Order.Volume*Order.Price;
		Cash      -=Order.Volume*Order.Price*Commission;
		Stocks    +=Order.Volume;
		printf("B %d %f Cash=%f LCash=%f Stocks=%d LStocks=%d \n", Order.Volume, Order.Price, Cash, LockedCash, Stocks, LockedStocks);
		listOrder.removeFirst();
	}
	else {
		SOrder& Order=listOrder.first();
		LockedStocks-=Order.Volume;
		Cash        +=Order.Volume*Order.Price;
		Cash        -=Order.Volume*Order.Price*Commission;
		printf("S %d %f Cash=%f LCash=%f Stocks=%d LStocks=%d \n", Order.Volume, Order.Price, Cash, LockedCash, Stocks, LockedStocks);
		listOrder.removeFirst();
	}
}
//--------------- cancle ---------------------
void C_TradeMaster::RemoveFirstOrder(QList<SOrder>& listOrder, int Type)
{
	if (Type=BUY){
		SOrder& Order=listOrder.first();
		LockedCash  -=Order.Price*Order.Volume;
		Cash        +=Order.Price*Order.Volume;
		listOrder.removeFirst();
	}	
	else {
		SOrder& Order=listOrder.first();
		LockedStocks -=Order.Volume;
		Stocks       +=Order.Volume;
		listOrder.removeFirst();
	}
}

void C_TradeMaster::RemoveLastOrder(QList<SOrder>& qOrder, int Type){
	if (Type==BUY){
		SOrder& Order=listMyBuyOrder.last();
		LockedCash  -=Order.Price*Order.Volume;
		Cash        +=Order.Price*Order.Volume;
		listMyBuyOrder.removeLast();
	}
	else {
		SOrder& Order=listMySellOrder.last();
		LockedStocks -=Order.Volume;
		Stocks       +=Order.Volume;
		listMySellOrder.removeLast();
	}
}


//--------------- insert ---------------------

bool C_TradeMaster::InsertFirstOrder(QList<SOrder>& listOrder, int Type, int Volume, float Price)
{
	SOrder Order;
	if (Type==BUY){
		Order.Price = Price;
		Order.Volume= MIN(Volume,Cash/Price);
		if (Order.Volume){
			LockedCash += Order.Price*Order.Volume;
			Cash       -= Order.Price*Order.Volume;
			listOrder.insert(0,Order);
			return 0;
		}
		return 1;
	}
	else {
		Order.Price = Price;
		Order.Volume= MIN(Stocks,Volume);
		if (Order.Volume){
			LockedStocks +=Order.Volume;
			Stocks       -=Order.Volume;
			listOrder.insert(0,Order);
			return 0;
		}
		return 1;
	}
}


bool C_TradeMaster::InsertLastOrder(QList<SOrder>& listOrder, int Type, int Volume, float Price)
{
	SOrder Order;
	if (Type==BUY){
		Order.Price = Price;
		Order.Volume= MIN(Volume,Cash/Price);
		if (Order.Volume){
			LockedCash  +=Order.Price*Order.Volume;
			Cash        -=Order.Price*Order.Volume;
			listOrder.append(Order);
			return 0;
		}
		return 1;
	}
	else {
		Order.Price = Price;
		Order.Volume= MIN(Stocks,Volume);
		if (Order.Volume){
			LockedStocks +=Order.Volume;
			Stocks       -=Order.Volume;
			listOrder.append(Order);
			return 0;
		}
		return 1;
	}
}


//-------------------------------------


 void C_TradeMaster::Parse(QQueue<SDeal>& qDeals){
	QQueue<SDeal>	qSecondDeals;
	QQueue<SDeal>	qBuyDeals;
	QQueue<SDeal>	qSellDeals;
	
	while (!qDeals.isEmpty()){
		FirstSecond(qDeals,qSecondDeals);
		SplitBuySell(qSecondDeals,qBuyDeals,qSellDeals);
		if (!qBuyDeals.isEmpty()){
			LastBuyDeal=qBuyDeals.last();
			while (!listMyBuyOrder.isEmpty() && LastBuyDeal.Price<=listMyBuyOrder.first().Price){
				CompleteFirstOrder(listMyBuyOrder,BUY);
			}
			while (!listHotBuyOrder.isEmpty() && LastBuyDeal.Price<=listHotBuyOrder.first().Price){
				CompleteFirstOrder(listHotBuyOrder,BUY);
			}
			qBuyDeals.clear();
		}
		if (!qSellDeals.isEmpty()){
			LastSellDeal=qSellDeals.last();
			while (!listMySellOrder.isEmpty() && LastSellDeal.Price>=listMySellOrder.first().Price){
				CompleteFirstOrder(listMySellOrder,SELL);
			}
			while (!listHotSellOrder.isEmpty() && LastSellDeal.Price>=listHotSellOrder.first().Price){
				CompleteFirstOrder(listHotSellOrder,SELL);
			}
			qSellDeals.clear();
		}
		MakeOrders();
	}
}

void C_TradeMaster::MakeOrders(){
	//---------------- remove orders ----------------
	while (!listMyBuyOrder.isEmpty() && LastBuyDeal.Price*(1-minDelta)<listMyBuyOrder.first().Price){
		RemoveFirstOrder(listMyBuyOrder,BUY);
	}
	while (!listMyBuyOrder.isEmpty() && LastBuyDeal.Price*(1-maxDelta)>listMyBuyOrder.last().Price){
		RemoveLastOrder(listMyBuyOrder,BUY);
	}

	while (!listMySellOrder.isEmpty() && LastSellDeal.Price*(1+minDelta)>listMySellOrder.first().Price){
		RemoveFirstOrder(listMySellOrder,SELL);
	}
	while (!listMySellOrder.isEmpty() && LastSellDeal.Price*(1+maxDelta)<listMySellOrder.last().Price){
		RemoveLastOrder(listMySellOrder,SELL);
	}

	//------------------ insert order----------------
	if ( listMyBuyOrder.isEmpty())
		InsertFirstOrder(listMyBuyOrder, BUY, GapVolume,    LastBuyDeal.Price*(1-minDelta)-GapPrice/2);
	if (!listMyBuyOrder.isEmpty()) {
		while (LastBuyDeal.Price*(1-minDelta)>listMyBuyOrder.first().Price+GapPrice)
			if (InsertFirstOrder(listMyBuyOrder, BUY,GapVolume,listMyBuyOrder.first().Price+GapPrice))
				break;

		while (LastBuyDeal.Price*(1-maxDelta)<listMyBuyOrder.last().Price-GapPrice)
			if (InsertLastOrder (listMyBuyOrder, BUY, GapVolume,listMyBuyOrder.last().Price-GapPrice))
				break;
	}

	if ( listMySellOrder.isEmpty())
		InsertFirstOrder(listMySellOrder, SELL, GapVolume,    LastSellDeal.Price*(1+minDelta)+GapPrice/2);
	if (!listMySellOrder.isEmpty()) {
		while (LastSellDeal.Price*(1+minDelta)<listMySellOrder.first().Price-GapPrice)
			if (InsertFirstOrder(listMySellOrder, SELL, GapVolume,    listMySellOrder.first().Price-GapPrice))
				break;
		while (LastSellDeal.Price*(1+maxDelta)>listMySellOrder.last().Price+GapPrice )
			if (InsertLastOrder(listMySellOrder, SELL, GapVolume,     listMySellOrder.last().Price+GapPrice))
				break;
	}

}
void C_TradeMaster::Close(){
	listMyBuyOrder.clear();
	Cash+=LockedCash;
	LockedCash=0;

	listMySellOrder.clear();
	Cash+=(Stocks+LockedStocks)*LastSellDeal.Price;
	Cash-=(Stocks+LockedStocks)*LastSellDeal.Price*Commission;
	LockedStocks=0;

}

