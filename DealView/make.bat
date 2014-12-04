rem set PATH=D:\SDK\Qt-4.4.4-static\bin;%PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
set PATH=%QT%\bin;%PATH%
call clean.bat
qmake -project
copy   DealView.pro /B + DealView.pri /B DealView.pro
qmake -tp vc DealView.pro