@echo off

rem # Skript um bei Steam den User zu wechseln
rem # In dem Skript steam_accounts.cmd m�ssen die Account Daten hinterlegt sein.

rem # Dieses Skrip kann man bei Steam der Bibliothek hinzuf�gen

rem # Men� "Spiele->steam-fremdes Spiel meine Bibliothek hinzuf�gen" w�hlen
rem # Dann Internetexplorer oder Wordpad ... w�hlen
rem # Anschlie�end das gew�hlte Programm 1x anklicken
rem # Jetzt Rechte Maustaste dr�cken und Eigenschaften w�hlen
rem # Hier den Namen z.B Steam-Account Heinz
rem # Ziel (Pfadname wo das Skript zu finden ist, es m�ssen alle Dateien im gleichen Verzeichniss sein) z.B "C:\Program Files (x86)\Steam-accounts\Steam_user_1.cmd"
rem # Start, hier den Namen des Verzeichnisses wo die Skripte gespeichert wurden eintragen z.B "C:\Program Files (x86)\Steam-accounts"
rem # Jetzt kann man in der Steam Bibliothek seine Accounts w�hlen.
rem #
rem # Steam beenden


rem Warten bis das Netzwerk l�uft

:WAIT_NET
 ping -n 1 www.google.de >NUL
if errorlevel 1 goto WAIT_NET

rem # Account Daten laden

call "%USERPROFILE%\AppData\Roaming\Microsoft\windows\Start Menu\Programs\Startup\steam-scripts\steam_accounts.cmd"


"C:\Program Files (x86)\Steam\Steam.exe" -login %Accountname_1% %Password_1%
