#ifndef SHARED_DEFINED
#define SHARED_DEFINED
#include "xmllogger.h"
#include "main.h"
#include <QSharedMemory>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#define MAX(a,b) ((a) < (b) ? (b) : (a))

//#define  MAX_TICS 1024

//#include "TransaqConnector.h"

#define LIMIT_TICKS  1024*256		// must be power of two
#define LIMIT_QUOTES 1024*256*4		// must be power of two
#define LIMIT_ORDERS 1024		// must be power of two
#define LIMIT_GLASSES 16 //32768*8
#define LIMIT_KILLS 128 //32768*8
#define GLASS_DEPTH 7
#define LIMIT_TRADES 1024

inline QString DateTime2Text(uint datetime)
{
	QDateTime dt; 
	dt.setTime_t(datetime);
	return dt.toString("yyyy-MM-dd hh:mm:ss");
}
inline QString DateTime2Text(QDateTime& dt)
{
	return dt.toString("yyyy-MM-dd hh:mm:ss");
}

inline QDateTime Text2DateTime(QString& date_time)
{
	return QDateTime::fromString(date_time,"yyyy-MM-dd hh:mm:ss");
}



struct S_XML_Tick{
	//QString secid;		// 4
	QString board;		// TQBR
	QString seccode;	// LKOH
	QString tradeno;	// 2397108618
	QString tradetime;	// 08.08.2014 09:59:59
	QString price;		// 1928.3
	QString quantity;	// 2
	QString period;		// L
	QString buysell;	// S
	QString toXML(){
		QString XML="<S_XML_Tick seccode='"+seccode+"' price='"+price+"' quantity='"+quantity+"' tradetime='"+tradetime+"' buysell='"+buysell+"' />";
		return XML;
	}
};


struct S_Tick{
	float		price;
	int			quantity;
	int			type;
	uint		datetime;
	
	S_Tick(){
		price=0;
		quantity=0;
		type=0;
		
	}
	bool isSell(){
		return type<0;
	}
	bool isBuy(){
		return type>=0;
	}
	void SetSellType(){
		type&=0xFF;
		type|=(-1)<<8;
	}
	// Продажа (более низкая цена). На бирже выставлена заявка на покупку. Спрос
	void SetBuyType(){
		type&=0xFF;
		type|=(1)<<8;
	}

	
	QString toXML(){
		QDateTime dt=DateTime();
		QString str_price;		str_price.setNum(price); 
		QString str_quantity;	str_quantity.setNum(quantity); 
		QString str_date;		str_date=dt.date().toString("yyyy-MM-dd");
		QString str_time;		str_time=dt.time().toString("hh:mm:ss");
		QString str_type;		str_type.setNum(type);
		QString XML="<S_Tick price='"+str_price+"' volume='"+str_quantity+"' date='"+str_date+"' time='"+str_time+"' type='"+str_type+"' />";
		return XML;
	}
	QString TextDate(){
		QDateTime dt;
		dt.setTime_t(datetime);
		QString str_date;		
		str_date=dt.date().toString("yyyy-MM-dd");
		return str_date;
	}
	QString TextTime(){
		QDateTime dt;
		dt.setTime_t(datetime);
		QString str_time;
		str_time=dt.time().toString("hh:mm:ss");
		return str_time;
	}
	QDateTime DateTime(){
		QDateTime dt;
		dt.setTime_t(datetime);
		return dt;
	}
};
//============================ S_EasyTicks ====================================

