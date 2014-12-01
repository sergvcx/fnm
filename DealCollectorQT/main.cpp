#include "string.h"
#include "MainWindow.h"
#include <QApplication>
#include "TransaqConnector.h"
extern CThreadAllDeals* pThreadAllDeals;
 int main(int argc, char *argv[])
	 {

	 //QApplication app(argc, argv);

	 //MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}
	 C_TransaqConnector TransaqConnector;
	 
	 QList<QString> List; List << "GMKN";
	 TransaqConnector.subscribe_ticks(List);
	 //TransaqConnector.subscribe();
	 //mainWin->show();
	Sleep(1000*100);
	 return 1;
	 //return app.exec();
 }