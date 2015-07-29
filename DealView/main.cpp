#include "MainWindow.h"

//S_RingEasyTicks ticks;

 int main(int argc, char *argv[])
 {

 	 
//  	 
//  	 C_StaticRingBuffer<S_Tick,100> TickRB;
//  	
//  	 
//  	 S_Tick a;
//  	 TickRB <<a;
//  	 ticks<<a;
//  	 ticks.Head()=a;

	 QApplication app(argc, argv);
	 //app.setWindowTitle("LefcoView");
	 //MyWidget widget;
	 MainWindow mainWindow;
	 mainWindow.show();
	 return app.exec();
 }