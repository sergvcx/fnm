#ifndef SHARED_DEFINED
#define SHARED_DEFINED

#include <QSharedMemory>
#include <QFile>
#include <QTextStream>
#include "xmllogger.h"
#define  MAX_TICS 1024

//#include "TransaqConnector.h"

#define LIMIT_TICKS  1024*1024		// must be power of two
#define LIMIT_QOUTES 1024*1024		// must be power of two
#define LIMIT_GLASSES 32768*16
#define GLASS_DEPTH 7

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
		QString XML="<S_XML_Tick seccode='"+seccode+"' price='"+price+"' quantity='"+quantity+"' tradetime='"+tradetime+"' buysell='"+buysell+"' >";
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
		QString XML="<S_Tick price='"+str_price+"' volume='"+str_quantity+"' date='"+str_date+"' time='"+str_time+"' type='"+str_type+"' >";
		return XML;
	}
	QDateTime DateTime(){
		QDateTime dt;
		dt.setTime_t(datetime);
		return dt;
	}
};


struct S_EasyTicks{
	S_Tick	data[LIMIT_TICKS];
	int		size;
	S_Tick& operator[] (int idx){
		return data[idx&(LIMIT_QOUTES-1)];
	}
	S_Tick& Last(){
		return data[(size-1)&(LIMIT_QOUTES-1)];
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
	uint NextSecondIndex(uint fromIndex){
		S_Tick* pTick=data+fromIndex;
		uint from_datetime=pTick->datetime;
		for(uint i=fromIndex; i<size; i++, pTick++)
			if (pTick->datetime>from_datetime)
				return i;
		return size;
	}
	void operator << (S_XML_Tick& Tick){
		bool ok;
		S_Tick& NewTick =data[ size   &(LIMIT_QOUTES-1)];
		S_Tick& LastTick=data[(size-1)&(LIMIT_QOUTES-1)];
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
			NewTick.SetSellType();
		else 
			NewTick.SetBuyType();
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
	float		price;
	int			buy;
	int			sell;
	uint		datetime;
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

	QString toXML(){
		QString XML="<S_XML_QuoteInfo seccode='"+seccode+"' price='"+price+"' buy='"+buy+"' sell='"+sell+"'>";
		return XML;
	}

};

struct S_Quote {
	int	  quantity;
	float price;
	QDateTime datetime_create;
	QDateTime datetime_update;
};

// void DiffGlass(QList<S_Quote>& listPrevBuyQuote, QList<S_Quote>& listPrevSellQuote, 
// 			   QList<S_Quote>& listCurrBuyQuote, QList<S_Quote>& listCurrSellQuote)
// {
// 	
// }

class C_SharedMemoryInstrument {
public:
	
	S_InstrumentInfo Info;
	S_EasyTicks	Ticks;
	//S_EasyQutes Quotes;
	void Init(){
		Ticks.Init();
	}
	struct {
		C_FixedGlass data[LIMIT_GLASSES];
		uint size;
		C_FixedGlass* FindDateTime(uint datetime, uint& fromIndex){
			C_FixedGlass* pGlass=data+fromIndex;
			for(; fromIndex<size; fromIndex++, pGlass++){
				if (datetime>=pGlass->datetime)
					return pGlass;
			}
			return 0;
		}
	} Glasses;
	struct S_EasyQuotes{
		C_EasyQuote data[LIMIT_QOUTES];
		int			size;
		S_EasyQuotes(){
			size=0;
		}
		
		void operator << (S_XML_QuoteInfo& QuoteInfo){
			bool ok;
			C_EasyQuote& quote=data[size&(LIMIT_QOUTES-1)];
			quote.price    =QuoteInfo.price.toFloat(&ok);
			quote.buy      =QuoteInfo.buy.toInt(&ok);
			if (!ok) quote.buy  =0;
			quote.sell     =QuoteInfo.sell.toInt(&ok);
			if (!ok) quote.sell =0;
			QDateTime dt;
			//dt.setTime_t(datetime);
			dt=QDateTime::currentDateTime();
			quote.datetime=dt.toTime_t();
			size++;
		}


		bool UpdateCurrentQuotes(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote, int history=100)
		{
			S_Quote NewQuote;
			bool isUpdated=false;
			for(int i=MAX(0,size-history); i<size; i++){
				C_EasyQuote& HistoryQuote=data[i&(LIMIT_QOUTES-1)];
				if (HistoryQuote.sell){
					//------------ if history quote is sell ---------
					if (listSellQuote.isEmpty() && HistoryQuote.sell>0 ){
						NewQuote.price   =HistoryQuote.price;
						NewQuote.quantity=HistoryQuote.sell;
						NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
						NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
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
									NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
									NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
									Iter.previous();
									Iter.insert(NewQuote);
									isUpdated=true;
									break;
								}
								if (HistoryQuote.price == CurQuote.price){
									CurQuote.quantity=HistoryQuote.sell;
									CurQuote.datetime_update.setTime_t(HistoryQuote.datetime);
									isUpdated=true;
									break;
								}
								if (!Iter.hasNext()){
									NewQuote.price			=HistoryQuote.price;
									NewQuote.quantity		=HistoryQuote.sell;
									NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
									NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
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
						NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
						NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
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
									NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
									NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
									Iter.previous();
									Iter.insert(NewQuote);
									isUpdated=true;
									break;
								}
								if (HistoryQuote.price == CurQuote.price){
									CurQuote.quantity		=HistoryQuote.buy;
									CurQuote.datetime_update.setTime_t(HistoryQuote.datetime);
									isUpdated=true;
									break;
								}
								if (!Iter.hasNext()){
									NewQuote.price			=HistoryQuote.price;
									NewQuote.quantity		=HistoryQuote.buy;
									NewQuote.datetime_create.setTime_t(HistoryQuote.datetime);
									NewQuote.datetime_update.setTime_t(HistoryQuote.datetime);
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

		QString toXML(int decimals,int history=100)
		{
			QList<S_Quote> Sell;
			QList<S_Quote> Buy;
			QString XML;

			UpdateCurrentQuotes(Buy,Sell);

			XML ="<quotes>\n"; // seccode='" + QString(Info.seccode)+"'>\n";


			for(int i=Sell.size()-1; i>=0; i--){
				S_Quote& Quote=Sell[i];
				QString price;
				QString quantity;
				QString create;
				QString update;
				price.setNum(Quote.price,'g',decimals);
				quantity.setNum(Quote.quantity);
				create=Quote.datetime_create.toString("yyyy-MM-dd hh:mm:ss");
				update=Quote.datetime_update.toString("yyyy-MM-dd hh:mm:ss");
				XML+="	<sell price='"+price+"' volume='"+quantity+"' create='"+create+"' update='"+update+"'>\n";
			}
			for(int i=0; i<Buy.size(); i++){
				S_Quote& Quote=Buy[i];
				QString price;
				QString quantity;
				QString create;
				QString update;
				price.setNum(Quote.price,'g',decimals);
				quantity.setNum(Quote.quantity);
				create=Quote.datetime_create.toString("yyyy-MM-dd hh:mm:ss");
				update=Quote.datetime_update.toString("yyyy-MM-dd hh:mm:ss");
				XML+="	<buy price='"+price+"' volume='"+quantity+"' create='"+create+"' update='"+update+"'>\n";
			}
			XML+="</quotes>";
			return XML;
		}
	} Quotes;

	
};


class C_Instrument {
private:
	
	QSharedMemory* pSharedMemory;
	
public:
	C_XML_Logger* pQuoteLog;
	C_XML_Logger* pTickLog;

	QList<S_Quote> listBuyQuote;
	QList<S_Quote> listSellQuote;
	QString strLastGlass;
	C_SharedMemoryInstrument* pData;
	
	uint min_datetime_filter;
	

	QString Name(){
		QString name(pData->Info.seccode);
		return name;
	}

	C_Instrument(){
		tail=0;
		pData=0;
		min_datetime_filter=0;
		tailLogQuote=0;
		
		pSharedMemory=0;
		pQuoteLog=0;
		pTickLog=0;

	}
	uint WhichDateTime(uint dt){
		int idx=-1;
		int i=0;
		for(int i=0; i<pData->Ticks.size;i++){
			if (pData->Ticks.data[i].datetime==dt){
				break;
			}
		}
		for(idx=i; idx<pData->Ticks.size; idx++){
			if (pData->Ticks.data[i].datetime!=dt){
				break;
			}
		}
		return idx;

	}
	//C_Insrument& operator = (const C_Insrument& Inst){
	//	this->pData=Inst.pData;
	//	this->pSharedMemory=Inst.pSharedMemory;
	//	return *this;
	//}
	
	unsigned tail;
	unsigned tailLogQuote;

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
		if (!pSharedMemory)
			return false;
		if (!pSharedMemory->attach()) 
			return false;
		pData=(C_SharedMemoryInstrument*)pSharedMemory->data();
		return (pData!=0);
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