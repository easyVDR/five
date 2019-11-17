#!/usr/bin/python

import httplib, urllib, re, sys, string, time, os

debug=1
show_evolution=4
output=0

#################
# Cover Site Code
#################
# 1 -> amazon.com
# 2 -> amazon.fr
# 3 -> darktown
# 4 -> images.google.com
# 5 -> coveralia

########################
# Some general functions
########################

def save_image(image_url,number):
        CMD="wget "+image_url+" -O "+OUTPUT+"_"+repr(number -1)+".jpg"
	cprint ("Command: " +CMD)

        os.system(CMD)

#	conn=urllib.urlopen(address)
#	data=conn.read()
#	conn.close()
#	f = open (OUTPUT+"_"+repr(number -1)+".jpg", 'wb')
#	f.write (data)
#	f.close ()

def save_data(buffer,number):
	f = open (OUTPUT+"_"+repr(number -1)+".jpg", 'wb')
#	f = open ("cover_"+repr(number)+".jpg", 'wb')
	f.write (buffer)
	f.close ()

def exit():
	global info_win
#	f = open (FILENAME+".result", 'w')
#	f.write (str(nbimage))
#	f.close ()
	if (show_evolution):
		if (nbimage>1):
			cprint(str(nbimage)+ " Covers gefunden, exiting :)")
		if (nbimage==1):
			cprint("1 Cover gefunden, exiting :)")
		if (nbimage==0):
			cprint("Kein Cover gefunden, exiting :(")
	if (output==1):
		time.sleep(3)
		info_win.destroy(info_win)
	sys.exit(0)
	
def cprint(text):
	global info_win
	if (output==1):
		info_win.change_text(text)
	else:
		print text

############
# AMAZON.COM
# No pattern : we use the search engine
# Look for "class=tiny", which generally indicates the cover
#############################################################################################
def amazon():
	global nbimage
	if (show_evolution):
		cprint("Suche auf amazon.com")
	AMAZONSEARCH=string.lower(string.replace(ARTIST," ","%20"))+"%20"+string.lower(string.replace(ALBUM," ","%20"));
	URLAMAZON="http://www.amazon.com/exec/obidos/search-handle-url/index%3Dmusic%26field-keywords%3D"+AMAZONSEARCH
	if (debug):
		print URLAMAZON
	conn = urllib.urlopen(URLAMAZON)
	data = conn.read()
	conn.close()
	z=re.search("were unable to find exact matches",data)
	if not(z): # on "z" -> really nothing found...
		s = re.search("href=.*.class=productImage", data)
		if (s):
			#Lucky, just one page!
			nbimage=nbimage+1
			if (show_evolution):
				cprint("Cover "+str(nbimage)+" gefunden auf amazon.com")
			z= re.search("http://.*.jpg",s.group())
			if not(z):
				z= re.search("http://.*.gif",s.group())
			save_image(z.group(),nbimage)
		else:
			#Search links... They're recognisable by "vi=glance, or v=glance"
			look="/exec.*.vi=glance"
			s = re.search(look, data)
			if not(s):
				look="/exec.*.v=glance&s=music"
				s = re.search(look, data)
			if not(s):
				look="/exec.*.v=glance.*.img src="
				s = re.search(look, data)
				if (s):
					s = re.search("/exec.*.v=glance",s.group())
			if (s):
				#Opening link... Hope it's a good one
				conn=urllib.urlopen("http://www.amazon.com"+s.group())
				data=conn.read()
				conn.close()
				s = re.search("href=.*.class=productImage", data)
				if (s):
					#Cover found
					nbimage=nbimage+1
					if (show_evolution):
						cprint ("Cover "+str(nbimage)+" gefunden auf amazon.com")
					z= re.search("http://.*.jpg",s.group())
					if not(z):
						z= re.search("http://.*.gif",s.group())
					save_image(z.group(),nbimage)
		
