#pragma once

#include <QTimer>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include "Monitor.h"
#include "DBMAnager.h"
#include "ThreadAllDeals.h"



class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	void closeEvent(QCloseEvent *event);
	
	CDBManager*	 pDBManager;

private slots:
	void newFile();
	void StartAllDeals();
	void about();
	void updateStatusBar();

private:
	void createActions();
	void createMenus();
	//void createContextMenu();
	//void createToolBars();
	void createStatusBar();
	QTimer *Timer;
	QTimer *StartStopTimer;			// Этот таймер подсоединяется к базе данных , запускает закачку сделок и отсоединияется вечером
	QTimer *SMSTimer;
	CMonitor* Monitor;
	QLabel *locationLabel;
	QLabel *formulaLabel;

	QMenu *fileMenu;
	QMenu *toolMenu;
	QAction *newAction;
	QAction *actStartAllDeals;
	QAction *actStopAllDeals;
	QAction *actEmptyAllDeals;
	QAction *actCheckTables;
	QAction *actDropTradings;
	QAction *actDropPortfolio;
	QAction *actCreatePortfolio;
	QAction *actAbout;
	QAction *actCreateFinInst;
	QAction *actCreateFromPortfolio;
	QAction *actCleanPortfolio;
	QAction *actDropAllEmpty;
	QAction *actDropAllSmall;
	QAction *actDropAllTrd;
	QAction *actDropAllReq;
	QAction *actAlterAll;
	QAction *actCreateQuikRequest;
	QAction *actCreateQuikTrade;
	QAction *actCreateQuikCurrent;
	QAction *actDeleteTradingDealToday;
	QAction *actUpdateAllInSecond;
	//QAction *actUpdateRates ;







};

