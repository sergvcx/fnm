#include <QPainter>
#include <math.h>

#include <QScrollArea>
#include <QScrollBar>
#include <QPaintEvent>
#include "QGraphField.h"
#include "MainWindow.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSQLError>
#include <QSQLQuery>

//#include "Classifier.h"
//#include "Trader.h"





QGraphField::QGraphField(QWidget *parent)
: QWidget(parent)
{
	MX=1;
	MY=1;
	
	WinHeight = 800;
	setPalette(QPalette(QColor(255, 255, 255)));
	setAutoFillBackground(true);
	//setBackground(QColor(250, 250, 250));
	//setPalette(QPalette(QColor(255, 255, 255)));
	//setAutoFillBackground(true);


	bViewOfferFlag=0;
	bViewRequestFlag=1;
	bViewTradeFlag=0;


	setMouseTracking(true);

	WatchDialog=new CWatchDialog(this);
	WatchDialog->raise();
	WatchDialog->activateWindow();
	WatchDialog->show();
}

void QGraphField::paintEvent(QPaintEvent * event)
{
	QMap<int,int> mapTime;
	QPainter painter(this);

	QColor clrBrown(255,155,0);
	QBrush SellBrush(clrBrown,Qt::SolidPattern);
	QBrush BuyBrush(Qt::blue,Qt::SolidPattern);


	QBrush RedBrush(Qt::red,Qt::SolidPattern);
	QBrush GreenBrush(Qt::green,Qt::SolidPattern);
	QBrush WhiteBrush(Qt::white,Qt::SolidPattern);
	QBrush GrayBrush(Qt::gray,Qt::SolidPattern);
	QBrush LightGrayBrush(QColor(230,230,230),Qt::SolidPattern);
	
	QPen DayPen	 (Qt::red,Qt::DashDotLine);
	QPen RedPen	 (Qt::red,Qt::SolidLine);
	QPen GreenPen(Qt::green,Qt::SolidLine);
	QPen WhitePen(Qt::white,Qt::SolidLine);
	QPen BlackPen(Qt::black,Qt::SolidLine);
	QPen GrayPen (Qt::gray,Qt::SolidLine);
	QPen LightGrayPen(QColor(230,230,230),Qt::SolidLine);

	QPen penSupply(clrBrown,Qt::SolidLine);
	QPen penDemand(Qt::blue,Qt::SolidLine);

	
	int x0,y0,x1,y1;
	QRect VisibleRect=event->rect();
	int LastIndex=vecDeal.size()-1;
	VisibleRect.getCoords(&x0,&y0,&x1,&y1);

	x0=pix2x(x0-100);
	x1=pix2x(x1+100);//+10;

	x0=MIN(x0,LastIndex);
	x1=MIN(x1,LastIndex); // последний элемент
	x0=MAX(1,x0); 
	x1=MAX(1,x1);

//	int nTime0=vecDeal[x0].nTime;
	int nDate0=vecDeal[x0].nDate;
//	int nTime1=vecDeal[x1].nTime;
	int nDate1=vecDeal[x1].nDate;

	//---------------- рисуем график сделок -----------------------------
	painter.setPen(BlackPen);
	int SwitchColor=5;
	printf("----------------\n");
	for(int idx=x0+1; idx<=x1; idx++){

		int x=x2pix(idx);
		mapTime[vecDeal[idx].nTime]=x;
		printf("%d %d\n",vecDeal[idx].nTime,x);

		if (vecDeal[idx].nType&IN_SECOND){
			SwitchColor=-SwitchColor;
			int idx0=idx;
			int idx1=idx;
			// Ищем последнюю сделку пролива
			int nTime=vecDeal[idx].nTime;
			for(;idx<=x1 && (vecDeal[idx].nTime==nTime) ; idx++){
				idx1=idx;
			} 
			idx=idx1;
			// рисуем рамку пролива
			//double Price0=vecDeal[idx0].Price;
			//double Price1=vecDeal[idx1].Price;

			//double Delta=(Price1-Price0)/Price0;
			//int DeltaColor=Delta*5000;
			//DeltaColor=MIN(DeltaColor,230);
			//DeltaColor=MAX(DeltaColor,-230);
			QBrush CustomBrush(QColor(230,230-SwitchColor,230+SwitchColor),Qt::SolidPattern);
			//painter.setBrush(LightGrayBrush);
			painter.setBrush(CustomBrush);
			
			painter.setPen(LightGrayPen);
			painter.drawRect(x2pix(idx0),0,x2pix(idx1)-x2pix(idx0),WinHeight);
			painter.setPen(BlackPen);
			// Рисуем сделки

		
			for(idx=idx0; idx<=idx1; idx++){
				
				//if (idx0)
				painter.drawLine(x2pix(idx-1),y2pix(vecDeal[idx-1].Price),x2pix(idx),y2pix(vecDeal[idx].Price));

				if (vecDeal[idx].IsSupply())
					painter.setBrush(GreenBrush);
				else if (vecDeal[idx].IsDemand())
					painter.setBrush(RedBrush);
				else 
					painter.setBrush(WhiteBrush);


				painter.drawRect(x2pix(idx)-1,y2pix(vecDeal[idx].Price)-1,3,3);
			}
			idx=idx1;
		}
		// Если одиночная сделка
		else {
			if (idx)
				painter.drawLine(x2pix(idx-1),y2pix(vecDeal[idx-1].Price),x2pix(idx),y2pix(vecDeal[idx].Price));
			if (vecDeal[idx].IsSupply())
				painter.setBrush(GreenBrush);
			else if (vecDeal[idx].IsDemand())
				painter.setBrush(RedBrush);
			else 
				painter.setBrush(WhiteBrush);
			painter.drawRect(x2pix(idx)-1,y2pix(vecDeal[idx].Price)-1,3,3);

		}

		
	}
	
	// ============== рисуем график заявок ====================
	//printf("%d--\n\n\n",0);
	if (bViewRequestFlag && vecRequest.size()>0){
		QMap<int, SRequest> mapRequest;
		
		mapRequest.clear();


		//int DealAbsTime0=vecDeal[x0].AbsTime(); // крайнее левое  абсолютное время
		int DealAbsTime1=vecDeal[x1].AbsTime(); // крайнее правое абсолютное время

		int idxReq0=0;
//		int idxReq1=0;
		int idxDeal0=x0;
//		int idxDeal1=x1;
		int idxReq;

		// Подгоняем дату
		for(idxReq0=0; idxReq0<vecRequest.size() && vecRequest[idxReq0].nDate<vecDeal[idxDeal0].nDate; idxReq0++){
		
		}
		
		for (idxReq=idxReq0; idxReq<vecRequest.size() && vecRequest[idxReq].AbsTime()<DealAbsTime1;){
				
			
			// составляем список заявок на данный момент
			int CurrReqAbsTime=vecRequest[idxReq].AbsTime();

			int idxDraw0=idxReq;
			int idxDraw1=idxReq;
			for (;idxReq<vecRequest.size() && vecRequest[idxReq].AbsTime()==CurrReqAbsTime; idxReq++){
				SRequest& Request=vecRequest[idxReq];
				int keyRequest=Request.Price*256+0.5;
				if (Request.nVolume==0){
					if (mapRequest.contains(keyRequest)){
						mapRequest.remove(keyRequest);
					}
				}
				else {
					if (mapRequest.contains(keyRequest)){
						mapRequest[keyRequest]=Request;
					}
					else {
						mapRequest.insert(keyRequest,Request);
					}
				}
			}

			
			idxDraw1=MIN(idxReq,vecRequest.size()-1);
			SRequest& Request=vecRequest[idxDraw0];

			if (nDate0<= Request.nDate && Request.nDate<=nDate1){
				if (mapTime.contains(vecRequest[idxDraw0].nTime)){
				if (mapTime.contains(vecRequest[idxDraw1].nTime)){
					int pixDealX0=mapTime[vecRequest[idxDraw0].nTime];
					int pixDealX1=mapTime[vecRequest[idxDraw1].nTime];

					QMapIterator<int, SRequest> i(mapRequest);
					while (i.hasNext()) {
						i.next();
						SRequest Request=i.value();
						int pixY=y2pix(Request.Price);
						_ASSERTE(Request.nVolume!=0);
						if (Request.nVolume>0){
							painter.setPen(penDemand);
						}
						else {
							painter.setPen(penSupply);
						}
						painter.drawLine(pixDealX0,pixY,pixDealX1,pixY);
					}
				}
				}
			}
			
			
		
		}
	}

	// -------------- рисуем свои сделки -------------
	
	if (bViewRequestFlag && vecTrade.size()>0){


//		int DealAbsTime0=vecDeal[x0].AbsTime(); // крайнее левое  абсолютное время
		int DealAbsTime1=vecDeal[x1].AbsTime(); // крайнее правое абсолютное время

		int idxDeal0=x0;
//		int idxDeal1=x0;
		int idxTrade0;

		// Подгоняем дату
		for(idxTrade0=0; idxTrade0<vecTrade.size() && vecTrade[idxTrade0].nDate<vecDeal[idxDeal0].nDate; idxTrade0++){

		}

		for (int idxTrade=idxTrade0; idxTrade<vecTrade.size() && vecTrade[idxTrade].AbsTime()<DealAbsTime1;idxTrade++){


			SMyDeal& MyDeal=vecTrade[idxTrade];

			if (mapTime.contains(MyDeal.nTime)){
				int pixY=y2pix(MyDeal.Price);
				int pixX=mapTime[MyDeal.nTime];
				if (MyDeal.nVolume>0){
					painter.setPen(penDemand);
				}
				else {
					painter.setPen(penSupply);
				}

				painter.drawRect(pixX-1,pixY-1,3,3);

			}
			else {
//				int g=0;
			}
		}
	}

	// -------------- рисуем границу дней ---------------------
	painter.setPen(DayPen);
	for(int idx=x0; idx<x1; idx++){
		if (vecDeal[idx].nDate!=vecDeal[idx-1].nDate){
			painter.drawLine(x2pix(idx),0,x2pix(idx),WinHeight);
		}
	}
	

	// -------------- рисуем рамку -------------------
	if (Statistic.idx1 > Statistic.idx0){

		int x0=x2pix(Statistic.idx0);
		int y0=y2pix(Statistic.PriceMax)-2;
		int x1=x2pix(Statistic.idx1);
		int y1=y2pix(Statistic.PriceMin)+2;
		QPoint points[5];
		points[0]= QPoint(x0,y0);
		points[1]= QPoint(x1,y0);
		points[2]= QPoint(x1,y1);
		points[3]= QPoint(x0,y1);
		points[4]= QPoint(x0,y0);

		painter.setPen(GreenPen);
		painter.drawPolyline(points,5);
	} else {
		int x0=x2pix(Statistic.idx0);
		painter.setPen(GreenPen);
		painter.drawLine(x0,0,x0,WinHeight);
	}
	WatchDialog->raise();


	painter.setPen(BlackPen);


	for(int i=0; i<100; i++){

		painter.drawLine(0,0,100,i);
	}

}




