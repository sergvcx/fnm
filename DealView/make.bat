rem set PATH=D:\SDK\Qt-4.4.4-static\bin;%PATH%
rem call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
set PATH=%QTDIR%\bin;%PATH%
rem set QMAKESPEC=win32-msvc2005
set QMAKESPEC=win32-msvc2015
call clean.bat
qmake -project
copy   DealView.pro /B + DealView.pri /B DealView.pro
qmake -tp vc DealView.pro