#include "DBManager.h"
#include <stdio.h>
#include <crtdbg.h>
#include <QDebug>

bool sql_open_database(QString db_name, QSqlDatabase& db_trading)
{
	uint unic=QDateTime::currentDateTime().toTime_t();
	db_trading = QSqlDatabase::addDatabase("QMYSQL",db_name+QString::number(unic));
	//db_trading = QSqlDatabase::addDatabase("QSQLITE", db_name + QString::number(unic));
	db_trading.setHostName("");
	db_trading.setDatabaseName(db_name);
	db_trading.setUserName("root");
	db_trading.setPassword("root");
	if (!db_trading.open()) {
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
		return false;
	}
	return true;
}
void sql_close_database(QSqlDatabase& db_trading)
{
	if (db_trading.isOpen()){
		db_trading.close();
	}
}

bool sql_drop_table(QSqlDatabase& db, QString Table ){
	QSqlQuery query(db);
	if (Table.contains("-")){
		Table.replace("-","_");
	}
	QString Cmd="DROP TABLE IF EXISTS " + Table;
	//--------------- drop -------------------------
	printf (STR(Cmd));
	query.exec(Cmd);
	if (!query.isActive()){
		printf (" - ERROR!\n");
		QMessageBox::critical(0, QObject::tr("Database drop Error"), query.lastError().text());
		return false;
	}
	else 
		printf (" - OK \n");
	return true;
}

bool sql_drop_tables(QSqlDatabase& db, QList<QString>& listTable, QString suffix ){
	for(int i=0; i<listTable.size(); i++){
		if (!sql_drop_table(db, listTable[i]+suffix))
			return false;
	}
	return true;
}


uint sql_get_last_datetime_from_seccode_deal(QSqlDatabase& db, QString seccode)
{
	QSqlQuery query(db);
	QString Cmd="SELECT * FROM " + seccode+ "_deal ORDER BY id DESC LIMIT 1";

	SDeal Deal;
	query.exec(Cmd);
	VALID(query);
	int query_size=query.size();
	uint last_dt=0;
	if (query_size==1)
		if (query.next()){
			Deal.Parse(query);
			QDateTime dt; 
			dt.setDate(Deal.GetQDate());
			dt.setTime(Deal.GetQTime());
			qDebug() << dt;
			last_dt=dt.toTime_t();
		}
	return last_dt;
}

uint sql_get_last_id_from_tbl(QSqlDatabase& db, QString tbl)
{
	QSqlQuery query(db);
	QString Cmd="SELECT MAX(id) FROM " + tbl;
	bool ok=query.exec(Cmd);
	_ASSERTE(ok);
	VALID(query);
	uint id=0;
	if (query.next())
		id=query.value(0).toInt();
	return id;
}


