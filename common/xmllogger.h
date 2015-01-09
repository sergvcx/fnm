#pragma once
#include <QFile>
#include <QTextStream>
#include <QString>
#ifndef LOGGER_DEFINED
#define LOGGER_DEFINED

#define LOGGER_APPEND 1 //QIODevice::Append
#define LOGGER_WRITE 2 //QIODevice::WriteOnly
#define LOGGER_READWRITE QIODevice::ReadWrite
#define LOGGER_TEXT QIODevice::Text
class C_XML_Logger
{
	QFile* logFile;
	QTextStream* logStream;	
public:
	C_XML_Logger(QString filename, int mode=LOGGER_APPEND  ){ //QIODevice::OpenModeFlag mode
		logFile=0;
		logStream=0;
		logFile = new QFile(filename);
		
		if (mode==LOGGER_WRITE)
			if(logFile->open(QIODevice::WriteOnly | QIODevice::Text)){
				logStream = new QTextStream(logFile);
			}
		if (mode==LOGGER_APPEND)
			if(logFile->open(QIODevice::Append | QIODevice::Text)){
				logStream = new QTextStream(logFile);
			}
		_ASSERTE(logFile);
		_ASSERTE(logStream);
	}
	void Header(){
		*logStream<<"<?xml version='1.0' encoding='UTF-8'?>" <<"\n";
		logStream->flush();
	}
	void flush(){
		logStream->flush();
	}
	//bool Reopen();
	QTextStream& operator << (char* log){
		_ASSERTE(logStream);
		*logStream	<< log;
		return *logStream;
	}
	QTextStream& operator << (QString& log){
		_ASSERTE(logStream);
		*logStream	<< log;
		return *logStream;
	}
	void close(){
		//logStream->close();
		logFile->close();
	}
};

class C_XML_Logger2
{
	QFile logFile;
	QTextStream* logStream;	
public:
	C_XML_Logger2(QString filename, int mode=LOGGER_APPEND  ){ //QIODevice::OpenModeFlag mode
		//logFile=0;
		//logStream=0;
		//logFile = new QFile(filename);

		if (mode==LOGGER_WRITE)
			if(logFile.open(QIODevice::WriteOnly | QIODevice::Text)){
				logStream = new QTextStream(&logFile);
			}
			if (mode==LOGGER_APPEND)
				if(logFile.open(QIODevice::Append | QIODevice::Text)){
					logStream = new QTextStream(&logFile);
				}
				//_ASSERTE(logFile);
				//_ASSERTE(logStream);
	}
	void Header(){
		*logStream<<"<?xml version='1.0' encoding='UTF-8'?>" <<"\n";
		logStream->flush();
	}
	void flush(){
		logStream->flush();
	}
	//bool Reopen();
	QTextStream& operator << (char* log){
		_ASSERTE(logStream);
		*logStream	<< log;
		return *logStream;
	}
	QTextStream& operator << (QString& log){
		_ASSERTE(logStream);
		*logStream	<< log;
		return *logStream;
	}
	void close(){
		//logStream->close();
		logFile.close();
	}
};




#endif