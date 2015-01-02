#pragma once


#include <QMessageBox>
#include <QSQLError>
#include <QSQLQuery>
#include <QVariant>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QQueue>
#include <QMutex>
#define IN_SECOND 1
#define FIRST_IN_SECOND 2
//#define LAST_IN_SECOND 4

#define STR(a) a.toAscii().data()
#define RUS(str) QString::fromLocal8Bit(str.toAscii().data())
#define ASCII(str) str.toAscii().data()
#define DISPLAY_ERROR(query) \
				printf(ASCII( query.lastError().text())); 	printf("\n"); \
				printf(ASCII( query.lastQuery())); 			printf("\n"); \
				QMessageBox::critical(0, QObject::tr("query exection Error"), query.lastError().text());

//#define VALID(query) if (!query.isActive()){ QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text()) ;}
#define VALID(query) if (!query.isActive()){ DISPLAY_ERROR(query);}

#define REQUEST_REGULAR 0
#define REQUEST_URGENT 1
#define REQUEST_IDLE 0
#define REQUEST_DIED 1024

#define REQUEST_NEW_SEND 64
#define REQUEST_NEW_ORDER		1
#define REQUEST_NEW_ACCEPTED	2
#define REQUEST_NEW_COMPLETED	4
#define REQUEST_NEW_REFUSED		8

#define REQUEST_KILL_SEND 4
#define REQUEST_KILL_ORDER		0x10
#define REQUEST_KILL_ACCEPTED	0x20
#define REQUEST_KILL_REMOVAL	0x40
#define REQUEST_KILL_REFUSED	0x80


#define REQUEST_SUPPLY	0x80000000
#define REQUEST_DEMAND  0x40000000
#define STATUS_REQUEST_VALID REQUEST_DIED*2 



#define STATUS_REPLY_VALID REQUEST_KILL_REFUSED*2




#define REQUEST_KILL_TTL 1000
#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef ABS
#define ABS(a)    ((a) < (0) ? -(a): (a))
#endif

inline int QDate2Int(QDate qDate){
	return qDate.year()*10000+qDate.month()*100+qDate.day();
}
inline int QTime2Int(QTime qTime){
	return qTime.hour()*10000+qTime.minute()*100+qTime.second();
}
inline int intDate(QDateTime& DateTime){
	QDate qDate = DateTime.date();
	int nDate=qDate.year()*10000+qDate.month() *100+qDate.day();
	return nDate;
	
}
inline int intTime(QDateTime& DateTime){
	QTime qTime = DateTime.time();
	int nTime=qTime.hour()*10000+qTime.minute()*100+qTime.second();
	return nTime;
}
inline QString time2str(int time){
	QTime Time(time/10000,(time%10000)/100,time%100);
	return Time.toString();
}
inline QString date2str(int date){
	QDate Date(date/10000,(date%10000)/100,date%100);
	return Date.toString();
}

#define IN_SECOND 1
#define FIRST_IN_SECOND 2
#define LAST_IN_SECOND 4

/*
struct PPP{
	QMutex mute;

};*/
#define EMPTY 0
/*
class C_Tick{
public:
	//QString secid;		// 4
	QString board;		// TQBR
	QString seccode;	// LKOH
	QString tradeno;	// 2397108618
	QString tradetime;	// 08.08.2014 09:59:59
	QString price;		// 1928.3
	QString quantity;	// 2
	QString period;		// L
	QString buysell;	// S
};
*/

class C_Tick{
public:
	//QString secid;		// 4
	QString board;		// TQBR
	QString seccode;	// LKOH
	QString tradeno;	// 2397108618
	QString tradetime;	// 08.08.2014 09:59:59
	QString price;		// 1928.3
	QString quantity;	// 2
	QString period;		// L
	QString buysell;	// S
};


struct SDeal{
	long long		nID;
	int		nDate;
	int		nTime;
	int		nVolume;
	float	Price;
	int		nType;
	SDeal (){
		nID		=EMPTY;
		nDate	=EMPTY;
		nTime	=EMPTY;
		nVolume	=EMPTY;
		Price	=EMPTY;
		nType	=EMPTY;
	}
	void Set(QDateTime &DateTime, int volume, float price, int type){
		QDate qDate = DateTime.date();
		QTime qTime = DateTime.time();
		nDate=qDate.year()*10000+qDate.month() *100+qDate.day();
		nTime=qTime.hour()*10000+qTime.minute()*100+qTime.second();
		nVolume=volume;
		Price =price;
		nType=type;
	}
	void Show(int br=1){
		char str[256];
		
		int insec=nType;
		if (br)
			sprintf(str," d:%d d:%d t:%d p:%.4f v:%d\t insec:%x \n",long(nID), nDate, nTime, Price, nVolume, insec);
		else 
			sprintf(str," d:%d d:%d t:%d p:%.4f v:%d\t insec:%x ",  long(nID), nDate, nTime, Price, nVolume, insec);
		printf(str);
	}
	void Parse(QSqlQuery& query){
		nID		= query.value(0).toInt();
		nDate	= query.value(1).toInt();
		nTime	= query.value(2).toInt();
		nVolume	= query.value(3).toInt();
		Price	= query.value(4).toDouble();
		nType	= query.value(5).toInt();
	}
	bool InSecond(){		// любая сделка у которой время сопадает с предыдущей или с последующей
		return (nType&IN_SECOND);
	}
	int IsLastInSecond(){
		return (nType&LAST_IN_SECOND);
	}	
	int IsFirstInSecond(){ // любая сделка у которой время отличатеся от предыдущей
		return (nType&FIRST_IN_SECOND);
	}

