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
		 //setlocale(LC_CTYPE, "");
		 cout << "Русский текст в консоли" << endl;


		 QByteArray msg = QByteArray::fromHex("cde5eff0e0e2e8ebfcedeee520f1eeeee1f9e5ede8e5204b4f4e5f544d5f484f53544b4e4620eef220d3cad2d121");
		 QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
		 QString strf = codec->toUnicode(msg);
		 qDebug() << strf;


		 QByteArray wtf_s(strf.toStdString().c_str()); //либо так
		 //wtf_s.append(strf); //либо так

		 QByteArray wtf = codec->fromUnicode(wtf_s);
		 qDebug() << wtf.toHex();



		// C_SharedPortfolio SharedPortfolio;

		 //SharedPortfolio <<  "GMKN" <<"LKOH" << "GAZP" << "SBER" << "SBERP" << "AFLT" << "MSTT" 
		//	 << "ODVA" <<"PLZL" <<"SVAV"  <<"NMTP"  <<"VTBR"  <<"MGNT"  <<"YNDX"  <<"NVTK"  <<"MTLRP"  <<"MSNG"  <<"IRAO"  <<"MTSS"  
		//	 <<"ROSN" <<"RTKM" <<"RTKMP" <<"HYDR" <<"NLMK" <<"CHMF" <<"URKA";


		
//		 printf(STR(sss));
	
	 QApplication app(argc, argv);

	 //MainWindow* mainWin=new MainWindow;
	 //if (argc==2){
	//	 if (strcmp(argv[1],"-auto")==0){
	//		pThreadAllDeals->start();
	//	 }
	 //}
	 //pThreadAllDeals->ReadPortfolio();
	//  mainWin->show();
	// return app.exec();

	 OpenXML();



	 

	 TransaqConnector.disconnect();
	 Sleep(1000);
	 TransaqConnector.connect();

	 TransaqConnector.server_status();
	 while(1);
	TransaqConnector.isConnected();
	 TransaqConnector.connect();
	 TransaqConnector.isConnected();
	 //while(!TransaqConnector.isConnected()){
	//	 printf("Transaq not connected\n");
	//	 Sleep(1000);
	// }
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

	QMap<QString,C_Instrument> mapInstrument;

	
	C_Instrument Instrument;
	Instrument.pSharedMemory=new QSharedMemory("GMKN");
	if (Instrument.pSharedMemory->attach()){
		
		return 1;
	}
	
	Instrument.pData=(C_SharedMemoryInstrument*)Instrument.pSharedMemory->data();	

	while (1){
	
		//QString xml_glass=Instrument.pData->Quotes.toXML();
		for(int i=0; i<Instrument.pData->Ticks.size; i++){
			//QString xml_glass=Instrument.pData->Ticks.data[i].;
			S_Tick &Tick=Instrument.pData->Ticks.data[i];
			printf("%f %d %d \n", Tick.price, Tick.quntity, Tick.datetime);
		}
		
		Sleep(1000);

	}

	
	//mapInstrument[seccode]=Instrument;


	//while 
	Sleep(1000*60*60*8);

	TransaqConnector.disconnect();
	Instrument.pSharedMemory->detach();
	//TransaqConnector.change_pass();
	
	CloseXML();
	 return 1;
	 
 }