struct S_EasyTicks{
	S_Tick	data[LIMIT_TICKS];
	size_t	size;
	S_Tick& operator[] (int idx){
		return data[idx&(LIMIT_TICKS-1)];
	}
	S_Tick& Last(){
		return data[(size-1)&(LIMIT_TICKS-1)];
	}
	void Init(){
		size=0;
		S_Tick* tick=data;
		for(int i=0; i<LIMIT_TICKS; i++,tick++){
			tick->price		=0;
			tick->quantity	=0;
			tick->type		=0;
			tick->datetime	=0;
		}
	}
	uint FindAfter(uint dt, uint fromIndex=0)
	{
		S_Tick* pTick=data+fromIndex;
		for(uint i=fromIndex; i<MIN(size,LIMIT_TICKS); i++, pTick++){
			if (pTick->datetime>dt)
				return i;
		}
		return size;
	}
	uint FindAfter(QString dt, uint fromIndex=0)
	{
		return FindAfter(Text2DateTime(dt).toTime_t(),fromIndex);
	}
	uint NextSecondIndex(uint fromIndex)
	{
		S_Tick* pTick=data+fromIndex;
		uint from_datetime=pTick->datetime;
		for(uint i=fromIndex; i<MIN(size,LIMIT_TICKS); i++, pTick++)
			if (pTick->datetime>from_datetime)
				return i;
		return size;
	}
	
	void operator << (S_Tick& Tick)
	{
		data[ size   &(LIMIT_TICKS-1)]=Tick;
		size++;
	}
	
	void operator << (S_XML_Tick& Tick)
	{
		bool ok;
		S_Tick& NewTick =data[ size   &(LIMIT_TICKS-1)];
		S_Tick& LastTick=data[(size-1)&(LIMIT_TICKS-1)];
		NewTick.type     =0;
		NewTick.price    =Tick.price.toFloat(&ok);
		NewTick.quantity =Tick.quantity.toInt(&ok);
		
		//qDebug() << Tick.tradetime;
		QDateTime dt= QDateTime::fromString(Tick.tradetime,"dd.MM.yyyy hh:mm:ss"); 
		ok = dt.isValid();
		if (ok){
			NewTick.datetime=dt.toTime_t();
			if (NewTick.datetime==LastTick.datetime){
				NewTick.type |=IN_SECOND;
				LastTick.type|=IN_SECOND;
			}
			else 
				NewTick.type|=FIRST_IN_SECOND;	
		}
		if (Tick.buysell=="S")
			//NewTick.SetSellType();
			NewTick.SetBuyType();
		else 
			//NewTick.SetBuyType();
			NewTick.SetSellType();
		size++;
	}
};


struct S_InstrumentInfo{
	char	seccode[16];
	char	shortname[16];
	int		decimals;
	char	active[16];
	char	secid[16];
	int		market;
	double	minstep;
	double	lotsize;
	char	board[16];
	
};


struct C_EasyQuote{
	uint		datetime;
	float		price;
	int			buy;
	int			sell;
};

struct C_FixedGlass
{
public:
	
	struct
	{
		float price;
		int quantity;
	} sell[GLASS_DEPTH], buy[GLASS_DEPTH];
	uint datetime;
	uint date;
	uint time;
	short sell_depth;
	short buy_depth;
};



struct S_XML_QuoteInfo {

	QString secid;	// "внутренний код">
	QString board;	// Идентификатор режима торгов по умолчанию 
	QString seccode;// Код инструмента </seccode>
	QString price;	// цена</price>
	QString source; // Источник котировки (маркетмейкер)</source>
	QString yield;	// доходность (актуально только для 	облигаций)</yield>
	QString buy;	// количество бумаг к покупке</buy>
	QString sell;	// количество бумаг к продаже</sell>
	uint	datetime;

	QString toXML(){
		QString XML="<S_XML_QuoteInfo seccode='"+seccode+"' price='"+price+"' buy='"+buy+"' sell='"+sell+"'/>";
		return XML;
	}

};

struct S_Quote {
	int	  quantity;
	float price;
	//QDateTime datetime_create;
	//QDateTime datetime_update;
};

struct S_Glass
{
	QList<S_Quote> listBuy;
	QList<S_Quote> listSell;
	uint fromIndex;
	uint toIndex;
	uint datetime;
	//uint fromDateTime;
	//uint toDateTime;
	S_Glass(){
		fromIndex=0;
		toIndex=0;
	}
};
// void DiffGlass(QList<S_Quote>& listPrevBuyQuote, QList<S_Quote>& listPrevSellQuote, 
// 			   QList<S_Quote>& listCurrBuyQuote, QList<S_Quote>& listCurrSellQuote)
// {
// 	
// }


