#!/usr/bin/python

import httplib, urllib, re, sys, string, time, os

debug=0

# Variables
ARTIST=sys.argv[1]
TITLE=sys.argv[2]
LYRICSDIR="/tmp/"
FILENAME=""
FORCE_RETRIEVE=0

##############
# BROWSER : GOOGLE
##################
def browse(Head,Address):
	global nbimage
	if (debug):
		print("Searching through google.com")
	host = Head
	h = httplib.HTTP(host)
	h.putrequest('GET', Address)
	h.putheader('Host', host)
	h.putheader('User-agent', 'Mozilla')
	h.endheaders()
	returncode, returnmsg, headers = h.getreply()
	foo = h.getfile()
	where=foo.read()
	s = re.search("http.*.>here</A>", where)
	if (s):
		RESULT=string.strip(string.replace(s.group(0),"\">here</A>"," "))
		conn = urllib.urlopen(RESULT)
		data = conn.read()
		f = open (FILENAME+".adv.lyr", 'w')
		f.write (data)
		f.close ()
	f = open (FILENAME+".lyr", 'w')
	f.write ("")
	f.close ()
	sys.exit(0)



##############
# PARSE
##############
def parse_lyrics(lyrics):
    try:
        query = re.compile('</script></td></tr></table>(.*)<p><hr size=1', re.IGNORECASE | re.DOTALL)
        full_lyrics = re.findall(query, lyrics)
        final = full_lyrics[0].replace("<br />\r\n","\n")
    except:
        try:
            query = re.compile('</script></td></tr></table>(.*)<br><br><a href="#', re.IGNORECASE | re.DOTALL)
            full_lyrics = re.findall(query, lyrics)
            final = full_lyrics[0].replace("<br />\r\n","\n")
        except:
	    f = open (LYRICSDIR+"temp.lyrics", 'w')
	    f.write ("Unusual page layout, no parsing built in yet...")
	    f.close ()
            final = "Unusual page layout, no parsing built in yet..."
    return str(final)




##############
# LYRC.COM.AR
# http://lyrc.com.ar/tema.php?songname=SONGTITLE&artist=ARTISTNAME
##################################################################
def lyrc():
	found=1
	global nbimage
	if (debug>0):
		print "[Lyrc] Searching on lyrc.com.ar ("+ARTIST+"-"+TITLE+")"
	URL="http://lyrc.com.ar/en/tema1en.php?artist="+string.replace(ARTIST," ","%20")+"&songname="+string.replace(TITLE," ","%20")
	conn = urllib.urlopen(URL)
	data = conn.read()
	conn.close()
	s = re.search("<message>nothing found</message>", data)
	if (s):
		found=0
	if (found==0):
		#Search the links
		f = open (FILENAME+".lyrics.xml", 'w')
		f.close ()
		if (debug>0):
			print "[Lyrc] --> lyrics not found :( ("+ARTIST+"-"+TITLE+")"
		sys.exit(0)
	else:
		# Are there multiple results?
		s = re.findall("<song>", data)
		if (len(s)>1 and FORCE_RETRIEVE==0):
			#YES!
			f = open (FILENAME+".lyrics.xml", 'w')
#			data=string.replace(data,"\n<","<") #You shouldn't go do linefeed like that in XML!!!
#			data=string.replace(data,"&","&amp;")
			f.write (data)
			f.close ()
			if (debug>0):
				print "[Lyrc] --> multiple lyrics found :S ("+ARTIST+"-"+TITLE+")"
		else:
			#NO, get lyrics
			URL="http://lyrc.com.ar/en/tema1en.php?artist="+string.replace(ARTIST," ","%20")+"&songname="+string.replace(TITLE," ","%20")+"&act=1"
			conn = urllib.urlopen(URL)
			data = conn.read()
			conn.close()
			f = open (FILENAME+".lyrics.xml", 'w')
#			data=string.replace(data,"\n<","<") #You shouldn't go do linefeed like that in XML!!!
#			data=string.replace(data,"&","&amp;")
#                        data=string.replace(data,"","")
			f.write (data)
			f.close ()

                        final_lyrics=parse_lyrics(data)

                        final_lyrics=string.replace(final_lyrics,"<br />","")
                         
                        if len(final_lyrics)< 2:
			  f = open (LYRICSDIR+"temp.lyrics", 'w')
			  f.write ("Page for song found, but its empty")
			  f.close ()
			  print "Page for song found, but its empty"
			else:  
			  f = open (LYRICSDIR+"temp.lyrics", 'w')
			  f.write (final_lyrics)
			  f.close ()
                          print final_lyrics

			if (debug>0):
				print "[Lyrc] --> lyrics found :) ("+ARTIST+"-"+TITLE+")"
###############
# The main code

#Remove ending spaces...
ARTIST=string.rstrip(ARTIST)
TITLE=string.rstrip(TITLE)
FILENAME=LYRICSDIR+ARTIST+"-"+TITLE

#global ARTIST
#global TITLE
#global FORCE_RETRIEVE

if (debug>0):
	print "ARTIST   : "+ARTIST
	print "SONG     : "+TITLE
        print "FILENAME : "+FILENAME
	print "Filename (if found) : "+FILENAME+".lyrics.xml"

lyrc()

if (debug<1):
        os.remove(FILENAME+".lyrics.xml")