	int Propobality(){
		return ((nType>>8));
	}
	// Купля (более высокая цена). На бирже выставлена заявка на продажу. Предложение
	int IsSupply(){
		return (nType>>8)<0;
	}
	int IsSell(){
		return (nType>>8)<0;
	}
	// Продажа (более низкая цена). На бирже выставлена заявка на покупку. Спрос
	int IsDemand(){
		return (nType>>8)>0;
	}
	int IsBuy(){
		return (nType>>8)>0;
	}
	// Купля (более высокая цена). На бирже выставлена заявка на продажу. Предложение
	void SetSupplyType(){
		nType&=0xFF;
		nType|=(-1)<<8;
	}
	// Продажа (более низкая цена). На бирже выставлена заявка на покупку. Спрос
	void SetDemandType(){
		nType&=0xFF;
		nType|=(1)<<8;
	}
	
	void SetDelay(){
		QTime	qDealTime=QTime(nTime/10000,(nTime%10000)/100,nTime%100);
		int		nDelay=qDealTime.secsTo(QTime::currentTime());
		nType&=~0xF0;
		nType|=(nDelay&0xF)<<4;
	}
	int GetDelay(){
		return (nType&0xF)>>4;
	}
	void SetFlag(int flag){
		nType|=flag;
	}
	void ClearFlag(int flag){
		nType&=~flag;
	}

	int AbsTime(){
		return ((nDate<<18) | nTime);
	}
	QTime GetQTime(){
		return QTime(nTime/10000,(nTime%10000)/100,nTime%100);
	}
	QDate GetQDate(){
		return QDate(nDate/10000,(nDate%10000)/100,nDate%100);
	}
	void SetQTime(QTime& qTime){
		nTime=qTime.hour()*10000+qTime.minute()*100+qTime.second();
	}
	void SetQDate(QDate& qDate){
		nDate=qDate.year()*10000+qDate.month()*100+qDate.day();
	}

	int TimeSecs(){
		QTime	qTime0(10,00,00);
		QTime	qTime1(nTime/10000,(nTime%10000)/100,nTime%100);
		int		nSecs=qTime1.secsTo(qTime0);
		return	-nSecs;
	}
};
typedef SDeal SDealData ;

struct SDealName: public SDealData{
	//int		nID;
	//int		nDate;
	//int		nTime;
	//int		nVolume;
	//float	Price;
	//int		nType;
	QString Name;

	SDealName (){
		nID=-1;
		nDate=-1;
		nTime=-1;
		nVolume=-1;
		Price=0;
		nType=0;
		Name="";
	}
	SDealName(QString &str, QDateTime &DateTime, int volume, float price, int type){
		Name=str;
		QDate qDate = DateTime.date();
		QTime qTime = DateTime.time();
		nDate=qDate.year()*10000+qDate.month() *100+qDate.day();
		nTime=qTime.hour()*10000+qTime.minute()*100+qTime.second();
		nVolume=volume;
		Price =price;
		nType=type;
	}


	void Show(int br=1){
		char str[256];
		int delay=GetDelay();
		int op=Propobality();
		int insec=nType&7;
		if (br)
			sprintf(str,"d:%d d:%d t:%d %s p:%.4f v:%d\t dir:%d op:%d insec:%d \n",nID, nDate, nTime, STR(Name), Price, nVolume, op, delay, insec);
		else 
			sprintf(str,"d:%d d:%d t:%d %s p:%.4f v:%d\t dir:%d op:%d insec:%d ",  nID, nDate, nTime, STR(Name), Price, nVolume, op, delay, insec);
		printf(str);
	}
	

};
/*
struct SRequest{
	SRequest(float pr, int vol){
		price=pr;
		volume=vol;
	}
	bool operator == (SRequest& op){
		if (price==op.price &&
			volume==op.volume )
		return 1;
		else 
			return 0;
	}
	float	price;
	int		volume;
};*/

template<class T> struct SRange {
	T min;
	T max;
};
struct SPortfolio;
struct SRequest2{
	float	Price;			// цена
	int		nVolume;		// колво акций(лотов)
	int		nTime;			// время подачи заявки (по компьютеру)
	int		nDate;			// дата подачи заявки (по компьютеру)
	int		nStatus;		// стаус заявки
	int		nTransID;		// идентификатор заявки
	QQueue<SDeal> queDeal;	// сделки которые совершились по данной заявке
};
struct SRequest{
public:
	int nType;
	int nErrors;
	//STransaqtion* pTransaction;
	//QTime KillTimeAccepted;
	SPortfolio *pPortfolio;
	QQueue<SDeal> queDeal;
	//QMutex mutex;
	SRequest(float pr, int vol){
		Price=pr;
		nVolume=vol;
		queDeal.clear();
	}

