#include "MainWindow.h"
#include <QApplication>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QFile>
#include <QDialog>
#include <QMap>
#include <QSlider>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include "ui_ui.h"
#include "main.h"
#include "XmlParse.h"
#define STR(a) a.toAscii().data()
#define RUS(str) QString::fromLocal8Bit(str.toAscii().data())
#define ASCII(str) str.toAscii().data()

//char ticks[]="<tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680416</tradeno><tradetime>08.08.2014 09:59:59</tradetime><price>6766</price><quantity>5</quantity><period>O</period><buysell>S</buysell></tick><ti></ti>";
//char ticks[]="<ticks><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode></tick><tick><secid>834</secid></tick></ticks>";//<board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680416</tradeno><tradetime>08.08.2014 09:59:59</tradetime><price>6766</price><quantity>5</quantity><period>O</period><buysell>S</buysell></tick><ti></ti>";

#include <iostream>
#include <fstream>
//#include "demo.h"
#include "fight.h"

#include "TransaqConnector.h"


//std::ofstream xmlfile;
typeFreeMemory FreeMemory;


void UnloadLibrary( HMODULE hm )
{
	try {
		FreeLibrary(hm);
	}
	catch (...) {
		std::cout<<"Fail in FreeLibrary"<<std::endl;
	}
}
extern CThreadAllDeals* pThreadAllDeals;






/*
class C_TickQueue{
	QQueue<C_Tick> stack;
	QMutex mutex;
public:
	void Enqueue(C_Tick& tick){
		QMutexLocker ml(&mutex);
		stack.enqueue(tick);
	};
	void Dequeue(C_Tick& Deal){
		QMutexLocker ml(&mutex);
		Deal=stack.dequeue();
	}
} TickQueue;
*/
std::ofstream xml_markets;
std::ofstream xml_ticks;
std::ofstream xml_securities;
std::ofstream xml_sec_info_upd;
std::ofstream warnings;
std::ofstream xml_server_status; 


void OpenXML(){

	setlocale(LC_CTYPE, "");

	xml_markets.open("xml_markets.xml");
	xml_markets<<"<?xml version='1.0' encoding='UTF-8'?>";
	xml_markets<<"<root>" <<std::endl;

	xml_ticks.open("xml_ticks.xml");
	xml_ticks<<"<?xml version='1.0' encoding='UTF-8'?>";
	xml_ticks<<"<root>" <<std::endl;

	xml_securities.open("xml_securities.xml");
	xml_securities<<"<?xml version='1.0' encoding='UTF-8'?>";
	xml_securities<<"<root>" <<std::endl;

	xml_sec_info_upd.open("xml_sec_info_upd.xml");
	xml_sec_info_upd<<"<?xml version='1.0' encoding='UTF-8'?>";
	xml_sec_info_upd<<"<root>" <<std::endl;

	xml_server_status.open("xml_server_status.xml");
	xml_server_status<<"<?xml version='1.0' encoding='UTF-8'?>";
	xml_server_status<<std::endl;

	
	warnings.open("warnings.log");
}

void CloseXML(){
	xml_markets<<"</root>";
	xml_ticks<<"</root>";
	xml_securities<<"</root>";
	xml_sec_info_upd<<"</root>";

	xml_markets.close();
	xml_ticks.close();
	xml_securities.close();
	xml_sec_info_upd.close();
	xml_server_status.close();
	warnings.close();
}



QMap<QString,S_SecInfo> mapSeccode;

