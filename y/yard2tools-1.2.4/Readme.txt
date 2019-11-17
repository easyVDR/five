Y.A.R.D. 2 USB Linux Software description:
Linux Software is open source and under GPL licence! 

A Y.A.R.D 2 daemon is used to communicate with other application. 
An input event as keyboard is generated to communicate with other application.

For lirc IR applications a lirc daemon patch available or
an own lirc compartible server implementation can be used.

Requirement
- Y.A.R.D.2 hardware :-)
- Add your user to group "dialout"


Y.A.R.D 2 Linux software:
+-----------------------------------------------------+
|                   Installation                      |
+-----------------------------------------------------+

Installation from git:
----------------------------------------------
1.  Get Y.A.R.D 2 linux software 
    $ git clone git://git.assembla.com/yard2srvd.git yard2

2.  You need to install libusb und libbsd and its header files to be able
    to compile. For Debian, Ubuntu and Linux Mint this can be found in package 
    libusb-dev and libbsd-dev

3.  Create and change to the build directory
    $ mkdir build; cd build

4.  configure and make the software
    $ ../configure
    $ make
    If you get an error message "aclocal or automake: command not found", use this:
    $ touch ../aclocal.m4 ../Makefile.in ../configure ../config.h.in
    $ make

    If you still get an error message "aclocal or automake: command not found", install
    autoconf/automake.
    For Debian, Ubuntu and Linux Mint this can be found in package 
    - autoconf (check installation with autoconf --version)
    - automake (check installation with automake --version)
    Now change to the main package directory and regenerate the autotool files 
    $ autoreconf -v -i -f

    Change back to the build directory
    $ cd build

    Start again with step 4.

Now you can find the software in the build directory. You can copy it to any
location you like on your machine or

5.  Install the software to the standard location (/usr/local/bin)
    $ sudo make install


Installation from launchpad as debian package:
----------------------------------------------
1.  Get Y.A.R.D.2 ppa 
    $ sudo add-apt-repository ppa:yard2team/stable
	
2.  Update database
    $ sudo apt-get update
	
3.  Install yard2tools
    $ sudo apt-get install yard2tools


If you can not add repository, you can download the yard2tools package manually:
https://launchpad.net/~yard2team/


Installation from source tarball:
----------------------------------------------
1.  get Y.A.R.D 2 linux software as tarball from Launchpad. Go to
    https://launchpad.net/~yard2team/+archive/stable/+packages
    and click on the yard2tools-<VERSION> link.
    Now download the source tarball, by clicking on the link
    yard2tools_<VERSION>.orig.tar.gz     

2.  Extract the tarball and start with step 2. from the description
    "Installation from git"  



+-----------------------------------------------------+
|                      Note:                          |  
|     After installation a reboot is required !!      |
+-----------------------------------------------------+

After reboot, yard2srvd should be running (if you use udev). Check this by
  $ ps auxwww | grep yard
You should get something like this
    /usr/bin/yard2srvd -d -c /etc/yard2/yard2tools.cfg

If not, check the syslog for error outputs concerning YARD2


+-----------------------------------------------------+
|                Program description                  |
+-----------------------------------------------------+
	
yard2srvd: 
Commandline options:
    -c <file>:     use config file 
	           (should be in /etc/yard2/yard2tools.cfg)
    -d, --daemon:  Start in daemon mode.
    -D <device>, --device=<device>:
                   The tty device for yard2. Default: /dev/ttyYARD2
                   or /dev/ttyACM0, checked in this order.
    -I, --input:   Use the Linux input system to deliver the key codes.
    -i <file>, --irmap=<file>:
                   The IR map filename (with path).
    -L, --lirc:    Use lircd server with yard2 patch.
    -O, --ownlirc:
                   Use own lirc implementation.
    -l <val>, --loglevel=<val> :
                   Loglevel: 1:error, 2:warning, 3:info, 4:verbose, 5:debug
    -s, --syslog:  Use syslog for logging.
    -1, --rdelay1: Set repeat delay 1 in ms. Default 250, accepted values 50..1000
    -2, --rdelay2: Set repeat delay 2 in ms. Default 100, accepted values 50..1000
    -h, --help:    Help text.

