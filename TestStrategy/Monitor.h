#pragma once
#include "main.h"
#include "Monitor.h"
#include <QTableWidget>
#include <QSqlDatabase>
class CMonitor : public QTableWidget
{
	Q_OBJECT

private:
	//QSqlDatabase db_mylefco;
	QSqlDatabase db_trading;
	void clear();
public:
	CMonitor(QWidget *parent = 0);
public slots:
	void updateMonitor();
	//bool autoRecalculate() const { return autoRecalc; }
	//QString currentLocation() const;
	//QString currentFormula() const;
	//QTableWidgetSelectionRange selectedRange() const;
	//bool readFile(const QString &fileName);
	//bool writeFile(const QString &fileName);
	//void sort(const SpreadsheetCompare &compare);
	
};
