#include "ThreadAllDeals.h"
#include "monitor.h"
#include <QQueue>
#include "main.h"
#include <QFile>
#include <QProcess>
CThreadAllDeals::CThreadAllDeals():QThread()
{
	stopped=true;
	db_index=0;
	// Открываем базу данных в которую пишет Quik
	//db_quik= QSqlDatabase::addDatabase("QMYSQL","AllDeals_quik");
	//db_quik.setHostName("");
	//db_quik.setDatabaseName("quik");
	//db_quik.setDatabaseName("quiktest");
	//db_quik.setUserName("root");
	//db_quik.setPassword("root");	
	
	// Открываем базу в которую пишем отсортированные сделки 
	db_trading = QSqlDatabase::addDatabase("QMYSQL","AllDeals_trading");
	db_trading.setHostName("");
	db_trading.setDatabaseName("trading");
	//db_trading.setDatabaseName("tradingtest");
	db_trading.setUserName("root");
	db_trading.setPassword("root");

	Connect();

}
void CThreadAllDeals::onTimer(){
	
	return ;
	//printf("On Timer\n");
	QTime Time;
	QTime StartTime(9,59);
	QTime StopTime(19,00);
	
	Time=QTime::currentTime();
	
	//printf("%d%d\n",isRunning(),isFinished());
	if (StartTime<Time &&  Time<StopTime){
		Connect();
		if (!isRunning())
			start();
	}


	if (Time>StopTime){
		if (isRunning())
			stop();
		if (isFinished())
			Disconnect();
	}
}

void CThreadAllDeals::onSMSTimer(){
	
	QTime Time;
	QDate Date;

	Time=QTime::currentTime();
	Date=QDate::currentDate();
	QString strTime=Time.toString();
	QString strDate=Date.toString(Qt::ISODate);
	/*
	int static SMS1Send=0;
	int static SMS2Send=0;

	QTime Time;
	QDate Date;
	QTime SMSTime1(11,00);
	QTime SMSTime2(16,00);

	

	if (Time<SMSTime1){
		SMS1Send=0;
		SMS2Send=0; 
	}*/

	if (!mapPortfolio.contains("SBER03"))
		return;
	if (!mapPortfolio.contains("SBERP03"))
		return;
	SPortfolio& sber=*mapPortfolio["SBER03"];
	SPortfolio& sberp=*mapPortfolio["SBERP03"];
	//if (mapPortfolio["SBER03"].quePastDeals.size()==0)
	//	return;
	//if (mapPortfolio["SBERP03"].quePastDeals.size()==0)
	//	return;
	//if// (Time>SMSTime1 && !SMS1Send){
	{
		//SMS1Send=1;
		//QFile file("sms.txt");
		FILE  * file;
		while((file=fopen ("D:\\Exchange\\sms.txt","wt"))==0);

		float demand=sber.pLastDealDemand->Price;	
		float supply=sber.pLastDealSupply->Price;
		//int date=sber..LastDealDemand
		fprintf(file,"%s %s \n",STR(strDate),STR(strTime));
		fprintf(file,"sb %.2f %.2f\n",demand,supply);

		demand=sberp.pLastDealDemand->Price;	
		supply=sberp.pLastDealSupply->Price;

		fprintf(file,"sbp %.2f %.2f\n",demand,supply);


		fclose(file);

	//	QObject *parent;
		//QString program = "D:\\Programs\\Mail.Ru\\magent.exe";
	//	QString program = "d:\\SDK\\MacroScheduler\\sms.scp";
	//	QStringList arguments;
		//arguments << "-style" << "motif";

		//QProcess *myProcess = new QProcess(parent);
		//myProcess->start(program, arguments);


	}
//	if (Time>SMSTime2 && !SMS2Send){
//		SMS2Send=1;
//	}
	
}

