#pragma once
#include "main.h"

#define DELETE_TABLE 0xFF
#define ACTIVE_TABLE 1

void DropTable(QSqlDatabase& db, QString Table );

class CDBManager : public QWidget
{
	Q_OBJECT
public:
	
	//QSqlDatabase db_mylefco ;
	//QSqlDatabase db_trading ;
	CDBManager(QWidget *parent = 0);
	~CDBManager();
	//QSqlDatabase db_mylefco;
	QSqlDatabase db_trading;
	//QSqlDatabase db_quik;
	//void QuikDropAndCreateRates(QString InstrumentCode);

	
	//------------- single table maintaince ---------------
	void TradingDrop_deal(QString StockCode);
	//void TradingCreate_deal(QString StockCode);
	void TradingCreate_trd(QString StockCode);
	void TradingCreate_req(QString StockCode);
	


// 	void QuikDrop_alldeals();
// 	void QuikCreate_alldeals();
// 
// 	void QuikDropRates(QString InstrumentCode);
// 	void QuikCreateRates(QString InstrumentCode);
// 
// 	void QuikDropFinInst();
// 	void QuikCreateFinInst();

	void TradingRemoveDuplicates(QString Instrument);
	//void Trading_UpdateInSecond(QString StockCode, int nDate0, int nDate1);
	//------------- multiple table maintaince -----------------
	
public slots:
	void	Disconnect();
	//void	EmptyAllDeals();
	//int		CheckTables();
	
	void TradingDropAll_trd();
	void TradingDropAll_req();
	void TradingDropAll();
	void TradingDropAllEmpty();
	void TradingDropAllSmall();
	void TradingCreateAllFrom_portfolio();
	void TradingDeleteDealsToday();
// 	void QuikDropAndCreateAllDeals();
// 	void QuikDropAndCreateFinInst();
// 	void QuikDropAndCreateRates();
// 	void QuikDrop_request();
// 	void QuikCreate_request();
// 	void QuikCreate_trade();
// 	void QuikCreate_current();

	void TradingDrop_portfolio();
	void TradingCreate_portfolio();
	void TradingClean_portfolio();
	void TradingAlterAll();
	//void TradingUpdateAllInSecond();

};

void sql_create_seccode_deal(QSqlDatabase& db_trading, QString seccode);
void sql_create_seccode_quote(QSqlDatabase& db_trading, QString seccode);
bool sql_open_database(QString db_name, QSqlDatabase& db_trading);
void sql_close_database(QSqlDatabase& db_trading);
uint sql_get_last_datetime_from_seccode_deal(QSqlDatabase& db, QString seccode);