struct S_XML_OrderInfo{
	QString transactionid;
	QString orderno;
	QString secid;
	QString board;
	QString seccode;
	QString client;
	QString status;
	QString buysell;
	QString brokerref;
	QString time;
	QString value; // объем заявки в копейках
	QString accruedint;
	QString settlecode;
	QString balance; //Неудовлетворенный остаток объема заявки в лотах (контрактах)
	QString price;
	QString quantity;
	QString result; //сообщение биржи в случае отказа выставить  заявку

};

struct S_XML_TradeInfo
{
	QString secid;		//Идентификатор бумаги
	QString tradeno;	//Номер сделки на биржеQString
	QString orderno;	//Номер заявки на биржеQString
	QString board;		//Идентификатор бордаQString /board; //
	QString seccode;	// Код инструмента QString /seccode; //
	QString client;	//Идентификатор клиентаQString /client; //
	QString buysell;	//B - покупка, S - продажаQString /buysell; //
	QString time;		//время сделкиQString /time; //
	QString brokerref; //примечаниеQString /brokerref; //
	QString value;		//объем сделкиQString /value; //
	QString comission; //комиссияQString /comission; //
	QString price;		//ценаQString /price; //
	QString quantity; //количество лотовQString /quantity; //
	QString yield;		//доходностьQString /yield; //
	QString accruedint; //НКДQString /accruedint; //
	QString tradetype; //тип сделки: ‘T’ – обычная ‘N’ – РПС ‘R’ – РЕПО 	‘P’ – размещениеQString /tradetype; //
	QString settlecode; //код поставкиQString /settlecode; //
	QString currentpos; //Текущая позиция по инструментуuint /currentpos; //
};

struct S_EasyTrade 
{
	unsigned long long tradeno;
	unsigned long long orderno;
	uint time;
	uint quantity;
	char buysell;
	float commision;
	float price;
	uint currentpos;
};

struct S_EasyTrades 
{
	S_EasyTrade data[LIMIT_TRADES];
	uint head;
	uint tail;

	void Init(){
		head=0;
		tail=0;
	}
	S_EasyTrade& operator [] (uint idx){
		return data[idx&(LIMIT_TRADES-1)];
	}
	bool operator << (S_XML_TradeInfo& Trade){
		if (head==tail+LIMIT_TRADES)
			return false;
		S_EasyTrade& trade=data[head&(LIMIT_TRADES-1)];
		if (Trade.buysell=="B")
			trade.buysell='B';
		else if (Trade.buysell=="S")
			trade.buysell='S';
		trade.orderno=Trade.orderno.toULongLong();
		trade.tradeno=Trade.orderno.toULongLong();
		trade.commision=Trade.comission.toFloat();
		trade.currentpos=Trade.currentpos.toUInt();
		trade.quantity=Trade.quantity.toUInt();
		trade.price=Trade.price.toFloat();
		trade.time=Text2DateTime(Trade.time).toTime_t();
		trade.time=Text2DateTime(Trade.time).toTime_t();
		head++;
		return true;
	}

};

struct S_CancelOrder {
	uint transactionid;
	struct {
		int  success;
		char result[128];
	} transaq;
};

struct S_NewOrder
{
	//-------- my order ------------
	float price;
	uint quantity;
	char buysell;
	bool bymarket;
	//--------- transaq reply ------
	struct {
		int  success;
		uint transactionid;
		char result[128];
	} transaq;
	//--------- micex reply --------
	struct {
		unsigned long long orderno;
		float price;
		uint quantity;
		uint time;
		uint accepttime;
		char result[128];
		uint balance;
	} server;
	
};


struct S_EasyOrders
{
	struct S_NewOrders
	{
		S_NewOrder data[LIMIT_ORDERS];
		uint head;
		uint tail;
		void Init(){
			head=0;
			tail=0;
		}
		S_NewOrder& Last(){
			_ASSERTE(head>0);
			return data[(head-1)&(LIMIT_ORDERS-1)];
		}
// 		S_NewOrder& First(){
// 			_ASSERTE(head==0);
// 			return data[(head-1)&(LIMIT_ORDERS-1)];
// 		}

