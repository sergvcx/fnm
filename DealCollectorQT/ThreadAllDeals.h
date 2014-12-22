#pragma once
#include <QThread>
#include "main.h"
#include "DBManager.h"
#include "shared.h"
class CThreadAllDeals:public QThread 
{
	char cmd[1024];
	Q_OBJECT
public: 
	CThreadAllDeals(void);	
	void setMessage(const QString &message); 
	//QSqlDatabase db_quik;
	QSqlDatabase db_mylefco;
	QSqlDatabase db_trading;
	QMap<QString,SPortfolio*> mapPortfolio;

	int ReadPortfolio();
	void HandleDeal(SDeal &Deal);
	void HandleOffer(SOffer &Offer);
	
	SDeal LastDeal; // Испольлзуется для сохраниения 
	CDBManager* pDBManager;
	void Connect();
	void Disconnect();
public slots:	
	void onTimer();
	void onSMSTimer();
	void run(); 
	void stop(); 
	void Parse(QQueue<S_XML_Tick>& TickQueue);
private: 
	QString messageStr; 
	volatile bool stopped;
	int db_index;
public:
	~CThreadAllDeals(void);
};
