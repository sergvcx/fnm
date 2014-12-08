#include "../common/Main.h"
#include "MainWindow.h"

#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
QScrollArea* extScrollArea;




void  SplitBuySell(QQueue<SDeal>& qDeals, QQueue<SDeal>& qBuyDeals,QQueue<SDeal>& qSellDeals ){
	SDeal Deal;
	while (!qDeals.isEmpty()){
		Deal=qDeals.dequeue();	
		if (Deal.IsSell()){
			qSellDeals<<Deal;
		}
		else 
			qBuyDeals<<Deal;
	}
}
void FirstSecond(QQueue<SDeal>& qDeals, QQueue<SDeal>& qInSecDeals ){
	SDeal Deal;
	if (qDeals.isEmpty())
		return;
	int nTime=qDeals.first().nTime;
	while (!qDeals.isEmpty() && nTime==qDeals.first().nTime){
		Deal=qDeals.dequeue();
		qInSecDeals.enqueue(Deal);
	}
}


struct SOrder{
	float	Price;			// цена
	int		Volume;		// колво акций(лотов)
	int		Time;			// время подачи заявки (по компьютеру)
	int		Date;			// дата подачи заявки (по компьютеру)
	int		Status;		// стаус заявки
	//int		nTransID;		// идентификатор заявки
	//QQueue<SDeal> queDeal;	// сделки которые совершились по данной заявке
};

class C_TradeMaster{
public:
	
	int		Stocks;
	int		LockedStocks;
	float 	Cash;
	float 	LockedCash;
	float 	Commission;
	float 	minDelta;
	float 	maxDelta;
	int     GapVolume;
	int		GapPrice;	
	int   	numTraps;

	SDeal	LastSellDeal;
	SDeal	LastBuyDeal;

	QList<SOrder>  listMySellOrder;
	QList<SOrder>  listMyBuyOrder;

	C_TradeMaster(){
		Cash=0;
		Stocks=0;
		LockedCash=0;
		LockedStocks=0;
		minDelta=0.2/100;
		maxDelta=1.0/100;
		Commission=0;//0.01/100;
	}
	void Init(float StartPrice, float MinDelta, float MaxDelta, int nTraps, float fCash, int nStocks){
		numTraps = nTraps;
		minDelta = MinDelta;
		maxDelta = MaxDelta;
		Cash     = fCash;
		Stocks   = nStocks;
		GapVolume= (Stocks+Cash/StartPrice)/numTraps;
		GapPrice = StartPrice*(maxDelta-minDelta)/numTraps;
	}
	//--------------- complete ---------------------

	void CompleteFirstBuyOrder(){
		SOrder& Order=listMyBuyOrder.first();
		LockedCash-=Order.Volume*Order.Price;
		Cash      -=Order.Volume*Order.Price*Commission;
		Stocks    +=Order.Volume;
		//printf("B %d %f Cash=%f LCash=%f Stocks=%d LStocks=%d \n", Order.Volume, Order.Price, Cash, LockedCash, Stocks, LockedStocks);
		listMyBuyOrder.removeFirst();
		
	}
	
	void CompleteFirstSellOrder(){
		SOrder& Order=listMySellOrder.first();
		LockedStocks-=Order.Volume;
		Cash        +=Order.Volume*Order.Price;
		Cash        -=Order.Volume*Order.Price*Commission;
		//printf("S %d %f Cash=%f LCash=%f Stocks=%d LStocks=%d \n", Order.Volume, Order.Price, Cash, LockedCash, Stocks, LockedStocks);
		listMySellOrder.removeFirst();
		
	}
	//--------------- cancle ---------------------
	void RemoveFirstBuyOrder(){
		SOrder& Order=listMyBuyOrder.first();
		LockedCash  -=Order.Price*Order.Volume;
		Cash        +=Order.Price*Order.Volume;
		listMyBuyOrder.removeFirst();
	}
	