		S_NewOrder* Insert(float price, uint quantity, char buysell, bool bymarket=false){
			_ASSERTE(head<tail+LIMIT_ORDERS);
			S_NewOrder& order=data[head&(LIMIT_ORDERS-1)];
			order.price=price;
			order.quantity=quantity;
			order.buysell=buysell;
			order.bymarket=bymarket;
			order.transaq.success=-1;
			order.transaq.transactionid=-1;
			order.transaq.result[0]=0;
			order.server.orderno=-1;
			order.server.result[0]=0;
			
			head++;
			while (order.transaq.success==-1){

			}
			if (order.transaq.success==false)
				return 0;
			else 
				return &order;
		}
		S_NewOrder& operator [] (uint idx)		{
			return data[idx&(LIMIT_ORDERS-1)];
		}
	} NewOrders;

	struct S_CancelOrders 
	{
		S_CancelOrder data[LIMIT_KILLS];
		uint head;
		uint tail;
		void Init(){
			head=0;
			tail=0;
		}
		bool Insert(uint id){
			_ASSERTE(head<tail+LIMIT_KILLS);
			S_CancelOrder& order=data[head&(LIMIT_KILLS-1)];
			order.transactionid=id;
			order.transaq.success=-1;
			order.transaq.result[0]=0;
			head++;
			while (order.transaq.success==-1){

			}
			if (order.transaq.success==false)
				return false;
			else 
				return true;
				//Sleep(100);
		}
		S_CancelOrder& operator[] (uint idx){
			return data[(idx)&(LIMIT_KILLS-1)];
		}
	} CancelOrders;

	
	bool operator << (S_XML_OrderInfo& Reply)
	{
		uint transactionid=Reply.transactionid.toUInt();
		_ASSERTE(transactionid);
		for(int idx=NewOrders.head-1; idx>=0; idx--){
			S_NewOrder& order=NewOrders[idx];
			if (order.transaq.transactionid==transactionid){
				order.server.orderno=Reply.orderno.toULongLong();
				order.server.price  =Reply.price.toFloat();
				order.server.quantity=Reply.quantity.toUInt();
				order.server.balance =Reply.balance.toUInt();
				strncpy(order.server.result,STR(Reply.result),127);
				return true;
			}
		}
		return false;
	}
	
	void Init(){
		CancelOrders.Init();
		NewOrders.Init();
	}
};

//======================= S_EasyQuotes ============================================
struct S_EasyQuotes{
	C_EasyQuote data[LIMIT_QUOTES];
	uint			size;
	S_EasyQuotes(){
		size=0;
	}
	void Init(){
		size=0;
	}
	C_EasyQuote& operator [] (uint idx)
	{
		return data[idx&(LIMIT_QUOTES-1)];
	}

	void operator << (S_XML_QuoteInfo& QuoteInfo){
		bool ok;
		C_EasyQuote& quote=data[size&(LIMIT_QUOTES-1)];
		quote.datetime =QuoteInfo.datetime;
		quote.price    =QuoteInfo.price.toFloat(&ok);
		quote.buy      =QuoteInfo.buy.toInt(&ok);
		if (!ok) quote.buy  =0;
		quote.sell     =QuoteInfo.sell.toInt(&ok);
		if (!ok) quote.sell =0;
		
		size++;
	}


