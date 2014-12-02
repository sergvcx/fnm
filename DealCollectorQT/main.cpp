#include "string.h"
#include "MainWindow.h"
#include <QApplication>
#include "TransaqConnector.h"
extern CThreadAllDeals* pThreadAllDeals;

extern bool isReadyToCommand;
 int main(int argc, char *argv[])
	 {
	setlocale(LC_CTYPE, "");
	 QApplication app(argc, argv);

	 MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}
	 OpenXML();
	 C_TransaqConnector TransaqConnector;

	 TransaqConnector.connect();
	 while (!isReadyToCommand)
		Sleep(1000);
	 
	 
	 QList<QString> List; List << "GMKN";
	 
	 //TransaqConnector.subscribe();
	 //mainWin->show();

	//Sleep(1000*10);
	//TransaqConnector.subscribe(QString("GMKN"));
	//TransaqConnector.server_status();
	//Sleep(1000*20);
	TransaqConnector.server_status();
	TransaqConnector.subscribe_ticks(List);
	Sleep(1000*60);
	TransaqConnector.disconnect();
	//TransaqConnector.change_pass();
	
	CloseXML();
	 return 1;
	 //return app.exec();
 }