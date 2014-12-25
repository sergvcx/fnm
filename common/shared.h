#ifndef SHARED_DEFINED
#define SHARED_DEFINED

#include <QSharedMemory>

#define  MAX_TICS 1024

#include "TransaqConnector.h"

#define LIMIT_TICKS  1024*1024		// must be power of two
#define LIMIT_QOUTES 1024*1024		// must be power of two



struct S_EasyTicks{
	S_Tick	data[LIMIT_TICKS];
	int		size;
	
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

class C_SharedMemoryInstrument {
public:
	
	S_InstrumentInfo Info;
	S_EasyTicks	Ticks;
	void Init(){
		Ticks.Init();
	}
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
			quote.datetime =QDateTime::currentDateTime();
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
						NewQuote.datetime_create=HistoryQuote.datetime;
						NewQuote.datetime_update=HistoryQuote.datetime;
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
									NewQuote.datetime_create=HistoryQuote.datetime;
									NewQuote.datetime_update=HistoryQuote.datetime;
									Iter.previous();
									Iter.insert(NewQuote);
									isUpdated=true;
									break;
								}
								if (HistoryQuote.price == CurQuote.price){
									CurQuote.quantity=HistoryQuote.sell;
									CurQuote.datetime_update=HistoryQuote.datetime;
									isUpdated=true;
									break;
								}
								if (!Iter.hasNext()){
									NewQuote.price			=HistoryQuote.price;
									NewQuote.quantity		=HistoryQuote.sell;
									NewQuote.datetime_create=HistoryQuote.datetime;
									NewQuote.datetime_update=HistoryQuote.datetime;
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
						NewQuote.datetime_create=HistoryQuote.datetime;
						NewQuote.datetime_update=HistoryQuote.datetime;
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
									NewQuote.datetime_create=HistoryQuote.datetime;
									NewQuote.datetime_update=HistoryQuote.datetime;
									Iter.previous();
									Iter.insert(NewQuote);
									isUpdated=true;
									break;
								}
								if (HistoryQuote.price == CurQuote.price){
									CurQuote.quantity		=HistoryQuote.buy;
									CurQuote.datetime_update=HistoryQuote.datetime;
									isUpdated=true;
									break;
								}
								if (!Iter.hasNext()){
									NewQuote.price			=HistoryQuote.price;
									NewQuote.quantity		=HistoryQuote.buy;
									NewQuote.datetime_create=HistoryQuote.datetime;
									NewQuote.datetime_update=HistoryQuote.datetime;
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

#define LOGGER_APPEND 1 //QIODevice::Append
#define LOGGER_WRITE 2 //QIODevice::WriteOnly
#define LOGGER_READWRITE QIODevice::ReadWrite
#define LOGGER_TEXT QIODevice::Text
class C_XML_Logger
{
	QFile* logFile;
	QTextStream* logStream;	
public:
	C_XML_Logger(QString filename, int mode=LOGGER_WRITE  ){ //QIODevice::OpenModeFlag mode
		logFile=0;
		logStream=0;
		logFile = new QFile(filename);
		
		if (mode==LOGGER_WRITE)
			if(logFile->open(QIODevice::WriteOnly | QIODevice::Text)){
				logStream = new QTextStream(logFile);
			}
		if (mode==LOGGER_APPEND)
			if(logFile->open(QIODevice::Append | QIODevice::Text)){
				logStream = new QTextStream(logFile);
			}
		_ASSERTE(logFile);
		_ASSERTE(logStream);
	}
	void Header(){
		*logStream<<"<?xml version='1.0' encoding='UTF-8'?>" <<"\n";
		logStream->flush();
	}
	void flush(){
		logStream->flush();
	}
	//bool Reopen();
	void operator << (char* log){
		_ASSERTE(logStream);
		*logStream	<< log;
	}
	QTextStream& operator << (QString& log){
		_ASSERTE(logStream);
		*logStream	<< log;
		return *logStream;
	}
	void close(){
		//logStream->close();
		logFile->close();
	}
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
	
	

	QString Name(){
		QString name(pData->Info.seccode);
		return name;
	}

	C_Instrument(){
		tail=0;
		pData=0;

		tailLogQuote=0;
		
		pSharedMemory=0;
		pQuoteLog=0;
		pTickLog=0;

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
		return true;
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