	bool UpdateCurrentQuotes(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote, int history=100)
	{
		S_Quote NewQuote;
		bool isUpdated=false;
		for(uint i=MAX(0,size-history); i<MIN(size,LIMIT_QUOTES); i++){
			C_EasyQuote& HistoryQuote=data[i&(LIMIT_QUOTES-1)];
			if (HistoryQuote.sell){
				//------------ if history quote is sell ---------
				if (listSellQuote.isEmpty() && HistoryQuote.sell>0 ){
					NewQuote.price   =HistoryQuote.price;
					NewQuote.quantity=HistoryQuote.sell;
					//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
					//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
					listSellQuote << NewQuote;
					isUpdated=true;
				}
				else {
					QMutableListIterator<S_Quote> Iter(listSellQuote);
					Iter.toFront();
					while (Iter.hasNext()) {
						S_Quote& CurQuote=Iter.next();
						if (HistoryQuote.sell==-1){
							if (HistoryQuote.price == CurQuote.price){
								Iter.remove();
								isUpdated=true;
								break;
							}
						}
						else {
							if (HistoryQuote.price < CurQuote.price ){
								NewQuote.price   =HistoryQuote.price;
								NewQuote.quantity=HistoryQuote.sell;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								Iter.previous();
								Iter.insert(NewQuote);
								isUpdated=true;
								break;
							}
							if (HistoryQuote.price == CurQuote.price){
								CurQuote.quantity=HistoryQuote.sell;
								//CurQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								isUpdated=true;
								break;
							}
							if (!Iter.hasNext()){
								NewQuote.price			=HistoryQuote.price;
								NewQuote.quantity		=HistoryQuote.sell;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								listSellQuote.append(NewQuote);
								isUpdated=true;
								break;
							}
						}
					}
				}
			}
			//------------ if history quote is buy ---------
			else {
				if (listBuyQuote.isEmpty() && HistoryQuote.buy>0 ){
					NewQuote.price   =HistoryQuote.price;
					NewQuote.quantity=HistoryQuote.buy;
					//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
					//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
					listBuyQuote << NewQuote;
					isUpdated=true;
				}
				else {
					QMutableListIterator<S_Quote> Iter(listBuyQuote);

					Iter.toFront();
					while (Iter.hasNext()) {
						S_Quote& CurQuote=Iter.next();
						if (HistoryQuote.buy==-1){
							if (HistoryQuote.price == CurQuote.price){
								Iter.remove();
								isUpdated=true;
								break;
							}
						}
						else {
							if (HistoryQuote.price > CurQuote.price ){
								NewQuote.price			=HistoryQuote.price;
								NewQuote.quantity		=HistoryQuote.buy;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								Iter.previous();
								Iter.insert(NewQuote);
								isUpdated=true;
								break;
							}
							if (HistoryQuote.price == CurQuote.price){
								CurQuote.quantity		=HistoryQuote.buy;
								//CurQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								isUpdated=true;
								break;
							}
							if (!Iter.hasNext()){
								NewQuote.price			=HistoryQuote.price;
								NewQuote.quantity		=HistoryQuote.buy;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								listBuyQuote.append(NewQuote);
								isUpdated=true;
								break;
							}
						}
					}
				}
			}
		}
		return isUpdated;
	}

