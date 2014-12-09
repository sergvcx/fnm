#include "TradeMaster2.h"


C_TradeMaster2::C_TradeMaster2(){
	Cash=0;
	Stocks=0;
	LockedCash=0;
	LockedStocks=0;
	minDelta=0.2/100;
	maxDelta=1.0/100;
	Commission=0;//0.01/100;
}
void C_TradeMaster2::Init(float StartPrice, float MinDelta, float MaxDelta, int nTraps, float fCash, int nStocks, float fHotDelta){
	C_TradeMaster::Init(StartPrice, MinDelta, MaxDelta, nTraps, fCash, nStocks);
	HotDelta = fHotDelta;
}

void C_TradeMaster2::MakeOrders(){
	
	C_TradeMaster::MakeOrders();
	
	if (int nHotStocks=Cash/LastBuyDeal.Price>1){
//		InsertFirstBuyOrder(nHotStocks,  LastBuyDeal.Price*(1+HotDelta));
	}

	
	if (Stocks){
	//	InsertFirstSellOrder(Stocks,  LastSellDeal.Price*(1-HotDelta));
	}


}
