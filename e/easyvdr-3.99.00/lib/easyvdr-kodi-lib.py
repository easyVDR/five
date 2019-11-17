#!/usr/bin/python
#25.12.2015 gb
import os
import sys
if(sys.argv[1])=="shutdown":
 os.system("echo SHUTDOWN >/tmp/.pch-exec")
elif(sys.argv[1])=="reboot":
 os.system("echo REBOOT >/tmp/.pch-exec")
elif(sys.argv[1])=="frontend":
 os.system("echo FRONTEND >/tmp/.pch-exec")