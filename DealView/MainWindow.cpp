#include "../common/Main.h"
#include "MainWindow.h"

#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
QScrollArea* extScrollArea;
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
	qGraphField->pInstrument=&Instrument;
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
	//qGraphField->vecDeal.resize(0);

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
	
	Instrument.Create(StockCode);
	Instrument.Init();
	SDeal Deal;
	S_Tick Tick;
	for(int i=0; query.next(); i++){
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
		//qGraphField->vecDeal << Deal;

		Instrument.pData->Ticks<< Tick;
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