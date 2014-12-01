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
#define STR(a) a.toAscii().data()
#define RUS(str) QString::fromLocal8Bit(str.toAscii().data())
#define ASCII(str) str.toAscii().data()

//char ticks[]="<tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680416</tradeno><tradetime>08.08.2014 09:59:59</tradetime><price>6766</price><quantity>5</quantity><period>O</period><buysell>S</buysell></tick><ti></ti>";
//char ticks[]="<ticks><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode></tick><tick><secid>834</secid></tick></ticks>";//<board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680416</tradeno><tradetime>08.08.2014 09:59:59</tradetime><price>6766</price><quantity>5</quantity><period>O</period><buysell>S</buysell></tick><ti></ti>";
char ticks[]="<ticks><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680813</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6783</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680814</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1935.6</price><quantity>16</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680815</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68</price><quantity>60</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680816</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68</price><quantity>1</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680817</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>67.93</price><quantity>99</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680818</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>67.93</price><quantity>47</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680819</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.27</price><quantity>180</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680824</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1933.9</price><quantity>10</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680825</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.15</price><quantity>192</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680828</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6784</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680829</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.28</price><quantity>100</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680830</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.4</price><quantity>30</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680831</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.4</price><quantity>80</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680832</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.16</price><quantity>2</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680833</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>100</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680835</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>100</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680836</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>100</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680838</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>50</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680839</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1933</price><quantity>10</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680840</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>40</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680841</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680842</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>10</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680843</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1933</price><quantity>20</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680844</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1933</price><quantity>10</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680845</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>50</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680849</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1932.7</price><quantity>9</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680850</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1932.5</price><quantity>1</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680851</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6784</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680852</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>1931</price><quantity>10</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680854</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.33</price><quantity>300</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680857</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.2</price><quantity>20</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680858</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.2</price><quantity>10</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680859</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.16</price><quantity>520</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680860</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6784</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680861</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.33</price><quantity>110</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680862</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.33</price><quantity>10</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680863</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.3</price><quantity>24</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680864</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.33</price><quantity>76</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680865</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.33</price><quantity>104</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680866</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.34</price><quantity>100</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680867</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.37</price><quantity>130</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680868</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.38</price><quantity>920</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680869</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.39</price><quantity>86</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680870</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.39</price><quantity>214</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680871</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.4</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680872</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6784</price><quantity>1</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680873</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6785</price><quantity>19</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680874</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6785</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680875</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.43</price><quantity>737</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680876</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.45</price><quantity>511</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680877</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6785</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680878</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>6785</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680879</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.48</price><quantity>40</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680880</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.42</price><quantity>150</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680881</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.42</price><quantity>96</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680882</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.42</price><quantity>4</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680886</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.48</price><quantity>50</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680887</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.48</price><quantity>100</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680889</tradeno><tradetime>08.08.2014 10:00:01</tradetime><price>68.48</price><quantity>10</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680900</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.42</price><quantity>16</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680901</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.42</price><quantity>20</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680902</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.42</price><quantity>20</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680903</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.42</price><quantity>44</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>4</secid><board>TQBR</board><seccode>LKOH</seccode><tradeno>2396680905</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>1930</price><quantity>20</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680906</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>6785</price><quantity>20</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680919</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.42</price><quantity>16</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680920</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.42</price><quantity>20</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>26</secid><board>TQBR</board><seccode>SBER</seccode><tradeno>2396680921</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>68.41</price><quantity>114</quantity><period>N</period><buysell>S</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680924</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>6785</price><quantity>1</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680925</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>6789</price><quantity>5</quantity><period>N</period><buysell>B</buysell></tick><tick><secid>833</secid><board>TQBR</board><seccode>GMKN</seccode><tradeno>2396680926</tradeno><tradetime>08.08.2014 10:00:02</tradetime><price>6798</price><quantity>37</quantity><period>N</period><buysell>B</buysell></tick></ticks>";

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




