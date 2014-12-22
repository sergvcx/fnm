#include "MainWindow.h"

#include "main.h"



 int main(int argc, char *argv[])
 {
	
	QDateTime dt;
	int t=sizeof(dt);
	QApplication app(argc, argv);
	//app.setWindowTitle("LefcoView");
	//MyWidget widget;
	MainWindow mainWindow;
	mainWindow.show();
	return app.exec();
 }