	bool UpdateGlass(S_Glass& Glass, uint toIndex, int history=100)
	{
		_ASSERTE((int)toIndex>=0);
		_ASSERTE(toIndex<size);
		uint fromIndex;
		Glass.datetime=data[toIndex&(LIMIT_QUOTES-1)].datetime;
		if (Glass.fromIndex <= toIndex-history && toIndex-history<= Glass.toIndex && Glass.toIndex<=toIndex){
			fromIndex=Glass.toIndex;
			Glass.toIndex=toIndex;
			
		}
		else {
			Glass.listBuy.clear();
			Glass.listSell.clear();
			fromIndex      =MAX(0,toIndex-history);
			Glass.fromIndex=MAX(0,toIndex-history);
			Glass.toIndex=toIndex;
			//Glass.fromDateTime
			//Glass.toDateTime=data[toIndex].datetime;
		}

		
		S_Quote NewQuote;
		bool isUpdated=false;
		
		for(uint i=fromIndex; i<=Glass.toIndex; i++){
			C_EasyQuote& HistoryQuote=data[i&(LIMIT_QUOTES-1)];
			if (HistoryQuote.sell){
				//------------ if history quote is sell ---------
				if (Glass.listSell.isEmpty() && HistoryQuote.sell>0 ){
					NewQuote.price   =HistoryQuote.price;
					NewQuote.quantity=HistoryQuote.sell;
					//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
					//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
					Glass.listSell << NewQuote;
					isUpdated=true;
				}
				else {
					QMutableListIterator<S_Quote> Iter(Glass.listSell);
					Iter.toFront();
					while (Iter.hasNext()) {
						S_Quote& CurQuote=Iter.next();
						if (HistoryQuote.sell==-1){
							if (HistoryQuote.price == CurQuote.price){
								Iter.remove();
								isUpdated=true;
								break;
							}
						}
						else {
							if (HistoryQuote.price < CurQuote.price ){
								NewQuote.price   =HistoryQuote.price;
								NewQuote.quantity=HistoryQuote.sell;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								Iter.previous();
								Iter.insert(NewQuote);
								isUpdated=true;
								break;
							}
							if (HistoryQuote.price == CurQuote.price){
								CurQuote.quantity=HistoryQuote.sell;
								//CurQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								isUpdated=true;
								break;
							}
							if (!Iter.hasNext()){
								NewQuote.price			=HistoryQuote.price;
								NewQuote.quantity		=HistoryQuote.sell;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								Glass.listSell.append(NewQuote);
								isUpdated=true;
								break;
							}
						}
					}
				}
			}
			//------------ if history quote is buy ---------
			else {
				if (Glass.listBuy.isEmpty() && HistoryQuote.buy>0 ){
					NewQuote.price   =HistoryQuote.price;
					NewQuote.quantity=HistoryQuote.buy;
					//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
					//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
					Glass.listBuy << NewQuote;
					Glass.fromIndex=i;
					isUpdated=true;
				}
				else {
					QMutableListIterator<S_Quote> Iter(Glass.listBuy);

					Iter.toFront();
					while (Iter.hasNext()) {
						S_Quote& CurQuote=Iter.next();
						if (HistoryQuote.buy==-1){
							if (HistoryQuote.price == CurQuote.price){
								Iter.remove();
								isUpdated=true;
								break;
							}
						}
						else {
							if (HistoryQuote.price > CurQuote.price ){
								NewQuote.price			=HistoryQuote.price;
								NewQuote.quantity		=HistoryQuote.buy;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								Iter.previous();
								Iter.insert(NewQuote);
								isUpdated=true;
								break;
							}
							if (HistoryQuote.price == CurQuote.price){
								CurQuote.quantity		=HistoryQuote.buy;
								//CurQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								isUpdated=true;
								break;
							}
							if (!Iter.hasNext()){
								NewQuote.price			=HistoryQuote.price;
								NewQuote.quantity		=HistoryQuote.buy;
								//NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
								//NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
								Glass.listBuy.append(NewQuote);
								isUpdated=true;
								break;
							}
						}
					}
				}
			}
		}
		return isUpdated;
	}

	QString toXML(int decimals,int history=100)
	{
		QList<S_Quote> Sell;
		QList<S_Quote> Buy;
		QString XML;

		UpdateCurrentQuotes(Buy,Sell,history);

		XML ="<quotes>\n"; // seccode='" + QString(Info.seccode)+"'>\n";


		for(int i=Sell.size()-1; i>=0; i--){
			S_Quote& Quote=Sell[i];
			QString price;
			QString quantity;
			QString create;
			QString update;
			price.setNum(Quote.price,'g',decimals);
			quantity.setNum(Quote.quantity);
			//create=Quote.datetime_create.toString("yyyy-MM-dd hh:mm:ss");
			//update=Quote.datetime_update.toString("yyyy-MM-dd hh:mm:ss");
			//create=DateTime2Text(Quote.datetime_create);
			//update=DateTime2Text(Quote.datetime_update);
			//XML+="	<sell price='"+price+"' volume='"+quantity+"' create='"+create+"' update='"+update+"'>\n";
			XML+="	<sell price='"+price+"' volume='"+quantity+"'>\n";
		}
		for(int i=0; i<Buy.size(); i++){
			S_Quote& Quote=Buy[i];
			QString price;
			QString quantity;
			QString create;
			QString update;
			price.setNum(Quote.price,'g',decimals);
			quantity.setNum(Quote.quantity);
			//create=Quote.datetime_create.toString("yyyy-MM-dd hh:mm:ss");
			//update=Quote.datetime_update.toString("yyyy-MM-dd hh:mm:ss");
			//create=DateTime2Text(Quote.datetime_create);
			//update=DateTime2Text(Quote.datetime_update);
			//XML+="	<buy price='"+price+"' volume='"+quantity+"' create='"+create+"' update='"+update+"'/>\n";
			XML+="	<buy price='"+price+"' volume='"+quantity+"'/>\n";
		}
		XML+="</quotes>";
		return XML;
	}
	// return 0 if not found and -1 in find_index

