#include "MainWindow.h"



 int main(int argc, char *argv[])
 {

	 QApplication app(argc, argv);
	 //app.setWindowTitle("LefcoView");
	 //MyWidget widget;
	 MainWindow mainWindow;
	 mainWindow.show();
	 return app.exec();
 }