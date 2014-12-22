#include <QVector>
#include <QThread>
#include <QMap>
#include "main.h"
class CDealContainer{
public:
	QVector<SDeal>	vecDeal;	
	QMap<int, QVector<int> >	mapTimeRef;
	//QVector<int>	vecDays ;
	QVector<int>	vecDateRef ;
	QSqlDatabase	db_trading;
	QString			StockCode;
	QString			connectionName;
	int nVecSize;
	CDealContainer(){
		//ConnectDB();
		StockCode="";
		//vecDateRef.reserve(1024);
		int t=vecDateRef.size();
	}

	void ConnectDB(){
		static int connectionID=0;
		connectionID++;
		connectionName="db_trading"+QString::number(connectionID);
		db_trading = QSqlDatabase::addDatabase("QMYSQL",connectionName);
		db_trading.setHostName("localhost");
		db_trading.setDatabaseName("trading");
		db_trading.setUserName("root");
		db_trading.setPassword("root");
		if (!db_trading.open()) {
			QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
		}
	}
	~CDealContainer(){
		db_trading.close();
		//QSqlDatabase::removeDatabase (connectionName );
		
	}
	//void DisconnectDB(){
	//	db_trading.close();
	//}
	int LoadDeals(int nDate0=0, int nDate1=0){

		int size=0;
		QSqlQuery query(db_trading);
		_ASSERTE(StockCode!="");
		//sprintf(str, "SELECT * FROM %s_deal WHERE ID>%d  ORDER BY 1 LIMIT 100;",STR(Portfolio.StockCode),Portfolio.LastID);
		//sprintf(str, "SELECT * FROM %s_deal WHERE( ID>%d ) ORDER BY 1 LIMIT 100;",STR(Portfolio.StockCode),Portfolio.LastID);
		//sprintf(str, "SELECT * FROM %s_deal WHERE( ID>%d AND trdate>=20090317) ORDER BY 1 ;",STR(Portfolio.StockCode),db_index);
		//sprintf(str, "SELECT * FROM %s_deal WHERE ( trdate>=%d AND trdate<=%d ) ORDER BY 1;",STR(StockCode), nDate0, nDate1);
		
		// смотрим размер таблицы
		QString Cmd="SHOW TABLE STATUS LIKE '" + StockCode +"_deal'";
		query.exec(Cmd);
		VALID(query);
		int Rows=1;
		if (query.next()){
			Rows=query.value(4).toInt();
		}
		
		//SDeal* p=new SDeal[25195420];
		int cap=vecDeal.capacity();
		vecDeal.reserve(Rows+1);
		vecDeal.resize(Rows+1);
		int VEC_BLAT_SIZE=vecDeal.size();
		_ASSERTE(VEC_BLAT_SIZE==Rows+1);
		//читаем таблицу
		unsigned db_index=0;
		int idx=0;
		int LastDate=0;

		while(1){
			if (nDate0!=0 && nDate1!=0)
				Cmd= "SELECT * FROM "+ StockCode + "_deal WHERE ( trdate>=" + QString::number(nDate0) + " AND trdate<=" + QString::number(nDate1) +" AND ID>" + QString::number(db_index)+") ORDER BY 1 LIMIT 10240;";
			else 
				//Cmd= "SELECT * FROM "+ StockCode + "_deal ORDER BY 1 ";
				Cmd= "SELECT * FROM "+ StockCode + "_deal WHERE ID>"+ QString::number(db_index)+ " ORDER BY 1 LIMIT 10240;";
				//Cmd= "SELECT * FROM "+ StockCode + "_deal WHERE ID>"+ QString::number(db_index)+ " ORDER BY 1 ;";
			query.exec(Cmd);
			VALID(query);
			nVecSize=query.size();
			if (nVecSize==0)
				break;
			//_ASSERTE(nVecSize>0);

			while (query.next()) {
				db_index   =query.value(0).toInt();
				_ASSERTE(idx<VEC_BLAT_SIZE);
				SDeal& Deal=vecDeal[idx];
				Deal.Parse(query);
				//if (LastDate!=Deal.nDate){
				//	vecDays<<idx;
				//	LastDate=Deal.nDate;
					//printf("\r%d%%",idx*100/Rows);
				//}
				idx++;
			}
		}
		// последнюю сделку продлируем на следующий день
		SDeal Deal;
		vecDeal[idx]=Deal;
		
		//vecDateRef<<idx;
		//printf("\n");
		size=vecDeal.size();
		return size;
	}
	void ParseDate(){
		vecDateRef.resize(0);
		int LastDate=0;
		int i=-1;
		for (i=1; i<vecDeal.size();i++){
			SDeal& Deal=vecDeal[i];
			if (LastDate<Deal.nDate){
				vecDateRef<<i;
				LastDate=Deal.nDate;
			}
		}
		i--;	// вернули указатель на последнюю имеющуся сделку
		vecDateRef<<i;
	}
	void ParseTime(int timeKey){
		QVector<int> vecTimeRef;
		vecTimeRef.resize(vecDateRef.size());
		for(int day=0; day<vecDateRef.size()-1; day++){
			for(int idx=vecDateRef[day]; idx<vecDateRef[day+1]; idx++){
				SDeal &Deal=vecDeal[idx];
				if (timeKey<=Deal.nTime){
					vecTimeRef[day]=idx;
					break;
				}
			}
		}
		mapTimeRef[timeKey]=vecTimeRef;
	}
	// ищет сделку Bid начиная с позиции idx+1 и со след. секунды 
	SDeal* FindBidAfter(int idx){
		int time=vecDeal[idx].nTime;
		int date=vecDeal[idx].nDate;
		idx++;
		while (idx<vecDeal.size() && (vecDeal[idx].IsDemand() || vecDeal[idx].nTime==time) ){
			idx++;
		}
		if (idx==vecDeal.size())
			return 0;
		if (vecDeal[idx].nDate!=date)
			return 0;
		return &vecDeal[idx];
	}
	// ищет сделку Ask начиная с позиции idx+1 и со след. секунды 
	SDeal* FindAskAfter(int idx){
		int time=vecDeal[idx].nTime;
		int date=vecDeal[idx].nDate;
		idx++;
		while (idx<vecDeal.size() &&(vecDeal[idx].IsSupply() || vecDeal[idx].nTime==time) ){
			idx++;
		}
		if (idx==vecDeal.size())
			return 0;
		if (vecDeal[idx].nDate!=date)
			return 0;
		return &vecDeal[idx];
	}
	// ищет сделку Bid начиная с позиции idx включительно
	SDeal* GetNextBid(int idx){
		int date=vecDeal[idx].nDate;
		while (vecDeal[idx].IsDemand()){
			idx++;
			if (idx==vecDeal.size())
				return 0;  
		}
		if (vecDeal[idx].nDate!=date)
			return 0;
		return &vecDeal[idx];
	}
	SDeal* GetNextAsk(int idx){
		int date=vecDeal[idx].nDate;
		while (vecDeal[idx].IsSupply()){
			idx++;
			if (idx==vecDeal.size())
				return 0;  
		}
		if (vecDeal[idx].nDate!=date)
			return 0;
		return &vecDeal[idx];
	}

};