	C_EasyQuote* FindBefore(uint datetime, uint from_index, uint& find_index){
		
		if (size==0)
			return 0;
		_ASSERTE(from_index>=0);
		_ASSERTE(from_index<size);
		find_index=from_index;
		C_EasyQuote* pQuote=data+find_index;
		if (pQuote->datetime < datetime){  // found . forward search of last bedore
			pQuote++;
			find_index++;
			for(; find_index<MIN(size,LIMIT_QUOTES); find_index++,pQuote++){
				if (datetime <= pQuote->datetime) // overcross with found. Stop
					break; 
			}
			find_index--;
			pQuote--;
			return pQuote;
		}
		// backward search
		else { // (datetime<=pQuote->datetime)  backward search
			find_index--;
			pQuote--;
			while ((int)find_index>=0){
				if (pQuote->datetime < datetime) // found .stop
					return pQuote;
				find_index--;
				pQuote--;
			}
		}
		return 0;
	}

} ;

//======================= C_SharedMemoryInstrument ============================================
class C_SharedMemoryInstrument {
public:
	
	S_InstrumentInfo Info;
	
	S_EasyTicks	 Ticks;
	S_EasyQuotes Quotes;
	S_EasyTrades Trades;
	S_EasyOrders Orders;
	void Init(){
		Ticks.Init();
		Glasses.Init();
		Quotes.Init();
		Trades.Init();
		Orders.Init();
	}
	struct {
		C_FixedGlass data[LIMIT_GLASSES];
		uint size;
		C_FixedGlass* FindInInterval(uint datetime,uint interval, uint& fromIndex){
			uint saveIndex=fromIndex;
			C_FixedGlass* pGlass=data+fromIndex;
			for(; fromIndex<size; fromIndex++, pGlass++){
				if (datetime<=pGlass->datetime)
					if (pGlass->datetime-datetime<=interval)
						return pGlass;
			}
			fromIndex=saveIndex;
			return 0;
		}
		C_FixedGlass* FindBefore(uint datetime, uint interval, uint& fromIndex){
			fromIndex=MAX(1,fromIndex);
			uint saveIndex=fromIndex;
			C_FixedGlass* pGlass=data+fromIndex;
			for(; fromIndex<size; fromIndex++, pGlass++){
				if (datetime<pGlass->datetime){
					if (pGlass->datetime-datetime<interval){
						pGlass--;
						fromIndex--;
						//_ASSERTE(fromIndex>=0);
						return pGlass;
					}
				}
			}
			fromIndex=saveIndex;
			return 0;
		}
		void Init(){
			size=0;
		}
	} Glasses;

	

	
};

//======================= C_Instrument ============================================

class C_Instrument {
private:
	
	QSharedMemory* pSharedMemory;
	
public:
	
	QList<S_Quote> listBuyQuote;
	QList<S_Quote> listSellQuote;
	QString strLastGlass;
	C_SharedMemoryInstrument* pData;
	
	struct {
		uint tail;
		uint tailQoute;
		uint tailLogQuote;
		uint lastDateTimeInDB;
		C_XML_Logger* pTickLog;
		S_EasyTicks* pTicks;
	} TickInfo;

