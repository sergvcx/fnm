#ifndef TRADE_MASTER2
#define TRADE_MASTER2

#include <QQueue>
#include "main.h"
#include "TradeMaster.h"


class C_TradeMaster2: public C_TradeMaster{
public:
	
	float HotDelta;
	C_TradeMaster2();
	void Init(float StartPrice, float MinDelta, float MaxDelta, int nTraps, float fCash, int nStocks , float HotDelta);
	void MakeOrders();

};


#endif