class CHisto{
private:
	double InvMaxMinSize;
	int nSize;	
public:
	QVector<int> vecCount;
	double Min;
	double Max;
	
	void Init(int size, double min, double max){
		vecCount.resize(size);
		InvMaxMinSize=(size-1)/(max-min);
		nSize=size;
		Max=max;
		Min=min;
		vecCount.fill(0);
	}

	void AddValue(double val){
		int idx=(val-Min)*InvMaxMinSize;
		if (idx>=0 && idx<nSize)
			vecCount[idx]++;
	}
	double Idx2Value(int idx){
		return Min+(Max-Min)*idx/(nSize-1);
	}
	int operator [](int i){
		if (i<0 || i>=nSize)
			return 0;
		else 
			return vecCount[i];
	}
	void TubeRuler(){
		for(int i=0; i<nSize; i++){
			printf("%.3f\t",Idx2Value(i)*100);
		}
		printf("%%\n");
	}
	void Tube(){
		for(int i=0; i<nSize; i++){
			printf("%d\t",vecCount[i]);
		}
		printf("\n");
	}
};

class CMultiHisto{
private:
	CHisto nulHisto;
	int nSize;
public:
	QVector<CHisto> vecHisto;
	void Init(int nHisto, int size,double min, double max){
		nSize=nHisto;
		vecHisto.resize(nHisto);
		for(int i=0; i<nHisto; i++){
			vecHisto[i].Init(size,min,max);
		}
		nulHisto.Init(size,min,max);
	}
	CHisto& operator [](int i){
		if (i<0 || i>=nSize)
			return nulHisto;
		else 
			return vecHisto[i];
	}
	void Tube(){
		vecHisto[0].TubeRuler();
		CHisto SumHisto;
		int CountSize=nulHisto.vecCount.size();
		SumHisto.Init(CountSize,0,0);
		for(int i=0; i<vecHisto.size(); i++){
			vecHisto[i].Tube();
			
			for(int j=0; j<CountSize; j++)
				SumHisto.vecCount[j]+=vecHisto[i].vecCount[j];
		}
		for(int i=0; i<CountSize; i++){
			printf(".\t");
		}
		printf("\n");
		SumHisto.Tube();

		

	}
};
class CDealStat{
public:

	void PeakTimeLineDistribution(CDealContainer& DealContainer,CHisto& Histo, int nTime0, int nTime1){
		QVector<SDeal>& vecDeal=DealContainer.vecDeal;
		for(int day=0; day<DealContainer.vecDateRef.size()-1; day++){
			int CurrDayIdx=DealContainer.vecDateRef[day];
			int NextDayIdx=DealContainer.vecDateRef[day+1];
			for(int idx=CurrDayIdx; idx<NextDayIdx; idx++){
				SDeal& Deal=vecDeal[idx];
				if (Deal.nTime<nTime0)
					continue;
				else if (Deal.nTime>nTime1)
					break;
				if (Deal.nType&IN_SECOND){
					double Price0=vecDeal[idx].Price;
					int nTime=Deal.nTime;
					while (vecDeal[idx].nTime==nTime){
						idx++;
					}
					idx--;
					double Price1=vecDeal[idx].Price;
					double Peak=ABS(Price1-Price0);
					Histo.AddValue(Peak/Price0);
				}
			}
		}

	}
	void Peak3TimeLineDistribution(CDealContainer& DealContainer,CMultiHisto& Histo){

		//for(int i=0; i<Histo.vecHisto.size(); i++){}
		PeakTimeLineDistribution(DealContainer,Histo[0], 103000, 104500);
		PeakTimeLineDistribution(DealContainer,Histo[1], 104500, 183000);
		PeakTimeLineDistribution(DealContainer,Histo[2], 183000, 184500);
	}
	void Peak15MinDistribution(CDealContainer& DealContainer,CMultiHisto& Histo){

		Histo.Init(33,9,0.001,0.01);
		PeakTimeLineDistribution(DealContainer,Histo[0], 103000, 104500);
		PeakTimeLineDistribution(DealContainer,Histo[1], 104500, 110000);
		
		PeakTimeLineDistribution(DealContainer,Histo[2], 110000, 111500);
		PeakTimeLineDistribution(DealContainer,Histo[3], 111500, 113000);
		PeakTimeLineDistribution(DealContainer,Histo[4], 113000, 114500);
		PeakTimeLineDistribution(DealContainer,Histo[5], 114500, 120000);
		
		PeakTimeLineDistribution(DealContainer,Histo[6], 120000, 121500);
		PeakTimeLineDistribution(DealContainer,Histo[7], 121500, 123000);
		PeakTimeLineDistribution(DealContainer,Histo[8], 123000, 124500);
		PeakTimeLineDistribution(DealContainer,Histo[9], 124500, 130000);

		PeakTimeLineDistribution(DealContainer,Histo[10], 130000, 131500);
		PeakTimeLineDistribution(DealContainer,Histo[11], 131500, 133000);
		PeakTimeLineDistribution(DealContainer,Histo[12], 133000, 134500);
		PeakTimeLineDistribution(DealContainer,Histo[13], 134500, 140000);

		PeakTimeLineDistribution(DealContainer,Histo[14], 140000, 141500);
		PeakTimeLineDistribution(DealContainer,Histo[15], 141500, 143000);
		PeakTimeLineDistribution(DealContainer,Histo[16], 143000, 144500);
		PeakTimeLineDistribution(DealContainer,Histo[17], 144500, 150000);

		PeakTimeLineDistribution(DealContainer,Histo[18], 150000, 151500);
		PeakTimeLineDistribution(DealContainer,Histo[19], 151500, 153000);
		PeakTimeLineDistribution(DealContainer,Histo[20], 153000, 154500);
		PeakTimeLineDistribution(DealContainer,Histo[21], 154500, 160000);

		PeakTimeLineDistribution(DealContainer,Histo[22], 160000, 161500);
		PeakTimeLineDistribution(DealContainer,Histo[23], 161500, 163000);
		PeakTimeLineDistribution(DealContainer,Histo[24], 163000, 164500);
		PeakTimeLineDistribution(DealContainer,Histo[25], 164500, 170000);

		PeakTimeLineDistribution(DealContainer,Histo[26], 170000, 171500);
		PeakTimeLineDistribution(DealContainer,Histo[27], 171500, 173000);
		PeakTimeLineDistribution(DealContainer,Histo[28], 173000, 174500);
		PeakTimeLineDistribution(DealContainer,Histo[29], 174500, 180000);

		PeakTimeLineDistribution(DealContainer,Histo[30], 180000, 181500);
		PeakTimeLineDistribution(DealContainer,Histo[31], 181500, 183000);
		PeakTimeLineDistribution(DealContainer,Histo[32], 183000, 184500);
	}
	