	void RemoveLastBuyOrder(){
		SOrder& Order=listMyBuyOrder.last();
		LockedCash  -=Order.Price*Order.Volume;
		Cash        +=Order.Price*Order.Volume;
		listMyBuyOrder.removeLast();
	}
	
	void RemoveFirstSellOrder(){
		SOrder& Order=listMySellOrder.first();
		LockedStocks -=Order.Volume;
		Stocks       +=Order.Volume;
		listMySellOrder.removeFirst();
	}

	void RemoveLastSellOrder(){
		SOrder& Order=listMySellOrder.last();
		LockedStocks -=Order.Volume;
		Stocks       +=Order.Volume;
		listMySellOrder.removeLast();
	}
	//--------------- insert ---------------------
	bool InsertFirstBuyOrder(int Volume, float Price){
		SOrder Order;
		Order.Price = Price;
		Order.Volume= MIN(Volume,Cash/Price);
		if (Order.Volume){
			LockedCash += Order.Price*Order.Volume;
			Cash       -= Order.Price*Order.Volume;
			listMyBuyOrder.insert(0,Order);
			return 0;
		}
		return 1;
	}
	
	bool InsertLastBuyOrder(int Volume, float Price){
		SOrder Order;
		Order.Price = Price;
		Order.Volume= MIN(Volume,Cash/Price);
		if (Order.Volume){
			LockedCash  +=Order.Price*Order.Volume;
			Cash        -=Order.Price*Order.Volume;
			listMyBuyOrder.append(Order);
			return 0;
		}
		return 1;
	}
	
	bool InsertFirstSellOrder(int Volume, float Price){
		SOrder Order;
		Order.Price = Price;
		Order.Volume= MIN(Stocks,Volume);
		if (Order.Volume){
			LockedStocks +=Order.Volume;
			Stocks       -=Order.Volume;
			listMySellOrder.insert(0,Order);
			return 0;
		}
		return 1;
	}

	bool InsertLastSellOrder(int Volume, float Price){
		SOrder Order;
		Order.Price = Price;
		Order.Volume= MIN(Stocks,Volume);
		if (Order.Volume){
			LockedStocks +=Order.Volume;
			Stocks       -=Order.Volume;
			listMySellOrder.append(Order);
			return 0;
		}
		return 1;
	}
	//-------------------------------------




	void  Parse(QQueue<SDeal>& qDeals){
		QQueue<SDeal>	qSecondDeals;
		QQueue<SDeal>	qBuyDeals;
		QQueue<SDeal>	qSellDeals;
		
		while (!qDeals.isEmpty()){
			FirstSecond(qDeals,qSecondDeals);
			SplitBuySell(qSecondDeals,qBuyDeals,qSellDeals);
			if (!qBuyDeals.isEmpty()){
				LastBuyDeal=qBuyDeals.last();
				while (!listMyBuyOrder.isEmpty() && LastBuyDeal.Price<=listMyBuyOrder.first().Price){
					CompleteFirstBuyOrder();
				}
				qBuyDeals.clear();
			}
			if (!qSellDeals.isEmpty()){
				LastSellDeal=qSellDeals.last();
				while (!listMySellOrder.isEmpty() && LastSellDeal.Price>=listMySellOrder.first().Price){
					CompleteFirstSellOrder();
				}
				qSellDeals.clear();
			}
			MakeOrders();
		}
	}
	