	void print(){
		printf("%d\t%d\t%.2f\t%d\t%d\t%d\t%d\n",nTransID,nOrder,Price,nVolume,nStatusRequest,nStatusReply,nType);
	}
	bool operator == (SRequest& op){
		if (Price==op.Price &&
			nVolume==op.nVolume )
		return 1;
			else 
		return 0;
	}
	//QString Instrument;
	float	Price;
	int		nVolume;
	SRange<float>	DealRange;
	//int		nLevel;
	int		nTime;
	int		nDate;
	QTime	nTimeReply;
	int		nTransID;
	int		nOrder;
	//int		nTransID;
	//int		nActive;
	unsigned nStatusRequest;// Эту переменную модифицирует только trader
	unsigned nStatusReply;	// Эту переменную модифицирует только terminal

	SRequest(){
		Price=0;
		nVolume=0;
		DealRange.min=0;
		DealRange.max=0;
		//nLevel=0;
		nTime=0;
		//nTimeReply=0;
		nDate=0;
		nStatusRequest=0;
		nStatusReply=0;
		nTransID=0;
		nOrder=0;
		nErrors=0;
		nType = REQUEST_REGULAR;

	}
	int AbsTime(){
		return ((nDate<<18) | nTime);
	}
	//QMutex mutex;
	/*
	void EnqueueDeal(SDeal& Deal){
		_ASSERTE(nStatus<=REQUEST_VALID);
		//mutex.lock();
		queDeal.enqueue(Deal);
		//mutex.unlock();
		_ASSERTE(nStatus<=REQUEST_VALID);
	}
	void DequeueDeal(){
		_ASSERTE(nStatus<=REQUEST_VALID);
		//mutex.lock();
		queDeal.dequeue();
		//mutex.unlock();
		_ASSERTE(nStatus<=REQUEST_VALID);
	}*/
	void SetStatus(int status){
		//_ASSERTE(nStatus<=REQUEST_VALID);
		//mutex.lock();
		nStatusRequest|=status;
		//mutex.unlock();
		
		//_ASSERTE(nStatusRequest<=REQUEST_VALID || (nStatusRequest&REQUEST_DIED));
	}
	void CleanStatus(int status){
		//_ASSERTE(nStatus<=REQUEST_VALID);
		//mutex.lock();
		nStatusRequest&=~status;
		//mutex.unlock();
		//_ASSERTE(nStatusRequest<=REQUEST_VALID);

	}
	int InStatus(int status){
		//_ASSERTE(nStatus<=REQUEST_VALID || (nStatus&REQUEST_DIED));
		//mutex.lock();
		int res =nStatusRequest&status;
		//mutex.unlock();
		//_ASSERTE(nStatus<=REQUEST_VALID);
		return res;
	}
	bool IsSupply(){
		return ((nStatusRequest&REQUEST_SUPPLY)!=0);
	}
	bool IsDemand(){
		return ((nStatusRequest&REQUEST_DEMAND)!=0);
	}
	int TimeSecs(){
		QTime	qTime0(10,00,00);
		QTime	qTime1(nTime/10000,(nTime%10000)/100,nTime%100);
		int		nSecs=qTime1.secsTo(qTime0);
		return	-nSecs;
	}
};
struct SGlass{
	QList<SRequest> listSupply;
	QList<SRequest> listDemand;
	int nTime;
	bool operator == (SGlass& op){
		return (nTime==op.nTime && listDemand==op.listDemand && listSupply==op.listSupply);
			
	}
	int TimeSecs(){
		QTime	qTime0(10,00,00);
		QTime	qTime1(nTime/10000,(nTime%10000)/100,nTime%100);
		int		nSecs=qTime1.secsTo(qTime0);
		return	-nSecs;
	}

};

struct SDealID{
	SRequest* pRequest;
	SDeal Deal;
};
struct SStatusID{
	SRequest* pRequest;
	int nStatus;
	int nOrder;
};

struct SPortfolio{

public:
	int		Status;			// db
	QString StockCode;		// db
	QString StockName;		// db
	int		LastOfferDate;	// db
	int		LastOfferTime;	// db
	int		Volume;			// db
	float	Cash;			// db
	float	Profit;			// db
	int		StocksToBuy;	// db
	int		StocksToSell;	// db
	float	BuyPrice;		// db - Минимальная цена покупки среди моих заявок
	float	SellPrice;		// db - Максимальная цена продажи среди моих заявок 
	float	LastOfferDemand;// db
	float	LastOfferSupply;// db
	//float	LastDealDemand;	// db
	//float	LastDealSupply;	// db

	float	Commission;
	float	CommissionRate;
	SDeal   LastDeal;
	SDealData*	pLastDealDemand;	// db
	SDealData* 	pLastDealSupply;	// db
	SDealData LastDealDemand;	// db
	SDealData LastDealSupply;	// db

	SDealData LastDealRecieved;	// Последня принятая сделка из БД от квика 
	SDealData LastDealStored;	// Последняя классифицированная и сохраненная в БД по инструменту
	

	long long		LastID;			// db - ID последней сделки
	SDeal*	pLastDealClassified;
	QQueue<SDealData>	quePastDeals;
	QQueue<SDealData>	queCurrDeals;
	QQueue<SDealData>	queDeals;

	QList<SRequest*> listRequestSupply;
	QList<SRequest*> listRequestDemand;
	QMutex mutex;

