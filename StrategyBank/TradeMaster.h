#include <QQueue>
#include "main.h"


void  SplitBuySell(QQueue<SDeal>& qDeals, QQueue<SDeal>& qBuyDeals,QQueue<SDeal>& qSellDeals );
void FirstSecond(QQueue<SDeal>& qDeals, QQueue<SDeal>& qInSecDeals );

struct SOrder{
	float	Price;			// ����
	int		Volume;		// ����� �����(�����)
	int		Time;			// ����� ������ ������ (�� ����������)
	int		Date;			// ���� ������ ������ (�� ����������)
	int		Status;		// ����� ������
	//int		nTransID;		// ������������� ������
	//QQueue<SDeal> queDeal;	// ������ ������� ����������� �� ������ ������
};

class C_TradeMaster{
public:
	
	int		Stocks;
	int		LockedStocks;
	float 	Cash;
	float 	LockedCash;
	float 	Commission;
	float 	minDelta;
	float 	maxDelta;
	int     GapVolume;
	int		GapPrice;	
	int   	numTraps;

	SDeal	LastSellDeal;
	SDeal	LastBuyDeal;

	QList<SOrder>  listMySellOrder;
	QList<SOrder>  listMyBuyOrder;

	C_TradeMaster();
	void Init(float StartPrice, float MinDelta, float MaxDelta, int nTraps, float fCash, int nStocks);
	//--------------- complete ---------------------

	void CompleteFirstBuyOrder();
	
	void CompleteFirstSellOrder();
	//--------------- cancle ---------------------
	void RemoveFirstBuyOrder();
	
	void RemoveLastBuyOrder();
	
	void RemoveFirstSellOrder();

	void RemoveLastSellOrder();
	//--------------- insert ---------------------
	bool InsertFirstBuyOrder(int Volume, float Price);
	
	bool InsertLastBuyOrder(int Volume, float Price);
	
	bool InsertFirstSellOrder(int Volume, float Price);

	bool InsertLastSellOrder(int Volume, float Price);
	//-------------------------------------

	void  Parse(QQueue<SDeal>& qDeals);
	
	void MakeOrders();
	void Close();
};