	struct {
		uint tail;
		C_XML_Logger* pQuoteLog;
		S_EasyQuotes* pQuotes;
	} QuoteInfo;

	S_Glass Glass;

	bool UpdateGlass(uint toIndex=0, int history=100){
		if (pData->Quotes.size==0)
			return false;
		if (toIndex==0)
			toIndex=pData->Quotes.size-1;
		return pData->Quotes.UpdateGlass(Glass,toIndex,history);
	}

	QString Name(){
		QString name(pData->Info.seccode);
		return name;
	}

	C_Instrument(){
		TickInfo.tail=0;
		TickInfo.tailQoute=0;
		TickInfo.tailLogQuote=0;
		TickInfo.pTickLog=0;
		TickInfo.lastDateTimeInDB=0;
		pData=0;
		pSharedMemory=0;
		QuoteInfo.tail=0;
		QuoteInfo.pQuoteLog=0;
		

	}
// 	uint WhichDateTime(uint dt){
// 		int idx=-1;
// 		uint i=0;
// 		for(int i=0; i<pData->Ticks.size;i++){
// 			if (pData->Ticks.data[i].datetime==dt){
// 				break;
// 			}
// 		}
// 		for(idx=i; idx<pData->Ticks.size; idx++){
// 			if (pData->Ticks.data[i].datetime!=dt){
// 				break;
// 			}
// 		}
// 		return idx;
// 
// 	}
	//C_Insrument& operator = (const C_Insrument& Inst){
	//	this->pData=Inst.pData;
	//	this->pSharedMemory=Inst.pSharedMemory;
	//	return *this;
	//}
	
	
	bool Create(QString seccode){
		pSharedMemory=new QSharedMemory(seccode);
		//if (!pSharedMemory)
		//	return false;
		_ASSERTE(pSharedMemory);
		if (pSharedMemory->isAttached())
			pSharedMemory->detach();
		bool ok=pSharedMemory->create(sizeof(C_SharedMemoryInstrument),QSharedMemory::ReadWrite);
		_ASSERTE(ok);
		if (ok)
			pData=(C_SharedMemoryInstrument*)pSharedMemory->data();
		
		_ASSERTE(pData);
		pData->Init();
		return true;
	}

	bool Attach(QString seccode){
		pSharedMemory=new QSharedMemory(seccode);
		if (!pSharedMemory){
			_ASSERTE(false);
			return false;
		}
		if (!pSharedMemory->attach()) {
			//_ASSERTE(false);
			return false;
		}
		pData=(C_SharedMemoryInstrument*)pSharedMemory->data();
		return (pData!=0);
	}

	bool Detach(){
		if (pSharedMemory)
			if (pSharedMemory->isAttached())
				return pSharedMemory->detach();
		
		return true;
	}


	void Init()
	{
		if (pSharedMemory->isAttached()){
			pData->Init();
		}	
	}
	
	
	void Lock(){
		pSharedMemory->lock();
	}
	void Unlock(){
		pSharedMemory->unlock();
	}

};
	



#define LIMIT_SECCODES 128
class C_SharedMemoryPortfolio {
	char active[LIMIT_SECCODES][16];
	int size;
	C_SharedMemoryPortfolio& operator << (char* sec_code){
		if (size<LIMIT_SECCODES){
			strcpy(active[size],sec_code);
			size++;
		}
	}
};


/*

class C_SharedPortfolio{
public:
	QList<QString> listActive;
	QMap<QString,QSharedMemory*> mapSharedMemory;
	QMap<QString,C_SharedInstrument*> mapSharedInstrument;

	C_SharedPortfolio& operator << (char* sec_code){
		QString seccode(sec_code);
		listActive<< seccode;
		mapSharedMemory[seccode]=new QSharedMemory(seccode);
		mapSharedMemory[seccode]->create(sizeof(C_SharedInstrument));

		mapSharedInstrument[seccode]=(C_SharedInstrument*)mapSharedMemory[seccode]->data();
		return *this;
	}
};
*/

#endif 