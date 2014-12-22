#ifndef SHARED_DEFINED
#define SHARED_DEFINED

#include <QSharedMemory>

#define  MAX_TICS 1024
//struct C_EasyTick{
//	C_Tick	data[MAX_TICS];
//	int		number;
//};

#include "TransaqConnector.h"

#define LIMIT_TICKS 1024*1024		// must be power of two
#define LIMIT_QOUTES 1024*1024		// must be power of two



class C_SharedMemoryInstrument {
public:
	
	S_InstrumentInfo Info;

	struct S_EasyTicks{
		S_Tick	data[LIMIT_TICKS];
		int		size;
		void operator << (S_XML_Tick& Tick){
			bool ok;
			/*
			C_EasyQuote& quote=data[size&(LIMIT_QOUTES-1)];
			quote.price    =QuoteInfo.price.toFloat(&ok);
			quote.buy      =QuoteInfo.buy.toInt(&ok);
			if (!ok) quote.buy  =0;
			quote.sell     =QuoteInfo.sell.toInt(&ok);
			if (!ok) quote.sell =0;
			quote.datetime =QDateTime::currentDateTime();
			*/
		}
	} Ticks;

	struct S_EasyQuotes{
		C_EasyQuote data[LIMIT_QOUTES];
		int			size;
		void operator << (S_XML_QuoteInfo& QuoteInfo){
			bool ok;
			C_EasyQuote& quote=data[size&(LIMIT_QOUTES-1)];
			quote.price    =QuoteInfo.price.toFloat(&ok);
			quote.buy      =QuoteInfo.buy.toInt(&ok);
			if (!ok) quote.buy  =0;
			quote.sell     =QuoteInfo.sell.toInt(&ok);
			if (!ok) quote.sell =0;
			quote.datetime =QDateTime::currentDateTime();
		}
		

		void UpdateCurrentQuotes(QList<S_Quote>& listBuyQuote, QList<S_Quote>& listSellQuote, int history=100)
		{
			S_Quote NewQuote;
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
					}
					else {
						QMutableListIterator<S_Quote> iSell(listSellQuote);
						while (iSell.hasNext()) {
							S_Quote& CurQuote=iSell.value();
							if (HistoryQuote.price == CurQuote.price){
								if (HistoryQuote.sell==-1)
									iSell.remove();
								else {
									CurQuote.quantity=HistoryQuote.sell;
									CurQuote.datetime_update=HistoryQuote.datetime;
								}
								break;
							}
							if (HistoryQuote.price < CurQuote.price){
								NewQuote.price   =HistoryQuote.price;
								NewQuote.quantity=HistoryQuote.sell;
								NewQuote.datetime_create=HistoryQuote.datetime;
								NewQuote.datetime_update=HistoryQuote.datetime;
								iSell.insert(NewQuote);
								break;
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
					}
					else {
						QMutableListIterator<S_Quote> iBuy(listBuyQuote);
						while (iBuy.hasNext()) {
							S_Quote& CurQuote=iBuy.value();
							if (HistoryQuote.price == CurQuote.price){
								if (HistoryQuote.sell==-1)
									iBuy.remove();
								else {
									CurQuote.quantity=HistoryQuote.buy;
									CurQuote.datetime_update=HistoryQuote.datetime;
								}
								break;
							}
							if (HistoryQuote.price < CurQuote.price){
								NewQuote.price   =HistoryQuote.price;
								NewQuote.quantity=HistoryQuote.buy;
								NewQuote.datetime_create=HistoryQuote.datetime;
								NewQuote.datetime_update=HistoryQuote.datetime;
								iBuy.insert(NewQuote);
								break;
							}
						}
					}
				}
			}
		}

		QString& toXML(int history=100)
		{
			QList<S_Quote> Sell;
			QList<S_Quote> Buy;
			QString XML;

			XML ="<quotes>\n"; // seccode='" + QString(Info.seccode)+"'>\n";


			for(int i=Sell.size()-1; i>=0; i--){
				S_Quote& Quote=Sell[i];
				QString price;
				QString quantity;
				QString create;
				QString update;
				price.setNum(Quote.price);
				quantity.setNum(Quote.quantity);
				create=Quote.datetime_create.toString();
				update=Quote.datetime_update.toString();
				XML+="	<sell price='"+price+"' volume='"+quantity+"' create='"+create+"' update='"+update+"'>\n";
			}
			for(int i=0; i<Buy.size(); i++){
				S_Quote& Quote=Buy[i];
				QString price;
				QString quantity;
				QString create;
				QString update;
				price.setNum(Quote.price);
				quantity.setNum(Quote.quantity);
				create=Quote.datetime_create.toString();
				update=Quote.datetime_update.toString();
				XML+="	<buy price='"+price+"' volume='"+quantity+"' create='"+create+"' update='"+update+"'>\n";
			}
			XML+="</quotes>\n";
			return XML;
		}
	} Quotes;

	
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