Info: Options -I, -L and -O are mutual exclusive.

The udev rule use these parameters:
-d
-c /etc/yard2/yard2tools.cfg

Change udev it if you want other option
udev is located in: /lib/udev/rules.d/60-usb-yard2.rules

Normally change the option in the config file /etc/yard2/yard2tools.cfg 
and do not change the udev rule !

There are two repeat delays which can be configured.
The 1st is for the 1st button press and the 2nd for all additional repeats of the same button.
Normally the first value is higher then the 2nd.  

--------------------------------------------------------------------------------------------------------------------


yard2config:
This is the main configuration program for the Y.A.R.D.2 hardware.
yard2srvd must be started before.
Items (hopefully) self explaning
LCD can be configured, but currently only a beta patch for LCDproc (Textmode) is available.
--------------------------------------------------------------------------------------------------------------------


yard2wakeup: [-C | -I | -S] [2] [3]
Commandline options:
	-C: Read current wake up from Y.A.R.D.2
	-I: New wake up time in time_t
	-S: New wake up time as string (dd.mm.yyyy MM:HH)
	[2]: reduction time in minutes 
	[3]: -Poff: Shutdown system

Example: 
yard2wakeup -S “20.12.2012 20:10” 10 = program 20.12.2012 20:00 
yard2wakeup -I 1046958562 

It can also called directly with VDR –s option. 

Y.A.R.D will be programmed with the new time (or wakeup disabled) 
In this case the system will be shutdown if the time is in the future. 

For VDR an automatic wakeup script is available.
yaVDR: Disable other shutdown scripts in WEB front end by setting to "None"
       Enable Y.A.R.D.2 wake up by setting YARD2_ENABLED="yes" in file /etc/vdr/vdr-addon-yard2wakeup.conf
--------------------------------------------------------------------------------------------------------------------

yard2record:
With this program you can create a remote control mapping config for the lirc server implementation and Input(x) in yard2srvd.
The configuration file is default located: home/[user]/yardirmap.txt" 
(udev rule use this file from /etc/yard2 ! Copy yardirmap.txt to /etc/yard2/)

You specify the loaction also with "-i" option for yard2record e.g.:
sudo yard2record -i /etc/yard2/yardirmap.txt
to change the file directly in /etc/yard2/ .

Format: ACSII
[IR CODE 16char]@[BUTTON NAME]@[Event ID]@
Example:
0701020304000000@KEY_LEFT@106@

Diffrent mode for yard2record are available:
1: Enter Button Name and Event ID on your own
2: Enter Button Name and Event ID will be selected automatically if Button Name is from input.h
3: Enter Event ID and Button name will be selected automatically if Event ID    is from input.h
Easiest mode is 3.

For yaVDR and easyVDR the default keymap is shown in a table.

--------------------------------------------------------------------------------------------------------------------

yard2lcdtest:
For Y.A.R.D.2 LCD version a small LCD test utility is provided.
Start yard2lcdtest and enter commands:

C      : Clear LCD
G[line]: Goto line x start with 0 (G0 = set cursor to line 0)
W[Text]: Write text to LCD (e.g. WHalloWorld)
--------------------------------------------------------------------------------------------------------------------

yard2flash:
Firmware update utility for Y.A.R.D.2

Flash procedure:
1.  Start yard2config and enter "U" to enter update mode
2.  yard2flash --programm [FW File]

After firmware update, Y.A.R.D.2 will do reset.
If you use udev rule, yard2srvd will be stopped and restarted after flash is complete,
otherwise yard2srvd waits up to 300 seconds for a reconnet.
If firmware update take more time, you have to restart yard2srvd.

--------------------------------------------------------------------------------------------------------------------

