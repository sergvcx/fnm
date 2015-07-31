#include "main.h"
#include "shared.h"

extern uint test_datetime;
#define TEST_MODE 
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

	C_S1_Strategy();
	C_S1_Strategy(QString logname, float sellmax, float sellmin, float buymax, float buymin);
	~C_S1_Strategy();
	bool operator << (C_SubVector<S_Tick>& vecTick);

	virtual void Update(float SpreadMin, float SpreadMax, uint datetime);

	virtual void Update(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote, uint datetime);
	float Profit(float ClosePrice);
};



class C_YoStrategy{
public:	
	C_Instrument* pInstrument;
	float cash;
	float commission;
	float commission_rate;

	S_MinMax<float> FastDeviation;
	S_MinMax<float> SlowDeviation;

	QList<S_NewOrder*> listSlowSellOrder;
	QList<S_NewOrder*> listFastSellOrder;
	QList<S_NewOrder*> listSlowBuyOrder;
	QList<S_NewOrder*> listFastBuyOrder;
	
	uint stocks_in_hand;
	uint stocks_in_sell;
	uint stocks_in_buy;

	uint fast_sell_datetime;
	uint fast_buy_datetime;

	C_YoStrategy(C_Instrument& Instrument, S_MinMax<float> SlowDev, S_MinMax<float> FastDev, float Cash, int Stocks, float Commission=0.1/100){
		pInstrument=&Instrument;
		SlowDeviation=SlowDev;
		FastDeviation=FastDev;
		cash=Cash;
		commission=0;
		commission_rate=Commission;
		stocks_in_hand=Stocks;
		stocks_in_sell=0;
		stocks_in_buy=0;
		listSlowSellOrder.clear();
		listFastSellOrder.clear();
		listSlowBuyOrder.clear();
		listFastBuyOrder.clear();
		Instrument.pData->ringNewOrders.Init();
		Instrument.pData->CancelOrders.Init();
		Instrument.pData->ringEasyTrades.Init();
	}

	bool CancelOrder(S_NewOrder* pNewOrder)
	{
		S_CancelOrders& Orders=pInstrument->pData->CancelOrders;
		S_CancelOrder*  pCancel=Orders.Insert(pNewOrder->transaq.transactionid);
		
#ifdef TEST_MODE
		pCancel->transaq.success=true;
		Orders.tail++;
#endif

		bool ok=pCancel->isSuccess();
		return ok;
	}

	S_NewOrder* NewOrder(float price, int quantity, char buysell ){

		S_RingNewOrders& Orders=pInstrument->pData->ringNewOrders;
		S_NewOrder* pOrder=Orders.Insert(price,quantity,buysell);

#ifdef TEST_MODE
		int static order_counter=0;
		pOrder->server.orderno=order_counter;
		pOrder->transaq.success=true;
		pOrder->transaq.transactionid=order_counter;
		pOrder->server.accepttime=test_datetime;
		pOrder->server.time=0;
		order_counter++;
#endif
		bool ok=pOrder->isSuccess();
		if (ok)
			return pOrder;
		else 
			return 0;
	}


	S_NewOrder* NewFastOrder(char buysell, S_MinMax<float>& spread)
	{
		float price;
		if (buysell=='S'){
			price=spread.max*(2+FastDeviation.min+FastDeviation.max)/2;
		}
		if (buysell=='B'){
			price=spread.min*(2-FastDeviation.min-FastDeviation.max)/2;
		}
		S_NewOrder* pOrder=NewOrder(price,1,buysell);
		return pOrder;
	}


	S_NewOrder* NewSlowOrder(char buysell, S_MinMax<float>& spread)
	{
		float price;
		if (buysell=='S'){
			price=spread.max*(2+SlowDeviation.min+SlowDeviation.max)/2;
		}
		if (buysell=='B'){
			price=spread.min*(2-SlowDeviation.min-SlowDeviation.max)/2;
		}
		S_NewOrder* pOrder=NewOrder(price,1,buysell);
		
		return pOrder;
	}


