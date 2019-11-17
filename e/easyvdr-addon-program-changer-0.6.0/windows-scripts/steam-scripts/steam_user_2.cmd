@echo off

rem # Skript um bei Steam den User zu wechseln
rem # In dem Skript steam_accounts.cmd müssen die Account Daten hinterlegt sein.

rem # Dieses Skrip kann man bei Steam der Bibliothek hinzufügen

rem # Menü "Spiele->steam-fremdes Spiel meine Bibliothek hinzufügen" wählen
rem # Dann Internetexplorer oder Wordpad ... wählen
rem # Anschließend das gewählte Programm 1x anklicken
rem # Jetzt Rechte Maustaste drücken und Eigenschaften wählen
rem # Hier den Namen z.B Steam-Account Heinz
rem # Ziel (Pfadname wo das Skript zu finden ist, es müssen alle Dateien im gleichen Verzeichniss sein) z.B "C:\Program Files (x86)\Steam-accounts\Steam_user_1.cmd"
rem # Start, hier den Namen des Verzeichnisses wo die Skripte gespeichert wurden eintragen z.B "C:\Program Files (x86)\Steam-accounts"
rem # Jetzt kann man in der Steam Bibliothek seine Accounts wählen.
rem #
rem # Steam beenden

taskkill /im steam.exe /f



rem # Account Daten laden

call steam_accounts.cmd


"C:\Program Files (x86)\Steam\Steam.exe" -login %Accountname_2% %Password_2%
