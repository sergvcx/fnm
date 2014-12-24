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
#include <QSharedMemory>
#include <QTextCodec>
#include "ui_ui.h"
#include "main.h"
#include "XmlParse.h"
#include <QtDebug>

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
std::ofstream xml_quotes;
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

	xml_quotes.open("xml_quotes.xml");
	xml_quotes<<"<?xml version='1.0' encoding='UTF-8'?>";
	xml_quotes<<std::endl;
	
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
	xml_quotes.close();
	warnings.close();
}
/*
void UpdateBuyQuote(S_QuoteInfo &QuoteInfo, QList<S_Quote>& listQuote){
	bool ok;
	S_Quote NewQuote;
	NewQuote.quantity= QuoteInfo.buy.toInt(&ok,10);  if (!ok) return;
	NewQuote.price=    QuoteInfo.price.toFloat(&ok); if (!ok) return;
	QMutableListIterator<S_Quote> i(listQuote);
	while (i.hasNext()) {
		S_Quote& Quote=i.value();
		if (NewQuote.price == Quote.price){
			if (NewQuote.quantity==-1)
				i.remove();
			else 
				Quote.quantity=NewQuote.quantity;
			return;
		}
		if (NewQuote.price > Quote.price){
			i.insert(NewQuote);
			return;
		}
	}
}

void UpdateSellQuote(S_QuoteInfo &QuoteInfo, QList<S_Quote>& listQuote){
	bool ok;
	S_Quote NewQuote;
	NewQuote.quantity= QuoteInfo.sell.toInt(&ok,10);  if (!ok) return;
	NewQuote.price=    QuoteInfo.price.toDouble(&ok); if (!ok) return;
	QMutableListIterator<S_Quote> i(listQuote);
	while (i.hasNext()) {
		S_Quote& Quote=i.value();
		if (NewQuote.price == Quote.price){
			if (NewQuote.quantity==-1)
				i.remove();
			else 
				Quote.quantity=NewQuote.quantity;
			return;
		}
		if (NewQuote.price < Quote.price){
			i.insert(NewQuote);
			return;
		}
	}
}

*/
//QMap<QString,S_Security> mapAllSecurity;
//QMap<QString,S_Security> mapSecurity;