###########
# AMAZON.FR
# No pattern : we use the search engine
# In best case, we found only one page... Now look for "Agrandissez cette image", which generally
# indicates the cover
#############################################
def amazonfr():
	global nbimage
	nbamazonfr=0
	if (show_evolution):
		cprint("Suche auf amazon.fr")
	AMAZONSEARCH=string.lower(string.replace(ARTIST," ","%20"))+"%26field-title%3D"+string.lower(string.replace(ALBUM," ","%20"));
	URLAMAZON="http://www.amazon.fr/exec/obidos/search-handle-url/index%3Dmusic-fr%26field-artist%3D"+AMAZONSEARCH	
	if (debug):
		print URLAMAZON
	conn = urllib.urlopen(URLAMAZON)
	data = conn.read()
	conn.close()
	s = re.search("http://images-eu.*.Agrandissez cette image", data)
	if (s):
		#Lucky, just one page!
		nbimage=nbimage+1
		if (show_evolution):
			cprint("Cover "+str(nbimage)+" gefunden auf amazon.fr")
		z= re.search("http://.*.jpg",s.group())
		if not(z):
			z= re.search("http://.*.gif",s.group())
		save_image(z.group(),nbimage)
		nbamazonfr=1
	else:
		#Search links... They're recognisable by "exec.*.qid"
		# I add ".*.img src" to get an album and not a side product (book, etc..)
		look="/exec.*.qid.*.img src="
		s = re.search(look, data)
		if (s):
			#Opening link... Hope it's a good one
			conn=urllib.urlopen("http://www.amazon.fr"+s.group())
			data=conn.read()
			conn.close()
			s = re.search("http://images-eu.*.Agrandissez cette image", data)
			if (s):
				#Cover found
				nbimage=nbimage+1
				nbamazonfr=1
				if (show_evolution):
					cprint("Cover "+str(nbimage)+" gefunden auf amazon.fr")
				z= re.search("http://.*.jpg",s.group())
				if not(z):
					z= re.search("http://.*.gif",s.group())
				save_image(z.group(),nbimage)
	if (nbamazonfr==0):
		#Maybe we could look without "ARTIST=xx" and "ALBUM=yy" but "LOOK=xx+yy"
		AMAZONSEARCH=string.lower(string.replace(ARTIST," ","%20"))+"%20"+string.lower(string.replace(ALBUM," ","%20"));
		URLAMAZON="http://www.amazon.fr/exec/obidos/search-handle-url/index%3Dmusic-fr%26field-keywords%3D"+AMAZONSEARCH	
		conn = urllib.urlopen(URLAMAZON)
		data = conn.read()
		conn.close()
		s = re.search("http://images-eu.*.Agrandissez cette image", data)
		if (s):
			#Lucky, just one page!
			nbimage=nbimage+1
			if (show_evolution):
				cprint("Cover "+str(nbimage)+" gefunden auf amazon.fr")
			z= re.search("http://.*.jpg",s.group())
			if not(z):
				z= re.search("http://.*.gif",s.group())
			save_image(z.group(),nbimage)
		else:
			#Search links... They're recognisable by "exec.*.qid"
			# I add ".*.img src" to get an album and not a side product (book, etc..)
			look="/exec.*.qid.*.img src="
			s = re.search(look, data)
			if (s):
				#Opening link... Hope it's a good one
				conn=urllib.urlopen("http://www.amazon.fr"+s.group())
				data=conn.read()
				conn.close()
				s = re.search("http://images-eu.*.Agrandissez cette image", data)
				if (s):
					#Cover found
					nbimage=nbimage+1
					if (show_evolution):
						cprint("Cover "+str(nbimage)+" gefunden auf amazon.fr")
					z= re.search("http://.*.jpg",s.group())
					if not(z):
						z= re.search("http://.*.gif",s.group())
					save_image(z.group(),nbimage)