	bool InvalidateOrders(S_MinMax<float>& spread)
	{
		bool isCancelRequested=false;
		
		
		if(!listFastBuyOrder.isEmpty()){
			S_NewOrder* pOrder=listFastBuyOrder.first();
			if (pOrder->price > spread.min*(1-FastDeviation.min)   ||
				pOrder->price < spread.min*(1-FastDeviation.max)){
			

				if (CancelOrder(pOrder)){
					pOrder->quantity=0;
				}
				else{
					_ASSERTE(false);
				}

				listFastBuyOrder.removeFirst();
				isCancelRequested=true;
			}
		}
		if(!listSlowBuyOrder.isEmpty()){
			S_NewOrder* pOrder=listSlowBuyOrder.first();
			if (pOrder->price > spread.min*(1-SlowDeviation.min) ||
				pOrder->price < spread.min*(1-SlowDeviation.max) ){
				if (CancelOrder(pOrder))
					pOrder->quantity=0;
				else{
					_ASSERTE(false);
				}
				listSlowBuyOrder.removeFirst();
				isCancelRequested=true;
			}
		}
		if(!listFastSellOrder.isEmpty()){
			S_NewOrder* pOrder=listFastSellOrder.first();

			if (pOrder->price < spread.max*(1+FastDeviation.min) ||
				pOrder->price > spread.max*(1+FastDeviation.max) ){

				if (CancelOrder(pOrder)){
					stocks_in_hand+=pOrder->quantity;
					pOrder->quantity=0;
				}else{
					_ASSERTE(false);
				}
				listFastSellOrder.removeFirst();
				isCancelRequested=true;
			}
		}
		if(!listSlowSellOrder.isEmpty()){
			S_NewOrder* pOrder=listSlowSellOrder.first();
			if (pOrder->price < spread.max*(1+SlowDeviation.min) ||
				pOrder->price > spread.max*(1+SlowDeviation.max) ){

				if (CancelOrder(pOrder)){
					stocks_in_hand+=pOrder->quantity;
					pOrder->quantity=0;
				}else{
					_ASSERTE(false);
				}
				listSlowSellOrder.removeFirst();
				isCancelRequested=true;
			}
		}
		return isCancelRequested;
	}
		 
	void RescanTrades()
	{
		S_RingEasyTrades& ringEasyTrades=pInstrument->pData->ringEasyTrades;
		S_RingNewOrders&  ringNewOrders =pInstrument->pData->ringNewOrders;

		for(uint i=ringEasyTrades.tail; i<ringEasyTrades.head ;i++){
			S_EasyTrade& Trade=ringEasyTrades[i];
			S_NewOrder* pOrder=ringNewOrders.FindOrderNo(Trade.orderno);	// если была сделка - есть за€вка . »щем за€вку по ее номеру 
			_ASSERTE(pOrder);
					
			
			ringEasyTrades.tail++;		// ! to early . Check it
			if (ringNewOrders.ptrTail()==pOrder)
				ringNewOrders.tail++;

			_ASSERTE(pOrder->price   ==Trade.price);
			_ASSERTE(pOrder->quantity>=Trade.quantity);
			_ASSERTE(pOrder->buysell ==Trade.buysell);

			
			if (Trade.buysell=='B'){
				pOrder->quantity-=Trade.quantity;
				stocks_in_hand  +=Trade.quantity;
				cash-=Trade.quantity*Trade.price;
				commission+=Trade.quantity*Trade.price*commission_rate;
				if (listFastBuyOrder.contains(pOrder)){
					listFastBuyOrder.removeOne(pOrder);
					continue;
				}
				if (listSlowBuyOrder.contains(pOrder)){
					listSlowBuyOrder.removeOne(pOrder);
					continue;
				}
			}
			if (Trade.buysell=='S'){

				pOrder->quantity-=Trade.quantity;
				cash+=Trade.quantity*Trade.price;
				commission+=Trade.quantity*Trade.price*commission_rate;
				if (listFastSellOrder.contains(pOrder)){
					listFastSellOrder.removeOne(pOrder);
					continue;
				}
				if (listSlowSellOrder.contains(pOrder)){
					listSlowSellOrder.removeOne(pOrder);
					continue;
				}
			}
			_ASSERTE(false);
		}
		stocks_in_buy =listFastBuyOrder.size() +listSlowBuyOrder.size();
		stocks_in_sell=listFastSellOrder.size()+listSlowSellOrder.size();

	}
	void Update(S_MinMax<float>& spread){
		bool isUpdated=true;
		
			RescanTrades();
			if (InvalidateOrders(spread)){
				
				}
while (isUpdated){
			
			isUpdated=NewOrder(spread);
			
		}
	}