	QQueue<SDealID> queDealID;
	QQueue<SStatusID> queStatusID;
	QQueue<SRequest*> queDemandTrash;
	QQueue<SRequest*> queSupplyTrash;

	/*
	void QueueDeal(SRequest* pRequest,SDeal& Deal){
		SDealID DealID={pRequest, Deal};
		DealID.Deal=Deal;
		mutex.lock();
		queDealID<<DealID;
		mutex.unlock();
	}
	*/
	void QueueTerminalReply(SRequest* pRequest, int nStatus, int nOrder){
		SStatusID StatusID={pRequest,nStatus, nOrder};
		mutex.lock();
		queStatusID<<StatusID;
		mutex.unlock();
	}

	SGlass Glass;

	int GetAutoIncrement(QSqlQuery& query){
		QString Cmd = "SHOW TABLE STATUS";
		query.exec(Cmd);
		if (!query.isActive()){
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
			return 0;
		}
		while(query.next()){
			QString Name=query.value(0).toString();
			if (Name!=StockCode.toLower()+"_deal")
				continue;
			int indx=query.value(10).toInt();
			return indx;
		}

	}
	SPortfolio(){
		Status=0;
		StockCode="";
		StockName="";
		LastOfferDate=0;
		LastOfferTime=0;
		Volume=0;
		Cash=0;
		Profit=0;
		StocksToBuy=0;
		StocksToSell=0;
		BuyPrice=0;
		SellPrice=0;
		LastID=0;
		pLastDealDemand=new SDeal;
		pLastDealSupply=new SDeal;
		//pLastDealClassified=&LastDealSupply;
		//SRequest d(0,0);
		//SRequest s(10000,0);
		//Glass.listDemand.append(d);
		//Glass.listSupply.append(s);

	}
	int Update(QSqlQuery& query, QList<QString>& listField){
		QString Cmd = QString("UPDATE _PORTFOLIO SET ");
		QString Comma=",";
		int listSize=listField.size();
		for(int i=0; i<listSize; i++){
			if (i==listSize-1)
				Comma="";
			QString& Field=listField[i];
			if		(Field=="status")
				Cmd+= "status=" +			QString::number(Status) + Comma;
			else if (Field=="lastofferdate")
				Cmd+= "lastofferdate=" +	QString::number(LastDealStored.nDate) + Comma;
			else if (Field=="lastoffertime")
				Cmd+= "lastoffertime=" +	QString::number(LastDealStored.nTime) + Comma;
			else if (Field=="volume")
				Cmd+= "volume=" +			QString::number(Volume) + Comma;
			else if (Field=="cash")
				Cmd+= "cash=" +				QString::number(Cash) + Comma;
			else if (Field=="profit")
				Cmd+= "profit=" +			QString::number(Profit) + Comma;
			else if (Field=="buyvol")
				Cmd+= "buyvol=" +			QString::number(StocksToBuy) + Comma;
			else if (Field=="sellvol")
				Cmd+= "sellvol=" +			QString::number(StocksToSell) + Comma;
			else if (Field=="buyprice")
				Cmd+= "buyprice=" +			QString::number(BuyPrice) + Comma;
			else if (Field=="sellprice")
				Cmd+= "sellprice=" +		QString::number(SellPrice) + Comma;
			else if (Field=="lastdealdemand"){
				Cmd+= "lastdealdemand=" +	QString::number(pLastDealDemand->Price) + Comma;
			}
			else if (Field=="lastdealsupply"){
				Cmd+= "lastdealsupply=" +	QString::number(pLastDealSupply->Price) + Comma;
			}
			else if (Field=="lastofferdemand")
				Cmd+= "lastofferdemand=" +	QString::number(LastOfferDemand) + Comma;
			else if (Field=="lastoffersupply")
				Cmd+= "lastoffersupply=" +	QString::number(LastOfferSupply) + Comma;
			else if (Field=="stname")
				Cmd+= "stname='" +			StockName + "',";
			else if (Field=="lastid")
				Cmd+= "lastid=" +			QString::number(LastDealStored.nID) + ",";
			else 
				_ASSERTE(0);
		}
		Cmd.remove(Cmd.length()-1,1);
		Cmd +=" WHERE stcode='"+ StockCode +"';";
		char str[1000];
		strcpy(str,STR(Cmd));

		query.exec(str);
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		else 
			return 1;		
	}
	int UpdateAll(QSqlQuery& query){
		QList<QString> listField;
		//listField << "status" << "lastofferdate" << "lastoffertime" << "lastdealsupply" << "lastdealdemand" << "stname" << "cash" << "volume" << "profit" << "buyvol" << "buyprice" << "sellvol" << "sellprice" << "lastid" ;
		listField << "status" << "lastofferdate" << "lastoffertime" << "lastdealsupply" << "lastdealdemand" << "stname" << "lastid" ;
		return Update(query,listField);
	}

	