int QGraphField::Rescale(){
	minY= 1000000;	
	maxY=0;
	int DataSize=vecDeal.size();
	
	for(int i=0; i<DataSize; i++){
		float val=vecDeal[i].Price;
		if (val>maxY)
			maxY=val;
		else if (val<minY)
			minY=val;
	}

	// pixy=y*MY+C;
	// 0=maxY*MY+C
	// winheight-1=minY*MY+C
	// MY=(winheight-1)/(minY-MaxY)
	// C =-maxY*MY
	// pixy=y*MY-maxY*MY;
	// pixy=(y-maxY)*MY
	

	MX=4;
	MY=-(WinHeight-1)/(maxY-minY);
	setMinimumSize(DataSize*MX,WinHeight);
	setMaximumSize(DataSize*MX,WinHeight);
	update();
	
	return 1;
}







void GetStat(QVector<SDeal>& vecDeal, SStatistic &Stat){
	//if (Stat.idx0>=Stat.idx1 || Stat.idx1>=vecDeal.size() || Stat.idx1>=vecDeal.size()){
		Stat.PriceDif=0;
		Stat.PriceMax=0;
		Stat.PriceMin=0;
		Stat.VolumeSup=0;
		Stat.VolumeDem=0;
		Stat.VolumeSum=0;
	//}
	//else {
		Stat.PriceMin=100000;
		Stat.PriceMax=0;
		Stat.VolumeSum=0;
		int size=vecDeal.size();
		_ASSERTE(Stat.idx0<vecDeal.size());
		_ASSERTE(Stat.idx1<vecDeal.size());
		for(int idx=Stat.idx0; idx<=Stat.idx1; idx++){
			double Price=vecDeal[idx].Price;
			int Volume=vecDeal[idx].nVolume;
			Stat.PriceMax=MAX(Price,Stat.PriceMax);
			Stat.PriceMin=MIN(Price,Stat.PriceMin);
			Stat.VolumeSum+=Volume;
			if (vecDeal[idx].IsSupply())
				Stat.VolumeSup+=Volume;
			else if (vecDeal[idx].IsDemand())
				Stat.VolumeDem+=Volume;
		}
		Stat.PriceDif=Stat.PriceMax-Stat.PriceMin;
		Stat.DensitySup=Stat.VolumeSup/Stat.PriceDif;
		Stat.DensityDem=Stat.VolumeDem/Stat.PriceDif;
	//}

}
void QGraphField::mousePressEvent(QMouseEvent *event){
	QPoint Point=event->pos();
//	int button	=event->button();
	int idx=MIN(vecDeal.size()-1,pix2x(Point.x()));
	switch(event->button()){
		case Qt::LeftButton :
				
				Statistic.idx0=idx;
				if (Statistic.idx0>=Statistic.idx1){
					Statistic.idx1=Statistic.idx0;
				}
				WatchDialog->labelDate0->setNum(vecDeal[idx].nDate);
				WatchDialog->labelTime0->setNum(vecDeal[idx].nTime);
				
				GetStat(vecDeal,Statistic);
				WatchDialog->labelPriceMax->setNum(Statistic.PriceMax);
				WatchDialog->labelPriceMin->setNum(Statistic.PriceMin);
				WatchDialog->labelPriceDif->setNum(Statistic.PriceDif*100/((Statistic.PriceMax+Statistic.PriceMin)/2));
				WatchDialog->labelVolumeSum->setNum(Statistic.VolumeSum);
				WatchDialog->labelVolumeSup->setNum(Statistic.VolumeSup);
				WatchDialog->labelVolumeDem->setNum(Statistic.VolumeDem);
				WatchDialog->labelDensitySup->setNum(Statistic.DensitySup);
				WatchDialog->labelDensityDem->setNum(Statistic.DensityDem);
	//			update();
			break;
		case Qt::RightButton:
				WatchDialog->labelDate1->setNum(vecDeal[idx].nDate);
				WatchDialog->labelTime1->setNum(vecDeal[idx].nTime);
				Statistic.idx1=idx;
				GetStat(vecDeal,Statistic);
				WatchDialog->labelPriceMax->setNum(Statistic.PriceMax);
				WatchDialog->labelPriceMin->setNum(Statistic.PriceMin);
				WatchDialog->labelPriceDif->setNum(Statistic.PriceDif*100/((Statistic.PriceMax+Statistic.PriceMin)/2));
				WatchDialog->labelVolumeSum->setNum(Statistic.VolumeSum);
				WatchDialog->labelVolumeSup->setNum(Statistic.VolumeSup);
				WatchDialog->labelVolumeDem->setNum(Statistic.VolumeDem);
				WatchDialog->labelDensitySup->setNum(Statistic.DensitySup);
				WatchDialog->labelDensityDem->setNum(Statistic.DensityDem);
	
			break;
		case Qt::NoButton:
			break;
	}
	
	//_ASSERTE(vecRequest.size()==Trader.vecRequest.size());
	//_ASSERTE(vecTrade.size()==Trader.vecTrade.size());
	update();
	
}
void QGraphField::mouseMoveEvent(QMouseEvent *event){
	QPoint Point=event->pos();
	
	int idx=MIN(vecDeal.size()-1,pix2x(Point.x()));
	double MousePrice=pix2y(Point.y());
	double Price=vecDeal[idx].Price;
	((MainWindow*)mainWin)->labelIndex->setNum(idx);
	((MainWindow*)mainWin)->labelDate->setText(date2str(vecDeal[idx].nDate));
	((MainWindow*)mainWin)->labelTime->setText(time2str(vecDeal[idx].nTime));
	((MainWindow*)mainWin)->labelVolume->setNum(vecDeal[idx].nVolume);
	((MainWindow*)mainWin)->labelPrice->setNum(Price);
	((MainWindow*)mainWin)->labelPriceMouse->setText(QString::number(MousePrice)+":"+QString::number((MousePrice-Price)*100/Price)+"%");


	
}