bool isReadyToCommand=false;
bool CALLBACK acceptor(BYTE *pData)
{
	int static counter=0;
	counter++;
	std::cout<<"******** CALLBACK -ENTER" << counter;
	
	fflush(stdout);
	QXmlStreamReader xml((char*)pData);
	while (!xml.atEnd() && !xml.hasError()){
		xml.readNext();
		if (xml.isStartDocument())
			continue;

		//printf(STR(xml.name().toString()));
		if (xml.isStartElement() && xml.name() == "markets"){
			xml_markets<<pData<<  std::endl;
		}
		//-------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "securities"){
			xml_securities<< pData <<  std::endl; 
			isReadyToCommand=false;
			std::cout<<"-security";
			xml.readNext();
			while (!(xml.isEndElement() && xml.name()=="securities" )){
				if (xml.isStartElement() && xml.name() == "security"){					
					S_SecInfo SecInfo;
					ParseSecurity(xml,SecInfo);
					if (SecInfo.seccode!=""){
						if (!mapSeccode.contains(SecInfo.seccode)){
							mapSeccode[SecInfo.seccode]=SecInfo;
							//QString info = SecInfo.seccode + " secid="+SecInfo.secid+ " shortname="+SecInfo.shortname;
							//std::cout<< ASCII(info) << std::endl;
						}
						else 
							std::cerr << "WARNING: mapSeccode already contains " << STR(SecInfo.seccode) <<std::endl;
					} else 
						std::cerr<< "WARNING: Empty seccode" <<std::endl;
						
				}
				xml.readNext();
			}
			std::cout<<"-security finished"; fflush(stdout);
			isReadyToCommand=true;
		}
		//-------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "sec_info_upd"){
			xml_sec_info_upd<<pData<<  std::endl; 
		}
		//------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "ticks"){
			isReadyToCommand=false;
			xml_ticks<<pData<<  std::endl; 
			std::cout<<"-ticks";
			QQueue<C_Tick> TickQueue;
			xml.readNext();
			while (!(xml.isEndElement() && xml.name()=="ticks" )){
				if (xml.isStartElement() && xml.name() == "tick"){					
					C_Tick tick;
					ParseTick(xml,tick);
					TickQueue.enqueue(tick);
				}
				xml.readNext();
			}
			std::cout<<"-ticks finished"; fflush(stdout);
			isReadyToCommand=true;
			//pThreadAllDeals->Parse(TickQueue);
		}
		//-------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "server_status"){
			xml_server_status<<pData<<  std::endl; 
			std::cout<<"-server_status";
			QXmlStreamAttributes attributes = xml.attributes();
			if (attributes.hasAttribute("id"))
				QString id = attributes.value("id").toString();

			if (attributes.hasAttribute("connected"))
				QString connected = attributes.value("connected").toString();

			if (attributes.hasAttribute("recover"))
				QString recover = attributes.value("recover").toString();

			if (attributes.hasAttribute("server_tz"))
				QString server_tz = attributes.value("server_tz").toString();
		}

	}


	FreeMemory(pData);
	std::cout<<" -EXIT **** " << counter << std::endl;
	return true;
}



	C_TransaqConnector::C_TransaqConnector(){
		error[0]=0;
		hm  = LoadLibraryA("txmlconnector.dll");
		if (hm) {
			try	{
				
				// Получение адресов функций библиотеки
	#ifdef CONNECTOR_166PLUS
				Initialize = reinterpret_cast<typeInitialize>(GetProcAddress(hm, "Initialize"));
				if (!Initialize)	{
					sprintf_s(error, buffSize, "\"InitLog\" not found (0x%X)", GetLastError());
					throw std::runtime_error(error);
				}

				UnInitialize = reinterpret_cast<typeUninitialize>(GetProcAddress(hm, "UnInitialize"));
				if (!UnInitialize)	{
					sprintf_s(error, buffSize, "\"UnInitialize\" not found (0x%X)", GetLastError());
					throw std::runtime_error(error);
				}
		
				SetLogLevel = reinterpret_cast<typeSetLogLevel>(GetProcAddress(hm, "SetLogLevel"));
				if (!SetLogLevel)	{
					sprintf_s(error, buffSize, "\"SetLogLevel\" not found (0x%X)", GetLastError());
					throw std::runtime_error(error);
				}
	#endif
		
				SetCallback = reinterpret_cast<typeSetCallback>(GetProcAddress(hm, "SetCallback"));
				if (!SetCallback)	{
					sprintf_s(error, buffSize, "\"SetCallback\" not found (0x%X)", GetLastError());
					throw std::runtime_error(error);
				}
		
				SendCommand = reinterpret_cast<typeSendCommand>(GetProcAddress(hm,"SendCommand"));
				if (!SendCommand)	{
					sprintf_s(error, buffSize, "\"SendCommand\" not found (0x%X)", GetLastError());
					throw std::runtime_error(error);
				}
		
				FreeMemory = reinterpret_cast<typeFreeMemory>(GetProcAddress(hm, "FreeMemory"));
				if (!FreeMemory)	{
					sprintf_s(error, buffSize, "\"FreeMemory\" not found (0x%X)", GetLastError());
					throw std::runtime_error(error);
				}

	#ifdef CONNECTOR_166PLUS
				// Инициализация библиотеки
				int level = 3;
				std::cout<<"Initializing library: \".\\LOGS\"  level: "<< level <<std::endl;
				BYTE* res = Initialize(reinterpret_cast<const BYTE*>(".\\LOGS"), level);
		
				if (res) {
					sprintf_s(error, buffSize, "Failed to initialize library: %s", res);
					FreeMemory(res);
					throw std::runtime_error(error);
				} else {
					std::cout<<"Library initialized successfully!"<<std::endl;
				}
	#endif
		
				SetCallback(acceptor);
		
				
				

	/*	
	#ifdef CONNECTOR_166PLUS
				// Смена уровня логирования библиотеки
				int newLevel = 2;
				std::cout<<"Log level change. Previous level: "<< level <<". New level: " << newLevel <<std::endl;
				res = SetLogLevel(newLevel);
				if (res) {
					std::cout<<"Failed to set log level: "<<res<<std::endl;
					FreeMemory(res);
				} else {
					std::cout<<"Log level changed successfully!"<<std::endl;
				}
	#endif
	*/
			}
			catch (std::runtime_error& e) {
				std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
				UnloadLibrary(hm);
			}
		}
	}

	C_TransaqConnector::~C_TransaqConnector() {
		

		
		
		try{	

		#ifdef CONNECTOR_166PLUS
			// Деинициализация библиотеки 
			std::cout<<"Uninitializing library..." <<std::endl;
			BYTE* res = UnInitialize();
			if (res) {
				sprintf_s(error, buffSize, "Failed to uninitialize library: %s", res);
				FreeMemory(res);
				throw std::runtime_error(error);
			} else {
				std::cout<<"Library uninitialized successfully!"<<std::endl;
			}
		#endif
		} 
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
		}
	}
	//=======================================================
	int C_TransaqConnector::connect(){
		try{	
			std::cout<<"Sending \"connect\" command..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(
					"<command id='connect'>"
					CONNECT_INFO
					"<logsdir>.\\LOGS\\</logsdir><loglevel>2</loglevel></command>"));
			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			Sleep(10000); 
			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
		}
		return 0;
	}
	int C_TransaqConnector::disconnect(){
		try{
			std::cout<<"Sending \"disconnect\" command..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(	"<command id='disconnect'/>"));
			std::cout<<reinterpret_cast<char*>(ss)<<std::endl;
			FreeMemory(ss);
		} 
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
		}
		return 0;
	}

	int C_TransaqConnector::server_status(){
		try {
			std::cout<<"Sending 'server status' server status..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='server_status'/>"));
			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}
	
	int C_TransaqConnector::subscribe_ticks(QList<QString>& SeccodeList, int tradeno){
		try {
			std::cout<<"Sending 'subscribe ticks' command..."<<std::endl;
			QString Cmd="<command id='subscribe_ticks'>";
			QString seccode;
			foreach(seccode,SeccodeList){
				if (!mapSeccode.contains(seccode)){
					std::cerr<< "ERROR: mapSeccode does not contains" << STR(seccode) << std::endl;
					//return 1;
				}
				Cmd+="<security secid='" + mapSeccode[seccode].secid + "' tradeno='1'/>"  ;
				//Cmd+="<security secid='24"   "' tradeno='1'/>" ;
			}
			Cmd+="</command>";
			
			char buf[]="<command id='subscribe_ticks'>"
					"<security secid='21' tradeno='1'/>"  
					"</command>";

			printf(STR(Cmd));
			//char* buffer=new char[512];
			//strcpy(buffer,STR(Cmd));
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			//BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(buf));
			//BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='subscribe_ticks'>"
			//	"<security secid='21' tradeno='1'/>"  
			//	"</command>"));

			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			//delete buffer;
			//Sleep(500000);
			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}

	int C_TransaqConnector::subscribe(QString& seccode){
			//char* buffer=new char[512];
			//QString Cmd="<command id='subscribe'>"
			//			"<alltrades><secid>"+mapSeccode[seccode].secid+"</secid></alltrades>"  
			//			"<quotations><secid>"+mapSeccode[seccode].secid+"</secid></quotations>" 
			//			"<quotes><secid>"+mapSeccode[seccode].secid+"</secid></quotes>"
			//			"</command>";



			std::cout<<"Sending 'subscribe' command..." << STR(seccode) <<std::endl;
			QString Cmd="<command id='subscribe'>"
				"<alltrades><secid>26</secid></alltrades>"  
				"<quotations><secid>26</secid></quotations>" 
				"<quotes><secid>26</secid></quotes>"
				"</command>";
			//char buffer[128];
			//strcpy(buffer,STR(Cmd));
			//BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='subscribe'>"
			//	"<alltrades><secid>26</secid></alltrades>"  
			//	"<quotations><secid>26</secid></quotations>" 
			//	"<quotes><secid>26</secid></quotes>"
			//	"</command>"));
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			FreeMemory(ss);
			return 0;
	}

	int C_TransaqConnector::get_securities(){
			/* 
			В команде 'get_securites' идентификаторы приведены для примера.
			В реальном коде необходимо использовать данные, присылаемые сервером
			*/
			std::cout<<"Sending \"get_securities\" command..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='get_securities'>"
				"</command>"));
			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			FreeMemory(ss);
			return 0;
	}
	int C_TransaqConnector::change_pass(){
		try{	
			std::cout<<"Sending \"change_pass\" command..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(
				"<command id='change_pass' oldpass='' newpass=''>"
				"</command>"));
			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}

	
