//#include "main.h"
//#include <process.h>
#include <QApplication>
#include <QAction>
#include <QLabel>
#include <QString>
#include <QMenu>
#include <QMainWindow>
#include <QScrollArea>
#include <QDateTimeEdit>
#include <QTimeEdit>
#include <QComboBox>
#include "QGraphField.h"
#include "shared.h"
/*
class CConsole : public QTableWidget
{
	Q_OBJECT

public:
	CConsole(QWidget *parent = 0);

	//bool autoRecalculate() const { return autoRecalc; }
	//QString currentLocation() const;
	//QString currentFormula() const;
	//QTableWidgetSelectionRange selectedRange() const;
	void clear();
	//bool readFile(const QString &fileName);
	//bool writeFile(const QString &fileName);
	//void sort(const SpreadsheetCompare &compare);
};

*/
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	//void closeEvent(QCloseEvent *event);
	void ReadProfile();
	QSqlDatabase db_trading;
	QString StockCode;
	QLabel *labelIndex;
	QLabel *labelDate;
	QLabel *labelTime;
	QLabel *labelPrice;
	QLabel *labelVolume;
	QLabel *labelPriceMouse;

	C_Instrument Instrument;
	void ReadRequest();
	void ReadDeal();
	void ReadMyDeals();
	
public slots:
	void Draw();
	void Attach();
	//void StartAllDeals();
	void about();
	void updateStatusBar();
	void SelectStock(int Item);
	void SetDateTime0(QDateTime qDT);
	void SetDateTime1(QDateTime qDT);
	
	
	void ViewRequest();
	void ViewOffer();
	void ViewTrade();

	


private:
	void createActions();
	void createMenus();
	//void createContextMenu();
	void createToolBars();
	void createStatusBar();


	QMenu *fileMenu;
	QMenu *toolMenu;
	QToolBar *toolBar;
	QAction	*newAction;
	QAction *actStartAllDeals;
	QAction *actStopAllDeals;
	QAction *actEmptyAllDeals;
	QAction *actCheckTables;
	QAction *actDropTradings;
	QAction *actCreatePortfolio;
	QAction *actAbout;
	QAction *actDraw;
	QAction *actAttach;
	QAction *actZoomInX;
	QAction *actZoomOutX;
	QAction *actZoomInY;
	QAction *actZoomOutY;
	QAction *actViewOfferFlag;
	QAction *actViewTradeFlag;
	QAction *actViewRequestFlag;
	
	
	QAction *actUnclassify;
	QAction *actHisto;
	QAction *actGoToStart;
	QAction *actNextDay;
	QAction *actPrevDay;
	QAction *actGoToEnd;


	QScrollArea		*qScrollArea;
	QGraphField		*qGraphField;
	QComboBox		*qStockComboBox;
	QDateTimeEdit	*qDateTimeEdit0;
	QDateTimeEdit	*qDateTimeEdit1;

	QDateTime DateTime0;
	QDateTime DateTime1;

	QVector<SPortfolio*> Portfolios;
// 	QVector<S_EasyTrade> Trade;
// 	QVector<C_EasyQuote> Quote;
// 	QVector<C_EasyTrade*> TimeLineTrade;
// 	QVector<C_EasyQuote*> TimeLineQuote;
// 	QVector<S_Tick*> TimeLineTrade;


};