	void MakeOrders(){
		//---------------- remove orders ----------------
		while (!listMyBuyOrder.isEmpty() && LastBuyDeal.Price*(1-minDelta)<listMyBuyOrder.first().Price){
			RemoveFirstBuyOrder();
		}
		while (!listMyBuyOrder.isEmpty() && LastBuyDeal.Price*(1-maxDelta)>listMyBuyOrder.last().Price){
			RemoveLastBuyOrder();
		}

		while (!listMySellOrder.isEmpty() && LastSellDeal.Price*(1+minDelta)>listMySellOrder.first().Price){
			RemoveFirstSellOrder();
		}
		while (!listMySellOrder.isEmpty() && LastSellDeal.Price*(1+maxDelta)<listMySellOrder.last().Price){
			RemoveLastSellOrder();
		}

		//------------------ insert order----------------
		if (listMyBuyOrder.isEmpty())
			InsertFirstBuyOrder(GapVolume,    LastBuyDeal.Price*(1-minDelta)-GapPrice/2);
		else {
			while (LastBuyDeal.Price*(1-minDelta)>listMyBuyOrder.first().Price+GapPrice)
				if (InsertFirstBuyOrder(GapVolume,    listMyBuyOrder.first().Price+GapPrice))
					break;

			while (LastBuyDeal.Price*(1-maxDelta)<listMyBuyOrder.last().Price-GapPrice)
				if (InsertLastBuyOrder (GapVolume,    listMyBuyOrder.last().Price-GapPrice))
					break;
		}

		if (listMySellOrder.isEmpty())
			InsertFirstSellOrder(GapVolume,    LastSellDeal.Price*(1+minDelta)+GapPrice/2);
		else {
			while (LastSellDeal.Price*(1+minDelta)<listMySellOrder.first().Price-GapPrice)
				if (InsertFirstSellOrder(GapVolume,    listMySellOrder.first().Price-GapPrice))
					break;
			while (LastSellDeal.Price*(1+maxDelta)>listMySellOrder.last().Price+GapPrice )
				if (InsertLastSellOrder(GapVolume,     listMySellOrder.last().Price+GapPrice))
					break;
		}

	}
	void Close(){
		listMyBuyOrder.clear();
		Cash+=LockedCash;
		LockedCash=0;
	
		listMySellOrder.clear();
		Cash+=(Stocks+LockedStocks)*LastSellDeal.Price;
		Cash-=(Stocks+LockedStocks)*LastSellDeal.Price*Commission;
		LockedStocks=0;
	
	}
};



class C_DealStreamer{
public:
	QVector<SDeal>		vecDeal; 
	QQueue<SDeal>		qDeals; 
	
	void Load(QVector<SDeal>& newDeals){
		vecDeal << newDeals;
	}
	C_TradeMaster *pTradeMaster; 

	void Run(){
		QQueue<SDeal> qBuyDeals;
		QQueue<SDeal> qSellDeals;
		int nTime=0;
		int nLastSendTime=0;
		for(int i=0; i<vecDeal.size(); i++){
			SDeal& Deal=vecDeal[i];
			pTradeMaster->Parse(qDeals);
		}
	}
};

MainWindow::MainWindow()
{

	DateTime0.setDate(QDate(2000,1,1));
	DateTime1.setDate(QDateTime::currentDateTime().date());

	//db_trading = QSqlDatabase::addDatabase("QMYSQL","trading1");
	//db_trading = QSqlDatabase::addDatabase("QMYSQL","trading240811");
	db_trading = QSqlDatabase::addDatabase("QMYSQL","trading");
	db_trading.setHostName("");
	db_trading.setDatabaseName("trading");
	db_trading.setUserName("root");
	db_trading.setPassword("root");
	if (!db_trading.open()) {
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}

	ReadProfile();
	
	qScrollArea = new QScrollArea;
	extScrollArea=qScrollArea ;
	qGraphField = new QGraphField(qScrollArea);
	qGraphField->mainWin=this;
	
	
	//qScrollArea->setBackgroundRole(QPalette::Dark);
	
	qScrollArea->setWidget(qGraphField);
	
	setCentralWidget(qScrollArea);
	
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();

	//qStockComboBox->setCurrentIndex(66);
	//SelectStock(qStockComboBox->currentIndex());
	resize(900,600);
	//Draw();

}

