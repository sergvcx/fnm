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
void ExtractInSecondTicks(QQueue<SDeal>& qDeals, QQueue<SDeal>& qInSecDeals ){
	SDeal Deal;
	if (qDeals.isEmpty())
		return;
	int nTime=qDeals.first().nTime;
	while (!qDeals.isEmpty() && nTime==qDeals.first().nTime){
		Deal=qDeals.dequeue();
		qInSecDeals.enqueue(Deal);
	}
}
bool GetBuyQuote(QQueue<SDeal>& qDeals, float& BuyQoute){
	for(int i=0;i<qDeals.size();i++){
		if (qDeals[i].IsBuy()){
			BuyQoute=qDeals[i].Price;
			return true;
		}
	}
	return false;
}
bool GetSellQuote(QQueue<SDeal>& qDeals, float& SellQoute){
	for(int i=0;i<qDeals.size();i++){
		if (qDeals[i].IsSell()){
			SellQoute=qDeals[i].Price;
			return true;
		}
	}
	return false;
}

C_TradeMaster::C_TradeMaster(){
	Cash=0;
	Stocks=0;
	LockedCash=0;
	LockedStocks=0;
	TrapNet.minDelta=0.2/100;
	TrapNet.maxDelta=1.0/100;
	Commission=0.07/100;//0.01/100;
	
}

void C_TradeMaster::Init(float StartPrice, float fCash, int nStocks, float minHotDelta, float maxHotDelta, float minTrapDelta, float maxTrapDelta){
	TrapNet.numTraps = nStocks;
	TrapNet.minDelta = minTrapDelta*StartPrice;
	TrapNet.maxDelta = maxTrapDelta*StartPrice;
//	TrapNet.GapVolume= nStocks/TrapNet.numTraps;
//	TrapNet.GapPrice = (TrapNet.maxDelta-TrapNet.minDelta)/TrapNet.numTraps;
	TrapNet.Status   = TRAP;

	HotNet.numTraps = nStocks;
	HotNet.minDelta = minHotDelta*StartPrice;
	HotNet.maxDelta = maxHotDelta*StartPrice;
	//HotNet.GapVolume= nStocks/HotNet.numTraps;
//	HotNet.GapPrice = (HotNet.maxDelta-HotNet.minDelta)/HotNet.numTraps;
	HotNet.Status  = HOT;

	Cash=0;
	Stocks=0;
	LockedCash=0;
	LockedStocks=0;
	Cash     = fCash;
	Stocks   = nStocks;
	Counter=0;
	
	//-minDela
	//
	// -Trap0
	//
	//--------
	//
	// -Trap1
	//
	//-------
	//
	// -Trap2
	//
	//-maxDelta

}
//--------------- complete ---------------------

