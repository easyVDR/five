#!/usr/bin/python
# coding: utf8

import serial, os, sys, time

#---- hier sind die Protokoll-Daten definiert ------------------------------------------

#               Name der im Menü,             Name im IR-Wakeup,  Code für Protokollbeschreibung
#               angezeigt wird                max. 16 Zeichen     (42 Bytes)

protokolle =   [["RC5",                        "RC5",              "01050E0000000C00020216122A27000000000000000000000000FFFFFFFFFFFF00FF2400000000000000"],
		["RC6 Mode0",                  "RC6 Mode0",        "0206163F1300000502020A091513000000000000000000000000FFFFFFFFFFFF00002400000000000000"],
		["RC6 Mode6 (MCE)",            "RC6 Mode6 (MCE)",  "0306263F13000F0502020A091513000000000000000000000000FFFFFFFFFFFF00062D00000000000000"],
		["RC6 MCE-Keyboard",           "RC6 MCE-Keyboard", "1306254200000001020207060D0D000000000000000000000000FFFFFFFFFFFF00432D00000000000000"],
		["HAMA MCE Remote",            "HAMA MCE Remote",  "0603112F0C000001020212102422000000000000000000000000FFFFFFFFFFFF00FF2D00000000000000"],
		["Harmony Profil: KLS VDR 1.6","KLS VDR 1.6",      "01050E0000000C00020316132A280000000000000000000000007F000000000000FF2400800500000000"],
		["Harmony Profil: VDR2",       "Harmony: VDR2",    "01050E0000000C00020215142A290000000000000000000000007F000000000000FF2400000F00000000"],
		["Soundgraph Imon Pad",        "Soundgr. ImonPad", "0F081D000000000001020C08000000000000000000000000000080FFFF0F000000FF4000490000100000"],
		["Sonavis / Revox",            "Sonavis / Revox",  "14031B0000000000020205040A09000000000000000000000000FFFFFFFFFFFF02FF2400000000000000"]]


		
read_value = ""				# globale Variable - enthält eingelesene Daten vom Gerät


#---- zuerst einige Hilfs-Funktionen ---------------------------------------------------------------

#---- C H E C K _ D E V I C E -----------------------# Prüft, ob der der Devicename existiert ---

def check_device(device):				
  handle = os.popen('ls /dev/ | grep '+device)
  line = handle.read()
  handle.close
  if line.find(device) == -1:
    return False
  return True


#**** ATRIC - Funktionen *********** Kommunikation mit dem IR-WakeupUSB ****************************

#---- A T R I C _ W R I T E _ R E A D --------------# sendet den übergebenen String an den Atric und ---
					            # wartet, falls reply=True ist, auf eine Antwort vom Gerät
def atric_write_read(str,reply):		    # die Funktion kehrt je nach Erfolg mit True/False zurück,		
  global read_value                                 # read_value enthält nach Rückkehr die eingelesenen Daten
  
  try:
    port = serial.Serial("/dev/"+device, 9600, timeout = 0.5)	# Port zum Device öffnen
  except:
    return False 						# Port konnte nicht geöffnet werden

  try:
#    port.write("L")					# zuerst IR-Empfang abschalten
    
    if len(str)>0: 					# nur wenn ein Befehl gesendet werden soll (str enthält Befehlssequenz),
      c=port.inWaiting()				#   Anzahl der Bytes im Empfangspuffer ermitteln
      if c>0:						#   wenn Daten im Empfangspuffer vorhanden sind,
	port.read(c)					#     dann diese zuerst einlesen (Puffer leeren)
      port.write(str)			 		#   danach den Befehl zum Atric senden
      
    if reply:						# wenn Antwort erwartet wird (reply == True)
      timeout=0						#   Zählvariable für Timeout auf 0 setzen
      count = port.inWaiting()				#   Anzahl der Bytes im Empfangspuffer lesen
      
      while ((timeout<10) and (count==0)):		# solange Empfangspuffer leer ist und Timeout-Zähler < 10 (max 1sec warten)
        time.sleep(0.1)					#   100ms warten
        count = port.inWaiting()			#   Anzahl der Bytes im Empfangspuffer lesen
        timeout+=1					#   Timeout-Zähler inkrementieren, dann Empfangspuffer erneut prüfen
        
      if count > 0:					# wenn Daten im Puffer
        read_value = port.read(count) or "FAIL"		#   versuchen die entspr. Anzahl Bytes einzulesen (wenn nix kommt, dann "FAIL")
      else:
        read_value = "FAIL"				#   wenn keine Daten im Puffer, dann auch "FAIL"
        
  except:
    read_value = "FAIL"					# bei Fehler den read_value ebenfalls auf "FAIL" setzen

  port.write("C")					# IR-Empfang wieder einschalten
  port.close()	 					# Port auf jeden Fall wieder schließen (falls andere Programme zugreifen wollen)
  if read_value == "FAIL":				# wenn "FAIL"
    return False					#   zurück mit False
  return True						#   sonst True


