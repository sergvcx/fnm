rem set PATH=D:\SDK\Qt-4.4.4-static\bin;%PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2005
call clean.bat
qmake -project
copy   TestStrategy.pro /B + TestStrategy.pri /B TestStrategy.pro
qmake -tp vc TestStrategy.pro