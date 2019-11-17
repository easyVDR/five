1. Get lirc
   git clone -b release_0.9.1b --single-branch git://git.code.sf.net/p/lirc/git lirc_091b

2. install libtool-1.3.3 and automake-1.4 and autoconf-2.13 
   apt-get libtool
   apt-get automake
   apt-get autoconf
   apt-get man2html

3. copy patch and \daemons and \remotes to lirc root directory "[lirc]\"

2. change to folder lirc_091b and apply patch to Lirc 0.9.1b (included in yard2srvd directory yard2_lircd_091b_patch.txt)
   patch –p1 < yard2_lircd_091b_patch.txt

5. run
   ./autogen.sh  

6. run
   ./configure --with-driver=yard2 

7. run
   make

8. run
   make install

9. Start LIRC
   sudo daemons/lircd -n -H yard2
   (sudo mkdir /var/run/lirc)

Hint: Use sudo if you hvae problems with installation

