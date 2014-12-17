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
	 pThreadAllDeals->ReadPortfolio();
	//  mainWin->show();
	// return app.exec();

	 OpenXML();
	 C_TransaqConnector TransaqConnector;

	 TransaqConnector.disconnect();
	 Sleep(1000);
	 TransaqConnector.connect();
	 while (!isReadyToCommand)
		Sleep(1000);
	 
	 
	 QList<QString> List; List << "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		 << "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
		 <<"ROSN" <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";
	 

	 QList<QString> QuoteList; QuoteList << "GMKN" ;

	 //TransaqConnector.subscribe();
	

	//Sleep(1000*10);
	//TransaqConnector.subscribe(QString("GMKN"));
	//TransaqConnector.server_status();
	//Sleep(1000*20);
	TransaqConnector.server_status();
	while (!isReadyToCommand)
		Sleep(1000);
	TransaqConnector.subscribe_ticks(List);
	//TransaqConnector.subscribe(QuoteList);
	Sleep(1000*60*60*8);
	TransaqConnector.disconnect();
	//TransaqConnector.change_pass();
	
	CloseXML();
	 return 1;
	 
 }