void C_TradeMaster::CompleteFirstOrder(QList<SOrder>& listOrder, int Type){
	if (Type&BUY){
		Counter++;
		SOrder& Order=listOrder.first();
		LockedCash-=Order.Volume*Order.Price;
		//_ASSERTE(LockedCash>=0);
		Cash      -=Order.Volume*Order.Price*Commission;
		
		Stocks    +=Order.Volume;
		//printf("B-%d %d %f Cash=%f LCash=%f Stocks=%d LStocks=%d \n",Order.Status, Order.Volume, Order.Price, Cash, LockedCash, Stocks, LockedStocks);
		listOrder.removeFirst();
	}
	else {
		SOrder& Order=listOrder.first();
		LockedStocks-=Order.Volume;
		//_ASSERTE(LockedStocks>=0);
		Cash        +=Order.Volume*Order.Price;
		Cash        -=Order.Volume*Order.Price*Commission;
		//printf("S-%d %d %f Cash=%f LCash=%f Stocks=%d LStocks=%d \n",Order.Status, Order.Volume, Order.Price, Cash, LockedCash, Stocks, LockedStocks);
		listOrder.removeFirst();
	}
}
//--------------- cancle ---------------------
void C_TradeMaster::RemoveFirstOrder(QList<SOrder>& listOrder, int Type)
{
	if (Type&BUY){
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

void C_TradeMaster::RemoveLastOrder(QList<SOrder>& listOrder, int Type){
	if (Type&BUY){
		SOrder& Order=listOrder.last();
		LockedCash  -=Order.Price*Order.Volume;
		Cash        +=Order.Price*Order.Volume;
		listOrder.removeLast();
	}
	else {
		SOrder& Order=listOrder.last();
		LockedStocks -=Order.Volume;
		Stocks       +=Order.Volume;
		listOrder.removeLast();
	}
}


//--------------- insert ---------------------

bool C_TradeMaster::InsertFirstOrder(QList<SOrder>& listOrder, int Type, int Volume, float Price)
{
	SOrder Order;
	if (Type&BUY){
		Order.Price = Price;
		Order.Volume= MIN(Volume,Cash/Price);
		if (Order.Volume){
			LockedCash += Order.Price*Order.Volume;
			Cash       -= Order.Price*Order.Volume;
			Order.Status=Type;
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
			Order.Status=Type;
			listOrder.insert(0,Order);
			return 0;
		}
		return 1;
	}
}


bool C_TradeMaster::InsertLastOrder(QList<SOrder>& listOrder, int Type, int Volume, float Price)
{
	SOrder Order;
	if (Type&BUY){
		Order.Price = Price;
		Order.Volume= MIN(Volume,Cash/Price);
		_ASSERTE(Order.Volume>=0);
		if (Order.Volume){
			LockedCash  +=Order.Price*Order.Volume;
			Cash        -=Order.Price*Order.Volume;
			Order.Status=Type;
			listOrder.append(Order);
			return 0;
		}
		return 1;
	}
	else {
		Order.Price = Price;
		Order.Volume= MIN(Stocks,Volume);
		_ASSERTE(Order.Volume>=0);
		if (Order.Volume){
			LockedStocks +=Order.Volume;
			Stocks       -=Order.Volume;
			Order.Status=Type;
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

void C_TradeMaster::CutNet(S_Net& Net)
{
	while (!Net.listBuy.isEmpty() && (LastBuyDeal.Price-Net.minDelta)<Net.listBuy.first().Price){
		RemoveFirstOrder(Net.listBuy,BUY);
 	}
	while (!Net.listBuy.isEmpty() && (LastBuyDeal.Price-Net.maxDelta)>Net.listBuy.last().Price){
		RemoveLastOrder (Net.listBuy,BUY);
	}
	while (!Net.listSell.isEmpty() && (LastSellDeal.Price+Net.minDelta)>Net.listSell.first().Price){
		RemoveFirstOrder(Net.listSell,SELL);
	}
	while (!Net.listSell.isEmpty() && (LastSellDeal.Price+Net.maxDelta)<Net.listSell.last().Price){
		RemoveLastOrder (Net.listSell,SELL);
	}
}

void C_TradeMaster::SewNet(S_Net& Net)
{

	if (Stocks==0 && LockedStocks==0)
		if ( Net.listBuy.isEmpty())
			InsertFirstOrder(Net.listBuy, BUY|Net.Status,1,    (LastBuyDeal.Price-(Net.minDelta+Net.maxDelta)/2));//-Net.GapPrice/2);

	if (Stocks==1)
		if ( Net.listSell.isEmpty())
			InsertFirstOrder(Net.listSell, SELL|Net.Status, 1,   SellQuote);

	/*
	if ( Net.listBuy.isEmpty())
		InsertFirstOrder(Net.listBuy, BUY|Net.Status, Net.GapVolume,    (LastBuyDeal.Price-Net.minDelta)-Net.GapPrice/2);
	if (!Net.listBuy.isEmpty()) {
		while (LastBuyDeal.Price-Net.minDelta>                           Net.listBuy.first().Price+Net.GapPrice)
			if (InsertFirstOrder(Net.listBuy, BUY|Net.Status, Net.GapVolume, Net.listBuy.first().Price+Net.GapPrice))
				break;

		while (Net.numTraps>Net.listBuy.size())
			if (InsertLastOrder (Net.listBuy, BUY|Net.Status, Net.GapVolume, Net.listBuy.last().Price-Net.GapPrice))
				break;
	}

	if ( Net.listSell.isEmpty())
		InsertFirstOrder(Net.listSell, SELL|Net.Status, Net.GapVolume,    (LastSellDeal.Price+Net.minDelta)+Net.GapPrice/2);
	if (!Net.listSell.isEmpty()) {
		while (LastSellDeal.Price+Net.minDelta<Net.listSell.first().Price-Net.GapPrice)
			if (InsertFirstOrder(Net.listSell, SELL|Net.Status, Net.GapVolume,    Net.listSell.first().Price-Net.GapPrice))
				break;
		while (Net.numTraps>Net.listSell.size() && Stocks)
			if (InsertLastOrder(Net.listSell, SELL|Net.Status, Net.GapVolume,     Net.listSell.last().Price+Net.GapPrice))
				break;
	}
	*/
}


void C_TradeMaster::MakeOrders(){
	//---------------- remove orders ----------------
	//printf("-------------------------------\ntrap %d %d\n",TrapNet.listBuy.size(), TrapNet.listSell.size());
	//printf("hot  %d %d\n",HotNet.listBuy.size(), HotNet.listSell.size());
	CutNet(TrapNet);
	//printf("trap %d %d\n",TrapNet.listBuy.size(), TrapNet.listSell.size());
	//CutNet(HotNet);
	//printf("hot  %d %d\n",HotNet.listBuy.size(), HotNet.listSell.size());
	//------------------ insert order----------------
	SewNet(TrapNet);
	//printf("trap %d %d\n",TrapNet.listBuy.size(), TrapNet.listSell.size());
	//SewNet(HotNet);
	//printf("hot  %d %d\n",HotNet.listBuy.size(), HotNet.listSell.size());


}
void C_TradeMaster::Close(){
	TrapNet.listBuy.clear();
	Cash+=LockedCash;
	LockedCash=0;

	TrapNet.listSell.clear();
	Cash+=(Stocks+LockedStocks)*LastSellDeal.Price;
	Cash-=(Stocks+LockedStocks)*LastSellDeal.Price*Commission;
	LockedStocks=0;

}