	double BuyAndSell(CDealContainer& DealContainer,double Peak,int nTime0, int nTime1){
		QVector<SDeal>& vecDeal=DealContainer.vecDeal;
		int count=0;
		double Profit=0;
		for(int day=0; day<DealContainer.vecDateRef.size()-1; day++){
			int CurrDayIdx=DealContainer.vecDateRef[day];
			int NextDayIdx=DealContainer.vecDateRef[day+1];
			for(int idx=CurrDayIdx; idx<NextDayIdx; idx++){
				SDeal& Deal=vecDeal[idx];
				if (Deal.nTime<nTime0)
					continue;
				else if (Deal.nTime>nTime1)
					break;
				if (Deal.nType&IN_SECOND){
					double Price0=vecDeal[idx].Price;
					int nTime=Deal.nTime;
					while (vecDeal[idx].nTime==nTime){
						idx++;
					}
					idx--;
					double Price1=vecDeal[idx].Price;
					double delta=(Price1-Price0)/Price0;
					if (delta>Peak){
						int IDX=idx+1;
						while(IDX<NextDayIdx){
							if (vecDeal[IDX].IsDemand()){
								Price1=vecDeal[IDX].Price;
								Profit+=(Price0*(1+Peak)-Price1)/Price0;
								Profit-=0.0008;
								count++;
								break;
							}
							IDX++;
						}
					}
					else if (delta < -Peak){
						int IDX=idx+1;
						while(IDX<NextDayIdx){
							if (vecDeal[IDX].IsSupply()){
								Price1=vecDeal[IDX].Price;
								Profit+=(Price1-Price0*(1-Peak))/Price0;
								Profit-=0.0008;
								count++;
								break;
							}
							IDX++;
						}
					}
				}
			}
		}
		return Profit*365.0/DealContainer.vecDateRef.size();
	}
	// Эта функция продает и покупает на свечках и тут же компенсирует баланс по первой обратной сделке
	// 
#define URGENT_SELL 1
#define URGENT_BUY  2 
#define NORMAL_TRADE 3

