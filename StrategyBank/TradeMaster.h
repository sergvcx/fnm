#ifndef TRADE_MASTER
#define TRADE_MASTER
#include <QQueue>
#include "main.h"


void  SplitBuySell(QQueue<SDeal>& qDeals, QQueue<SDeal>& qBuyDeals,QQueue<SDeal>& qSellDeals );
void ExtractInSecondTicks(QQueue<SDeal>& qDeals, QQueue<SDeal>& qInSecDeals );

struct SOrder{
	float	Price;			// цена
	int		Volume;		// колво акций(лотов)
	int		Time;			// время подачи заявки (по компьютеру)
	int		Date;			// дата подачи заявки (по компьютеру)
	int		Status;		// стаус заявки
	//int		nTransID;		// идентификатор заявки
	//QQueue<SDeal> queDeal;	// сделки которые совершились по данной заявке
};
#define BUY 1
#define SELL 2
#define TRAP   4
#define HOT    8

class C_EasyTrade
{
public:


};
class C_TradeMaster{
public:
	int		Counter;
	int		Stocks;
	int		LockedStocks;
	float 	Cash;
	float 	LockedCash;
	float 	Commission;
	
	float BuyQuote;
	float SellQuote;

	SDeal	LastSellDeal;
	SDeal	LastBuyDeal;
	struct S_Net {
		float 	minDelta;
		float 	maxDelta;
		int     GapVolume;
		float	GapPrice;	
		int   	numTraps;
		QList<SOrder>  listSell;
		QList<SOrder>  listBuy;
		int     Status;
	} TrapNet, HotNet; 

	//QList<SOrder>  listMySellOrder;
	//QList<SOrder>  listMyBuyOrder;

	//QList<SOrder>  listHotSellOrder;
	//QList<SOrder>  listHotBuyOrder;

	C_TradeMaster();
	void Init(float StartPrice, float fCash, int nStocks, float minHotDelta, float maxHotDelta, float minTrapDelta, float maxTrapDelta);
	//--------------- complete ---------------------

	void CompleteFirstOrder(QList<SOrder>& qOrder, int Type);
	
	
	//--------------- cancle ---------------------
	void RemoveFirstOrder(QList<SOrder>& listOrder, int Type);
	
	void RemoveLastOrder(QList<SOrder>& listOrder, int Type);
	

	//--------------- insert ---------------------
	bool InsertFirstOrder(QList<SOrder>& qOrder, int Type, int Volume, float Price);
	
	bool InsertLastOrder(QList<SOrder>& qOrder, int Type, int Volume, float Price);
	
	//-------------------------------------
	void CutNet(S_Net& Net);
	void SewNet(S_Net& Net);
	void  Parse(QQueue<SDeal>& qDeals);
	
	virtual void MakeOrders();
	void Close();
};

#endif