##############
# DARKTOWN.COM
# pattern is for front, back : 
# http://img.darktown.com/audio/x/artist_name_with_underscore_-_album_name_with_underscore_a.jpg
# http://img.darktown.com/audio/x/artist_name_with_underscore_-_album_name_with_underscore_b.jpg
# Or sometimes : (First letter is uppercase)
# http://img.darktown.com/audio/x/Artist_name_with_underscore_-_Album_name_with_underscore_a.jpg
# http://img.darktown.com/audio/x/Artist_name_with_underscore_-_Album_name_with_underscore_b.jpg
# Where x is the first letter of artistname
###########################################
def darktown():
	global nbimage
	if (show_evolution):
		cprint("Suche auf darktown.com")
	DARKSEARCH=string.lower(string.replace(ARTIST+" "," ","_"))+"-_"+string.lower(string.replace(ALBUM+" "," ","_"));
	HEAD=DARKSEARCH[0]
	URLDARK="http://img.darktown.to/cover/audio/download/"+HEAD+"/"+DARKSEARCH
	if (debug):
		print URLDARK
	conn = urllib.urlopen(URLDARK+"a.jpg")
	data = conn.read()
	BUFF=data[2]+data[3]+data[4]
	if (not(re.search("DOC", BUFF))):
		#Hurray, we've found something
		nbimage=nbimage+1
		if (show_evolution):
			cprint("Cover "+str(nbimage)+" gefunden auf darktown.com")
		save_data(data,nbimage)
		#Maybe a back cover too?
		conn = urllib.urlopen(URLDARK+"b.jpg")
		data = conn.read()
		BUFF=data[2]+data[3]+data[4]
		if (not(re.search("DOC", BUFF))):
			#Yes, back is there too!
			nbimage=nbimage+1
			if (show_evolution):
				cprint("Cover "+str(nbimage)+" gefunden auf darktown.com")
			save_data(data,nbimage)

##############
# GOOGLE.COM
# Using image search feature from google :
# http://images.google.com/images?q=artist+name+song+name
# then getting href=/imgres?imgurl=URL&imgrefurl
###########################################
def google():
	global nbimage
	if (show_evolution):
		cprint("Suche Cover auf images.google.com")
	SEARCH=string.lower(string.replace(ARTIST+" "," ","+"))+string.lower(string.replace(ALBUM," ","+"));

#	URL= "http://images.google.com/images?q="+SEARCH
#	conn = urllib.urlopen(URL)
#	data = conn.read()
#	conn.close()

	URL="/images?q="+SEARCH
	host = "images.google.com"
	h = httplib.HTTP(host)
	h.putrequest('GET', URL)
	h.putheader('Host', host)
	h.putheader('User-agent', 'Mozilla')
	h.endheaders()
	returncode, returnmsg, headers = h.getreply()
	foo = h.getfile()
	data=foo.read()

	# .jpg
	s = re.search("imgurl=.*?(jpg&imgrefurl)",data)
	if (s):
		s= re.search("imgurl=(?!.*imgurl=).*?(jpg&imgrefurl)",s.group())
	# .JPG
	if (not(s)):
		s = re.search("imgurl=.*?(JPG&imgrefurl)", data)
		if (s):
			s = re.search("imgurl=(?!.*imgurl=).*?(JPG&imgrefurl)", data)
	# .jpeg
	if (not(s)):
		s = re.search("imgurl=.*?(jpeg&imgrefurl)", data)
		if (s):
			s = re.search("imgurl=(?!.*imgurl=).*?(jpeg&imgrefurl)", data)
	# .gif
	if (not(s)):
		s = re.search("imgurl=.*?(gif&imgrefurl)", data)
		if (s):
			s = re.search("imgurl=(?!.*imgurl=).*?(jpeg&imgrefurl)", data)		
	# .GIF
	if (not(s)):
		s = re.search("imgurl=.*?(GIF&imgrefurl)", data)
		if (s):
			s = re.search("imgurl=(?!.*imgurl=).*?(GIF&imgrefurl)", data)		
	if (s):
		#Hurray, we've found something
		nbimage=nbimage+1
		if (show_evolution):
			cprint("Cover "+str(nbimage)+" gefunden auf images.google.com")

		image_url=string.replace(s.group(),"imgurl=","")
		image_url=string.replace(image_url,"&imgrefurl","")
		if (debug):
		    print image_url

		save_image(image_url,nbimage)					

#                conn = urllib.urlopen(image_url)
#		data = conn.read()
#                BUFF=data[2]+data[3]+data[4]
#                if (not(re.search("DOC", BUFF))):
#                        #Yes, back is there too!
#                        nbimage=nbimage+1
#                        if (show_evolution):
#				cprint("Cover "+str(nbimage)+" found on images.google.com")
#			save_image(data,nbimage)					