void MainWindow::createActions(){
	newAction = new QAction(tr("&New"), this);
	newAction->setIcon(QIcon(":/images/new.png"));
	newAction->setShortcut(QKeySequence::New);
	newAction->setStatusTip(tr("Create a new spreadsheet file"));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	actStartAllDeals	= new QAction(tr("&Start all deals"), this);
	actStartAllDeals->setStatusTip(tr("Start all deals"));
	actStopAllDeals		= new QAction(tr("&Stop all deals"), this);
	actStopAllDeals->setStatusTip(tr("Stop All Deals"));
	actEmptyAllDeals	= new QAction(tr("&Empty mylefco.AllDeals table"), this);
	actCheckTables		= new QAction(tr("&Check all Tables"), this);
	actDropTradings		= new QAction(tr("&DropTradings"), this);
	actStopAllDeals->setStatusTip(tr("Drop all trading.tables except _portfolio"));
	actCreatePortfolio	= new QAction(tr("&CreatePortfolio"), this);
	actHisto			= new QAction(tr("&Histogram"), this);
	connect(actHisto, SIGNAL(triggered()), qGraphField, SLOT(Histo()));

	actAbout = new QAction(tr("&about"), this);
	actAbout->setStatusTip(tr("Create a new spreadsheet file"));
	connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
	
	actDraw = new QAction(tr("Draw"),this);
	connect(actDraw, SIGNAL(triggered()), this, SLOT(Draw()));

	actZoomInX = new QAction("<X>", this);
	connect(actZoomInX, SIGNAL(triggered()), qGraphField, SLOT(ZoomInX()));

	actZoomOutX = new QAction(">X<", this);
	connect(actZoomOutX, SIGNAL(triggered()), qGraphField, SLOT(ZoomOutX()));

	actZoomInY = new QAction("<Y>", this);
	connect(actZoomInY, SIGNAL(triggered()), qGraphField, SLOT(ZoomInY()));

	actZoomOutY = new QAction(">Y<", this);
	connect(actZoomOutY, SIGNAL(triggered()), qGraphField, SLOT(ZoomOutY()));

	actViewOfferFlag = new QAction("View offer legend",this);
	actViewOfferFlag->setCheckable(true);
	connect(actViewOfferFlag, SIGNAL(triggered()), qGraphField, SLOT(ViewOffer()));
	
	actViewTradeFlag = new QAction("View trade legend",this);
	actViewTradeFlag->setCheckable(true);
	connect(actViewTradeFlag, SIGNAL(triggered()), qGraphField, SLOT(ViewTrade()));

	actViewRequestFlag = new QAction("View request legend",this);
	actViewRequestFlag->setCheckable(true);
	actViewRequestFlag->setChecked(qGraphField->bViewRequestFlag);
	connect(actViewRequestFlag, SIGNAL(triggered()), this, SLOT(ViewRequest()));


	

	actGoToStart = new QAction("<<", this);
	connect(actGoToStart, SIGNAL(triggered()),	qGraphField, SLOT(GoToStart()));

	actPrevDay  = new QAction("<", this);
	connect(actPrevDay, SIGNAL(triggered()),	qGraphField, SLOT(PrevDay()));

	actNextDay= new QAction(">", this);
	connect(actNextDay, SIGNAL(triggered()),	qGraphField, SLOT(NextDay()));

	actGoToEnd= new QAction(">>", this);
	connect(actGoToEnd, SIGNAL(triggered()),	qGraphField, SLOT(GoToEnd()));


}
void MainWindow::createMenus(){
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAction);
	fileMenu->addSeparator();
	//fileMenu->addAction(exitAction);
	fileMenu = menuBar()->addMenu(tr("&Tool"));
	fileMenu->addAction(actStartAllDeals);
	fileMenu->addAction(actStopAllDeals);
	fileMenu->addSeparator();
	fileMenu->addAction(actCheckTables		);
	fileMenu->addAction(actEmptyAllDeals	);
	fileMenu->addAction(actDropTradings		);
	fileMenu->addAction(actCreatePortfolio	);
	fileMenu = menuBar()->addMenu(tr("&View"));
	fileMenu->addAction(actViewOfferFlag	);
	fileMenu->addAction(actViewTradeFlag	);
	fileMenu->addAction(actViewRequestFlag	);
	fileMenu->addAction(actHisto);

	fileMenu = menuBar()->addMenu(tr("&Help"));
	fileMenu->addAction(actAbout);


}
void MainWindow::createToolBars(){
	
	qStockComboBox	= new QComboBox;
	qDateTimeEdit0	= new QDateTimeEdit(DateTime0);
	qDateTimeEdit1	= new QDateTimeEdit(DateTime1);	
	connect(qDateTimeEdit0, SIGNAL(dateTimeChanged(QDateTime)),this, SLOT(SetDateTime0(QDateTime)));
	connect(qDateTimeEdit1, SIGNAL(dateTimeChanged(QDateTime)),this, SLOT(SetDateTime1(QDateTime)));

	qDateTimeEdit0->setMinimumDate(QDate(2010,01,01));
	qDateTimeEdit0->setMaximumDate(QDateTime::currentDateTime().date());
	qDateTimeEdit1->setMinimumDate(QDate(2010,01,01));
	qDateTimeEdit1->setMaximumDate(QDateTime::currentDateTime().date());
	qDateTimeEdit0->setCalendarPopup(1);
	qDateTimeEdit1->setCalendarPopup(1);

	QStringList qStockList;

//	char name[100];
	Portfolios.empty();
	for(int i=0; i<Portfolios.size();i++)
	{
		QString qItem=Portfolios[i]->StockCode + "	:" + RUS(Portfolios[i]->StockName);
		qStockList<<qItem;
	}
	qStockComboBox->addItems(qStockList);
	qStockComboBox->setCurrentIndex(0);
	//qStockComboBox->SelectStock(0);


	connect(qStockComboBox,  SIGNAL(activated(int)),this, SLOT(SelectStock(int)));


	toolBar = addToolBar(tr("&Main"));
	toolBar->addAction(actStartAllDeals);
	toolBar->addWidget(qStockComboBox);
	toolBar->addWidget(qDateTimeEdit0);
	toolBar->addWidget(qDateTimeEdit1);

	toolBar = addToolBar(tr("&Draw"));
	toolBar->addAction(actDraw);

	toolBar->addAction(actZoomInX);
	toolBar->addAction(actZoomOutX);
	toolBar->addAction(actZoomInY);
	toolBar->addAction(actZoomOutY);
	toolBar->addAction(actGoToStart);
	toolBar->addAction(actPrevDay);
	toolBar->addAction(actNextDay);
	toolBar->addAction(actGoToEnd);

	
}

