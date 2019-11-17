@echo off
rem # 11.09.14 Bleifuss
rem # Steam Autostart setup  
rem #"%AUTOSTART_PATH%\%SOURCE_PATH%\steam-scripts\steam_autostart.cmd"
rem # Dieses Skript kopiert das steam_user_1.cmd skript in den Autostart
rem # Außerdem wird die Datei steam_accounts angelegt wenn sie nicht vorhanden ist
rem # Es werden alle Datein auf den Desktop kopiert

rem # In dem Skript steam_accounts.cmd müssen die Account Daten hinterlegt sein.

set SOURCE_PATH=windows-tools
set AUTOSTART_PATH=%USERPROFILE%\AppData\Roaming\Microsoft\windows\Start Menu\Programs\Startup


if NOT exist "steam_accounts.cmd" (
 copy  steam_accounts_template.cmd  "steam_accounts.cmd"
)

if NOT exist "%AUTOSTART_PATH%\%SOURCE_PATH%\steam-scripts" (
 xcopy "..\steam-scripts" "%AUTOSTART_PATH%\steam-scripts\" /S /I
)

cd "%AUTOSTART_PATH%\steam-scripts"

if NOT exist "%AUTOSTART_PATH%\steam_autostart.cmd" (
 copy  steam_autostart.cmd  "%AUTOSTART_PATH%\steam_autostart.cmd"
)


echo Beim nächsten PC Start wird Steam mit den Konto Daten von Steam_user_1 gestartet
pause
