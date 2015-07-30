#include "shared.h"
class C_TestConnector
{
public:
	C_Instrument* pInstrument;
	C_TestConnector(C_Instrument& Instrument){
		tradeno=0;
		pInstrument=&Instrument;
	}

	long long tradeno;
	void Close(C_YoStrategy& Strategy){
		if (Strategy.stocks_in_sell){
			Strategy.cash+=Strategy.stocks_in_sell*pInstrument->pData->ringEasyTicks.refLast().price;
			Strategy.stocks_in_sell=0;
			Strategy.listFastSellOrder.clear();
			Strategy.listSlowSellOrder.clear();
		}
		if (Strategy.stocks_in_hand){
			Strategy.cash+=Strategy.stocks_in_hand*pInstrument->pData->ringEasyTicks.refLast().price;
			Strategy.stocks_in_hand=0;
		}
	}
	void Trade(C_YoStrategy& Strategy){
		S_RingEasyTrades& ringEasyTrades=pInstrument->pData->ringEasyTrades;
		S_RingNewOrders&  ringNewOrders=pInstrument->pData->ringNewOrders;
		S_RingEasyTicks & ringEasyTicks =pInstrument->pData->ringEasyTicks;
		S_RingEasyQuotes& ringEasyQuotes=pInstrument->pData->ringEasyQuotes;

		ringEasyTicks.tail=0;
		ringEasyTrades.Init();
		ringNewOrders.Init();
		uint curr=0;
		S_Glass Glass;
		uint find_index=0;
		
		while (!ringEasyTicks.isEmpty()){
			uint lastDatetime=ringEasyTicks.refTail().datetime;
			
			if (!ringEasyQuotes.isEmpty()){
				if (ringEasyQuotes.FindBefore(lastDatetime, find_index, find_index)){
					ringEasyQuotes.UpdateGlass(Glass,find_index,100);
					if (!Glass.listSell.isEmpty() && !Glass.listBuy.isEmpty()){
						S_MinMax<float> spread;
						spread.max=Glass.listSell.first().price;
						spread.min=Glass.listBuy.first().price;
						test_datetime=Glass.datetime;
						//Strategy.Update(spread);
						if (Strategy.InvalidateOrders(spread)){

						}
						bool isUpdated=true;
						while (isUpdated){
							isUpdated=Strategy.NewOrder(spread);
						}
					}
				}
			}	
			// in second parsing
			
			for( ; !ringEasyTicks.isEmpty() && (lastDatetime==ringEasyTicks.refTail().datetime); ringEasyTicks.tail++){
				S_Tick& tick=ringEasyTicks.refTail();
				for(uint idxOrder=ringNewOrders.tail; idxOrder<ringNewOrders.head; idxOrder++ ){
					S_NewOrder& Order=ringNewOrders[idxOrder];
						
					if (Order.quantity==0){	// if canceled or completed then skip
						if (idxOrder==ringNewOrders.tail)
							ringNewOrders.tail++;
						continue;
					}
					if (Order.server.time)	// if already marked as completed then skip 
						continue;
					if (Order.buysell=='S' && tick.isSell())
						if (Order.price<=tick.price){
							ringEasyTrades.Insert(Order.price,Order.quantity,Order.buysell,Order.server.orderno,tradeno++,tick.datetime);
							//_ASSERTE(Order.server.time==0);
							Order.server.time=tick.datetime; // use as sign that order is completed 
							continue;
						}
					if (Order.buysell=='B' && tick.isBuy())
						if (Order.price>=tick.price){
							ringEasyTrades.Insert(Order.price,Order.quantity,Order.buysell,Order.server.orderno,tradeno++,tick.datetime);
							//_ASSERTE(Order.server.time==0);
							Order.server.time=tick.datetime; // use as sign that order is completed 
							continue;
						}
				} 
			}
		
			Strategy.RescanTrades();
			

			
		}
	}
};