int QGraphField::pix2x(int pixX){
	return int(pixX/MX+0.5);
}	
double QGraphField::pix2y(int pixY){
	//return 	minY-(pixY-WinHeight)/MY;
	return pixY/MY+maxY;
}
int QGraphField::x2pix(int x){
	return int(x*MX);
}
int QGraphField::y2pix(double y){
	//return int(WinHeight+(minY-y)*MY);
	return int ((y-maxY)*MY);//(WinHeight+(minY-y)*MY);

	// pixy=y*MY+C;
	// 0=maxY*MY+C
	// winheight-1=minY*MY+C
	// MY=(winheight-1)/(minY-MaxY)
	// C =-maxY*MY
	// pixy=y*MY-maxY*MY;
	// pixy=(y-maxY)*MY
	// y =pixy/MY+maxY

}
extern QScrollArea* extScrollArea;
void QGraphField::ZoomInX(){
	double factor=sqrt(2.0);
	MX*=factor;
	setMinimumSize(vecDeal.size()*MX,WinHeight);
	setMaximumSize(vecDeal.size()*MX,WinHeight);
	QScrollBar* sb = extScrollArea->horizontalScrollBar();
	sb->setValue(int(sb->value()*factor+sb->pageStep()/2*(factor-1)));
	update();
}
void QGraphField::ZoomOutX(){
	double factor=1/sqrt(2.0);
	MX*=factor;
	setMinimumSize(vecDeal.size()*MX,WinHeight);
	setMaximumSize(vecDeal.size()*MX,WinHeight);
	QScrollBar* sb = extScrollArea->horizontalScrollBar();
	sb->setValue(int(sb->value()*factor+sb->pageStep()/2*(factor-1)));
	update();
}
void QGraphField::ZoomInY(){
	double factor=sqrt(2.0);
	MY*=factor;
	WinHeight*=factor;
	setMinimumSize(vecDeal.size()*MX,WinHeight);
	setMaximumSize(vecDeal.size()*MX,WinHeight);
	QScrollBar* sb = extScrollArea->verticalScrollBar();
	sb->setValue(int(sb->value()*factor+sb->pageStep()/2*(factor-1)));
	update();
}
void QGraphField::ZoomOutY(){
	double factor=1/sqrt(2.0);
	MY*=factor;
	WinHeight*=factor;
	setMinimumSize(vecDeal.size()*MX,WinHeight);
	setMaximumSize(vecDeal.size()*MX,WinHeight);
	QScrollBar* sb = extScrollArea->verticalScrollBar();
	sb->setValue(int(sb->value()*factor+sb->pageStep()/2*(factor-1)));
	update();
}



void QGraphField::GoToStart(){
	QScrollBar* sb = extScrollArea->horizontalScrollBar();
	sb->setValue(0);
	update();
}


void QGraphField::PrevDay(){
	
	//QScrollBar* sb = extScrollArea->horizontalScrollBar();
	//sb->setValue(int(sb->value()*factor+sb->pageStep()/2*(factor-1)));
	// update();
}


void QGraphField::NextDay(){

}


void QGraphField::GoToEnd(){
	QScrollBar* sb = extScrollArea->horizontalScrollBar();
	sb->setValue(sb->maximum());
	update();
}

void QGraphField::Histo(){

	CHistoDialog* HistoDialog=new CHistoDialog;
	HistoDialog->Calculate(vecDeal,Statistic.idx0, Statistic.idx1);
	HistoDialog->show();
}


