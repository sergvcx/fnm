"%AES%"\aescrypt -e fight.h
if %ERRORLEVEL% neq 0 exit /b
del fight.h