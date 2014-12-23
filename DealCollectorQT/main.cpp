#include "string.h"
#include "MainWindow.h"
#include <QApplication>
#include "TransaqConnector.h"

#include "shared.h"
extern CThreadAllDeals* pThreadAllDeals;

#include <QTextCodec>
#include <QTDebug>
using namespace std;

C_TransaqConnector TransaqConnector;


 int main(int argc, char *argv[])
 {
	 setlocale(LC_ALL, "Russian");
	// cout << "Русский текст в консоли" << endl;


	// C_SharedPortfolio SharedPortfolio;

 //SharedPortfolio <<  "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
	//	 << "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
	//	 <<"ROSN" <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";

	
	 QApplication app(argc, argv);

	 MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}

	//  mainWin->show();
	// return app.exec();

	 OpenXML();



	 

	 //TransaqConnector.disconnect();
	 //Sleep(1000);
	 TransaqConnector	<< "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		 				<< "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
		 				<<"ROSN"  <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";


	if (TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
	}
	else 
		TransaqConnector.connect();
	pThreadAllDeals->ReadPortfolio();
	while(!TransaqConnector.isConnected()){
		qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
		Sleep(500);
	}
	
	qDebug() << "Connected=" << TransaqConnector.ServerStatus.connected << " state=" << TransaqConnector.ServerStatus.status <<"\n";
	//TransaqConnector.server_status();
	//TransaqConnector.isConnected();
	 //TransaqConnector.connect();
	 //TransaqConnector.isConnected();
	 //while(!TransaqConnector.isConnected()){
	//	 printf("Transaq not connected\n");
	//	 Sleep(1000);
	// }

	 
	QMap<QString,C_Instrument> mapInstrument;
	for(int i=0; i<TransaqConnector.listActive.size();i++){
		QString seccode=TransaqConnector.listActive.at(i);
		C_Instrument Instrument;
		bool ok=Instrument.Attach(seccode);
		if (ok)
			mapInstrument[seccode]=Instrument;
	}

	 

	 C_Instrument Instrument;
	 

	// int dec=Instrument.pData->Info.decimals;
	

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
	
	 if (TransaqConnector.subscribe_ticks(QString("GMKN"))){
		TransaqConnector.disconnect();
		return 1;
	 }
	//if (!TransaqConnector.subscribe_ticks(TransaqConnector.listActive))
	//	TransaqConnector.disconnect();

	//QMap<QString,C_Instrument> mapInstrument;



	while (1){
		//Instrument.pData->Quotes.UpdateCurrentQuotes()
		QString xml_glass=Instrument.pData->Quotes.Current();
		qDebug() << xml_glass;
		
		
		for(int i=0; i<TransaqConnector.listActive.size();i++){
			QString seccode=TransaqConnector.listActive.at(i);
			C_Instrument& Instrument=mapInstrument[seccode];
			S_EasyTicks& Ticks=Instrument.pData->Ticks;
			for(;Instrument.tail<Ticks.size; Instrument.tail++){
				qDebug() << Ticks.data[Instrument.tail].toXML() << "\n";

			}
			
		}
		//for(int i=0; i<Instrument.pData->Ticks.size; i++){
			//QString xml_glass=Instrument.pData->Ticks.data[i].;
		//	S_Tick &Tick=Instrument.pData->Ticks.data[i];
		//	printf("%f %d %d \n", Tick.price, Tick.quntity, Tick.datetime);
		//}
		
		Sleep(1000);

	}

	
	//mapInstrument[seccode]=Instrument;


	//while 
	Sleep(1000*60*60*8);

	TransaqConnector.disconnect();
	//Instrument.pSharedMemory->detach();
	//TransaqConnector.change_pass();
	
	CloseXML();
	 return 1;
	 
 }