	void Parse(QSqlQuery& query){
		Status					= query.value(1).toInt();
		StockCode				= query.value(2).toString();	
		StockName				= query.value(3).toString();	
		LastOfferDate			= query.value(4).toInt();	
		LastOfferTime			= query.value(5).toInt();	
		Volume					= query.value(6).toInt();
		Cash					= query.value(7).toDouble();
		Profit					= query.value(8).toDouble();
		StocksToBuy				= query.value(9).toInt();	
		StocksToSell			= query.value(10).toInt();
		BuyPrice				= query.value(11).toDouble();
		SellPrice				= query.value(12).toDouble();
		LastOfferDemand 		= query.value(13).toDouble();
		LastOfferSupply 		= query.value(14).toDouble();
		pLastDealDemand->Price	= query.value(15).toDouble();
		pLastDealSupply->Price	= query.value(16).toDouble();
		LastDealDemand.Price	= pLastDealDemand->Price;
		LastDealSupply.Price	= pLastDealSupply->Price;
		LastID					= query.value(17).toLongLong();
		LastDealStored.nDate=LastOfferDate;
		LastDealStored.nTime=LastOfferTime;
		LastDealStored.nID  =LastID;
			
	}
	
	
	int Select(QSqlQuery& query){
		char cmd[1000];
		sprintf(cmd,"SELECT * FROM _portfolio WHERE stcode='%s';",STR(StockCode));
		query.exec(cmd);
		VALID(query);

		if ( query.next()){
			Parse(query);
		}
		return 1;
	}
	
	int Create(QSqlQuery& query){
		query.exec("DROP TABLE IF EXISTS _portfolio;");
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		else 
			printf ("Dropping Portfolio - OK \n");

		query.exec("CREATE TABLE IF NOT EXISTS _PORTFOLIO \
				   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
				   status			INT NULL, \
				   stcode			CHAR(25) NULL, \
				   stname			CHAR(128) NULL, \
				   lastofferdate	INT UNSIGNED NULL, \
				   lastoffertime	INT UNSIGNED NULL, \
				   volume			INT NULL, \
				   cash				FLOAT, \
				   profit			FLOAT, \
				   buyvol			INT NULL, \
				   sellvol			INT NULL, \
				   buyprice			FLOAT, \
				   sellprice		FLOAT, \
				   lastofferdemand	FLOAT, \
				   lastoffersupply	FLOAT, \
				   lastdealdemand	FLOAT, \
				   lastdealsupply	FLOAT, \
				   lastid			BIGINT UNSIGNED NULL, \
				   rows				INT UNSIGNED NULL, \
				   lot				INT UNSIGNED NULL, \
				   step				FLOAT, \
				   PRIMARY KEY (ID));");
		if (!query.isActive()){
			printf("Creating Portfolio - ERROR\n");
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
			return 0;
		}
		else {
			printf ("Creating Portfolio - OK \n");
			return 1;
		}
	}
	
	int Insert(QSqlQuery& query){
		QString Cmd = QString("INSERT INTO _PORTFOLIO ") +
		"(status, stcode, stname, lastofferdate, lastoffertime, volume, cash, profit, lastid, rows) " +
		"VALUES(" +
		QString::number(Status) + "," +
		"'" + StockCode + "'," +
		"'" + StockName + "'," +
		QString::number(LastOfferDate) + "," +
		QString::number(LastOfferTime) + "," +
		QString::number(Volume) + "," +
		QString::number(Cash) + "," +
		QString::number(Profit) + "," +
		QString::number(LastID) + "," +
		QString::number(0) + ");";

		char str[1000];
		strcpy(str,STR(Cmd));

		query.exec(str);
		if (!query.isActive())
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		else 
			return 1;
	}
	
	
	void StoreQueue(QSqlQuery& query){
	
			
		if (queCurrDeals.size()==0)
			return;
		
		if (queCurrDeals.size()==1){
		//while (!queCurrDeals.isEmpty()){
			
			
			SDealData& Deal= queCurrDeals.first();
			
			int err=query.prepare("INSERT INTO " + StockCode+ "_deal (trdate,trtime,volume,price,trtype)  VALUES (?, ?, ?, ?, ?)");
			
			if (err==false){
				printf(ASCII( query.lastQuery()));
				printf("\n");
				printf(ASCII( query.lastError().text()));
				printf("\n");
				QMessageBox::critical(0, QObject::tr("Cannot open database"), QObject::tr("Unable to establish a database connection.\n"), QMessageBox::Cancel);
			}
			query.addBindValue(Deal.nDate);
			query.addBindValue(Deal.nTime);
			query.addBindValue(Deal.nVolume);
			query.addBindValue(Deal.Price);
			query.addBindValue(Deal.nType);
			
			if (!query.exec()){
				printf(ASCII( query.lastQuery()));
				printf("\n");
				printf(ASCII( query.lastError().text()));
				printf("\n");
				QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
			}
			printf(STR(StockCode));
			Deal.Show();
			queCurrDeals.dequeue();		
			return;	

		}
		
		
		int err=query.prepare("INSERT INTO " + StockCode+ "_deal (trdate,trtime,volume,price,trtype)  VALUES (?, ?, ?, ?, ?)");
			

		QVariantList listDate;
		QVariantList listTime;
		QVariantList listVolume;
		QVariantList listPrice;
		QVariantList listType;
		int rows=0;
		while (!queCurrDeals.isEmpty()){
			rows++;
			SDealData& Deal= queCurrDeals.first();
			listDate	<< Deal.nDate;
			listTime	<< Deal.nTime;
			listVolume	<< Deal.nVolume;
			listPrice	<< Deal.Price;
			listType	<< Deal.nType;
			printf(STR(StockCode));
			Deal.Show();
			queCurrDeals.dequeue();
		}
		
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
		printf("ok! %d rows inserted\n",rows);

	}