	bool NewOrder(S_MinMax<float>& spread)
	{
		
		stocks_in_buy =listFastBuyOrder.size() +listSlowBuyOrder.size();
		stocks_in_sell=listFastSellOrder.size()+listSlowSellOrder.size();
		_ASSERTE(stocks_in_buy+stocks_in_sell<3);
		if (stocks_in_buy+stocks_in_sell==2) 
			return false; // no new orders

		// Add new orders
		if (stocks_in_hand>0){
			if (stocks_in_sell==0 ){			// Start slow sell
				S_NewOrder* pOrder=NewSlowOrder('S',spread); //slow
				stocks_in_sell=1;
				stocks_in_hand--;
				listSlowSellOrder<<pOrder;
				return true;
			}
			else if (stocks_in_sell==1){		// Start fast sell
				S_NewOrder* pOrder=NewFastOrder('S',spread); //slow
				stocks_in_sell=2;
				stocks_in_hand--;
				listFastSellOrder<<pOrder;
				return true;
			}
			_ASSERTE(false);
		}
		
		if (stocks_in_buy==0){		// slow buy
			S_NewOrder* pOrder=NewSlowOrder('B',spread);
			stocks_in_buy=1;
			listSlowBuyOrder<<pOrder;
			return true;
		}
		if (stocks_in_sell==0 && stocks_in_buy==1){		// fast buy
			S_NewOrder* pOrder=NewFastOrder('B',spread); //fast
			stocks_in_buy=2;
			listFastBuyOrder<<pOrder;
			return true;
		}
		return false;
	}
	
};




class C_SimpleStrategy{
public:	
	C_Instrument* pInstrument;
	float money;
	float money_in_buy;
	float commission;
	float commission_rate;

	uint stocks_in_hand;
	uint stocks_in_sell;
	uint stocks_in_buy;

	S_MinMax<float>		Deviation;
		
	S_NewOrder* sellOrder;
	S_NewOrder* buyOrder;
	
	//uint fast_sell_datetime;
	//uint fast_buy_datetime;


	C_SimpleStrategy (C_Instrument& Instrument, float Cash, int Stocks, float Commission=0.1/100){
		pInstrument=&Instrument;
		money=Cash;
		money_in_buy=0;
		commission=0;
		commission_rate=Commission;
		stocks_in_hand=Stocks;
		stocks_in_sell=0;
		stocks_in_buy=0;
		Instrument.pData->ringNewOrders.Init();
		Instrument.pData->CancelOrders.Init();
		Instrument.pData->ringEasyTrades.Init();
	}

	bool sendCancelOrder(S_NewOrder* pNewOrder)
	{
		S_CancelOrders& Orders=pInstrument->pData->CancelOrders;
		S_CancelOrder*  pCancel=Orders.Insert(pNewOrder->transaq.transactionid);

//#ifdef TEST_MODE
//		pCancel->transaq.success=true;
//		Orders.tail++;
//#endif

		//bool ok=pCancel->waitCompleted();
		while(pCancel->isTransaqSuccess());
		return pCancel->isServerSuccess();
	}