lirctest:
To test the lirc communication for lircd and own lirc server, you can use ./lirctest.
(You can also use irw from lirc package)
This will connect to the lirc socket and print out the lirc string if remote control button is pressed.
--------------------------------------------------------------------------------------------------------------------


+-----------------------------------------------------+
|                Input Event server                   |
+-----------------------------------------------------+

yard2srvd can simulate a keyboard as an input event.
To activate it use -I option (yard2srvd default udev rule)

Use yard2record to generate a IR-Button <-> keyboard map file

This virtual keyboard can be used with any program.

This is the default mode in /etc/yard2/yard2tools.cfg.
It should run with any program which accept keyboard commands.
-> Tested with yaVDR, easyVDR -

Important: Lircd is not required ! 


+-----------------------------------------------------+
|                Own lirc server                      |
+-----------------------------------------------------+

yard2srvd has a small basic lirc compartible server implmented. 
To activate it use -O option (yard2srvd)
Starting/Using lirc is not necessray if you are using the implementation in yard2srvd.

For testing you have to start it with
sudo ./yard2srvd -O
sudo is necessary to create a lirc compartible socket in /var/run/lirc/lircd

The remote control config file is not compartible with the lirc.conf file.
To create a new one use yard2record

+-----------------------------------------------------+
|                lircd patch                          |
+-----------------------------------------------------+

yard2srvd can also be used with lircd.
To activate it use -L option (yard2srvd)
A readme.txt and patch is located in [yard2tools]/lirc


+-----------------------------------------------------+
|                Output monitoring                    |
+-----------------------------------------------------+
To see the output of yard2srvd use
  $ sudo tail -f /var/log/syslog


+-----------------------------------------------------+
|                   Maintainer                        |
+-----------------------------------------------------+

For the Maintainer:
----------------------------------------------------
First install autoconf, automake and libtool

# Regenerate all autotool files, after changing configure.ac or any Makefile.am.
  $ autoreconf -v -i -f

Steps to create the Debain Package on Launchpad (https://launchpad.net/~yard2team/+archive/stable)
As a prerequisite, you have to have a PGP key, a user account on Launchpad and the public key
uploaded to your Lauchpad account.
Moreover, you need to install the devscripts and dput packets. 

* After configure and make (step 4 above), create the source tarball in the build subdir
  $ make dist
 or
  $ make distcheck
 The latter will try to rebuild the package, after creating the tarball.

* Copy the source tarball to another location and rename it to have the form
  <package name>_<upstream version>.orig.tar.gz
  (Important: rename '-' to '_')

* Unpack the source and change into the unpacked directory:
  $ tar xzf <package name>_<upstream version>.orig.tar.gz
  $ cd <package name>-<upstream version>

* Now we need the debian directory made. The initial way to do this, is described in the
  tutorial at the end. But there is an easier way.
  - Get the already present debian tarball from Launchpad. Go to
    https://launchpad.net/~yard2team/+archive/stable/+packages
    and click on the yard2tools-<VERSION> link.
    Now download the debian extension tarball, by clicking on the link
    yard2tools_<VERSION>.debian.tar.gz 
  - Unpack it and copy the debian directory into the unpacked source directory.
  - Edit the debian/changelog file for the new version and describe what you are changing.
  - Save the debian directory else where for later use

* Build the debian source package
  $ debuild -S

* Upload the debian source package to Launchpad
  $ cd ..
  $ dput ppa:yard2team/stable <package name>_<version from debain/changelog>_source.changes

* Wait for an eMail from Launchpad with an OK or Error message.
  If you got the OK message, Launchpad will build the binary packages out of the just
  uploaded source package.

A more detailed tutorial can be found here:
  http://ubuntulinuxtipstricks.blogspot.co.at/2010/08/is-packaging-new-software-hard.html

The long story:
  http://packaging.ubuntu.com/html/

The original more longer story:
  https://wiki.debian.org/Packaging

