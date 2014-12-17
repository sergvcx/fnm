#ifndef CANNONFIELD_H
#define CANNONFIELD_H

#include <QSQLError>
#include <QSQLQuery>
#include <QString>
#include <QWidget>
#include <QDateTime>
#include "../common/main.h"
#include <crtdbg.h>
#include <QPainter>
#include <QRect>
#include <QPaintEvent>
#include "ui_WatchDialog.h"

extern QSqlDatabase db_trading;


class CHistoDialog: public QWidget{
	public:
	QVector<float> vecHisto;
	float maxDif;
	CHistoDialog(){
		setPalette(QPalette(QColor(255, 255, 255)));
		setAutoFillBackground(true);
		vecHisto.resize(20);
		vecHisto.fill(0);
		maxDif=float(0.01);
		resize(800,600);
		
	}
	int sign(int val){
		if (val>0) return 1;
		else if (val<0) return -1;
		else return 0;
	}
	void Calculate(	QVector<SDeal> &vecDeal, int idx0, int idx1){
		int Counter=0;
		for(int idx=idx0; idx<idx1; idx++){
			// Если пролив
			if (vecDeal[idx].nTime==vecDeal[idx+1].nTime){
				int		nTime=vecDeal[idx].nTime;
				float	Price=vecDeal[idx].Price;
				int		nSignType=sign(vecDeal[idx].nType);
				float	LastPrice=Price;
				for(idx = idx+1 ; idx<=idx1 && (vecDeal[idx].nTime==nTime) && (sign(vecDeal[idx].nType)==nSignType); idx++){
					LastPrice=vecDeal[idx].Price;
				} 
				float	Dif=ABS(Price-LastPrice)/Price;
				int		idxHisto=Dif*vecHisto.size()/maxDif;
				idxHisto=MIN(vecHisto.size()-1,idxHisto);
				vecHisto[idxHisto]++;
				Counter++;
			}
		}
		for(int i=0; i<vecHisto.size(); i++){
			vecHisto[i]/=Counter;
		}
	}
	void paintEvent(QPaintEvent * event)
	{
		QPainter painter(this);



		//QBrush RedBrush(Qt::red,Qt::SolidPattern);
		//QBrush GreenBrush(Qt::green,Qt::SolidPattern);
		//QBrush WhiteBrush(Qt::white,Qt::SolidPattern);
		//QBrush GrayBrush(Qt::gray,Qt::SolidPattern);
		QBrush BarBrush(Qt::gray,Qt::SolidPattern);

		int x0,y0,x1,y1;
		QRect VisibleRect=event->rect();
		VisibleRect.getCoords(&x0,&y0,&x1,&y1);

		double scaleX=x1/vecHisto.size();
		int BarWidth=y1/vecHisto.size();
		painter.setBrush(BarBrush);
		for(int i=0; i<vecHisto.size();i++){
			painter.drawRect(i*scaleX,y1-vecHisto[i]*y1,BarWidth,vecHisto[i]*y1);
		}

	}	
};


class CWatchDialog : public QDialog , public Ui_Dialog {
	Q_OBJECT
private:

public:
		CWatchDialog(QWidget *parent = 0){
			parent;
		setupUi(this);
	}
	~CWatchDialog(){
	}
};

struct SStatistic{
	int idx0;
	int idx1;
	int VolumeSum;
	int VolumeSup;
	int VolumeDem;
	double PriceMax;
	double PriceMin;
	double PriceDif;
	double DensitySup;
	double DensityDem;
	SStatistic(){
		Reset();
	}
	void Reset(){
		idx0=0;
		idx1=0;
	}
} ;


class QGraphField :
	public QWidget
{
	Q_OBJECT

public:
	SStatistic Statistic;
	CWatchDialog* WatchDialog;
	QGraphField(QWidget *parent = 0);
	~QGraphField(){
		//delete WatchDialog;
	}
	void* mainWin;
	//QSqlDatabase db_trading;
	//int ReadData(QDateTime DateTime0, QDateTime DateTime1, QString StockCode);
	QVector<SPortfolio*> Portfolios;

	QString StockCode;
	QString StockName;
	int		bViewOfferFlag;
	int		bViewRequestFlag;
	int		bViewTradeFlag;

	//int		bReadTradeFlag;
	//int		bReadRequestFlag;
	//int		bReadOfferFlag;

	int		Rescale();

	int		pix2x(int pixX);
	double	pix2y(int pixY);
	int		x2pix(int x);
	int		y2pix(double y);

	
	
	QVector<SDeal>		vecDeal; 
//	QVector<SDealPeriod> vec1SecDeal;
//	QVector<SDealPeriod> vec5SecDeal;
//	QVector<SDealPeriod> vec15SecDeal;
//	QVector<SDealPeriod> vec1MinDeal;
//	QVector<SDealPeriod> vec5MinDeal;

	QVector<SOffer>		vecOffer;
	QVector<SMyDeal>	vecTrade;
	QVector<SRequest>	vecRequest;

public slots:
	void ZoomInX();
	void ZoomOutX();
	void ZoomInY();
	void ZoomOutY();
	//void ViewOffer();
	//void ViewTrade();
	//void ViewRequest();

	//void ReadOffer();
	//void ReadTrade();
	


	void GoToStart();
	void PrevDay();
	void NextDay();
	void GoToEnd();

	
	
		
	void Histo();

protected:
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);

private:

	float	minY;
	float	maxY;

	double	MX;
	double	MY;
	int		WinHeight;


};


#endif
