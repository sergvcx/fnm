#include "Monitor.h"

CMonitor::CMonitor(QWidget *parent){

	parent;
	//stopped=true;
	//db_index=0;


	// Открываем базу в которую пишем отсортированные сделки 
	db_trading = QSqlDatabase::addDatabase("QMYSQL","Monitor_trading");
	db_trading.setHostName("");
	db_trading.setDatabaseName("trading");
	db_trading.setUserName("root");
	db_trading.setPassword("root");
	if (!db_trading.open()) {
		QMessageBox::critical(0, QObject::tr("Database Error"), db_trading.lastError().text());
	}


	clear();

	
}

void CMonitor::updateMonitor(){
/*
	QSqlQuery query(db_trading);
	query.exec("SELECT * FROM _portfolio ;");
	if (!query.isActive()){
		printf("ERROR - trading._portfolio doesn'y exist\n");
		return;
	}
	//QMessageBox::critical(0, QObject::tr("Database Error"), query.lastError().text());

	int rows=query.size();
	setRowCount(rows);
	int row=0;


	while (query.next())
	{
		SPortfolio Portfolio;
		Portfolio.Status			= query.value(1).toInt();
		Portfolio.StockCode		= query.value(2).toString();	
		Portfolio.StockName		= query.value(3).toString();	
		Portfolio.LastOfferDate	= query.value(4).toInt();	
		Portfolio.LastOfferTime	= query.value(5).toInt();	
		Portfolio.Volume			= query.value(6).toInt();
		Portfolio.Profit			= query.value(7).toDouble();


		QTableWidgetItem *item0 = new QTableWidgetItem;
		item0->setText(Portfolio.StockCode);
		setItem(row,0, item0);

		QTableWidgetItem *item1 = new QTableWidgetItem;
		item1->setText(RUS(Portfolio.StockName));
		setItem(row,1, item1);

		QTableWidgetItem *item2 = new QTableWidgetItem;
		item2->setText(QString::number(Portfolio.LastOfferDate)+"\n"+QString::number(Portfolio.LastOfferTime));
		setItem(row,2, item2);


		QTableWidgetItem *item3 = new QTableWidgetItem;
		item3->setText(QString::number(Portfolio.Volume));
		setItem(row,3, item3);
		row++;
	}
	*/
}
void CMonitor::clear()
{
	//int RowCount=4;
	int ColumnCount=4;
	//setRowCount(RowCount);
	setColumnCount(ColumnCount);
	//for (int i = 0; i < ColumnCount; ++i) {
	//	QTableWidgetItem *item = new QTableWidgetItem;
	//	item->setText(QString(QChar('A' + i)));
	//	setHorizontalHeaderItem(i, item);
	//}
	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0->setText("Stock Code");
	setHorizontalHeaderItem(0, item0);
	
	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1->setText("Stock Name");
	setHorizontalHeaderItem(1, item1);

	setCurrentCell(0, 0);
	update();
}