void MainWindow::createStatusBar()
{
	labelIndex = new QLabel("-------");
	labelIndex->setMinimumWidth(40);//(setIndent(4);
	labelIndex->setAlignment(Qt::AlignRight);

	labelDate = new QLabel("-------");
	labelDate->setMinimumWidth(40);//(setIndent(4);
	labelDate->setAlignment(Qt::AlignRight);
	//labelDate->setMinimumSize(labelDate->sizeHint());
	//labelDate->sesetMinimumSize(8);

	labelTime = new QLabel("-------");
	labelTime->setMinimumWidth(40);//(setIndent(4);
	labelTime->setAlignment(Qt::AlignRight);

	//labelTime->setIndent(4);
	//labelTime->setAlignment(Qt::AlignHCenter);
	
	labelPrice = new QLabel("-------");
	labelPrice->setMinimumWidth(40);//(setIndent(4);
	labelPrice->setAlignment(Qt::AlignLeft);
	//labelPrice->setIndent(4);

	labelVolume = new QLabel("-------");
	labelVolume->setMinimumWidth(40);//(setIndent(4);
	labelVolume->setAlignment(Qt::AlignLeft);
	
	labelPriceMouse= new QLabel("-------");
	labelPriceMouse->setMinimumWidth(40);
	labelPriceMouse->setAlignment(Qt::AlignLeft);
	
	//labelVolume->setIndent(4);

	statusBar()->addWidget(labelIndex);
	statusBar()->addWidget(labelDate);
	statusBar()->addWidget(labelTime);
	statusBar()->addWidget(labelPrice);
	statusBar()->addWidget(labelVolume);
	statusBar()->addWidget(labelPriceMouse);

	//connect(spreadsheet, SIGNAL(currentCellChanged(int, int, int, int)),
	//	this, SLOT(updateStatusBar()));
	//connect(spreadsheet, SIGNAL(modified()),
	//	this, SLOT(spreadsheetModified()));
	updateStatusBar();
}
void MainWindow::updateStatusBar()
{
	//labelDate->setText("asdasd");//spreadsheet->currentLocation());
	//labelTime->setText("12312");//spreadsheet->currentFormula());
}


