@echo off
rem # 11.09.14 Bleifuss
rem # Dieses Skript beendet Windows wenn kein Rechner im Netwerk gefunden wird

rem # In dem Skript steam_accounts.cmd müssen die Account Daten hinterlegt sein.


@echo off

set CLIENT_1=localhost
set CLIENT_2=vxxpc
set CLIENT_3=pxxc
set CLIENT_4=pxxc
set CLIENT_5=pcxxc


:loop1
rem 10 Minuten Pause
timeout /T 600 >NUL

 ping -n 1 %CLIENT_1% >NUL
 if errorlevel 1 (goto CL2)
 goto loop1

:CL2
echo cl2
 ping -n 1 %CLIENT_2% >NUL
 if errorlevel 1 (goto CL3)
 goto loop1

:CL3
 ping -n 1 %CLIENT_3% >NUL
 if errorlevel 1 (goto CL4)
 goto loop1

:CL4
 ping -n 1 %CLIENT_4% >NUL
 if errorlevel 1 (goto CL5)
 goto loop1

:CL5
 ping -n 1 %CLIENT_5% >NUL
 if errorlevel 1 (goto SHUTDOWN1)
 goto loop1


:SHUTDOWN1

shutdown /s /f /t 20