void sql_write_ticks ( QSqlQuery& query, QString seccode, S_RingEasyTicks& ticks, unsigned filterDateTime)
{
	if (ticks.isEmpty())
		return;

	int isOk=query.prepare("INSERT INTO " + seccode+ "_deal (trdate,trtime,volume,price,trtype)  VALUES (?, ?, ?, ?, ?)");
	_ASSERTE(isOk);

	QVariantList listDate;
	QVariantList listTime;
	QVariantList listVolume;
	QVariantList listPrice;
	QVariantList listType;

	//if (echo)
	//	qDebug()<< "<" +seccode+ ">";

	
	while(ticks.tail<ticks.head){
		S_Tick& tick = ticks.refTail();
		if (tick.datetime>filterDateTime){
			listDate	<< tick.DateTime().toString("yyyyMMdd");
			listTime	<< tick.DateTime().toString("hhmmss");
			listVolume	<< tick.quantity;
			listPrice	<< tick.price;
			listType	<< tick.type;
		}
		ticks.tail++;
	}
	//if (echo)
	//	qDebug()<< "</" +seccode+ ">";

	query.addBindValue(listDate);
	query.addBindValue(listTime);
	query.addBindValue(listVolume);
	query.addBindValue(listPrice);
	query.addBindValue(listType);

	printf("execBatch...");
	if (!query.execBatch()){
		printf(ASCII( query.lastQuery()));
		printf("\n");
		printf(ASCII( query.lastError().text()));
		printf("\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}

	//printf("ok! %d ticks inserted\n",size);

}




void sql_write_trades(QSqlQuery& query, QString seccode, S_RingEasyTrades& trades, unsigned filterDateTime)
{
	if (trades.isEmpty())
		return;

	

	int isOk = query.prepare("INSERT INTO " + seccode + "_deal (trdate,trtime,volume,price,trtype)  VALUES (?, ?, ?, ?, ?)");
	_ASSERTE(isOk);

	QVariantList listDate;
	QVariantList listTime;
	QVariantList listVolume;
	QVariantList listPrice;
	QVariantList listType;

	//if (echo)
	//	qDebug()<< "<" +seccode+ ">";


	while (trades.tail<trades.head) {
		S_EasyTrade& trade = trades.refTail();


		if (trade.datetime>filterDateTime) {
			QString date = DateTime2TextDate(trade.datetime);
			printf(STR(date));
			listDate << date;
			QString time = DateTime2TextTime(trade.datetime);
			listTime << time;
			listVolume << trade.quantity;
			listPrice << trade.price;
			listType << ((trade.buysell=='S')? (1) << 8:(-1)<<8);
		}
		trades.tail++;
	}
	//if (echo)
	//	qDebug()<< "</" +seccode+ ">";

	query.addBindValue(listDate);
	query.addBindValue(listTime);
	query.addBindValue(listVolume);
	query.addBindValue(listPrice);
	query.addBindValue(listType);

	printf("execBatch...");
	if (!query.execBatch()) {
		printf(ASCII(query.lastQuery()));
		printf("\n");
		printf(ASCII(query.lastError().text()));
		printf("\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}

	//printf("ok! %d ticks inserted\n",size);

}

void sql_write_quotes( QSqlQuery& query, QString seccode, S_RingEasyQuotes& quotes, bool echo)
{


	if (quotes.isEmpty())
		return;
// HERE ERROR AFTER WEEKEND !!!!
	int isOk=query.prepare("INSERT INTO " + seccode+ "_quote (datetime,price,buy,sell)  VALUES (?, ?, ?, ?)");
	_ASSERTE(isOk);

	QVariantList listDatetime;
	QVariantList listSell;
	QVariantList listBuy;
	QVariantList listPrice;


	if (echo)
		qDebug()<< "<" +seccode+ ">";
	while (quotes.tail<quotes.head){
		C_EasyQuote& quote =quotes.refTail(); ///!!! 	_ASSERTE(tail<=indx && indx<head);

		listDatetime<< quote.datetime;
		listPrice	<< quote.price;
		listBuy  	<< quote.buy;
		listSell	<< quote.sell;
		quotes.tail++;
	}
	if (echo)
		qDebug()<< "</" +seccode+ ">";

	query.addBindValue(listDatetime);
	query.addBindValue(listPrice);
	query.addBindValue(listBuy);
	query.addBindValue(listSell);

	//printf("execBatch...");
	if (!query.execBatch()){
		printf(ASCII( query.lastQuery()));
		printf("\n");
		printf(ASCII( query.lastError().text()));
		printf("\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}

	//printf("ok! %d ticks inserted\n",size);

}


uint sql_read_ticks(QSqlDatabase& db, QString StockCode, QDateTime& fromDT, QDateTime& toDT, S_RingEasyTicks& ticks)
{
	QSqlQuery query(db);

	int nDate0=intDate(fromDT);
	int nDate1=intDate(toDT);
	int nTime0=intTime(fromDT);
	int nTime1=intTime(toDT);

	char cmd[200];
	if (nDate0==nDate1)
		sprintf(cmd,"SELECT * FROM %s_deal WHERE trdate=%d AND trtime>=%d AND trtime<=%d",STR(StockCode),nDate0,nTime0,nTime1);
	if (nDate0<nDate1)
		sprintf(cmd,"SELECT * FROM %s_deal WHERE trdate>=%d AND trdate<=%d",STR(StockCode),nDate0,nDate1);

	
	query.exec(cmd);
	VALID(query);
	

	SDeal Deal;
	S_Tick Tick;
	int i;
	for(i=0; query.next(); i++){
		Deal.nDate  =query.value(1).toInt();
		Deal.nTime  =query.value(2).toInt();
		Deal.nVolume=query.value(3).toInt();
		Deal.Price  =query.value(4).toDouble();
		Deal.nType  =query.value(5).toInt();
		
		QDateTime dt;
		dt.setDate(Deal.GetQDate());
		dt.setTime(Deal.GetQTime());
		Tick.datetime=dt.toTime_t();
		Tick.quantity=Deal.nVolume;
		Tick.price   =Deal.Price;
		Tick.type	 =Deal.nType;
		
		ticks<< Tick;
	}
	return i;
}



uint sql_read_quotes(QSqlDatabase& db, QString StockCode, QDateTime& fromDT, QDateTime& toDT, S_RingEasyQuotes& quotes)
{
	QSqlQuery query(db);
	uint from_dt=fromDT.toTime_t();
	uint to_dt  =toDT.toTime_t();
	
	char cmd[200];
	sprintf(cmd,"SELECT * FROM %s_quote WHERE datetime>=%d AND datetime<=%d",STR(StockCode),from_dt,to_dt);
	
	query.exec(cmd);
	VALID(query);
	
	int i;
	for(i=0; query.next(); i++){
		C_EasyQuote quote;// =quotes[quotes.size];

		quote.datetime	=query.value(1).toInt();
		quote.price		=query.value(2).toFloat();
		quote.buy		=query.value(3).toInt();
		quote.sell		=query.value(4).toInt();
		//quotes.size++;
		quotes << quote;
	}
	return i;
}
uint sql_show_tables(QSqlDatabase& db, QString suffix, QList<QString>& listTables)
{
	QSqlQuery query(db);
	
	QString cmd="show tables like '%" +suffix+"'";
	
	query.exec(cmd);
	VALID(query);
	
	int i;
	for(i=0; query.next(); i++){
		listTables<< 	query.value(0).toString();
		//qDebug()<<listTables.last();
	}
	return i;
}

bool sql_check_table(QSqlDatabase& db, QString tbl){
	QSqlQuery query(db);
	QString cmd="CHECK TABLE "+tbl;
	query.exec(cmd);
	//VALID(query);
	 if (!query.isActive()){
		 DISPLAY_ERROR(query);
		 return false;
	 }
	return true;
}

bool sql_check_database(QSqlDatabase& db){

	QList<QString> listTables;
	sql_show_tables(db, "", listTables);
	_ASSERTE(listTables.size());
	for(int i=0; i<listTables.size();i++){
		QString tbl=listTables[i];
		printf("checking %s ...",STR(tbl));
		if (sql_check_table(db,tbl)==false){
			printf("ERROR!\n");
			return false;
		}
		printf("OK!\n");

	}
	return true;

}

bool sql_switch_buysell(QSqlDatabase& db, QString tbl, uint index)
{
	QSqlQuery query(db);
	QString cmd = "SELECT * FROM " +tbl+ " WHERE ID=" + QString::number(index);

	bool ok=query.exec(cmd);
	_ASSERTE(ok);
	VALID(query);


	SDeal Deal;
	query.next();
	//uint id     =query.value(0).toInt();
	Deal.nDate  =query.value(1).toInt();
	Deal.nTime  =query.value(2).toInt();
	Deal.nVolume=query.value(3).toInt();
	Deal.Price  =query.value(4).toDouble();
	Deal.nType  =query.value(5).toInt();

	if (Deal.IsBuy())
		Deal.SetSupplyType();
	else 
		Deal.SetDemandType();
	
	cmd= "UPDATE "+tbl+ " SET trtype=" +QString::number(Deal.nType)+ " WHERE ID="+ QString::number(index);
	ok=query.exec(cmd);
	_ASSERTE(ok);
	return ok;
}
bool sql_switch_all_buysell(QSqlDatabase& db)
{
	QStringList listTables;
	QSqlQuery query(db);
	sql_show_tables(db,"_deal",listTables);
	for(int i=0; i<listTables.size();i++){
		
 		QString tbl=listTables[i];
		qDebug()<< tbl;
		QString cmd= "SELECT * FROM "+ tbl + " WHERE trdate>=20141224 LIMIT 1";
		bool ok=query.exec(cmd);
		_ASSERTE(ok);
		VALID(query);
		_ASSERTE(query.next());

		SDeal Deal;
		uint ID     =query.value(0).toInt();
		Deal.nDate  =query.value(1).toInt();
		Deal.nTime  =query.value(2).toInt();
		Deal.nVolume=query.value(3).toInt();
		Deal.Price  =query.value(4).toDouble();
		Deal.nType  =query.value(5).toInt();

		uint lastID=sql_get_last_id_from_tbl(db, tbl);
		for(uint id=ID; id<=lastID; id++)
			sql_switch_buysell(db, tbl,id);
	}
	return true;
}


void sql_create_seccode_deal(QSqlDatabase& db_trading, QString seccode)
{
	if (seccode.contains("-")){
		seccode.replace("-","_");
	}
	if (!db_trading.isValid() ){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	if (db_trading.isOpenError() ){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	QSqlQuery query(db_trading);
	//------------------------------------------------------------------
	//  Создаем таблицу сделок с этой акцией если она отсутсвует 
	char cmd[1000];
	strcpy(cmd,"CREATE TABLE IF NOT EXISTS ");
	strcat(cmd,STR(seccode));
	strcat(cmd,"_deal");
	printf(cmd);
	strcat(cmd," (ID INT UNSIGNED NOT NULL AUTO_INCREMENT,\
			   trdate INT UNSIGNED NULL,\
			   trtime INT UNSIGNED NULL,\
			   volume INT UNSIGNED NULL,\
			   price  FLOAT NULL,\
			   trtype INT NULL,\
			   PRIMARY KEY (ID));");
	// Err in creation DIXY-002D because of -- !!!! !

	query.exec(cmd);
	if (!query.isActive()){
		printf(" - ERROR\n");
		printf("%s",cmd);
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else
		printf(" - OK!\n");


}


void sql_create_seccode_quote(QSqlDatabase& db_trading, QString seccode)
{
	if (seccode.contains("-")){
		seccode.replace("-","_");
	}
	if (!db_trading.isValid() ){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	if (db_trading.isOpenError() ){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}


	//------------------------------------------------------------------
	//  Создаем таблицу сделок с этой акцией если она отсутсвует 

	QString cmd="CREATE TABLE IF NOT EXISTS " + seccode + "_quote";
	cmd+= " (ID INT UNSIGNED NOT NULL AUTO_INCREMENT,\
		  datetime INT UNSIGNED NULL,\
		  price  FLOAT NULL,\
		  buy INT  NULL,\
		  sell INT  NULL,\
		  PRIMARY KEY (ID));";

	QSqlQuery query(db_trading);
	query.exec(cmd);
	if (!query.isActive()){
		qDebug() << "ERROR in:" << cmd;
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else
		printf(" - OK!\n");


}