/*
void MainWindow::closeEvent(QCloseEvent *event){

}*/

void MainWindow::StartAllDeals(){
	//qThreadAllDeals->start();
}



void MainWindow::about()
{
	QMessageBox::about(this, tr("About DealCollector"),
		tr("<h2>Deal Collector 1.0</h2>"
		"build from " __DATE__" " __TIME__ 
		"<p>Copyright &copy; 2008 Software Inc."
		));


	
}


void MainWindow::ReadProfile(){
	QSqlQuery query(db_trading);
	//query.exec("SELECT * FROM _portfolio ORDER BY rows DESC;");
	query.exec("SELECT * FROM _portfolio ;");
	VALID(query);

	Portfolios.empty();
	while (query.next())
	{
		SPortfolio* pPorfolio=new SPortfolio;
		SPortfolio& Porfolio=*pPorfolio;
		Porfolio.Status			= query.value(1).toInt();
		Porfolio.StockCode		= query.value(2).toString();	
		Porfolio.StockName		= query.value(3).toString();	
		Porfolio.LastOfferDate	= query.value(4).toInt();	
		Porfolio.LastOfferTime	= query.value(5).toInt();	
		Porfolio.Volume			= query.value(6).toInt();
		Porfolio.Profit			= query.value(7).toDouble();

		QString qItem=Porfolio.StockCode + "	:" + RUS(Porfolio.StockName);
		Portfolios<< pPorfolio;
	}
}

void MainWindow::SelectStock(int Item)
{
	//int CurrentStockIdx=Item;
	StockCode = Portfolios[Item]->StockCode;
	QString StockName = Portfolios[Item]->StockName;
	QString cmd="SELECT * FROM "+StockCode+"_deal LIMIT 1";
	QSqlQuery query(db_trading);
	QDateTime MinimumDateTime;
	QDateTime MaximumDateTime;
	query.exec(cmd);
	if (query.next()){
		int nDate  =query.value(1).toInt();
		int nTime  =query.value(2).toInt();
		MinimumDateTime.setDate(QDate(nDate/10000,(nDate%10000)/100,nDate%100));
		MinimumDateTime.setTime(QTime(nTime/10000,(nTime%10000)/100,nTime%100));
		
	}
	
	cmd="SELECT * FROM "+StockCode+"_deal WHERE ID=(SELECT MAX(ID) FROM "+ StockCode+ "_deal) LIMIT 1";
	query.exec(cmd);
	VALID(query);
	if (query.next()){
		int nDate  =query.value(1).toInt();
		int nTime  =query.value(2).toInt();
		MaximumDateTime.setDate(QDate(nDate/10000,(nDate%10000)/100,nDate%100));
		MaximumDateTime.setTime(QTime(nTime/10000,(nTime%10000)/100,nTime%100));
	}
	qDateTimeEdit0->setMinimumDateTime(MinimumDateTime);
	qDateTimeEdit1->setMinimumDateTime(MinimumDateTime);
	qDateTimeEdit0->setMaximumDateTime(MaximumDateTime);
	qDateTimeEdit1->setMaximumDateTime(MaximumDateTime);
	if (DateTime0<MinimumDateTime)
		DateTime0=MinimumDateTime;
	if (DateTime1>MaximumDateTime)
		DateTime1=MaximumDateTime;
	//qDateTimeEdit0->setDateTime(DateTime0);
	//qDateTimeEdit1->setDateTime(DateTime1);
	qDateTimeEdit0->setDateTime(MinimumDateTime);
	qDateTimeEdit1->setDateTime(MaximumDateTime);

}

