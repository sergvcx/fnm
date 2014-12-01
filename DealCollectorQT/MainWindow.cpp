#include "main.h"

#include "MainWindow.h"
#include <QMapIterator>
#include <QMenuBar>
#include <QLabel>
#include <QStatusBar>

CThreadAllDeals* pThreadAllDeals;
MainWindow::MainWindow()
{
	SPortfolioErr p,g;
	SPortfolioErr& f=p;
	//p=g;
	pDBManager		= new CDBManager;
	pThreadAllDeals	= new CThreadAllDeals;
	pThreadAllDeals->pDBManager=pDBManager;
	

	Timer = new QTimer(this);
	StartStopTimer = new QTimer(this);
	SMSTimer = new QTimer(this);
	Monitor = new CMonitor;

	setCentralWidget(Monitor);

	createActions();
	createMenus();
	createStatusBar();

	//Timer* timer = new Timer;
	//timer->start();
	//pThreadAllDeals->ReadPortfolio();
	//int rows=pThreadAllDeals->StockMap.size();
	//Monitor->setRowCount(rows);
	//
	//QMapIterator<QString, SPortfolio> i(pThreadAllDeals->StockMap);
	//int row=0;
	//while (i.hasNext()){
	//	//sum += i.next().value();
	//	i.next();
	//	QTableWidgetItem *item0 = new QTableWidgetItem;
	//	item0->setText(i.key());
	//	Monitor->setItem(row,0, item0);

	//	QTableWidgetItem *item1 = new QTableWidgetItem;
	//	item1->setText(RUS(i.value().StockName));
	//	Monitor->setItem(row,1, item1);
	//	row++;
	//}

}

void MainWindow::createActions(){
	newAction = new QAction(tr("&New"), this);
	newAction->setIcon(QIcon(":/images/new.png"));
	newAction->setShortcut(QKeySequence::New);
	newAction->setStatusTip(tr("Create a new spreadsheet file"));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	actStartAllDeals	= new QAction(tr("&Start all deals"), this);
	actStopAllDeals		= new QAction(tr("&Stop all deals"), this);
	actEmptyAllDeals	= new QAction(tr("&Drop and create Quik.AllDeals table"), this);
	actCreateFinInst	= new QAction(tr("&Drop and create Quik.FinInst table"), this);
	//actUpdateRates		= new QAction(tr("&Create new Quik.Rates tables"), this);
	actCheckTables		= new QAction(tr("&Check all Tables"), this);
	actDropTradings		= new QAction(tr("&Trading:Drop all except portfolio"), this);
	actCreateFromPortfolio = new QAction(tr("&Trading:Create all from portfolio"), this);
	
	actDropPortfolio	= new QAction(tr("&Drop   Trading._portfolio"), this);
	actCreatePortfolio	= new QAction(tr("&Create Trading._portfolio"), this);
	actCleanPortfolio	= new QAction(tr("&Clean  Trading._portfolio"), this);
	
	actDropAllEmpty		= new QAction(tr("&Drop all empty tradings.tables"), this);
	actDropAllSmall		= new QAction(tr("&Drop all small tradings.tables_***"), this);
	actDropAllTrd		= new QAction(tr("&Drop all trading.*_trd"), this);
	actDropAllReq		= new QAction(tr("&Drop all trading.*_req"), this);
	actAlterAll  		= new QAction(tr("&Alter all to MyIASM"), this);
	actCreateQuikRequest= new QAction(tr("&Create quik.request"), this);
	actCreateQuikTrade  = new QAction(tr("&Create quik.trade"), this);
	actCreateQuikCurrent= new QAction(tr("&Create quik.current"), this);

	actDeleteTradingDealToday= new QAction(tr("&Delete all today deals from trading.*_deal"), this);
	actUpdateAllInSecond= new QAction(tr("&Update all in second from trading.*_deal"), this);

	//actCreatePortfolio	= new QAction(tr("&CreatePortfolio"), this);

	actStopAllDeals->setStatusTip(tr("Stop All Deals"));
	actStartAllDeals->setStatusTip(tr("Start all deals"));
	actDropTradings->setStatusTip(tr("Drop all trading.tables except _portfolio"));


	
	//connect(actStartAllDeals, SIGNAL(triggered()), this, SLOT(newFile()));
	//connect(actStartAllDeals, SIGNAL(triggered()), this, SLOT(StartAllDeals()));
	connect(actStartAllDeals,	SIGNAL(triggered()), pThreadAllDeals, SLOT(start()));
	connect(actStopAllDeals,	SIGNAL(triggered()), pThreadAllDeals, SLOT(stop()));
	connect(actEmptyAllDeals,	SIGNAL(triggered()), pDBManager, SLOT(QuikDropAndCreateAllDeals()));
	connect(actCreateFinInst,	SIGNAL(triggered()), pDBManager, SLOT(QuikDropAndCreateFinInst()));
	
	
	connect(actCheckTables,		SIGNAL(triggered()), pDBManager, SLOT(CheckTables()));
	connect(actDropTradings,	SIGNAL(triggered()), pDBManager, SLOT(TradingDropAll()));
	connect(actCreateFromPortfolio,	SIGNAL(triggered()), pDBManager, SLOT(TradingCreateAllFrom_portfolio()));
	connect(actCleanPortfolio,	SIGNAL(triggered()), pDBManager, SLOT(TradingClean_portfolio()));
	
	connect(actDropPortfolio,	SIGNAL(triggered()), pDBManager, SLOT(TradingDrop_portfolio()));
	connect(actCreatePortfolio,	SIGNAL(triggered()), pDBManager, SLOT(TradingCreate_portfolio()));
	connect(actDropAllEmpty,	SIGNAL(triggered()), pDBManager, SLOT(TradingDropAllEmpty()));
	connect(actDropAllSmall,	SIGNAL(triggered()), pDBManager, SLOT(TradingDropAllSmall()));
	connect(actDropAllTrd,		SIGNAL(triggered()), pDBManager, SLOT(TradingDropAll_trd()));
	connect(actDropAllReq,		SIGNAL(triggered()), pDBManager, SLOT(TradingDropAll_req()));
	connect(actAlterAll,		SIGNAL(triggered()), pDBManager, SLOT(TradingAlterAll()));
	connect(actDeleteTradingDealToday,		SIGNAL(triggered()), pDBManager, SLOT(TradingDeleteDealsToday()));

	connect(actUpdateAllInSecond,SIGNAL(triggered()), pDBManager, SLOT(TradingUpdateAllInSecond()));
	


	actAbout = new QAction(tr("&about"), this);
	actAbout->setStatusTip(tr("Create a new spreadsheet file"));
	//connect(actStartAllDeals, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(Timer, SIGNAL(timeout()), Monitor, SLOT(updateMonitor()));
	connect(StartStopTimer, SIGNAL(timeout()), pThreadAllDeals, SLOT(onTimer()));
	connect(SMSTimer, SIGNAL(timeout()), pThreadAllDeals, SLOT(onSMSTimer()));
	connect(actCreateQuikRequest, SIGNAL(triggered()), pDBManager, SLOT(QuikCreate_request()));
	connect(actCreateQuikTrade  , SIGNAL(triggered()), pDBManager, SLOT(QuikCreate_trade()));
	connect(actCreateQuikCurrent, SIGNAL(triggered()), pDBManager, SLOT(QuikCreate_current()));
	

	StartStopTimer->start(1000);
	//Timer->start(1000);
	// SMSTimer->start(1000);

}
void MainWindow::createMenus(){
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAction);
	fileMenu->addSeparator();
	//fileMenu->addAction(exitAction);
	fileMenu = menuBar()->addMenu(tr("&Tool"));
	
	fileMenu->addSeparator();
	fileMenu->addAction(actStartAllDeals);
	fileMenu->addAction(actStopAllDeals);
	fileMenu->addSeparator();
	fileMenu->addAction(actCheckTables		);
	fileMenu->addAction(actEmptyAllDeals	);
	fileMenu->addAction(actCreateFinInst);