	double RealBuyAndSell(CDealContainer& DealContainer,double Peak,int nTime0, int nTime1){
		QVector<SDeal>& vecDeal=DealContainer.vecDeal;
		//int count=0;
		//double Profit=0;
		
		//int nLastSupplyIdx=0;
		//int nLastDemandIdx=0;

		double MySupplyPrice=vecDeal[0].Price*(1+Peak);
		double MyDemandPrice=vecDeal[0].Price*(1-Peak);
		//int		MySupplyVol=1;
		//int		MyDemandVol=1;
		double Tax=0;
		double Cash=0;
		int		Volume=0;
		int		state=NORMAL_TRADE;
		//double UrgentPeak;
		//double NormalPeak;
		//double SupplyPeak;
		//double DemandPeak;
		for(int day=0; day<DealContainer.vecDateRef.size()-1; day++){
			int CurrDayIdx=DealContainer.vecDateRef[day];
			int NextDayIdx=DealContainer.vecDateRef[day+1];
			// Движемся по дням
			for(int idx=CurrDayIdx; idx<NextDayIdx; idx++){
				SDeal& Deal=vecDeal[idx];
				// Пропускаем нерабочие интервалы
				if (Deal.nTime<nTime0){
					if (Deal.IsSupply())
						MySupplyPrice=vecDeal[idx].Price*(1+Peak);
					else 
						MyDemandPrice=vecDeal[idx].Price*(1-Peak);
					continue;
				}
				else if (Deal.nTime>nTime1)
					break;

	
				if(Deal.IsSupply()){
				
					if (Deal.nType&IN_SECOND){
						// Ищем последнюю внутрисекундную сделку 
						idx++;
						while (vecDeal[idx].nTime==Deal.nTime && vecDeal[idx].IsSupply()){
							idx++;
						}			
						idx--;
					}		
			
					if (state==NORMAL_TRADE){
						// если мы продали
						if (vecDeal[idx].Price>=MySupplyPrice){
							Cash+=MySupplyPrice;
							Tax +=MySupplyPrice*0.0004;
							state=URGENT_BUY;
							Volume=-1;
							// Ищем будущую Demand сделку
							for(int i=idx+1; i<NextDayIdx; i++){
								if (vecDeal[i].nTime==Deal.nTime)
									continue;
								if (vecDeal[i].IsDemand()){
									MyDemandPrice=vecDeal[i].Price;
									break;
								}
							}
						}
						MySupplyPrice=vecDeal[idx].Price*(1+Peak);
					}
					else if (state==URGENT_SELL){
						if (vecDeal[idx].Price>=MySupplyPrice){
							Cash+=MySupplyPrice;
							Tax +=MySupplyPrice*0.0004;
							state=NORMAL_TRADE;
							Volume=0;
							MySupplyPrice=vecDeal[idx].Price*(1+Peak);
						}
						// если не продали
						else {
							// держим срочную цену на продажу							
						}
					}
					else { // URGENT_BUY
						MySupplyPrice=vecDeal[idx].Price*(1+Peak);
					}
				}
				else  // Is.Demand()
				{
					if (Deal.nType&IN_SECOND){
						// Ищем последнюю внутрисекундную сделку 
						idx++;
						while (vecDeal[idx].nTime==Deal.nTime && vecDeal[idx].IsDemand()){
							idx++;
						}			
						idx--;
					}					


					if (state==NORMAL_TRADE){
						// если мы купили
						if (vecDeal[idx].Price<=MyDemandPrice){
							Cash-=MyDemandPrice;
							Tax +=MyDemandPrice*0.0004;
							state=URGENT_SELL;
							Volume=1;
							// Ищем будущую Supply сделку
							for(int i=idx+1; i<NextDayIdx; i++){
								if (vecDeal[i].nTime==Deal.nTime)
									continue;
								if (vecDeal[i].IsSupply()){
									MySupplyPrice=vecDeal[i].Price;
									break;
								}
							}
						}
						MyDemandPrice=vecDeal[idx].Price*(1-Peak);
					}
					else if (state==URGENT_BUY){
						if (vecDeal[idx].Price<=MyDemandPrice){
							Cash-=MyDemandPrice;
							Tax +=MyDemandPrice*0.0004;
							state=NORMAL_TRADE;
							Volume=0;
							MyDemandPrice=vecDeal[idx].Price*(1-Peak);
						}
						// если не продали
						else {

						}
					}
					else { // URGEN_SELL
						MyDemandPrice=vecDeal[idx].Price*(1-Peak);
					}
				}
			}
			if (Volume==1){
				Cash+=(MyDemandPrice+MySupplyPrice)/2;
				Tax +=(MyDemandPrice+MySupplyPrice)/2*0.0004;
			} 
			else if (Volume==-1){
				Cash-=(MyDemandPrice+MySupplyPrice)/2;
				Tax +=(MyDemandPrice+MySupplyPrice)/2*0.0004;
			}
		}
		
		return (Cash-Tax)/(MyDemandPrice+MySupplyPrice)*365.0/DealContainer.vecDateRef.size();
	}

