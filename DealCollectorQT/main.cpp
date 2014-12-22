#include "string.h"
#include "MainWindow.h"
#include <QApplication>
#include "TransaqConnector.h"

#include "shared.h"
extern CThreadAllDeals* pThreadAllDeals;

extern bool isReadyToCommand;

C_TransaqConnector TransaqConnector;

 int main(int argc, char *argv[])
	 {
		// C_SharedPortfolio SharedPortfolio;

		 //SharedPortfolio <<  "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		//	 << "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
		//	 <<"ROSN" <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";


		
//		 printf(STR(sss));
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



	 

	 TransaqConnector.disconnect();
	 Sleep(1000);
	 TransaqConnector.connect();
	 while(!TransaqConnector.isConnected()){
		 printf("Transaq not connected\n");
		 Sleep(1000);
	 }
	 printf("Transaq connected!\n");

	 
	 TransaqConnector << "GMKN" ;
	 //<<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
	//	 << "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
	//	 <<"ROSN" <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";
	 

	

	//Sleep(1000*10);
	//TransaqConnector.subscribe(QString("GMKN"));
	//TransaqConnector.server_status();
	//Sleep(1000*20);
	//TransaqConnector.server_status();
	//while (!isReadyToCommand)
	//	Sleep(1000);
	 if (TransaqConnector.subscribe(TransaqConnector.listActive)){
		TransaqConnector.disconnect();
		return 1;
	 }
	
	// if (TransaqConnector.subscribe_ticks(QString("GMKN"))){
	//	TransaqConnector.disconnect();
	//	return 1;
	 //}
	//if (!TransaqConnector.subscribe_ticks(TransaqConnector.listActive))
	//	TransaqConnector.disconnect();
	
	Sleep(1000*60*60*8);
	TransaqConnector.disconnect();
	//TransaqConnector.change_pass();
	
	CloseXML();
	 return 1;
	 
 }