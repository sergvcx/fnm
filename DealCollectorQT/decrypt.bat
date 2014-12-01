"%AES%"\aescrypt -d fight.h.aes
if %ERRORLEVEL% neq 0 exit /b
del *.aes