void CThreadAllDeals::Connect(){

	//while (!db_quik.isOpen())
	//{
	//	if (!db_quik.open()) {
			//QMessageBox::critical(0, QObject::tr("Database Error"), db_quik.lastError().text());
	//		printf("ERROR: quik has not been connected\n");	
	//	}
	//	else {
	//		printf("quik has been connected\n");	
	//	}
	//	msleep(1000);
	//}
	

	//while(!db_trading.isOpen())
	{
		if (!db_trading.open()) {
			QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
			printf("ERROR: trading has not been connected\n");	
		}
		else {
			printf("trading has been connected\n");	
		}
		msleep(1000);
	}
}
void CThreadAllDeals::Disconnect(){
	stopped=true;

	if (db_trading.isOpen()){
		db_trading.close();
		printf("trading disconnected\n");	
	}

}

// Считываем список акций с которыми будем работать из таблицы _Portfolio 
int CThreadAllDeals::ReadPortfolio(){
	if (db_trading.isOpenError() || !db_trading.isValid()){
		printf("database open ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	int nMaxID=0;
	QSqlQuery query(db_trading);
	QString Cmd="SELECT * FROM _portfolio ORDER BY 3 ;";
	QQueue<QString> queInstrument;
	printf(STR(Cmd));
	query.exec(Cmd);
	if (!query.isActive()){
		printf(" - ERROR\n");
		QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
	}
	else 
		printf(" - OK\n");
	while (query.next())
	{
		SPortfolio* pPortfolio=new SPortfolio;
		SPortfolio& Portfolio=*pPortfolio;
		Portfolio.Parse(query);
		/*
		Portfolio.Status			= query.value(1).toInt();
		Portfolio.StockCode			= query.value(2).toString();	
		Portfolio.StockName			= query.value(3).toString();	
		Portfolio.LastOfferDate		= query.value(4).toInt();	
		Portfolio.LastOfferTime		= query.value(5).toInt();	
		Portfolio.Volume			= query.value(6).toInt();
		Portfolio.Profit			= query.value(7).toDouble();
		Portfolio.LastID			= query.value(6).toInt();
		*/
		// Добавляем в множество информацию об Акции
		
		mapPortfolio[Portfolio.StockCode]=pPortfolio;
		//printf("use portfolio - %s\n",STR(Portfolio.StockCode));
		// создаем таблицы если они не созданы
		queInstrument << Portfolio.StockCode;
		if (Portfolio.LastID>nMaxID)
			nMaxID=Portfolio.LastID;
	}
	// если нет баз данных то создаем... deal и trd
	while (!queInstrument.isEmpty()){
		if (mapPortfolio[queInstrument.head()]->Status>0){
//			pDBManager->TradingCreate_deal(queInstrument.head()); 
			pDBManager->TradingCreate_trd(queInstrument.head()); 
		}
		queInstrument.dequeue();
	}
	return nMaxID;
}


///=============================================================================
// Главная функция приема и сортировки сделок
//==============================================================================
void CThreadAllDeals::run(){


}

void CThreadAllDeals::Parse(QQueue<S_XML_Tick>& TickQueue)
{
	
//	if (!db_trading.isOpen()){
//		printf("Error: trading is disconnected\n");	
//		_ASSERTE(0);
//	}
	
//	printf("isRunning...\n");
	int nCurrGlassTimeLive=10;
	int nPastGlassTimeLive=60;

	int nCurrDealTimeLive=3;
	int nPastDealTimeLive=60;

	stopped=false;

	//QQueue<SOffer> QueueOffer;
	//QQueue<SDeal>  QueueAllDeals;
	long long db_index; //= ReadPortfolio();

	//db_index=0;

	char str[200];
	QString		db_instrument;
	QString		db_stcode;
	QString		db_stdate;
	QString		db_sttime;
	QDateTime	db_daytime;
	int			db_stvolume;
	float		db_stprice;

	float		db_price;
	float		db_supply;
	float		db_demand;	
	QString		db_stockname;
	int			db_demvol;
	int			db_supvol;
	float		db_step;
	int			db_lot;		
	QString		db_operation;

	//QSqlQuery query(db_quik);
	QSqlQuery trquery(db_trading);
	// Первый запрос - вычисление последнего индекса.
	/*
	if(db_index==0){
		//sprintf(str,"SELECT * FROM AllDeals WHERE ID=(SELECT MAX(ID) FROM AllDeals);");
		sprintf(str,"SELECT * FROM AllDeals");
		query.exec(str);
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		bool ret;
		if (ret=query.last()){
			db_index=query.value(0).toInt();
		}
		//_ASSERTE(ret);
	}
	*/
	//query.lastError().showMessage();
	//int numRows = query.size();
	//db_index-=1000;
	//printf ("Deal accepting started\n");
	//QMap<QString,QQueue<SDeal>> mapQueueDeal;
	//while(!stopped)
	{

		//=============== чтение сделок ====================
		//printf("----------(read) ------------\n");
		//sprintf(str,"SELECT * FROM AllDeals WHERE ID>%d  ORDER BY 1 LIMIT 1000;",db_index);
//		query.exec(str);
//		if (!query.isActive())
//			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
//		printf("----------(parse) ------------\n");

		while (!TickQueue.isEmpty()) 
		{
			
			S_XML_Tick Tick=TickQueue.dequeue();
			//printf(STR(Tick.seccode));
			// читаем поля строки из БД
			bool ok;
			db_index	= Tick.tradeno.toLongLong(); //printf(STR(Tick.tradeno));
			db_stcode	= Tick.seccode;
			db_stdate	= Tick.tradetime.left(10.); //08.08.2014 10:01:52
			db_sttime	= Tick.tradetime.right(8);
			db_stvolume	= Tick.quantity.toInt();
			db_stprice	= Tick.price.toDouble();
			db_operation= Tick.buysell;

			
			
			// если нету такой акции то добовляем в список  
			if (!mapPortfolio.contains(db_stcode)){
				SPortfolio* pPortfolio=new SPortfolio;
				SPortfolio& Portfolio=*pPortfolio;
				Portfolio.StockCode=db_stcode;
				Portfolio.Status=0;
				Portfolio.LastID=0;
				// добавляем в таблицу портфелей новый портфель
				Portfolio.Insert(trquery);
				mapPortfolio[db_stcode]=pPortfolio;
			}
		
			
			// обрабатываем сделку если она в активном статусе
			SPortfolio& Portfolio=*mapPortfolio[db_stcode];
			SDealData & LDR=Portfolio.LastDealRecieved;
			SDealData & LDS=Portfolio.LastDealStored;
			if (Portfolio.Status==0)
				continue;
				
			//if (Portfolio.LastID>=db_index)
			if (LDS.nID>=db_index)
				continue;

			QDate qDate=QDate::fromString(db_stdate, "dd.MM.yyyy");
			QTime qTime=QTime::fromString(db_sttime,"hh:mm:ss");
			QDateTime DateTime(qDate,qTime);
			//SDeal Deal(db_stcode, DateTime, db_stvolume, db_stprice, 0);
			LDR.Set(DateTime, db_stvolume, db_stprice, 0);
			
			//if (Deal.nDate<Portfolio.LastOfferDate)
			// если запись уже есть в базе
			if (LDR.nDate<LDS.nDate)
				continue;
			// если запись уже есть в базе
			//if (Deal.nDate==Portfolio.LastOfferDate && Deal.nTime<Portfolio.LastOfferTime)
			if (LDR.nDate==LDS.nDate && LDR.nTime<=LDS.nTime)
				continue;

			if (Portfolio.StockCode.contains("-")){
				continue;
			}

			LDR.nID=db_index;
			if (db_operation=="B" ){
				LDR.SetSupplyType();
				Portfolio.LastDealSupply=LDR;
			}
			else if (db_operation=="S" ){
				LDR.SetDemandType();
				Portfolio.LastDealDemand=LDR;
			}
			else {
				
				//printf(STR(db_operation));printf("\n");
				//printf(STR(RUS(db_operation)));printf("\n");
				//QMessageBox::critical(0,db_operation, db_operation);
				//_ASSERTE(0);
				//!!!
				//continue;
			}

			//Deal.SetDelay();
			//Deal.Show();
			
			//Portfolio.LastID		=Deal.nID;
			//Portfolio.LastOfferDate	=Deal.nDate;
			//Portfolio.LastOfferTime	=Deal.nTime;

			// закидываем сделку в текущую очередь
			Portfolio.queCurrDeals.enqueue(LDR);
				
			
		
			
		}
	


		// ============== InSecond классификация ======================
		printf("----------(classification------------\n");
		int ret=db_trading.transaction();
		foreach(QString StockCode, mapPortfolio.keys()){
			SPortfolio& Portfolio=*mapPortfolio[StockCode];
			if (Portfolio.queCurrDeals.isEmpty())
				continue;
			SDealData & LDS=Portfolio.LastDealStored;
			if (Portfolio.Status==0)
				continue;
			//iterPortfolio = mapPortfolio.begin();
			//while (iterPortfolio != mapPortfolio.end()) { // end-??
			//QString StockCode =iterPortfolio.value().StockCode;
			//SPortfolio& Portfolio=mapPortfolio[StockCode];
			//QQueue<SDeal>&  queCurrDeals=Portfolio.queCurrDeals;
			//QTime qCurrTime=QTime::currentTime();
			//int   nCurrTimeSecs=-qCurrTime.secsTo(QTime(10,0,0));
			//int   nCurrentTime=qCurrTime.toString("hhmmss").toInt();
				
			
			SDealData& Deal0=LDS;
			SDealData& Deal1=Portfolio.queCurrDeals[0];
			if (Deal0.nTime!=Deal1.nTime)
				Deal1.SetFlag(FIRST_IN_SECOND);
			else{
				Deal0.SetFlag(IN_SECOND);
				Deal1.SetFlag(IN_SECOND);
			}

			for(int i=1; i<Portfolio.queCurrDeals.size();i++){
				SDealData& Deal0=Portfolio.queCurrDeals[i-1];
				SDealData& Deal1=Portfolio.queCurrDeals[i];
				if (Deal0.nTime<Deal1.nTime)
					Deal1.SetFlag(FIRST_IN_SECOND);
				else{
					Deal0.SetFlag(IN_SECOND);
					Deal1.SetFlag(IN_SECOND);
				}
			}
			LDS=Portfolio.queCurrDeals.last();
			
 			Portfolio.StoreQueue(trquery); //dequeue inside
			
			
			
		}
		
		//============= сохраняем portfolio ==============
		// сохраняем последние значения в портфели
		printf("--------- (update portfolio)-----------\n");
		foreach (SPortfolio* pPortfolio, mapPortfolio.values())
		{
			pPortfolio->UpdateAll(trquery);
		}
		db_trading.commit();
		msleep(5);
	}
	if (stopped){
		QSqlQuery query(db_trading);
		foreach (SPortfolio* pPortfolio, mapPortfolio.values()){
			pPortfolio->UpdateAll(query);
		}
	}
	printf("isFinished...\n");

}

void CThreadAllDeals::stop()
{
	stopped=true;
	
}


CThreadAllDeals::~CThreadAllDeals(void)
{
	QSqlQuery query(db_trading);
	foreach (SPortfolio* pPortfolio, mapPortfolio.values())
	{
		pPortfolio->UpdateAll(query);
	}
}

/*
void CThreadAllDeals::HandleDeal(SDeal &Deal){
	//char* sCodeName, int nMetaDate,int nMetaTime,  float fPrice, int nVolume){
	static QSqlQuery query1(db_trading);
	sprintf(cmd,"INSERT INTO %s_deal (trdate,trtime,volume,price,trtype) VALUES(%d,%d,%d,%f,%d);",STR(Deal.Name),Deal.nDate,Deal.nTime,Deal.nVolume, Deal.Price, Deal.nType);
	query1.exec(cmd);


} 
*/
/*
void CThreadAllDeals::HandleOffer(SOffer &Offer){
	if (Offer.Demand<1 || Offer.Supply<1)
		return;
	QSqlQuery query(db_trading);
	SPortfolio Portfolio=mapPortfolio[Offer.Name];
	if (Offer.Demand!=Portfolio.LastOfferDemand || Offer.Supply!=Portfolio.LastOfferSupply){
		Offer.Show();
		sprintf(cmd,"INSERT INTO trading.%s_off (trdate, trtime, demand, supply, demvol, supvol) VALUES(%d, %d, %f, %f, %d, %d );",STR(Offer.Name), Offer.nDate, Offer.nTime, Offer.Demand, Offer.Supply, Offer.nDemVol, Offer.nSupVol);
		query.exec(cmd);
		//printf("%s\n",cmd);
		if (!query.isActive())
			QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
		Portfolio.LastOfferDate=Offer.nDate;
		Portfolio.LastOfferTime=Offer.nTime;
		Portfolio.LastOfferDemand=Offer.Demand;
		Portfolio.LastOfferSupply=Offer.Supply;
		mapPortfolio[Offer.Name]=Portfolio;
	} 
}/

//============= чтение карманов  ==================

/*
QMap<QString, SPortfolio>::const_iterator iterPortfolio = mapPortfolio.begin();
while (iterPortfolio != mapPortfolio.end()) {
QString StockCode =iterPortfolio.value().StockCode;
SPortfolio& Portfolio=mapPortfolio[StockCode];

QTime qCurrTime=QTime::currentTime();
int   nCurrentTime=qCurrTime.toString("hhmmss").toInt();

QString Cmd="SELECT * FROM rates_" + StockCode + " ORDER BY 4;";
query.exec(Cmd);
if (!query.isActive())
QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());
if (query.size()==0){
iterPortfolio++;			
continue;
}

Cmd="DELETE FROM rates_" + StockCode + ";";
QSqlQuery query2; 
query2.exec(Cmd);
if (!query2.isActive())
QMessageBox::critical(0, QObject::tr("Database Error"), query2.lastError().text());


// наполняем стакан
SGlass& Glass=Portfolio.Glass;
Glass.nTime =nCurrentTime;
while (query.next() && !stopped) {
//db_instrument	= query.value(1).toString();
db_demvol		= query.value(2).toInt();
db_price		= query.value(3).toDouble();
db_supvol		= query.value(4).toInt();
if (db_demvol>0){
SRequest Request(db_price,db_demvol);
if (db_price>Glass.listDemand[0].price){
Glass.listDemand.insert(0,Request);
int i=0;
while(i<Glass.listSupply.size()){
if (db_price>=Glass.listSupply[i].price){
Glass.listSupply.removeAt(i);
continue;
}
else
break;
i++;
}
}
else {
for(int i=0; i<Glass.listDemand.size()-1; i++){
if (db_price==Glass.listDemand[i].price){
Glass.listDemand[i]=Request;
break;
}
else if (Glass.listDemand[i].price>db_price && db_price>Glass.listDemand[i+1].price){
Glass.listDemand.insert(i+1,Request);
break;
}
}
}
}
else if (db_supvol>0){
SRequest Request(db_price,db_supvol);
if (db_price<Glass.listSupply[0].price) {
Glass.listSupply.insert(0,Request);
int i=0;
while(i<Glass.listDemand.size()){
if (db_price<=Glass.listDemand[i].price){
Glass.listDemand.removeAt(i);
continue;
}
else
break;
i++;
}
}
else {
for(int i=0; i<Glass.listSupply.size()-1; i++){
if (db_price==Glass.listSupply[i].price){
Glass.listSupply[i]=Request;
break;
}
else if (Glass.listSupply[i].price<db_price && db_price<Glass.listSupply[i+1].price){
Glass.listSupply.insert(i+1,Request);
break;
}
}
}
} 
else {
_ASSERTE(0);
}

while (Glass.listDemand.size()>16){
Glass.listDemand.removeAt(Glass.listDemand.size()-2);
}
while (Glass.listSupply.size()>16){
Glass.listSupply.removeAt(Glass.listSupply.size()-2);
}
}

//if (StockCode=="LKOH"){
//	float d=0;
//	float f=0;
//	int dv=0;
//	int sv=0;
//	if (Glass.listDemand.size()){
//		d=Glass.listDemand[0].price;
//		dv=Glass.listDemand[0].volume;
//	}
//	if (Glass.listSupply.size()){
//		f=Glass.listSupply[0].price;
//		sv=Glass.listSupply[0].volume;
//	}
//	//printf("GlasTime:%d \tDemand:%f\t Supply:%f dv:%d sv:%d\n",nCurrentTime, d,f,dv,dv); 
//	printf("final\n");
//	printf("GlasTime:%d ",nCurrentTime); 
//	for(int i=Glass.listDemand.size()-1; i>=0; i--)
//		printf("%.2f:%d ",Glass.listDemand[i].price,Glass.listDemand[i].volume);
//	printf(" ]=[ ");
//	for(int i=0; i<Glass.listSupply.size(); i++)
//		printf("%.2f:%d ",Glass.listSupply[i].price,Glass.listSupply[i].volume);
//	printf("\n");
//}



iterPortfolio++;

}
*/
//============= Demand-Supply классификация =====================
/*		
iterPortfolio = mapPortfolio.begin();
while (iterPortfolio != mapPortfolio.end()) { // end-??
QString StockCode =iterPortfolio.value().StockCode;
SPortfolio& Portfolio=mapPortfolio[StockCode];
QQueue<SDeal>&  queCurrDeals=Portfolio.queCurrDeals;
QTime qCurrTime=QTime::currentTime();
int   nCurrTimeSecs=-qCurrTime.secsTo(QTime(10,0,0));
int   nCurrentTime=qCurrTime.toString("hhmmss").toInt();
//QTime qDealTime(Deal.nTime/10000,(Deal.nTime%10000)/100,Deal.nTime%100);

// сначала классифицируем очередь по предыдущей сделке 
//SDeal LastDeal=Portfolio.Las
int nBestTimeGlassSupply ;
int nBestTimeGlassDemand;
SDeal LastDealInQueuePastDeal;

for(int i=0; i<queCurrDeals.size();i++){
SDeal& Deal=queCurrDeals[i];
//int DealDelay=nCurrentTime-Deal.nTime;
//printf("Deal delay = %d\n",DealDelay);

// Тупая <> классификация
if (Deal.Price>=Portfolio.LastDealSupply.Price){
if (Deal.TimeSecs()-Portfolio.LastDealSupply.TimeSecs()>10)
Deal.SetSupplyPropobality(2);
else 
Deal.SetSupplyPropobality(Portfolio.LastDealSupply.Propobality()+1);
}
if (Deal.Price<=Portfolio.LastDealDemand.Price){
if (Deal.TimeSecs()-Portfolio.LastDealSupply.TimeSecs()>10)
Deal.SetDemandPropobality(2);
else 
Deal.SetDemandPropobality(Portfolio.LastDealDemand.Propobality()+1);
}
// классификация по стаканам
SGlass& Glass=Portfolio.Glass;
double MinSupplyDiff=1000;
int idxSupplyRequest=-1;
int idxDemandRequest=-1;
for(int i=0; i<Glass.listSupply.size();i++){
double Diff=ABS(Deal.Price-Glass.listSupply[i].price);
if (Diff<MinSupplyDiff){
MinSupplyDiff=Diff;
idxSupplyRequest=i;
}
else 
break;
}
double MinDemandDiff=1000;
for(int i=0; i<Glass.listDemand.size();i++){
double Diff=ABS(Deal.Price-Glass.listDemand[i].price);
if (Diff<MinDemandDiff){
MinDemandDiff=Diff;
idxDemandRequest=i;
}
else 
break;

}
if (MinDemandDiff<MinSupplyDiff/2){
Deal.SetDemandPropobality(10);
}
else if (MinSupplyDiff<MinDemandDiff/2){
Deal.SetSupplyPropobality(10);
}
else {
// классифицируем по объемам
}


// Удаляем совершившиеся запросы из стакана
if (Deal.IsDemand()){
Portfolio.LastDealDemand=Deal;
if (idxDemandRequest>=0){
if (Glass.listDemand[idxDemandRequest].price==Deal.Price){
int remVolume=Glass.listDemand[idxDemandRequest].volume-Deal.nVolume;
if (remVolume<=0)
Glass.listDemand.removeAt(idxDemandRequest);
else {
Glass.listDemand[idxDemandRequest].volume=remVolume;
}

}
}
}
else if (Deal.IsSupply()){
Portfolio.LastDealSupply=Deal;
if (idxSupplyRequest>=0){
if (Glass.listSupply[idxSupplyRequest].price==Deal.Price){
int remVolume=Glass.listSupply[idxSupplyRequest].volume-Deal.nVolume;
if (remVolume<=0)
Glass.listSupply.removeAt(idxSupplyRequest);
else {
Glass.listSupply[idxSupplyRequest].volume=remVolume;
}
}
}
}
else {
//	_ASSERTE(0);
}

}
*/