##############
# COVERALIA.COM
# pattern is for front, back : 
# http://www.coveralia.com/audio/x/Artist_Name-Album_Name-Frontal.jpg
# http://www.coveralia.com/audio/x/Artist_Name-Album_Name-Trasera.jpg
# http://www.coveralia.com/audio/x/Artist_Name-Album_Name-CD.jpg
# Where x is the first letter of artistname
# If artistname=albuname, then use "Artist_Name--Frontal.jpg , etc...
###########################################
def coveralia():
        found=1
	global nbimage
	if (show_evolution):
		cprint("Suche Cover auf coveralia.com")
	if (string.lower(ARTIST)==string.lower(ALBUM)):
		SEARCH=string.replace(string.capwords(ARTIST)+"--"," ","_");
	else:
		SEARCH=string.replace(string.replace(string.capwords(ARTIST)+"-"+string.capwords(ALBUM)+"-","."," ")," ","_");
	HEAD=string.lower(SEARCH[0])
	URL="http://www.coveralia.com/audio/"+HEAD+"/"+SEARCH
	if (debug):
		print URL

	conn = urllib.urlopen(URL+"Frontal.jpg")
	data = conn.read()
	conn.close()
	s = re.search("HTTP 404", data)
	if (s):
	        found=0
	else:
	        found=1	

        if (found==0):		
	        cprint("coveralia: Kein Frontcover gefunden")
        else:
	        #Hurray, we've found something
        	nbimage=nbimage+1
		cprint("Cover "+str(nbimage)+" gefunden auf coveralia.com")
		save_data(data,nbimage)


	#Maybe a back cover too?
	conn = urllib.urlopen(URL+"Trasera.jpg")
	data = conn.read()
	conn.close()
        s = re.search("HTTP 404", data)
	if (s):
	        found=0
	else:
	        found=1
		
	if (found==0):	
	        cprint("coveralia: Kein Backcover gefunden")
        else:
         	#Yes, back is there too!
		nbimage=nbimage+1
		cprint("Cover "+str(nbimage)+" gefunden auf coveralia.com")
		save_data(data,nbimage)


	#Maybe a cd cover too?
	conn = urllib.urlopen(URL+"CD.jpg")
	data = conn.read()
	conn.close()
	s = re.search("HTTP 404", data)
	if (s):
	        found = 0
	else:
	        found = 1       

        if (found==0):
                cprint("coveralia: Kein CD-Inlet gefunden")
	else:
		#Yes, cd is there too!
		nbimage=nbimage+1
		cprint("Cover "+str(nbimage)+" gefunden auf coveralia.com")
		save_data(data,nbimage)

#############
# Grab Covers
#############
def get_covers():
	getall=sys.argv[1]
	sites=sys.argv[2]
	for site in sites[:]:
		if site == '1':
			amazon()
			if getall == '0':
				if nbimage > 0:
					exit()
		if site == '2':
			amazonfr()
			if getall == '0':
				if nbimage > 0:
					exit()
##		if site == '3':
##			darktown()
##			if getall == '0':
##				if nbimage > 0:
##					exit()
		if site == '3':
			google()
			if getall == '0':
				if nbimage > 0:
					exit()
##		if site == '5':
##			coveralia()
##			if getall == '0':
##				if nbimage > 0:
##					exit()
	exit()
	

###############
# The main code

COVERDIR=sys.argv[3]
ARTIST=sys.argv[4]
ALBUM=sys.argv[5]
OUTPUT=sys.argv[6]
FILENAME=""
CMD=""

#l=len(sys.argv)
#r=range(4,l)
global nbimage
nbimage=0
where=0
#for i in r:
#	BUFF=sys.argv[i]
#	if (re.search("__00__", BUFF)):
#		where=where+1
#	else:
#		if (where==0):
#			ARTIST=ARTIST+BUFF+" "
#		else:
#			if (where==1):
#				ALBUM=ALBUM+BUFF+" "
#			else:
#				#This is for "advanced search"
#
#				FILENAME=FILENAME+BUFF+" "
#Remove ending spaces...
ARTIST=string.rstrip(ARTIST)
ALBUM=string.rstrip(ALBUM)
FILENAME=string.rstrip(FILENAME)
OUTPUT=string.rstrip(OUTPUT)

if (where==2):
	#This is for "advanced search"
	FILENAME=COVERDIR+ARTIST+" - "+ALBUM+"("+FILENAME+") "
else:
	FILENAME=COVERDIR+ARTIST+" - "+ALBUM

if (debug):
        print "Coverdir : "+COVERDIR
	print "Artist : "+ARTIST
	print "Album : "+ALBUM
	print "Output : "+OUTPUT
	print "Filename : "+FILENAME+".jpg"


get_covers()