	QString State(){
		QString Str;
		StocksToSell=0;	
		StocksToBuy=0;	
		float	SupplyWorth=0;
		float	DemandWorth=0;

		for(int i=0; i<listRequestSupply.size();i++){
			SRequest* pRequest=listRequestSupply.at(i);
			StocksToSell-=pRequest->nVolume;
			SupplyWorth +=-pRequest->nVolume*pRequest->Price;
			Str +=  "supply: order="+QString::number(pRequest->nOrder)+ " vol=" +QString::number(pRequest->nVolume) + " price=" +QString::number(pRequest->Price) + " status="+QString::number(pRequest->nStatusRequest,16)+"\n";
		}
		for(int i=0; i<listRequestDemand.size();i++){
			SRequest* pRequest=listRequestDemand.at(i);
			StocksToBuy+=pRequest->nVolume;
			DemandWorth += pRequest->nVolume*pRequest->Price;
			Str += "demand: order="+QString::number(pRequest->nOrder)+ " vol=" +QString::number(pRequest->nVolume) + " price=" + QString::number(pRequest->Price) +  " status=" + QString::number(pRequest->nStatusRequest,16)+"\n";
		}

	
		Str+= "cash=" + QString::number(Cash) + " volume=" + QString::number(Volume) + " tobuy="+QString::number(StocksToBuy) + " tosell="+QString::number(StocksToSell) + " commission="+QString::number(Commission);

		if (pLastDealDemand	){
			Profit=(Volume+StocksToSell)*pLastDealDemand->Price+Cash+DemandWorth;
			Str+=" profit="+QString::number(Profit);
		}
		return Str;
	}
	
};

struct SPortfolioErr{

public:
	int		Status;			// db
	QString StockCode;		// db
	QString StockName;		// db
	int		LastOfferDate;	// db
	int		LastOfferTime;	// db
	int		Volume;			// db
	float	Cash;			// db
	float	Profit;			// db
	int		StocksToBuy;	// db
	int		StocksToSell;	// db
	float	BuyPrice;		// db - Минимальная цена покупки среди моих заявок
	float	SellPrice;		// db - Максимальная цена продажи среди моих заявок 
	float	LastOfferDemand;// db
	float	LastOfferSupply;// db
	float	LastDealDemand;	// db
	float 	LastDealSupply;	// db

	float	Commission;
	float	CommissionRate;
	SDeal*	pLastDealDemand;	// db
	SDeal* 	pLastDealSupply;	// db


	long long		LastID;			// db - ID последней сделки
	SDeal*	pLastDealClassified;
	QQueue<SDeal>	quePastDeals;
	QQueue<SDeal>	queCurrDeals;
	QQueue<SDeal>	queDeals;

	QList<SRequest*> listRequestSupply;
	QList<SRequest*> listRequestDemand;
	QMutex mutex;

	QQueue<SDealID> queDealID;
	QQueue<SStatusID> queStatusID;
	QQueue<SRequest*> queDemandTrash;
	QQueue<SRequest*> queSupplyTrash;

	/*
	void QueueDeal(SRequest* pRequest,SDeal& Deal){
	SDealID DealID={pRequest, Deal};
	DealID.Deal=Deal;
	mutex.lock();
	queDealID<<DealID;
	mutex.unlock();
	}
	*/
	void QueueTerminalReply(SRequest* pRequest, int nStatus, int nOrder){
		SStatusID StatusID={pRequest,nStatus, nOrder};
		mutex.lock();
		queStatusID<<StatusID;
		mutex.unlock();
	}

	SGlass Glass;