#---- C H E C K _ A T R I C -------------------------# Prüft, ob das Device ein IR-WakeupUSB ist ---

def check_atric():				

  if atric_write_read("VC",True):		# sende 'VC' = Atric-Gerätecode erfragen
    if ord(read_value) == 2:			# wenn Gerätecode 2 empfangen wurde
      return True				# ist es wohl der Atric IR-WakeupUSB
    
  return False					# sonst zurück mit False


#---- Read Version-------------------------# Prüft, ob das Device ein IR-WakeupUSB ist ---

def atric_read_version():				

  return atric_write_read("VV",True)		# sende 'VV' = Atric-Geräteversion erfragen


#---- A T R I C _ W R I T E _ R E M O T E N A M E ----# Schreibt den Namen der Fernbedienung in den IR-WakeupUSB ---

def atric_write_remoteName(remotename):				

  return atric_write_read("FNSACS"+remotename,False)	# sende 'FNSACS' = Namen der Fernbedienung schreiben + remoteName


#---- A T R I C _ W R I T E _ P R O T O C O L --------# Schreibt die IR-Protokolldefinition in den IR-WakeupUSB ---

def atric_write_protocol(protocol_array):				

  str=""
  for i in range(0,42):
    j = int(protocol_array[i*2]+protocol_array[(i*2)+1],16)	# Hex-Array in Binär-Array umwandeln
    str=str+(chr(j))

  return atric_write_read("FPSACS"+str,False)			# sende 'FPSACS' = Protkolldefinition schreiben
    

#---- A T R I C _ R E A D _ A C T I O N _ C O U N T --# liest die Anzahl der gespeicherten Schalt-Aktionen ---

def atric_read_action_count():				

  return atric_write_read("SCR",True)		      # sende 'SCR' = Count lesen, erwarte Antwort vom Gerät


#**** Eingabe-Funktionen zur Benutzer-Kommunikation **********************************************************  

#---- K E Y B O A R D _ Y E S _ O R _ N O ------------# erwartet Benutzer-Eingabe "J/N", gibt True/False zurück
						      # bricht bei ESC oder CTRL+C den Skript komplett ab
def keyboard_yes_or_no(text):                         # gibt "text" aus und erwartet "j","J","n","N" als Eingabe
  fehler = 1
  while fehler == 1:
    try:
      eingabe = raw_input(text)
      if eingabe <> 'j' and eingabe <> 'J' and eingabe <> 'n' and eingabe <> 'N':
	raise
      fehler = 0
    except:
      if 'eingabe' not in locals() or ord(eingabe) == 27:
	print "\n Abbruch...\n"
	sys.exit(0)
      print " Falsche Eingabe! Geben Sie nur 'J' oder 'N' ein!"
  
  if eingabe == 'j' or eingabe == 'J':
    return True
  else:
    return False


#---- K E Y B O A R D _ N U M B E R -----------------# erwartet Benutzer-Eingabe (Ziffer) ---
						     # bricht bei ESC oder CTRL+C den Skript komplett ab
def keyboard_number(text, minimal, maximal):	     # gibt "text" aus und erwartet Eingabe zwischen "minimal" und "maximal"
  fehler = 1
  while fehler == 1:
    try:
      eingabe = raw_input(text)
      if ord(eingabe[0]) == 27:
	raise
      zahl=int(eingabe)
      if zahl < minimal or zahl > maximal:
        raise
      fehler = 0
    except:
      if 'eingabe' not in locals() or ord(eingabe[0]) == 27:
	print "\n Abbruch...\n"
	sys.exit(0)
      print " Falsche Eingabe!"
      
  return zahl

#---- K E Y B O A R D _ S T R I N G -----------------# erwartet Benutzer-Eingabe (String) ---
						     # bricht bei ESC oder CTRL+C den Skript komplett ab