void MainWindow::SetDateTime0(QDateTime qDT){
	DateTime0=qDT;
}
void MainWindow::SetDateTime1(QDateTime qDT){
	DateTime1=qDT;
}
void MainWindow::Draw(){
	ReadDeal();

	//C_DealStreamer DealStreamer;
	C_TradeMaster  TradeMaster;
	
	QQueue<SDeal> qDeals;
	
	
	
	float maxDelta =1.0/100;
	for(float minDelta =0.01/100; minDelta<maxDelta/2; minDelta+=0.01/100){
		//QQueue<SDeal> qDeals1;
		//qDeals1<<qDeals;
		for (int i=0; i<qGraphField->vecDeal.size(); i++)
			qDeals<<qGraphField->vecDeal[i];

		float price =qDeals.first().Price;
		TradeMaster.Init(price, minDelta, maxDelta, 4, 100000, 0);
		TradeMaster.Parse(qDeals);
		TradeMaster.Close();
		printf("Cash=%f LCash=%f Stocks=%d LStocks=%d \n", TradeMaster.Cash, TradeMaster.LockedCash, TradeMaster.Stocks, TradeMaster.LockedStocks);
	}
	//DealStreamer.
	//DealStreamer.vecDeal<< qGraphField->vecDeal;
	//if (qGraphField->bViewRequestFlag){
	//	ReadRequest();
	//	ReadMyDeals();
	//}
	qGraphField->Rescale();
	qGraphField->Statistic.Reset();
	qGraphField->update();
	
}

void MainWindow::ViewRequest(){
	qGraphField->bViewRequestFlag=!qGraphField->bViewRequestFlag;
	if (qGraphField->bViewRequestFlag)
		ReadRequest();
	update();
}

void MainWindow::ViewOffer(){
	qGraphField->bViewOfferFlag=!qGraphField->bViewOfferFlag;
	update();
}
void MainWindow::ViewTrade(){
	qGraphField->bViewTradeFlag=!qGraphField->bViewTradeFlag;
	update();
}

void MainWindow::ReadMyDeals(){
	qGraphField->vecTrade.resize(0);
	int nDate0=intDate(DateTime0);
	int nDate1=intDate(DateTime1);
	int nTime0=intTime(DateTime0);
	int nTime1=intTime(DateTime1);

	char cmd[200];
	if (nDate0==nDate1)
		sprintf(cmd,"SELECT * FROM %s_trd WHERE trdate=%d AND trtime>=%d AND trtime<=%d",STR(StockCode),nDate0,nTime0,nTime1);
	if (nDate0<nDate1)
		sprintf(cmd,"SELECT * FROM %s_trd WHERE trdate>=%d AND trdate<=%d",STR(StockCode),nDate0,nDate1);

	printf("> %s\n",cmd);



	QSqlQuery query(db_trading);
	query.exec(cmd);
	VALID(query);


	SMyDeal MyDeal;
	for(int i=0; query.next(); i++){
		MyDeal.nDate  =query.value(1).toInt();
		MyDeal.nTime  =query.value(2).toInt();
		MyDeal.nVolume=query.value(3).toInt();
		MyDeal.Price  =query.value(4).toDouble();
		//Deal.Balance=query.value(5).toInt();
		qGraphField->vecTrade << MyDeal;
	}
}

void MainWindow::ReadRequest(){
	qGraphField->vecRequest.resize(0);
	int nDate0=intDate(DateTime0);
	int nDate1=intDate(DateTime1);
	int nTime0=intTime(DateTime0);
	int nTime1=intTime(DateTime1);

	//------ чтение заявок --------------------------------------

	char cmd[200];
	if (nDate0==nDate1)
		sprintf(cmd,"SELECT * FROM %s_req WHERE trdate=%d AND trtime>=%d AND trtime<=%d",STR(StockCode),nDate0,nTime0,nTime1);
	if (nDate0<nDate1)
		sprintf(cmd,"SELECT * FROM %s_req WHERE trdate>=%d AND trdate<=%d",STR(StockCode),nDate0,nDate1);
	printf(cmd);

	QSqlQuery query(db_trading);
	query.exec(cmd);
	VALID(query);

	SRequest Request(0,0);
	for(int i=0; query.next(); i++){
		Request.nDate  =query.value(1).toInt();
		Request.nTime  =query.value(2).toInt();
		Request.nVolume=query.value(3).toInt();
		Request.Price  =query.value(4).toDouble();
		qGraphField->vecRequest << Request;
	}
}