	int GetAutoIncrement(QSqlQuery& query){
		QString Cmd = "SHOW TABLE STATUS";
		query.exec(Cmd);
		if (!query.isActive()){
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
			return 0;
		}
		while(query.next()){
			QString Name=query.value(0).toString();
			if (Name!=StockCode.toLower()+"_deal")
				continue;
			int indx=query.value(10).toInt();
			return indx;
		}

	}
	SPortfolioErr(){
		Status=0;
		StockCode="";
		StockName="na";
		LastOfferDate=0;
		LastOfferTime=0;
		Volume=0;
		Cash=0;
		Profit=0;
		StocksToBuy=0;
		StocksToSell=0;
		BuyPrice=0;
		SellPrice=0;
		LastID=0;
		//pLastDealClassified=&LastDealSupply;
		//SRequest d(0,0);
		//SRequest s(10000,0);
		//Glass.listDemand.append(d);
		//Glass.listSupply.append(s);

	}
	int Update(QSqlQuery& query, QList<QString>& listField){
		QString Cmd = QString("UPDATE _PORTFOLIO SET ");
		QString Comma=",";
		int listSize=listField.size();
		for(int i=0; i<listSize; i++){
			if (i==listSize-1)
				Comma="";
			QString& Field=listField[i];
			if		(Field=="status")
				Cmd+= "status=" +			QString::number(Status) + Comma;
			else if (Field=="lastofferdate")
				Cmd+= "lastofferdate=" +	QString::number(LastOfferDate) + Comma;
			else if (Field=="lastoffertime")
				Cmd+= "lastoffertime=" +	QString::number(LastOfferTime) + Comma;
			else if (Field=="volume")
				Cmd+= "volume=" +			QString::number(Volume) + Comma;
			else if (Field=="cash")
				Cmd+= "cash=" +				QString::number(Cash) + Comma;
			else if (Field=="profit")
				Cmd+= "profit=" +			QString::number(Profit) + Comma;
			else if (Field=="buyvol")
				Cmd+= "buyvol=" +			QString::number(StocksToBuy) + Comma;
			else if (Field=="sellvol")
				Cmd+= "sellvol=" +			QString::number(StocksToSell) + Comma;
			else if (Field=="buyprice")
				Cmd+= "buyprice=" +			QString::number(BuyPrice) + Comma;
			else if (Field=="sellprice")
				Cmd+= "sellprice=" +		QString::number(SellPrice) + Comma;
			else if (Field=="lastdealdemand")
				Cmd+= "lastdealdemand=" +	QString::number(LastDealDemand) + Comma;
			else if (Field=="lastdealsupply")
				Cmd+= "lastdealsupply=" +	QString::number(LastDealSupply) + Comma;
			else if (Field=="lastofferdemand")
				Cmd+= "lastofferdemand=" +	QString::number(LastOfferDemand) + Comma;
			else if (Field=="lastoffersupply")
				Cmd+= "lastoffersupply=" +	QString::number(LastOfferSupply) + Comma;
			else if (Field=="stname")
				Cmd+= "stname='" +			StockName + "',";
			else if (Field=="lastid")
				Cmd+= "lastid=" +			QString::number(LastID) + ",";
			else 
				_ASSERTE(0);
		}
		Cmd.remove(Cmd.length()-1,1);
		Cmd +=" WHERE stcode='"+ StockCode +"';";
		char str[1000];
		strcpy(str,STR(Cmd));

		query.exec(str);
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		else 
			return 1;		
	}
	int UpdateAll(QSqlQuery& query){
		QList<QString> listField;
		//listField << "status" << "lastofferdate" << "lastoffertime" << "lastdealsupply" << "lastdealdemand" << "stname" << "cash" << "volume" << "profit" << "buyvol" << "buyprice" << "sellvol" << "sellprice" << "lastid" ;
		listField << "status" << "lastofferdate" << "lastoffertime" << "lastdealsupply" << "lastdealdemand" << "stname" << "lastid" ;
		return Update(query,listField);
	}



	void Parse(QSqlQuery& query){
		Status					= query.value(1).toInt();
		StockCode				= query.value(2).toString();	
		StockName				= query.value(3).toString();	
		LastOfferDate			= query.value(4).toInt();	
		LastOfferTime			= query.value(5).toInt();	
		Volume					= query.value(6).toInt();
		Cash					= query.value(7).toDouble();
		Profit					= query.value(8).toDouble();
		StocksToBuy				= query.value(9).toInt();	
		StocksToSell			= query.value(10).toInt();
		BuyPrice				= query.value(11).toDouble();
		SellPrice				= query.value(12).toDouble();
		LastOfferDemand 		= query.value(13).toDouble();
		LastOfferSupply 		= query.value(14).toDouble();
		LastDealDemand			= query.value(15).toDouble();
		LastDealSupply			= query.value(16).toDouble();
		LastID					= query.value(17).toLongLong();

	}


	int Select(QSqlQuery& query){
		char cmd[1000];
		sprintf(cmd,"SELECT * FROM _portfolio WHERE stcode='%s';",STR(StockCode));
		query.exec(cmd);
		VALID(query);

		if ( query.next()){
			Parse(query);
		}
		return 1;
	}

	int Create(QSqlQuery& query){
		query.exec("DROP TABLE IF EXISTS _portfolio;");
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		else 
			printf ("Dropping Portfolio - OK \n");

		query.exec("CREATE TABLE IF NOT EXISTS _PORTFOLIO \
				   (ID INT UNSIGNED NOT NULL AUTO_INCREMENT, \
				   status			INT NULL, \
				   stcode			CHAR(25) NULL, \
				   stname			CHAR(128) NULL, \
				   lastofferdate	INT UNSIGNED NULL, \
				   lastoffertime	INT UNSIGNED NULL, \
				   volume			INT NULL, \
				   cash				FLOAT, \
				   profit			FLOAT, \
				   buyvol			INT NULL, \
				   sellvol			INT NULL, \
				   buyprice			FLOAT, \
				   sellprice		FLOAT, \
				   lastofferdemand	FLOAT, \
				   lastoffersupply	FLOAT, \
				   lastdealdemand	FLOAT, \
				   lastdealsupply	FLOAT, \
				   lastid			BIGINT UNSIGNED NULL, \
				   rows				INT UNSIGNED NULL, \
				   lot				INT UNSIGNED NULL, \
				   step				FLOAT, \
				   PRIMARY KEY (ID));");
		if (!query.isActive()){
			printf("Creating Portfolio - ERROR\n");
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
			return 0;
		}
		else {
			printf ("Creating Portfolio - OK \n");
			return 1;
		}
	}