def keyboard_string(text):			     # gibt "text" aus und erwartet Zeichenkette als Eingabe
  fehler = 1
  while fehler == 1:
    try:
      eingabe = raw_input(text)
      if ord(eingabe[0]) == 27:
	raise
      fehler = 0
    except:
      if 'eingabe' not in locals() or ord(eingabe[0]) == 27:
	print "\n Abbruch...\n"
	sys.exit(0)
      print " Falsche Eingabe!"
      
  return eingabe




#---- Ende des Funktions-Bereichs, hier beginnt die Hauptroutine ------------------------


#**** H A U P T - R O U T I N E *********************************************************


os.system("clear");						# zuerst den Bildschirm löschen


# dann prüfen, welcher der vorgegebenen Anschlüsse existiert und ob da ein IR-WakeupUSB angeschlossen ist

devices = ["input/ir-auto_dtc","ttyatric","ttyACM0","ttyACM1","ttyACM2","ttyACM3","ttyACM4"]	# Liste der zu prüfenden Devicenamen, kann beliebig geändert/ergänzt werden

print "\n\n Suche nach einem angeschlossenen IR-WakeupUSB...\n"
found = False
for device in devices:
  if check_device(device):			# Devicename überhaupt vorhanden
    if check_atric():				# ein Atric IR-WakeupUSB an diesem Device ?
      found = True				# JA -> dann found-Flag setzen 
      break					# und Schleife vorzeitig beenden

if not found:					# wenn kein IR-Wakeup gefunden wurde   
  print "\n Es konnte kein IR-WakeupUSB gefunden werden.\n"
  if keyboard_yes_or_no("\n Wurde der IR-WakeupUSB an einen USB-Port angeschlossen? [J/N]: "):		# Nachfragen, ob überhaupt einer angeschlossen ist

    while not found:				# wenn "J" gedrückt wurde, nach dem Devicenamen fragen und danach Gerät erneut suchen
      
      device = keyboard_string("\n Geben Sie den Devicenamen an, unter dem der IR-WakeupUSB angemeldet ist. (ohne '/dev/')\n z.B. 'ttyACM0' oder drücken Sie die ESC-Taste + Enter zum Abbrechen: ")                             

      if check_device(device):			# Devicename überhaupt vorhanden
	found = check_atric()			# ein Atric IR-WakeupUSB an diesem Device -> dann found-Flag setzen 

      if not found:
	print "\n An dem Anschluss "+device+" wurde kein IR-WakeupUSB gefunden!\n"
  else:
    print "\n Schließen Sie zuerst den IR-WakeupUSB an einen USB-Port an\n und starten Sie das Programm danach erneut.\n"
    sys.exit(0)


# wenn wir hier ankommen, wurde ein IR-WakeupUSB gefunden und der dazugehörige Devicename steht in "device"

print "\n Ein IR-WakeupUSB wurde an /dev/"+device+" gefunden.\n"

eingabe=0
while eingabe!=3:

# gewünschte Funktion abfragen

  print "\n Wählen Sie die gewünschte Funktion aus.\n\n 1 = Fernbedienung auswählen\n 2 = Ein-/Ausschaltcodes bearbeiten\n 3 = Aktuelle Werte anzeigen und Programm Beenden\n"
  eingabe = keyboard_number(" Bitte geben die entsprechende Nummer ein: ",1,3)
  print "\n"


#---- Fernbedienungs-Definition auswählen ----------------------------------------------------

  if eingabe==1:				# Fernbedienung auswählen
    os.system("clear")				# zuerst den Bildschirm löschen
    print "\n Wählen Sie aus folgenden Fernbedienungen die passende aus:\n"
    for i in range(0,9):
      print("    "+str(i+1)+". "+protokolle[i][0])
    print("    0. zurück zum Hauptmenü\n")
    i = keyboard_number(" Bitte geben die entsprechende Nummer ein: ",0,9)
    
#---- ausgewähltes Protokoll zum Gerät übertragen --------------------------------------------

    if i>0:
      result = atric_write_remoteName(protokolle[i-1][1])
      if result:
        result = atric_write_protocol(protokolle[i-1][2])

      if result:
	print " Protokoll-Einstellungen wurden erfolgreich aufs Gerät übertragen\n"
      else:
        print " Bei der Übertragung ist ein Fehler aufgetreten!\n"
    
      raw_input(" Weiter mit Tastendruck")


#---- Ein-/Ausschaltcode bearbeiten -----------------------------------------------------------

  elif eingabe==2:

    if atric_read_action_count():
      print " Anzahl der gespeicherten Aktionen: "+str(ord(read_value[0]))
    else:
      print " Fehler!"
   
    raw_input(" Funktion ist noch nicht fertiggestellt\n\n Weiter mit Tastendruck")