void MainWindow::ReadDeal(){
	//QDateTime DateTime0, QDateTime DateTime1, QString StockCode
	qGraphField->vecDeal.resize(0);

	int nDate0=intDate(DateTime0);
	int nDate1=intDate(DateTime1);
	int nTime0=intTime(DateTime0);
	int nTime1=intTime(DateTime1);

	//--------------------------------------------

	QSqlQuery query(db_trading);
	char cmd[200];
	if (nDate0==nDate1)
		sprintf(cmd,"SELECT * FROM %s_deal WHERE trdate=%d AND trtime>=%d AND trtime<=%d",STR(StockCode),nDate0,nTime0,nTime1);
	if (nDate0<nDate1)
		sprintf(cmd,"SELECT * FROM %s_deal WHERE trdate>=%d AND trdate<=%d",STR(StockCode),nDate0,nDate1);

	printf("> %s\n",cmd);

	query.exec(cmd);
	VALID(query);

	SDeal Deal;
	for(int i=0; query.next(); i++){
		Deal.nDate  =query.value(1).toInt();
		Deal.nTime  =query.value(2).toInt();
		Deal.nVolume=query.value(3).toInt();
		Deal.Price  =query.value(4).toDouble();
		Deal.nType  =query.value(5).toInt();
		qGraphField->vecDeal << Deal;
	}


}
/*
//--------------------------------------------
if (bViewOfferFlag){
	if (nDate0==nDate1)
		sprintf(cmd,"SELECT * FROM %s_off WHERE trdate=%d AND trtime>=%d AND trtime<=%d",STR(StockCode),nDate0,nTime0,nTime1);
	if (nDate0<nDate1)
		sprintf(cmd,"SELECT * FROM %s_off WHERE trdate>=%d AND trdate<=%d",STR(StockCode),nDate0,nDate1);

	printf("> %s\n",cmd);

	query.exec(cmd);
	VALID(query);

	SOffer Offer;
	vecOffer.resize(0);
	for(int i=0; query.next(); i++){
		Offer.nDate  =query.value(1).toInt();
		Offer.nTime  =query.value(2).toInt();
		Offer.Demand =query.value(3).toDouble();
		Offer.Supply =query.value(4).toDouble();
		Offer.nDemVol=query.value(5).toInt();
		Offer.nSupVol=query.value(6).toInt();
		vecOffer << Offer;
	}
}
vecOffer.resize(0);
vecTrade.resize(0);
vecRequest.resize(0);

//--------------------------------------------

//QSqlQuery query(db_trading);
//char cmd[200];
if (bViewTradeFlag){
	if (nDate0==nDate1)
		sprintf(cmd,"SELECT * FROM %s_trd WHERE trdate=%d AND trtime>=%d AND trtime<=%d",STR(StockCode),nDate0,nTime0,nTime1);
	if (nDate0<nDate1)
		sprintf(cmd,"SELECT * FROM %s_trd WHERE trdate>=%d AND trdate<=%d",STR(StockCode),nDate0,nDate1);

	printf("> %s\n",cmd);

	query.exec(cmd);
	VALID(query);


	SMyDeal MyDeal;
	for(int i=0; query.next(); i++){
		MyDeal.nDate  =query.value(1).toInt();
		MyDeal.nTime  =query.value(2).toInt();
		MyDeal.nVolume=query.value(3).toInt();
		MyDeal.Price  =query.value(4).toDouble();
		//Deal.Balance=query.value(5).toInt();
		vecTrade << MyDeal;
	}
}

//}	
return 1;

*/