//	fileMenu->addAction(actUpdateRates);
	fileMenu->addAction(actEmptyAllDeals	);
	fileMenu->addAction(actDropTradings		);
	fileMenu->addAction(actCreateFromPortfolio);
	fileMenu->addAction(actCleanPortfolio);
	
	fileMenu->addAction(actDropPortfolio	);
	fileMenu->addAction(actCreatePortfolio);
	fileMenu->addAction(actDropAllEmpty);
	fileMenu->addAction(actDropAllSmall);
	fileMenu->addAction(actDropAllTrd);
	fileMenu->addAction(actDropAllReq);
	fileMenu->addAction(actAlterAll);
	fileMenu->addAction(actCreateQuikRequest);
	fileMenu->addAction(actCreateQuikTrade);
	fileMenu->addAction(actCreateQuikCurrent);
	fileMenu->addAction(actDeleteTradingDealToday);
	fileMenu->addAction(actUpdateAllInSecond);



	fileMenu = menuBar()->addMenu(tr("&Help"));
	fileMenu->addAction(actAbout);


}
void MainWindow::closeEvent(QCloseEvent *event){

}

void MainWindow::newFile(){
int g=1;
}
void MainWindow::StartAllDeals(){
	pThreadAllDeals->start();
}


void MainWindow::createStatusBar()
{
	locationLabel = new QLabel(" W999 ");
	locationLabel->setAlignment(Qt::AlignHCenter);
	locationLabel->setMinimumSize(locationLabel->sizeHint());

	formulaLabel = new QLabel("-------");
	formulaLabel->setIndent(3);

	statusBar()->addWidget(locationLabel);
	statusBar()->addWidget(formulaLabel, 1);

	//connect(spreadsheet, SIGNAL(currentCellChanged(int, int, int, int)),
	//	this, SLOT(updateStatusBar()));
	//connect(spreadsheet, SIGNAL(modified()),
	//	this, SLOT(spreadsheetModified()));
	updateStatusBar();
}
void MainWindow::updateStatusBar()
{
	locationLabel->setText("asdasd");//spreadsheet->currentLocation());
	formulaLabel->setText("12312");//spreadsheet->currentFormula());
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About DealCollector"),
		tr("<h2>Deal Collector 1.0</h2>"
		"build from " __DATE__" " __TIME__ 
		"<p>Copyright &copy; 2008 Software Inc."
		));


	
}