	double CommonBuyAndSell(CDealContainer& DealContainer,double Peak,int nTime0, int nTime1){
		QVector<SDeal>& vecDeal=DealContainer.vecDeal;
		//int count=0;
		//double Profit=0;

		//int nLastSupplyIdx=0;
		//int nLastDemandIdx=0;

		double MySupplyPrice=vecDeal[0].Price*(1+Peak);
		double MyDemandPrice=vecDeal[0].Price*(1-Peak);
		//int		MySupplyVol=1;
		//int		MyDemandVol=1;
		double Tax=0;
		double Cash=0;
		//double UrgentPeak;
		//double NormalPeak;
		//double SupplyPeak;
		//double DemandPeak;
		int		Volume=0;
		int		state=URGENT_BUY;

		for(int day=0; day<DealContainer.vecDateRef.size()-1; day++){


			int CurrDayIdx=DealContainer.vecDateRef[day];
			int NextDayIdx=DealContainer.vecDateRef[day+1];
			// Движемся по дням
			for(int idx=CurrDayIdx; idx<NextDayIdx; idx++){
				SDeal& Deal=vecDeal[idx];
				// Пропускаем нерабочие интервалы
				if (Deal.nTime<nTime0){
					if (Deal.IsSupply())
						MySupplyPrice=vecDeal[idx].Price*(1+Peak);
					else 
						MyDemandPrice=vecDeal[idx].Price*(1-Peak);
					continue;
				}
				else if (Deal.nTime>nTime1)
					break;


				if(Deal.IsSupply()){

					if (Deal.nType&IN_SECOND){
						// Ищем последнюю внутрисекундную сделку 
						idx++;
						while (vecDeal[idx].nTime==Deal.nTime && vecDeal[idx].IsSupply()){
							idx++;
						}			
						idx--;
					}		

					if (state==NORMAL_TRADE){
						_ASSERTE(Volume==1);
						// если мы продали
						if (vecDeal[idx].Price>=MySupplyPrice){
							Cash+=MySupplyPrice;
							Tax +=MySupplyPrice*0.0004;
							state=URGENT_BUY;
							Volume=0;
							// Ищем будущую Demand сделку
							for(int i=idx+1; i<NextDayIdx; i++){
								if (vecDeal[i].nTime==Deal.nTime)
									continue;
								if (vecDeal[i].IsDemand()){
									MyDemandPrice=vecDeal[i].Price;
									break;
								}
							}
						}
						MySupplyPrice=vecDeal[idx].Price*(1+Peak);
					}
					else if (state==URGENT_SELL){
						_ASSERTE(Volume==2);
						if (vecDeal[idx].Price>=MySupplyPrice){
							Cash+=MySupplyPrice;
							Tax +=MySupplyPrice*0.0004;
							state=NORMAL_TRADE;
							Volume=1;
							MySupplyPrice=vecDeal[idx].Price*(1+Peak);
						}
						// если не продали
						else {
							// держим срочную цену на продажу							
						}
					}
					else { // URGENT_BUY
						MySupplyPrice=vecDeal[idx].Price*(1+Peak);
					}
				}
				else  // Is.Demand()
				{
					if (Deal.nType&IN_SECOND){
						// Ищем последнюю внутрисекундную сделку 
						idx++;
						while (vecDeal[idx].nTime==Deal.nTime && vecDeal[idx].IsDemand()){
							idx++;
						}			
						idx--;
					}					


					if (state==NORMAL_TRADE){
						_ASSERTE(Volume==1);
						// если мы купили
						if (vecDeal[idx].Price<=MyDemandPrice){
							Cash-=MyDemandPrice;
							Tax +=MyDemandPrice*0.0004;
							state=URGENT_SELL;
							Volume=2;
							// Ищем будущую Supply сделку
							for(int i=idx+1; i<NextDayIdx; i++){
								if (vecDeal[i].nTime==Deal.nTime)
									continue;
								if (vecDeal[i].IsSupply()){
									MySupplyPrice=vecDeal[i].Price;
									break;
								}
							}
						}
						MyDemandPrice=vecDeal[idx].Price*(1-Peak);
					}
					else if (state==URGENT_BUY){
						_ASSERTE(Volume==0);
						if (vecDeal[idx].Price<=MyDemandPrice){
							Cash-=MyDemandPrice;
							Tax +=MyDemandPrice*0.0004;
							state=NORMAL_TRADE;
							Volume=1;
							MyDemandPrice=vecDeal[idx].Price*(1-Peak);
						}
						// если не продали
						else {

						}
					}
					else { // URGEN_SELL
						MyDemandPrice=vecDeal[idx].Price*(1-Peak);
					}
				}
			}
			if (Volume>0){
				Cash+=Volume*(MyDemandPrice+MySupplyPrice)/2;
				Tax +=Volume*(MyDemandPrice+MySupplyPrice)/2*0.0004;
				Volume=0;
				state=URGENT_BUY;
			} 
			//else if (Volume==-1){
			//	Cash-=(MyDemandPrice+MySupplyPrice)/2;
			//	Tax +=(MyDemandPrice+MySupplyPrice)/2*0.0004;
			//}
		}

		return (Cash-Tax)/(MyDemandPrice+MySupplyPrice)*365.0/DealContainer.vecDateRef.size();
	}


};