void ParseTick(QXmlStreamReader& xml, C_Tick& tick ){
	//QMap<QString, QString> Tick;
	/* Let's check that we're really getting a tick. */
	if(xml.tokenType() != QXmlStreamReader::StartElement &&	xml.name() == "tick") {
		return;
	}
	xml.readNext();
	//printf(STR(xml.name().toString()));
	while(!(xml.tokenType() == QXmlStreamReader::EndElement &&	xml.name() == "tick")) {
		if(xml.tokenType() == QXmlStreamReader::StartElement) {
			if(xml.name() == "secid") {
				tick.secid= xml.readElementText();
				continue;
			}
			if(xml.name() == "board") {
				tick.board= xml.readElementText();
				continue;
			}
			if(xml.name() == "tradeno"){
				tick.tradeno= xml.readElementText();
				continue;
			}
			if(xml.name() == "tradetime"){
				tick.tradetime= xml.readElementText();
				continue;
			}
			if(xml.name() == "price"){
				tick.price= xml.readElementText();
				continue;
			}
			if(xml.name() == "quantity"){
				tick.quantity= xml.readElementText();
				continue;
			}
			if(xml.name() == "period"){
				tick.period= xml.readElementText();
				continue;
			}
			if(xml.name() == "buysell"){
				tick.buysell= xml.readElementText();
				continue;
			}
		}
		// ...and next... 
		xml.readNext();
		//printf(STR(xml.name().toString()));
	}
	return;
}
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





bool CALLBACK acceptor(BYTE *pData)
{
	int static counter=0;
	counter++;
	std::cout<<"******** CALLBACK ******" << counter << std::endl;
	
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
		if (xml.isEndElement() && xml.name() == "securities"){
			xml_securities<<pData<<  std::endl; 
		}
		if (xml.isEndElement() && xml.name() == "sec_info_upd"){
			xml_sec_info_upd<<pData<<  std::endl; 
		}

		if (xml.isStartElement() && xml.name() == "ticks"){
			xml_ticks<<pData<<  std::endl; 

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
			pThreadAllDeals->Parse(TickQueue);
		}
	}


	FreeMemory(pData);
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
		
				OpenXML();
				connect();

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
		

		disconnect();
		CloseXML();
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
			std::cout<<"Sending \"server status\" server status..."<<std::endl;
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id=\"server_status\"/>"));
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
			std::cout<<"Sending \"subscribe ticks\" command..."<<std::endl;
			QString Cmd="<command id='subscribe_ticks'>";
			QString seccode;
			foreach(seccode,SeccodeList){
				//Cmd+="<security secid='" + SecidMap[seccode] + "' tradeno='1'/>"  
				Cmd+="<security secid='24"   "' tradeno='1'/>" ;
			}
			Cmd+="</command>";
			
			char buf[]="<command id='subscribe_ticks'>"
					"<security secid='21' tradeno='1'/>"  
					
					
				"</command>";

			printf(STR(Cmd));
			char* buffer=new char[512];
			strcpy(buffer,STR(Cmd));
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(buf));
			std::cout<<reinterpret_cast<const char*>(ss)<<std::endl;
			delete buffer;
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

	int C_TransaqConnector::subscribe(){
			/* 
			В команде 'subscribe' идентификаторы приведены для примера.
			В реальном коде необходимо использовать данные, присылаемые сервером
			*/
			std::cout<<"Sending \"subscribe\" command..."<<std::endl;
			char buffer[]="<command id='subscribe'>"
				"<alltrades><secid>26</secid></alltrades>"  
				"<quotations><secid>26</secid></quotations>" 
				"<quotes><secid>26</secid></quotes>"
				"</command>";
			//BYTE* ss = SendCommand(reinterpret_cast<BYTE*>("<command id='subscribe'>"
			//	"<alltrades><secid>26</secid></alltrades>"  
			//	"<quotations><secid>26</secid></quotations>" 
			//	"<quotes><secid>26</secid></quotes>"
			//	"</command>"));
			BYTE* ss = SendCommand(reinterpret_cast<BYTE*>(buffer));
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

	

	void C_TransaqConnector::OpenXML(){

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

	}

	void C_TransaqConnector::CloseXML(){
		xml_markets<<"</root>";
		xml_ticks<<"</root>";
		xml_securities<<"</root>";
		xml_sec_info_upd<<"</root>";
	}