	S_NewOrder* sendNewOrder(float price, int quantity, char buysell )
	{
		S_RingNewOrders& ringNewOrders=pInstrument->pData->ringNewOrders;
		S_NewOrder* pOrder=ringNewOrders.Insert(price,quantity,buysell);

//#ifdef TEST_MODE
//		int static order_counter=0;
//		pOrder->server.orderno=order_counter;
//		pOrder->transaq.success=true;
//		pOrder->transaq.transactionid=order_counter;
//		pOrder->server.accepttime=test_datetime;
//		pOrder->server.time=0;
//		order_counter++;
//#endif
		while(!pOrder->isTransaqSuccess());
		while(!pOrder->isServerSuccess());
		return pOrder;
	}



	bool checkOrders(S_MinMax<float>& spread)
	{
		if(buyOrder){
			if (buyOrder->price < spread.min*0.99){
				if (sendCancelOrder(buyOrder)){
					stocks_in_buy=0;
				}
				buyOrder=0;
			}
		}
		if(sellOrder){
			if (sellOrder->price > spread.max*1.01){
				if (sendCancelOrder(sellOrder)){
					stocks_in_hand+=sellOrder->quantity;
					stocks_in_sell=0;
				}
				sellOrder=0;
			}
		}
		return true;
	}

	void rescanTrades()
	{
		S_RingEasyTrades& ringEasyTrades=pInstrument->pData->ringEasyTrades;
		S_RingNewOrders&  ringNewOrders =pInstrument->pData->ringNewOrders;

		for(uint i=ringEasyTrades.tail; i<ringEasyTrades.head ;i++){
			S_EasyTrade& Trade=ringEasyTrades[i];
			uint idxOrder;
			bool isOk=ringNewOrders.findOrderNo(Trade.orderno, idxOrder);
			_ASSERTE(isOk);
			if (!isOk) continue;
			S_NewOrder& order=ringNewOrders[idxOrder];
			
			_ASSERTE(order.price   ==Trade.price);
			_ASSERTE(order.quantity>=Trade.quantity);
			_ASSERTE(order.buysell ==Trade.buysell);

			if (Trade.buysell=='B'){
				_ASSERTE(buyOrder);
				stocks_in_hand  +=Trade.quantity;
				stocks_in_buy   -=Trade.quantity;
				commission      +=Trade.quantity*Trade.price*commission_rate;
				order.quantity  -=Trade.quantity;
				if (order.quantity==0)
					buyOrder=0;
			} else if (Trade.buysell=='S'){
				_ASSERTE(sellOrder);
				money +=Trade.quantity*Trade.price;
				stocks_in_sell-=Trade.quantity;
				commission+=Trade.quantity*Trade.price*commission_rate;
				order.quantity-=Trade.quantity;
				if (order.quantity==0)
					sellOrder=0;
			} else _ASSERTE(0);

			ringEasyTrades.tail++;

			while  (ringNewOrders.tail< ringNewOrders.head) {
				if (ringNewOrders.refTail().quantity==0)
					ringNewOrders.tail++;
				else 
					break;
			}

		}
	}

	bool makeNewOrders(S_MinMax<float>& spread)
	{
		if (stocks_in_sell==0 && stocks_in_hand){
			sellOrder=sendNewOrder(spread.max,stocks_in_hand,'S');
			_ASSERTE(sellOrder);
			stocks_in_sell=stocks_in_hand;
			stocks_in_hand=0;
			return true;
		}

		if (stocks_in_buy==0 ){		//  buy
			buyOrder=sendNewOrder(spread.min,3,'B');
			stocks_in_buy=3;
		}
		return false;
	}

	void update(S_MinMax<float>& spread){
		rescanTrades();
		checkOrders(spread);
		makeNewOrders(spread);
	}
};