	int Insert(QSqlQuery& query){
		QString Cmd = QString("INSERT INTO _PORTFOLIO ") +
			"(status, stcode, stname, lastofferdate, lastoffertime, volume, cash, profit) " +
			"VALUES(" +
			QString::number(Status) + "," +
			"'" + StockCode + "'," +
			"'" + StockName + "'," +
			QString::number(LastOfferDate) + "," +
			QString::number(LastOfferTime) + "," +
			QString::number(Volume) + "," +
			QString::number(Cash) + "," +
			QString::number(Profit) + ");";

		char str[1000];
		strcpy(str,STR(Cmd));

		query.exec(str);
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		else 
			return 1;
	}
	QString State(){
		QString Str;
		StocksToSell=0;	
		StocksToBuy=0;	
		float	SupplyWorth=0;
		float	DemandWorth=0;

		for(int i=0; i<listRequestSupply.size();i++){
			SRequest* pRequest=listRequestSupply.at(i);
			StocksToSell-=pRequest->nVolume;
			SupplyWorth +=-pRequest->nVolume*pRequest->Price;
			Str +=  "supply: order="+QString::number(pRequest->nOrder)+ " vol=" +QString::number(pRequest->nVolume) + " price=" +QString::number(pRequest->Price) + " status="+QString::number(pRequest->nStatusRequest,16)+"\n";
		}
		for(int i=0; i<listRequestDemand.size();i++){
			SRequest* pRequest=listRequestDemand.at(i);
			StocksToBuy+=pRequest->nVolume;
			DemandWorth += pRequest->nVolume*pRequest->Price;
			Str += "demand: order="+QString::number(pRequest->nOrder)+ " vol=" +QString::number(pRequest->nVolume) + " price=" + QString::number(pRequest->Price) +  " status=" + QString::number(pRequest->nStatusRequest,16)+"\n";
		}


		Str+= "cash=" + QString::number(Cash) + " volume=" + QString::number(Volume) + " tobuy="+QString::number(StocksToBuy) + " tosell="+QString::number(StocksToSell) + " commission="+QString::number(Commission);

		if (pLastDealDemand	){
			Profit=(Volume+StocksToSell)*pLastDealDemand->Price+Cash+DemandWorth;
			Str+=" profit="+QString::number(Profit);
		}
		return Str;
	}

};


struct SOffer{
	int nID;
	int nDate;
	int nTime;
	float Demand;
	float Supply;
	int nDemVol;
	int nSupVol;
	QString Name;

	SOffer(){
		nDate=0;
		nTime=0;
		nDemVol=0;
		nSupVol=0;
		Demand=0;
		Supply=0;
	}

	SOffer(QString& name, QDateTime &DateTime,  float demand, float supply,int demVol, int supVol){
		Name=name;

		QDate qDate = DateTime.date();
		QTime qTime = DateTime.time();
		nDate=qDate.year()*10000+qDate.month()*100+qDate.day();
		nTime=qTime.hour()*10000+qTime.minute()*100+qTime.second();
		nDemVol=demVol;
		nSupVol=supVol;
		Demand=	demand;
		Supply=	supply;
	};

	void Show(){
		char str[256];
		sprintf(str,"#%d %d %s %f %f \n", nDate, nTime, STR(Name), Supply, Demand);
		printf(str);
	}
	void Parse(QSqlQuery& query){
		nID		= query.value(0).toInt();
		nDate	= query.value(1).toInt();
		nTime	= query.value(2).toInt();
		Demand	= query.value(3).toDouble();
		Supply	= query.value(4).toDouble();
		nDemVol	= query.value(5).toInt();
		nSupVol	= query.value(6).toInt();
	}
};



struct SMyDeal {
	int		nDate;
	int		nTime;
	int		nVolume;
	float	Price;
	int		InRange(float BasePrice, float LoPercent, float HiPercent){
		if ( BasePrice*LoPercent<= Price && Price <=BasePrice*HiPercent)
			return 1;
		else 
			return 0;
	}
	SMyDeal(){
		nDate=0;
		nTime=0;
		nVolume=0;
		Price=0;
	}
	void Show(){
		char str[256];
		sprintf(str,"%d %d %f %d \n", nDate, nTime, Price, nVolume);
		printf(str);
	}
	int AbsTime(){
		return ((nDate<<18) | nTime);
	}
};

class CSqlQuik {


};

class CSqlTrading {

};
/*
struct SOrder{
	QList<SRequest*> listNewRequestSupply;
	QList<SRequest*> listNewRequestDemand;
	QList<SRequest*> listKillRequestSupply;
	QList<SRequest*> listKillRequestDemand;
	int IsEmpty(){
		int listKillRequestSupply_size=listKillRequestSupply.size();
		int listKillRequestDemand_size=listKillRequestDemand.size();
		int listNewRequestDemand_size =listNewRequestDemand.size();
		int listNewRequestSupply_size =listNewRequestSupply.size();

		if (listKillRequestSupply_size==0 &&
			listKillRequestDemand_size==0 &&
			listNewRequestDemand_size==0 &&
			listNewRequestSupply_size==0
			)
			return 1;
		else	
			return 0;
	}
};
*/