#---- Aktuelle Werte ausgeben und Programm Beenden

os.system("clear");

print "\n\n Aktuelle Einstellungen im Atric-USB: \n"

if atric_read_version():
    print " Hardware-Version: "+str(hex(ord(read_value[0])))+"; Firmware-Version: "+str(hex(ord(read_value[1])))

print "\n\n Wenn die folgenden 3 Parameter (Atric_FB_*) in die lircd.conf.irman der zugehörigen Fernbedienung kopiert werden"
print "   dann wird beim auswählen der Fernbedienung auch immer gleich der Atric-USB passend parametriert"
print " easyvdr stellt die lircd.conf.irman mit passenden Konfigurationsparametern für den Atric-USB gerne für alle User bereit!\n"

if atric_write_read("FNR",True):
    print "\n Atric_FB_Name_Str=\""+str(read_value)+"\""
if atric_write_read("FPR",True):
    zeichenzahl = len(read_value)
    text = ""
    for i in range(0,zeichenzahl):
        zchn = "{:02X}".format(ord(read_value[i]))
        text=text+zchn

    print " Atric_FB_Conf_Str=\""+text+"\""
    #print "Def. RC6 Mode6 (MCE)0306263F13000F0502020A091513000000000000000000000000FFFFFFFFFFFF00062D00000000000000"

    if atric_read_action_count():
      #print "\n Anzahl der gespeicherten Schalt-Aktionen: "+str(ord(read_value[0]))
      Kommando="SAR"
      for j in range(0,ord(read_value[0])):
      #for j in range(0,6):
        Aktion = (chr(int(("0"+str(j)),16)))
        if atric_write_read(Kommando+Aktion,True):
          zeichenzahl = (((ord(read_value[0]))/32)+2)
	  #zchn = "{:02X}".format(ord(read_value[0]))
	  #anzzchn = "{:02X}".format((ord(read_value[0]))/32)
          #print "1. Zeichen="+zchn+" => Relevanter Wert="+anzzchn+" (+2)Zeichen der ausgelesenen Zeichen"
	  # es Werden immer 2 Byte zuviel Ausgelesen, mit dem Hex-Wert, der "OK" entspricht ?!
          text = ""
          for i in range(0,zeichenzahl):
            zchn = "{:02X}".format(ord(read_value[i]))
            #zchn = str(hex(ord(read_value[i])))
            text=text+zchn
          print " Atric_FB_Pwr_On_"+str(j)+"=\""+str(text)+"\" (Schaltaktion="+str(zeichenzahl)+"-Zeichen-Wert="+str((zeichenzahl-2))+" Zeichen des FB-Codes)"

      print  " "
      print "  Bsp-Schaltaktion=\"47E83D0F\"      (Schaltaktion=4-Zeichen-Wert=2 Zeichen des FB-Codes)"
      print  "            Byte 1 :4| |   |        Wert/2=Anz. Byte FB-Code, hier 2"
      print  "            Byte 1 : 7 |   |        Immer 7 Für Einschalter Eco"
      print  "            Byte 2 :  E8   |        Schalt- und LED-Fkt.(Bit ABCD.EFGH  - A ist msb)"
      print  "            Byte 3+:    3D0F        2..6 Byte FB-Code (hier 2) - siehe lirc.conf"
      print  "  Schalt-Funktion und LED-Signal beim Schalten (Byte 2)"
      print  "         msb Bit A : Ein           (1 : FB-Code schaltet ein)"
      print  "             Bit B : Aus           (1 : FB-Code schaltet aus)"
      print  "             Bit CD: LED bei Ein   (10: grüne / 01: rote / 00: keine LED bei Ein)"
      print  "             Bit EF: LED bei Aus   (10: grüne / 01: rote / 00: keine LED bei Aus)"
      print  "         lsb Bit GH: - ohne Bedeutung - "
      print  " Speicherdump:"
    else:
      print " Fehler, keine Schaltaktion"

Kommando="EB"
for j in range(0,8):
    Aktion = (chr(int(("0"+str(j)),16)))
    if atric_write_read(Kommando+Aktion,True):
      zeichenzahl = len(read_value)
      text = ""
      for i in range(0,zeichenzahl):
        zchn = "{:02X}".format(ord(read_value[i]))
        #zchn = str(hex(ord(read_value[i])))
        text=text+zchn
    print " Block_"+str(j)+"=\""+text+"\""
print "     Byte: 0 1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 A B C D E F"

#  os.system("clear");

