#!/usr/bin/env python
# 
# script fuer easyvdr geaendert
# mango 05.01.2012
# mango add new url 26.04.2013
# mango add new url 08.09.2015

import urllib 
import sys
import os
import re
import shutil

# leere und loesche verzeichnis
folder_path = '/media/easyvdr01/net-radio'
for file_object in os.listdir(folder_path):
    file_object_path = os.path.join(folder_path, file_object)
    if os.path.isfile(file_object_path):
        os.unlink(file_object_path)
    else:
        shutil.rmtree(file_object_path)

# anlegen verzeichnis
verzeichnis = "/media/easyvdr01/net-radio"

if not os.path.exists(verzeichnis):
    os.mkdir(verzeichnis)

## change me ######
outdir = "/media/easyvdr01/net-radio/"
tmpdir = "/tmp/vtn/"
###################

## url = "http://74.86.56.147/vtunerapp/vtuner4vf/asp/appupdate/updateflush.asp?Flush=No"
## url = "http://74.86.58.246/vtunerapp/vtuner4vf/asp/appupdate/updateflush.asp?Flush=No"
url = "http://www.vtuner.com/vtunerapp/vtuner4vf/asp/appupdate/updateflush.asp?Flush=No"

name={}
stream={}
coder={}
location={}
music={}


# merkwuerdigen Zeichensatz der URLs korrigieren
def substChar(e):
    tab1=['o','p','q','r','s','t','u','v','w','x','y','z','a','b','c','d','e','f','g','h','i','j','k','l','m','n']
    tab2=['O','P','Q','R','S','T','U','V','W','X','Y','Z','A','B','C','D','E','F','G','H','I','J','K','L','M','N']
    s=""
    for j in e:
        if j in tab1:
	    s+=tab1[ord(j)-ord('a') ]
	elif j in tab2:
	    s+=tab2[ord(j)-ord('A') ]
	elif j=='!':
	   s+='0'
	elif j=='@':
	   s+='1'
	elif j=='#':
	   s+='2'
	elif j=='$':
	   s+='3'
	elif j=='%':
	   s+='4'
	elif j=='^':
	   s+='5'
	elif j=='&':
	   s+='6'
	elif j=='*':
	   s+='7'
	elif j=='(':
	   s+='8'
	elif j==')':
	   s+='9'
	elif j=='<':
	   s+=':'
	elif j=='>':
	   s+='='
	elif j=='6':
	   s+='&'
	else:
	    s+=j;
    return s

def createpls(basedir, datafile, idx1, idx2):
    f=open(datafile,"r")
    entry = f.readlines()
    f.close()

    fw=0
    fileName=""

    for i in entry:
	elements = i.split('\t')
	for id in elements[idx2].rstrip().split(' '):
	    try:
		# Streams fuer MP3-Plugin nur *.pls
		if coder[id].split(' ')[0]=="MP3": 
		    path = basedir + "/" + elements[idx1] + "/" # + location[id] + "/"
		    fname = re.sub("/","_", name[id] + " (" + music[id]) + ").m3u"
		    try:
			os.makedirs(path)
		    except:
			pass
		    f=open(path + fname, "w")
		    f.write(stream[id])
		    f.write("\n")
		    f.close()
		    print path + name[id]
	    except:
		print "illegal id:", id
		pass

# step 1: senderdatei holen und nach tmp
try:
   os.makedirs(outdir)
except:
   pass
try:
   os.makedirs(tmpdir)
except:
   pass

try:
    f=open(tmpdir + "stations","r")
    entry = f.readlines()
    f.close()
    print "found stations"
except:
    print "get ", url
    data = urllib.urlopen(url)
    entry = data.read()
    data.close()

    f=open(tmpdir + "stations","w")
    f.write(entry)
    f.close()

    f=open(tmpdir + "stations","r")
    entry = f.readlines()
    f.close()
    print "found stations"


# step 2: Senderdatei zerlegen und auch nach tmpdir, es entstehen 
# diverse vtn-Files
print "parsing..."

fw=0
fileName=""

os.mkdir
for i in entry:
    elements = i.split("::")
    if elements[0]=="file" and len(elements)==2:
        if (fw):
	    fw.close()
	fileName = elements[1].rstrip()
        fw = open (tmpdir + fileName,"w")
	continue
    mark = i.split(":")	
    if mark[0] == (fileName + " - start"):
        continue
    if mark[0] == (fileName + " - end"):
        continue

    if (fw!=0):
        if (fileName == "station.vtn"):
	    elements = i.split("\t")
	    n=0
	    i=""
	    for e in elements:
		if n==5 or n==6 or n==8:
		   i += substChar(e)
		else:   
		   i += e
		n=n+1
		if n<len(elements):
		    i+='\t'   
        fw.write(i)


        elements = i.split('\t')
        if len(elements)<6:
            continue
        name[elements[0]]     = elements[1]
        location[elements[0]] = elements[2]
        music[elements[0]]    = elements[3]
        coder[elements[0]]    = elements[4]

        try:
            str = elements[6]
            s=str.split('&')[0]
            if s[0]=='m':
                s = s[1:]
            for part in str.split('&'):
                subpart = part.split('=')
                if subpart[0]=="link":
                    s = subpart[1]
            stream[elements[0]] = s
        except:
            print "SHIT ..what a superb errorhandling ;)"

if (fw):
    fw.close()


# step 3: senderdatei in m3u-Files zerlegen
createpls(outdir + "laender/afrika",        tmpdir + "africa.vtn",2, 4)
createpls(outdir + "laender/europa",        tmpdir + "europe.vtn",2, 4)
createpls(outdir + "laender/internet",        tmpdir + "internet.vtn",2, 4)
createpls(outdir + "laender/ocean" ,        tmpdir + "ocean.vtn",2, 4)
createpls(outdir + "laender/mittelamerika", tmpdir + "centrala.vtn",2, 4)
createpls(outdir + "laender/mitost",        tmpdir + "middle.vtn",2, 4)
createpls(outdir + "laender/nordamerika",   tmpdir + "northa.vtn",2, 4)
createpls(outdir + "laender/suedamerika",   tmpdir + "southa.vtn",2, 4)
createpls(outdir + "musik",                 tmpdir + "music.vtn",2, 4)
createpls(outdir + "sprache",               tmpdir + "talk.vtn",2, 4)
createpls(outdir + "fun",                   tmpdir + "fun.vtn",0, 1)


# verzeichnis wechsel
from os import chdir
chdir('/media/easyvdr01/net-radio')


# wieder mal umlaut probleme beseitigen
os.system("convmv -f iso-8859-15 -t utf-8 -r --notest .")

# rechte vergeben
os.system( 'chown -R vdr:vdr /media/easyvdr01/net-radio')

# svdrpsend 
os.system("svdrpsend mesg Parsen und erstellen von Playlisten beendet!")         
