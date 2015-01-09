#include "DBManager.h"
#include <stdio.h>
#include <crtdbg.h>
#include <windows.h>
#include <QDebug>

bool sql_open_database(QString db_name, QSqlDatabase& db_trading)
{
	uint unic=QDateTime::currentDateTime().toTime_t();
	db_trading = QSqlDatabase::addDatabase("QMYSQL",db_name+QString::number(unic));
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

//uint sql_get_max_id(QSqlDatabase& db_trading)

CDBManager::CDBManager(QWidget *parent): QWidget(parent){

	QTime time;
	time.currentTime();
	int min=time.minute();
/*
	db_quik= QSqlDatabase::addDatabase("QMYSQL","dbmanager_quik"+QString::number(min));
	db_quik.setHostName("");
	db_quik.setDatabaseName("quik");
	//db_quik.setDatabaseName("quiktest");
	db_quik.setUserName("root");
	db_quik.setPassword("root");
	if (!db_quik.open()) {
		QMessageBox::critical(0, QObject::tr("Database Error"), db_quik.lastError().text());
	}
*/

	db_trading = QSqlDatabase::addDatabase("QMYSQL","dbmanager_trading"+QString::number(min));
	db_trading.setHostName("");
	db_trading.setDatabaseName("trading");
	//db_trading.setDatabaseName("tradingtest");
	db_trading.setUserName("root");
	db_trading.setPassword("root");
	if (!db_trading.open()) {
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}
	


}

CDBManager::~CDBManager(){
}

void CDBManager::Disconnect(){

	
	QSqlDatabase::removeDatabase("trading"); 

}

/*
void CDBManager::EmptyAllDeals(){
	QSqlQuery query(db_mylefco);
	query.exec("DELETE FROM AllDeals;");
	if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	else 
		printf ("Cleaning Alldeals - OK \n");


	query.exec("DROP TABLE IF EXISTS alldeals;");
	if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	else 
		printf ("DROP TABLE IF EXISTS alldeals; - OK \n");


	query.exec("CREATE TABLE IF NOT EXISTS MYLEFCO.ALLDEALS \
		(ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
		stexchange INT UNSIGNED NULL, \
		stcode    CHAR(25) NULL, \
		stmode CHAR(4) NULL, \
		stnumber FLOAT NULL, \
		daytime DATETIME NULL, \
		volume FLOAT, \
		price FLOAT, \
		PRIMARY KEY (ID));");
	if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	else 
		printf ("Creating Alldeals - OK \n");

	query.exec("ALTER TABLE alldeals ENGINE=MyISAM");
	if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	else 
		printf ("Set ENGINE=MyISAM - OK \n");


	query.exec("optimize table AllDeals;");
	if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	else 
		printf ("Optimizing Alldeals - OK \n");

}*/




void CDBManager::TradingDrop_portfolio(){
	sql_drop_table(db_trading,"_portfolio");
}
void CDBManager::TradingCreate_portfolio(){
	QSqlQuery query(db_trading);
	SPortfolio Portfolio;
	Portfolio.Create(query);
}


// void CDBManager::QuikDrop_alldeals(){
// 	sql_drop_table(db_quik,"alldeals");
// }

// void CDBManager::QuikCreate_current(){
// 	QSqlQuery query(db_quik);
// 	//--------------- create -------------------------
// 	printf ("CREATE TABLE QUIK.CURRENT;		");
// 	query.exec("CREATE TABLE CURRENT \
// 			   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
// 			   instrument	CHAR(200) NULL, \
// 			   stcode		CHAR(12) NULL, \
// 			   shortname	CHAR(20) NULL, \
// 			   lot			INT UNSIGNED NULL, \
// 			   step			FLOAT NULL, \
// 			   demand		FLOAT NULL, \
// 			   supply		FLOAT NULL, \
// 			   PRIMARY KEY (ID));");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");	
// /*
// 	//--------------- alter -------------------------
// 	printf ("ALTER TABLE alldeals ENGINE=MyISAM;");
// 	query.exec("ALTER TABLE alldeals ENGINE=MyISAM");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database alter Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");	
// 
// 	printf("optimize table AllDeals;			");
// 	query.exec("optimize table AllDeals;");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");
// */
// }
// void CDBManager::QuikCreate_alldeals(){
// 	QSqlQuery query(db_quik);
// 	//--------------- create -------------------------
// 	printf ("CREATE TABLE ALLDEALS;		");
// 	query.exec("CREATE TABLE ALLDEALS \
// 			   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
// 				stcode		CHAR(12) NULL, \
// 				stdate		CHAR(20) NULL, \
// 				sttime		CHAR(15) NULL, \
// 				stvolume	FLOAT NULL, \
// 				stprice		FLOAT NULL, \
// 				stoperation	CHAR(15) NULL, \
// 			   PRIMARY KEY (ID));");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");	
// 	
// 	//--------------- alter -------------------------
// 	printf ("ALTER TABLE alldeals ENGINE=MyISAM;");
// 	query.exec("ALTER TABLE alldeals ENGINE=MyISAM");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database alter Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");	
// 		
// 	printf("optimize table AllDeals;			");
// 	query.exec("optimize table AllDeals;");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");
// 	
// }

// void CDBManager::QuikDropFinInst(){
// 	sql_drop_table(db_quik,"FININST");
// }
// void CDBManager::QuikCreateFinInst(){
// 	QSqlQuery query(db_quik);
// 	//--------------- create -------------------------
// 	printf ("CREATE TABLE FININST;		");
// 	query.exec("CREATE TABLE FININST \
// 			   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
// 			   instrument	CHAR(200) NULL, \
// 			   stcode		CHAR(12) NULL, \
// 			   demvol		INT UNSIGNED NULL, \
// 			   demand		FLOAT NULL, \
// 			   supply		FLOAT NULL, \
// 			   supvol		FLOAT NULL, \
// 			   PRIMARY KEY (ID));");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	else 
// 		printf ("- OK \n");	
// 
// }
// 
// 
// void CDBManager::QuikDropRates(QString InstrumentCode){
// 	sql_drop_table(db_quik,"rates_");
// }
// void CDBManager::QuikCreateRates(QString InstrumentCode){
// 	QSqlQuery query(db_quik);
// 
// 	QString TableName="rates_"+InstrumentCode;
// 	QString Cmd="CREATE TABLE IF NOT EXISTS "+ TableName+
// 		" (ID INT UNSIGNED NOT NULL AUTO_INCREMENT,"+
// 		" instrument	CHAR(200) NULL,"+ 
// 		" demvol		INT UNSIGNED NULL,"+
// 		" price		FLOAT NULL,"+
// 		" supvol		INT UNSIGNED NULL,"+
// 		" PRIMARY KEY (ID));";
// 	printf (STR(Cmd));
// 	//--------------- create -------------------------
// 	
// 	int err=query.exec(Cmd);
// 	//err=query.isValid();
// 	//QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	if (!query.isActive()){
// 		printf (" - ERROR\n");		
// 		QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	}
// 	else 
// 		printf ("- OK \n");	
// 
// 
// }
/*
void CDBManager::QuikUpdateRates(){
	char str[200];

	QSqlQuery query(db_quik);
	
	query.exec("SELECT * FROM FinInst ;");
	if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());

	while (query.next())
	{
		//int instrumentst			= query.value(1).toInt();
		QString stcode				= query.value(2).toString();	
		QuikDropAndCreateRates(stcode);
	
	}
}*/
//void CDBManager::TradingDrop_deal(QString StockCode){
//	sql_drop_table(db_trading,StockCode);
//}


void CDBManager::TradingCreate_req(QString StockCode)
{
	if (!db_trading.isValid() ){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	if (db_trading.isOpenError() ){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	QSqlQuery query(db_trading);
	StockCode.replace("-","_");
	//------------------------------------------------------------------
	//  —оздаем таблицу сделок с этой акцией если она отсутсвует 
	char cmd[1000];
	strcpy(cmd,"CREATE TABLE IF NOT EXISTS ");
	strcat(cmd,STR(StockCode));
	strcat(cmd,"_req");
	printf(cmd);
	strcat(cmd," (ID INT UNSIGNED NOT NULL AUTO_INCREMENT,\
			   trdate INT UNSIGNED NULL,\
			   trtime INT UNSIGNED NULL,\
			   volume INT NULL,\
			   price  FLOAT NULL,\
			   PRIMARY KEY (ID));");

	query.exec(cmd);
	if (!query.isActive()){
		printf(" - ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else
		printf(" - OK!\n");


}
void CDBManager::TradingCreate_trd(QString StockCode)
{
	if (StockCode.contains("-")){
		StockCode.replace("-","_");
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
	//  —оздаем таблицу сделок с этой акцией если она отсутсвует 
	char cmd[1000];
	strcpy(cmd,"CREATE TABLE IF NOT EXISTS ");
	strcat(cmd,STR(StockCode));
	strcat(cmd,"_trd");
	printf(cmd);
	strcat(cmd," (ID INT UNSIGNED NOT NULL AUTO_INCREMENT,\
			   trdate INT UNSIGNED NULL,\
			   trtime INT UNSIGNED NULL,\
			   volume INT NULL,\
			   price  FLOAT NULL,\
			   balance  FLOAT NULL,\
			   PRIMARY KEY (ID));");

	query.exec(cmd);
	if (!query.isActive()){
		printf(" - ERROR\n");
		printf("%s\n",cmd);
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else
		printf(" - OK!\n");
}
void CDBManager::TradingDropAll_trd(){
	if (db_trading.isOpenError() || !db_trading.isValid()){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	QSqlQuery query(db_trading);
	QString Cmd="SELECT * FROM _portfolio ORDER BY 3 ;";
	QQueue<QString> queInstrument;
	printf(STR(Cmd));
	query.exec(Cmd);
	QString Instrument;
	while (query.next())
	{
		Instrument = query.value(2).toString();	
		queInstrument << Instrument;
	}

	while (!queInstrument.isEmpty()){
		//if (queInstrument.head().contains("_trd")){
		sql_drop_table(db_trading,queInstrument.head()+"_trd");
		//}
		queInstrument.dequeue();
	}
}

void CDBManager::TradingDropAll_req(){
	if (db_trading.isOpenError() || !db_trading.isValid()){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	QSqlQuery query(db_trading);
	QString Cmd="SELECT * FROM _portfolio ORDER BY 3 ;";
	QQueue<QString> queInstrument;
	printf(STR(Cmd));
	query.exec(Cmd);
	QString Instrument;
	while (query.next())
	{
		Instrument = query.value(2).toString();	
		queInstrument << Instrument;
	}

	while (!queInstrument.isEmpty()){
		sql_drop_table(db_trading,queInstrument.head()+"_req");
		queInstrument.dequeue();
	}
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
	//  —оздаем таблицу сделок с этой акцией если она отсутсвует 
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
	//  —оздаем таблицу сделок с этой акцией если она отсутсвует 

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

void CDBManager::TradingCreateAllFrom_portfolio(){
	if (db_trading.isOpenError() || !db_trading.isValid()){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	QSqlQuery query(db_trading);
	QString Cmd="SELECT * FROM _portfolio ORDER BY 3 ;";
	QQueue<QString> queInstrument;
	printf(STR(Cmd));
	query.exec(Cmd);
	QString Instrument;
	while (query.next())
	{
		Instrument = query.value(2).toString();	
		queInstrument << Instrument;
		//Portfolio.Status			= query.value(1).toInt();
		//Portfolio.StockCode			= query.value(2).toString();	
		//Portfolio.StockName		= query.value(3).toString();	
		//Portfolio.LastOfferDate	= query.value(4).toInt();	
		//Portfolio.LastOfferTime	= query.value(5).toInt();	
		//Portfolio.Volume			= query.value(6).toInt();
		//Portfolio.Profit			= query.value(7).toDouble();

		// создаем таблицы если они не созданы
	}
	
	while (!queInstrument.isEmpty()){
		//TradingCreate_deal(queInstrument.head()); 
		TradingCreate_trd (queInstrument.head()); 
		TradingCreate_req (queInstrument.head()); 
		queInstrument.dequeue();
	}
}
void CDBManager::TradingClean_portfolio(){
	if (db_trading.isOpenError() || !db_trading.isValid()){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	QSqlQuery query(db_trading);
	QString Cmd="SELECT * FROM _portfolio ORDER BY 3 ;";
	QQueue<QString> queInstrument;
	printf(STR(Cmd));
	query.exec(Cmd);
	QString Instrument;
	while (query.next())
	{
		Instrument = query.value(2).toString();	
		queInstrument << Instrument;
		//Portfolio.Status			= query.value(1).toInt();
		//Portfolio.StockCode			= query.value(2).toString();	
		//Portfolio.StockName		= query.value(3).toString();	
		//Portfolio.LastOfferDate	= query.value(4).toInt();	
		//Portfolio.LastOfferTime	= query.value(5).toInt();	
		//Portfolio.Volume			= query.value(6).toInt();
		//Portfolio.Profit			= query.value(7).toDouble();

		// создаем таблицы если они не созданы
	}

	while (!queInstrument.isEmpty()){
		QString Cmd="SHOW TABLES LIKE '"+queInstrument.head()+"_deal'";
		query.exec(Cmd);
		VALID(query);
		if (!query.next()){
			printf("%s doesn't exists\n", STR(queInstrument.head()));
			Cmd="DELETE FROM _portfolio WHERE stcode='"+queInstrument.head()+"'";
			printf("%s\n",STR(Cmd));
			query.exec(Cmd);
			VALID(query);
		}
		else {
			Cmd="SHOW TABLE STATUS LIKE '"+queInstrument.head()+"_deal'";
			query.exec(Cmd);
			VALID(query);
			if (query.next()){
				int rows=query.value(4).toInt();
				Cmd="UPDATE _portfolio SET rows=" + QString::number(rows) + " WHERE stcode='"+ queInstrument.head()+"'";
				query.exec(Cmd);
				VALID(query);
			}
		}
		queInstrument.dequeue();
	}
}

void CDBManager::TradingDeleteDealsToday(){
	// —читываем названи€ таблиц 
	QSqlQuery query(db_trading);
	QString Cmd="SHOW TABLES FROM TRADING";
	printf(STR(Cmd));
	query.exec(Cmd);

	QQueue<QString> queInstrument;
	if (!query.isActive()){
		printf( " - ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else 
		printf( " - OK \n");

	while (query.next()) {
		QString title = query.value(0).toString();
		if (title=="_portfolio")
			continue;
		queInstrument << title;
	}

	while (!queInstrument.isEmpty()){
		QString TableName=queInstrument.dequeue();
		if (!TableName.contains("_deal"))
			continue;
		QDate qDate=QDate::currentDate();
		QString Today=qDate.toString("yyyyMMdd");

		QString Cmd="DELETE FROM trading." + TableName + " WHERE trdate="+ Today;
		//--------------- drop -------------------------
		printf (STR(Cmd));
		query.exec(Cmd);
		if (!query.isActive()){
			printf (" - ERROR \n");
			QMessageBox::critical(0, QObject::tr("Database drop Error"), query.lastError().text());
		}
		else 
			printf (" - OK \n");
		//queInstrument.dequeue();
	}




}


void CDBManager::TradingDropAll(){
	// —читываем названи€ таблиц 
	QSqlQuery query(db_trading);
	QString Cmd="SHOW TABLES FROM TRADING";
	printf(STR(Cmd));
	query.exec(Cmd);

	QQueue<QString> queInstrument;
	if (!query.isActive()){
		printf( " - ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else 
		printf( " - OK \n");

	while (query.next()) {
		QString title = query.value(0).toString();
		if (title=="_portfolio")
			continue;
		queInstrument << title;
	}

	while (!queInstrument.isEmpty()){
		QString TableName=queInstrument.head();
		QString Cmd="DROP TABLE " + TableName;
		//--------------- drop -------------------------
		printf (STR(Cmd));
		query.exec(Cmd);
		if (!query.isActive()){
			printf (" - ERROR \n");
			QMessageBox::critical(0, QObject::tr("Database drop Error"), query.lastError().text());
		}
		else 
			printf (" - OK \n");
		queInstrument.dequeue();
	}
	

	

}
void CDBManager::TradingDropAllEmpty(){
	// —читываем названи€ таблиц 
	QSqlQuery query(db_trading);
	QString Cmd="SHOW TABLES FROM TRADING";
	printf(STR(Cmd));
	query.exec(Cmd);

	QQueue<QString> queInstrument;
	if (!query.isActive()){
		printf( " - ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else 
		printf( " - OK \n");

	while (query.next()) {
		QString title = query.value(0).toString();
		if (title=="_portfolio")
			continue;
		queInstrument << title;
	}

	while (!queInstrument.isEmpty()){
		QString TableName=queInstrument.head();
		QString Cmd="SELECT * FROM " + TableName + " LIMIT 10";
		query.exec(Cmd);
		if (!query.isActive()){
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
			continue;
		}

		//--------------- drop -------------------------
		if (!query.next()){
			QString Cmd="DROP TABLE " + TableName;
			//--------------- drop -------------------------
			printf (STR(Cmd));
			query.exec(Cmd);
			if (!query.isActive()){
				printf (" - ERROR \n");
				QMessageBox::critical(0, QObject::tr("Database drop Error"), query.lastError().text());
			}
			else 
				printf (" - OK \n");
		} else {
			int g=1;
		}
		queInstrument.dequeue();
	}
}

void CDBManager::TradingDropAllSmall(){
	// —читываем названи€ таблиц 
	QSqlQuery query(db_trading);
	QString Cmd="SHOW TABLES FROM TRADING";
	printf(STR(Cmd));
	query.exec(Cmd);

	QQueue<QString> queInstrument;
	VALID(query);

	while (query.next()) {
		QString title = query.value(0).toString();
		if (title=="_portfolio")
			continue;
		queInstrument << title;
	}

	while (!queInstrument.isEmpty()){
		QString TableName=queInstrument.head();
		
		if (TableName.contains("_deal")){
			QString Cmd="SHOW TABLE STATUS LIKE '" + TableName +"'";
			query.exec(Cmd);
			VALID(query);

			//--------------- drop -------------------------
			if (query.next()){
				int Rows=query.value(4).toInt();
				printf("%s %d\n",STR(TableName),Rows);
				if (Rows<1000){
					//QString Cmd="DROP TABLE " + TableName;
					QSqlQuery query2(db_trading);
					Cmd="DROP TABLE IF EXISTS " + TableName;
					printf("%s\n",STR(Cmd));
					query2.exec(Cmd);
					VALID(query2);

					Cmd.replace("_deal","_req");
					printf("%s\n",STR(Cmd));
					query2.exec(Cmd);
					VALID(query2);

					Cmd.replace("_req","_trd");
					printf("%s\n",STR(Cmd));
					query2.exec(Cmd);
					VALID(query2);

				}
				
				//--------------- drop -------------------------
				//printf (STR(Cmd));
				//query.exec(Cmd);
			
			}
		}
		queInstrument.dequeue();
	}
}

void CDBManager::TradingAlterAll(){
	// —читываем названи€ таблиц 
	QSqlQuery query(db_trading);
	QString Cmd="SHOW TABLES FROM TRADING";
	printf(STR(Cmd));
	query.exec(Cmd);

	QQueue<QString> queInstrument;
	if (!query.isActive()){
		printf( " - ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else 
		printf( " - OK \n");

	while (query.next()) {
		QString title = query.value(0).toString();
		//if (title=="_portfolio")
		//	continue;
		queInstrument << title;
	}

	
	while (!queInstrument.isEmpty()){
		QString TableName=queInstrument.head();
		QString Cmd="ALTER TABLE " + TableName + "  ENGINE=MyISAM";
		printf (STR(Cmd));
		query.exec(Cmd);
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database alter Error"), query.lastError().text());
		else 
			printf ("- OK \n");	
		
		queInstrument.dequeue();
	}
}

// 
// void CDBManager::QuikDropAndCreateAllDeals(){
// 	QuikDrop_alldeals();
// 	QuikCreate_alldeals();
// 
// }
// void CDBManager::QuikDropAndCreateFinInst(){
// 
// }
// void CDBManager::QuikDropAndCreateRates(){
// 
// }

// 
// int CDBManager::CheckTables(){
// 
// 	int checkok=1;
// 	QSqlQuery queryl(db_quik);
// 	
// 	queryl.exec("CHECK TABLE ALLDEALS");
// 
// 	printf("checking quik.alltables...\n");
// 	if (!queryl.isActive()){
// 		printf( " - ERROR\n");
// 		QMessageBox::critical(0, QObject::tr("Database Error"), queryl.lastError().text());
// 	}
// 	//else 
// 	//	printf( " - OK \n");
// 
// 	
// 	while (queryl.next()) {
// 		QString title1 = queryl.value(0).toString();
// 		QString title2 = queryl.value(1).toString();
// 		QString title3 = queryl.value(2).toString();
// 		QString title4 = queryl.value(3).toString();
// 		QString title=":: "+title1+" "+title2+" "+title3+ " "+title4;
// 		printf(STR(title));
// 		printf("\n");
// 		_ASSERTE (title4=="OK");
// 	}
// 
// 
// 	
// 
// 	printf("checking tables in trading database...\n");
// 	// —читываем названи€ таблиц 
// 	QSqlQuery query(db_trading);
// 	query.exec("SHOW TABLES FROM TRADING");
// 
// 	while (query.next()) {
// 		QString title = query.value(0).toString();
// 
// 		if (!query.isActive())
// 			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
// 		//query.lastError().showMessage();
// 
// 		//printf("%s",STR(title));
// 
// 		
// 
// 		QSqlQuery CheckQuery(db_trading);
// 		QString CheckStr="CHECK TABLE "+title;
// 		CheckQuery.exec(CheckStr);
// 		printf("checking %s ...\n",STR(title));
// 		if (!CheckQuery.isActive()){
// 			QMessageBox::critical(0, QObject::tr("Database Error"), CheckQuery.lastError().text());
// 			printf("- ERROR\n");
// 			checkok=0;
// 		}
// 		//else {
// 		//	printf("- OK\n");
// 		//}
// 		while (CheckQuery.next()) {
// 			QString title1 = CheckQuery.value(0).toString();
// 			QString title2 = CheckQuery.value(1).toString();
// 			QString title3 = CheckQuery.value(2).toString();
// 			QString title4 = CheckQuery.value(3).toString();
// 			QString err=":: "+title1+" "+title2+" "+title3+ " "+title4;
// 			printf(STR(err));
// 			printf("\n");
// 			if (title4!="OK"){
// 				QSqlQuery RepairQuery(db_trading);
// 				QString Str="REPAIR TABLE "+title;
// 				RepairQuery.exec(Str);
// 				if (!RepairQuery.isActive()){
// 					QMessageBox::critical(0, QObject::tr("Database Error"), RepairQuery.lastError().text());
// 					printf("- ERROR\n");
// 				}
// 				while (RepairQuery.next()) {
// 					QString title1 = RepairQuery.value(0).toString();
// 					QString title2 = RepairQuery.value(1).toString();
// 					QString title3 = RepairQuery.value(2).toString();
// 					QString title4 = RepairQuery.value(3).toString();
// 					QString err=":: "+title1+" "+title2+" "+title3+ " "+title4;
// 					printf(STR(err));
// 					printf("\n");
// 				}
// 			}
// 		}
// 
// 	}
// 
// 	//------------------------------
// 	if (checkok)
// 		printf ("Table checking - OK!!! \n");
// 	else
// 		printf ("Table checking - FAILED!!! \n");
// 	return true;
// 
// }

// 
// void CDBManager::QuikDrop_request(){
// 
// }
// void CDBManager::QuikCreate_request(){
// 	QSqlQuery query(db_quik);
// 	//--------------- create -------------------------
// 	printf ("CREATE TABLE REQUEST;		");
// 	query.exec("CREATE TABLE REQUEST \
// 			   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
// 			   stcode		CHAR(12) NULL, \
// 			   stdate		CHAR(20) NULL, \
// 			   sttime		CHAR(15) NULL, \
// 			   stvolume		FLOAT, \
// 			   stprice		FLOAT, \
// 			   strests		INT UNSIGNED NULL,\
// 			   stoperation	CHAR(15) NULL, \
// 			   ststatus		CHAR(15) NULL, \
// 			   stnumber		INT UNSIGNED NULL,\
// 			   sttransid	INT UNSIGNED NULL,\
// 			   PRIMARY KEY (ID));");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	else 
// 		printf ("Creation quik.request - OK \n");	
// 
// 	/*
// 	//--------------- alter -------------------------
// 	printf ("ALTER TABLE alldeals ENGINE=MyISAM;");
// 	query.exec("ALTER TABLE alldeals ENGINE=MyISAM");
// 	if (!query.isActive())
// 	QMessageBox::critical(0, QObject::tr("Database alter Error"), query.lastError().text());
// 	else 
// 	printf ("- OK \n");	
// 
// 	printf("optimize table AllDeals;			");
// 	query.exec("optimize table AllDeals;");
// 	if (!query.isActive())
// 	QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
// 	else 
// 	printf ("- OK \n");
// 	*/
// }
// void CDBManager::QuikCreate_trade(){
// 	QSqlQuery query(db_quik);
// 	//--------------- create -------------------------
// 	printf ("CREATE TABLE TRADE;		");
// 	query.exec("CREATE TABLE TRADE \
// 			   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
// 			   stcode		CHAR(12) NULL, \
// 			   stdate		CHAR(20) NULL, \
// 			   sttime		CHAR(15) NULL, \
// 			   stvolume		FLOAT, \
// 			   stprice		FLOAT, \
// 			   stoperation	CHAR(15) NULL, \
// 			   stnumber		INT UNSIGNED NULL,\
// 			   storder		INT UNSIGNED NULL,\
// 			   PRIMARY KEY (ID));");
// 	if (!query.isActive())
// 		QMessageBox::critical(0, QObject::tr("Database create Error"), query.lastError().text());
// 	else 
// 		printf ("Creation quik.trade - OK \n");	
// 
// 	/*
// 	//--------------- alter -------------------------
// 	printf ("ALTER TABLE alldeals ENGINE=MyISAM;");
// 	query.exec("ALTER TABLE alldeals ENGINE=MyISAM");
// 	if (!query.isActive())
// 	QMessageBox::critical(0, QObject::tr("Database alter Error"), query.lastError().text());
// 	else 
// 	printf ("- OK \n");	
// 
// 	printf("optimize table AllDeals;			");
// 	query.exec("optimize table AllDeals;");
// 	if (!query.isActive())
// 	QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
// 	else 
// 	printf ("- OK \n");
// 	*/
// }

// void CDBManager::TradingUpdateAllInSecond(){
// 
// 	QSqlQuery query(db_trading);
// 	query.exec("SHOW TABLES FROM TRADING");
// 	VALID(query);
// 	QQueue<QString> queTableDeals;
// 	while (query.next()) {
// 		QString title = query.value(0).toString();
// 		if (title.contains("_deal")){
// 			queTableDeals<<title;
// 		}
// 	}
// 
// 	while(!queTableDeals.isEmpty()){
// 		QString table=queTableDeals.dequeue();
// 		Trading_UpdateInSecond(table, 20090501,20090630);
// 	}
// }
// 
// void CDBManager::Trading_UpdateInSecond(QString StockCode, int nDate0, int nDate1){
// 	QSqlQuery query(db_trading);
// 	//QString Cmd="SELECT * FROM " + StockCode + "_deal WHERE trdate> "+QString::number(nDate) + " ORDER BY  1";
// 	QString Cmd="SELECT * FROM " + StockCode + " WHERE trdate>= "+QString::number(nDate0) + " AND trdate<= " + QString::number(nDate1) + " ORDER BY  1";
// 	printf(STR(Cmd));
// 	printf("\n");
// 	query.exec(Cmd);
// 	VALID(query);
// 
// 
// 	SDeal Deal;
// 	QVector<SDeal> vecDeal;
// 	printf("reading...\n");
// 	while (query.next()){
// 		
// 		Deal.Parse(query);
// 		vecDeal<<Deal;
// 	}
// 	printf("updating...\n");
// 	//--- first -------
// 	if (vecDeal.size()>0){
// 		SDeal& Deal=vecDeal[0];
// 		Deal.nType&=0xFFFFFF00;
// 		Deal.SetFlag(FIRST_IN_SECOND);
// 		if (vecDeal.size()>1)
// 			if (Deal.nTime==vecDeal[1].nTime)
// 				Deal.SetFlag(IN_SECOND);
// 		
// 		if (Deal.IsSupply())
// 			Deal.SetSupplyType();
// 		else
// 			Deal.SetDemandType();
// 		QString Cmd="UPDATE " + StockCode + " SET trtype=" + QString::number(Deal.nType) + " WHERE ID="+ QString::number(Deal.nID);
// 		query.exec(Cmd);
// 		VALID(query);
// 	}
// 
// 	for(int i=1; i<vecDeal.size()-2;i++){
// 		SDeal& Deal=vecDeal[i];
// 		Deal.nType&=0xFFFFFF00;
// 		if (vecDeal[i-1].nTime<Deal.nTime)
// 			Deal.SetFlag(FIRST_IN_SECOND);
// 		else 
// 			Deal.SetFlag(IN_SECOND);
// 		if (Deal.nTime==vecDeal[i+1].nTime){
// 			Deal.SetFlag(IN_SECOND);
// 		}
// 		if (Deal.IsSupply())
// 			Deal.SetSupplyType();
// 		else
// 			Deal.SetDemandType();
// 
// 		QString Cmd="UPDATE " + StockCode + " SET trtype=" + QString::number(Deal.nType) + " WHERE ID="+ QString::number(Deal.nID);
// 		query.exec(Cmd);
// 		VALID(query);
// 
// 	}
// 	//--- last -------
// 	if (vecDeal.size()>1){
// 		int size=vecDeal.size();
// 		SDeal& Deal=vecDeal[size-1];
// 		Deal.nType&=0xFFFFFF00;
// 		if (vecDeal[size-2].nTime<Deal.nTime)
// 			Deal.SetFlag(FIRST_IN_SECOND);
// 		else 
// 			Deal.SetFlag(IN_SECOND);
// 
// 		if (Deal.IsSupply())
// 			Deal.SetSupplyType();
// 		else
// 			Deal.SetDemandType();
// 
// 		QString Cmd="UPDATE " + StockCode + " SET trtype=" + QString::number(Deal.nType) + " WHERE ID="+ QString::number(Deal.nID);
// 		query.exec(Cmd);
// 		VALID(query);
// 	}
// 
// 
// }

/*
void CDManager::TradingRemoveDuplicates(QString Instrument){
	QSqlQuery query(db_trading);
	QString Cmd="SELECT * from ";
	//query.exec("SCREATE TABLE QUIK.REQUEST \);


}*/
/*
void CDBManager::TradingOptimizeAll(){


query.exec("ALTER TABLE " + StockCode+"_deal ENGINE=MyISAM");
if (!query.isActive())
QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
else 
printf ("Set ENGINE=MyISAM - OK \n");




	int checkok=1;
	QSqlQuery queryl(db_mylefco);
	queryl.exec("SHOW TABLES FROM trading");

	printf("checking tables in mylefco...\n");
	while (queryl.next()) {
		QString title = queryl.value(0).toString();

		if (!queryl.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), queryl.lastError().text());
		//query.lastError().showMessage();

		printf("%s",title.toAscii().data());



		QSqlQuery CheckQuery(db_mylefco);
		QString CheckStr="CHECK TABLE "+title;
		CheckQuery.exec(CheckStr);
		if (!CheckQuery.isActive()){
			QMessageBox::critical(0, QObject::tr("Database Error"), CheckQuery.lastError().text());
			checkok=0;
		}
		else {
			printf("- OK\n");
		}
	}
	
}*/

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



void Ticks2Mysql ( QSqlQuery& query, QString seccode, S_EasyTicks& ticks,   uint fromIndex, uint count, unsigned afterDateTime)
{
	if (count==0)
		return;

	int err=query.prepare("INSERT INTO " + seccode+ "_deal (trdate,trtime,volume,price,trtype)  VALUES (?, ?, ?, ?, ?)");

	QVariantList listDate;
	QVariantList listTime;
	QVariantList listVolume;
	QVariantList listPrice;
	QVariantList listType;

	//if (echo)
	//	qDebug()<< "<" +seccode+ ">";

	S_Tick* pTick = &ticks.data[fromIndex];
	for (int i=0; i<count; i++,pTick++){
		if (pTick->datetime>afterDateTime){
			listDate	<< pTick->DateTime().toString("yyyyMMdd");
			listTime	<< pTick->DateTime().toString("hhmmss");
			listVolume	<< pTick->quantity;
			listPrice	<< pTick->price;
			listType	<< pTick->type;
		}
		//if (echo)
		//	qDebug()<<"    " << tick.toXML() ;//<< "\n";
	
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

void Quotes2Mysql( QSqlQuery& query, QString seccode, S_EasyQuotes& quotes, uint fromIndex, uint count, bool echo)
{


	if (count==0)
		return;

	int err=query.prepare("INSERT INTO " + seccode+ "_quote (datetime,price,buy,sell)  VALUES (?, ?, ?, ?)");

	QVariantList listDatetime;
	QVariantList listSell;
	QVariantList listBuy;
	QVariantList listPrice;


	if (echo)
		qDebug()<< "<" +seccode+ ">";
	for (uint i=fromIndex; i<fromIndex+count; i++){
		C_EasyQuote& quote =quotes[i];

		listDatetime<< quote.datetime;
		listPrice	<< quote.price;
		listBuy  	<< quote.buy;
		listSell	<< quote.sell;
		//if (echo)
		//	qDebug()<<"    " << tick.toXML() ;//<< "\n";
		//}
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


uint sql_read_ticks(QSqlDatabase& db, QString StockCode, QDateTime& fromDT, QDateTime& toDT, S_EasyTicks& ticks)
{
	QSqlQuery query(db);


	//QDateTime DateTime0; DateTime0.setTime_t(fromDT);
	//QDateTime DateTime1; DateTime1.setTime_t(fromDT);

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



uint sql_read_quotes(QSqlDatabase& db, QString StockCode, QDateTime& fromDT, QDateTime& toDT, S_EasyQuotes& quotes)
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
		C_EasyQuote& quote =quotes[quotes.size];

		quote.datetime	=query.value(1).toInt();
		quote.price		=query.value(2).toFloat();
		quote.buy		=query.value(3).toInt();
		quote.sell		=query.value(4).toInt();
		quotes.size++;
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