extern C_TransaqConnector TransaqConnector;
bool CALLBACK acceptor(BYTE *pData)
{
	QMap<QString,QQueue<S_XML_Tick>> mapTick;
	QMap<QString,QQueue<S_XML_QuoteInfo>> mapQuote;
	QMap<QString,S_XML_SecInfo> mapSecInfo;

	TransaqConnector.isBusy=true;
	int static counter=0;
	counter++;
	//std::cout<<"******** CALLBACK -ENTER" << counter;
	//printf("<<<<<< CALLBACK - ");
	
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
			//std::cout<<"-security";
			xml.readNext();
			while (!(xml.isEndElement() && xml.name()=="securities" )){
				if (xml.isStartElement() && xml.name() == "security"){					
					S_XML_SecInfo SecInfo;
					ParseSecurity(xml,SecInfo);
					if (SecInfo.seccode=="")
						std::cerr<< "ERROR: Empty seccode" <<std::endl;
					else{
						qDebug() << SecInfo.seccode ;
						if (TransaqConnector.mapInstrument.contains(SecInfo.seccode)){
							TransaqConnector.mapInstrument[SecInfo.seccode].pData->Info=SecInfo;
							//QString fname="xml_quote_"+SecInfo.seccode+".xml";
							 //mapSecurity[SecInfo.seccode].xml_quote.open("dd");   //fname.toAscii().data());
							 //xml_markets<<"<?xml version='1.0' encoding='UTF-8'?>";
							 //xml_markets<<"<root>" <<std::endl;
						}
					} 
						
				}
				xml.readNext();
			}
			//std::cout<<"-security finished" ; fflush(stdout);
		}
		//-------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "quotes"){
			xml_quotes << pData << std::endl;
			
			std::cout<<"-quotes";
			xml.readNext();
			while (!(xml.isEndElement() && xml.name()=="quotes" )){
				if (xml.isStartElement() && xml.name() == "quote"){					
					S_XML_QuoteInfo QuoteInfo;
					ParseQuote(xml,QuoteInfo);
					
					mapQuote[QuoteInfo.seccode].enqueue(QuoteInfo);
					//TransaqConnector.queueQuote<<QuoteInfo;
					/*
					if (QuoteInfo.seccode!=""){
						QMap<QString,S_Security>::iterator itSec=mapSecurity.find(QuoteInfo.seccode);
						if (itSec!=mapSecurity.end()){
							//UpdateBuyQuote (QuoteInfo,itSec->listBuyQuote);
							//UpdateSellQuote(QuoteInfo,itSec->listSellQuote);
						}
						else 
							std::cerr << "WARNING: mapSecurity not found " << STR(QuoteInfo.seccode) <<std::endl;
					} else 
						std::cerr<< "ERROR: Empty seccode" <<std::endl;
					*/
				}
				xml.readNext();
			}
			std::cout<<"-security finished" ; fflush(stdout);
			
		}
		//-------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "sec_info_upd"){
			xml_sec_info_upd<<pData<<  std::endl; 
		}
		//------------------------------------------------------------
		if (xml.isStartElement() && xml.name() == "ticks"){
			
			xml_ticks<<pData<<  std::endl; 
			//std::cout<<"-ticks";
			QQueue<S_XML_Tick> TickQueue;
			xml.readNext();
			while (!(xml.isEndElement() && xml.name()=="ticks" )){
				if (xml.isStartElement() && xml.name() == "tick"){					
					S_XML_Tick Tick;
					ParseTick(xml,Tick);
					//printf(".");
					qDebug() << Tick.toXML();
				
					mapTick[Tick.seccode].enqueue(Tick);

				}
				xml.readNext();
			}
			//std::cout<<"-ticks finished"; fflush(stdout);
			
			//pThreadAllDeals->Parse(TickQueue);
		}
		//--------------------- server_status ----------------------------------------
		if (xml.isStartElement() && xml.name() == "server_status"){
			xml_server_status<<pData<<  std::endl; 
			
			QXmlStreamAttributes attributes = xml.attributes();
			if (attributes.hasAttribute("id"))
				TransaqConnector.ServerStatus.id = attributes.value("id").toString();

			if (attributes.hasAttribute("connected"))
				TransaqConnector.ServerStatus.connected = attributes.value("connected").toString();

			if (attributes.hasAttribute("recover"))
				TransaqConnector.ServerStatus.recover = attributes.value("recover").toString();

			if (attributes.hasAttribute("server_tz"))
				TransaqConnector.ServerStatus.server_tz = attributes.value("server_tz").toString();
			
			TransaqConnector.ServerStatus.status= xml.readElementText();
			
			qDebug() <<"[server_status-callback:] " << TransaqConnector.ServerStatus.status << "\n"; 

		}

	}

	foreach ( QString seccode, mapTick.keys()){
		if (TransaqConnector.mapInstrument.contains(seccode)){
			C_Instrument& Instrument=TransaqConnector.mapInstrument[seccode];
			QQueue<S_XML_Tick>& queueTick=mapTick[seccode];
			Instrument.Lock();
			while (!queueTick.isEmpty()){
				S_XML_Tick& tick=queueTick.head();
				*Instrument.pTickLog<<tick.toXML() <<"\n";
				Instrument.pData->Ticks << tick;
				queueTick.removeFirst();
			}
			Instrument.Unlock();
		}
	}
	foreach ( QString seccode, mapQuote.keys()){
		if (TransaqConnector.mapInstrument.contains(seccode)){
			C_Instrument& Instrument=TransaqConnector.mapInstrument[seccode];
			QQueue<S_XML_QuoteInfo>& queueQuote=mapQuote[seccode];
			Instrument.Lock();
			while (!queueQuote.isEmpty()){
				S_XML_QuoteInfo& quote=queueQuote.head();
				*Instrument.pQuoteLog << quote.toXML() << "\n";
				Instrument.pData->Quotes<< quote ;
				queueQuote.removeFirst();
			}
			Instrument.Unlock();
		}
	}
	

		
	FreeMemory(pData);
	//printf(" -EXIT **** \n");
	TransaqConnector.isBusy=false;
	//printf(" CALLBACK  >>>>>>>>>>");
	return true;
}


	//============ init ===========================================
	C_TransaqConnector::C_TransaqConnector(){
		isBusy=false;
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
	//============ deinit ===========================================
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
	//============ connect ===========================================
	int C_TransaqConnector::connect()
	{
		try{	
			QString Cmd;
			Cmd = "<command id='connect'>\n";
			Cmd+= CONNECT_INFO;
			Cmd+= "		<logsdir>.\\LOGS\\</logsdir>\n";
			Cmd+= "		<loglevel>2</loglevel>\n";
			Cmd+= "</command>";
			qDebug() << "[connect-command:]\n" << Cmd+"\n"; 
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  Echo = codec->toUnicode((char*)ss); 
			qDebug() << "[connect-echo   :]\n" << Echo +"\n"; 

			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
		}
		return 0;
	}
	//============ disconnect ===========================================
	int C_TransaqConnector::disconnect()
	{
		try{
			QString  Cmd="<command id='disconnect'/>";
			qDebug() << "[disconnect-command:]\n" << Cmd << "\n"; 
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  Echo = codec->toUnicode((char*)ss); 
			qDebug() << "[connect-echo   :]\n" << Echo +"\n"; 
			FreeMemory(ss);
		} 
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
		}
		return 0;
	}
	//============ server_status ===========================================
	int C_TransaqConnector::server_status()
	{
		try {
			std::cout<<"Sending 'server status' server status..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='server_status'/>"));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  en = codec->toUnicode((char*)ss); 
			qDebug() << en+"\n"; 

			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}
	//============ subscribe (quotes only) ===========================================
	int C_TransaqConnector::subscribe(QList<QString>& SeccodeList)
	{
		try {
			std::cout<<"Sending 'subscribe' command..."<<std::endl;
			QString Cmd;
			QString seccode;
			Cmd ="<command id='subscribe'>";
			Cmd+=	"<quotes>";
			foreach(seccode,SeccodeList){
				if (!TransaqConnector.mapInstrument.contains(seccode)){
					std::cerr<< "ERROR: mapSecurity does not contains" << STR(seccode) << std::endl;
					return 1;
				}
				Cmd+=	"<security>";
				Cmd+=		"<board>"+ QString(TransaqConnector.mapInstrument[seccode].pData->Info.board) +"</board>";
				Cmd+=		"<seccode>" + seccode + "</seccode>"  ;
				Cmd+=	"</security>";
			}
			Cmd+=	"</quotes>";
			Cmd+="</command>";
			

			qDebug() << Cmd+"\n";
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  en = codec->toUnicode((char*)ss); 
			qDebug() << "[subscribe echo:]" << en << "\n"; 

			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}
	//============ subscribe_ticks (list) ===========================================
	int C_TransaqConnector::subscribe_ticks(QList<QString>& SeccodeList, int tradeno)
	{
		try {
			std::cout<<"Sending 'subscribe ticks' command..."<<std::endl;
			QString Cmd;
			QString seccode;
			Cmd ="<command id='subscribe_ticks'>";
			foreach(seccode,SeccodeList){
				if (!TransaqConnector.mapInstrument.contains(seccode)){
					std::cerr<< "ERROR: mapSecurity does not contains" << STR(seccode) << std::endl;
					continue;
				}
				Cmd+="<security secid='" + QString(TransaqConnector.mapInstrument[seccode].pData->Info.secid) + "' tradeno='1'/>"  ;
			}
			Cmd+="</command>";
			qDebug() << Cmd+"\n";
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  en = codec->toUnicode((char*)ss); 
			qDebug() << "[subscribe_ticks echo:]"<< en << "\n"; 

			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}
	//============ subscribe_ticks (seccode) ===========================================
	int C_TransaqConnector::subscribe_ticks(QString& seccode)
	{
		try {
			std::cout<<"Sending 'subscribe ticks' command..."<<std::endl;
			
			if (!TransaqConnector.mapInstrument.contains(seccode)){
				std::cerr<< "ERROR: TransaqConnector does not contains" << STR(seccode) << std::endl;
				return 1;
			}
			QString Cmd;
			Cmd ="<command id='subscribe_ticks'>";
			Cmd+=	"<security>";
			Cmd+=		"<board> TQBR </board>";
			Cmd+=		"<seccode>"+seccode+"</seccode>";
			Cmd+=		"<tradeno>1</tradeno>";
			Cmd+=	"</security>";
			Cmd+=	"<filter>false</filter>";
			Cmd+="</command>";

			printf(STR(Cmd));
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(STR(Cmd)));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  en = codec->toUnicode((char*)ss); 
			qDebug() << en+"\n"; 

			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}

	//============ get_securities  ===========================================
	int C_TransaqConnector::get_securities(){
			//В команде 'get_securites' идентификаторы приведены для примера.
			//В реальном коде необходимо использовать данные, присылаемые сервером
			std::cout<<"Sending \"get_securities\" command..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='get_securities'></command>"));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  en = codec->toUnicode((char*)ss); 
			qDebug() << en+"\n"; 
			FreeMemory(ss);
			return 0;
	}
	//============ change_pass  ===========================================
	int C_TransaqConnector::change_pass(){
		try{	
			std::cout<<"Sending \"change_pass\" command..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='change_pass' oldpass='' newpass=''></command>"));
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QString  en = codec->toUnicode((char*)ss); 
			qDebug() << en+"\n"; 
			FreeMemory(ss);
		}
		catch (std::runtime_error& e) {
			std::cout<<"A fatal error occurred: "<<e.what()<<std::endl;
			UnloadLibrary(hm);
			return 1;
		}
		return 0;
	}
	//=====================================================================
	C_TransaqConnector& C_TransaqConnector::operator<< (QString seccode){
		listActive << seccode;
		C_Instrument Instrument;
		Instrument.Create(seccode);
		Instrument.pQuoteLog= new C_XML_Logger(seccode+"_quote.xml");
		Instrument.pQuoteLog->Header();
		Instrument.pTickLog= new C_XML_Logger(seccode+"_tick.xml");
		Instrument.pTickLog->Header();
		bool ok;
		
		mapInstrument[seccode]=Instrument;
		
		return *this;
	}

	bool C_TransaqConnector::isConnected(){
		//while (isBusy)
		//	Sleep(500);
		//server_status();
		//Sleep(500);
		//while (isBusy)
		//	Sleep(500);
		return (ServerStatus.connected=="true");
	}