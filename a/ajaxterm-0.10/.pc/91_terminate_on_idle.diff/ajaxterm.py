#!/usr/bin/env python

""" Ajaxterm """

try:
	import psyco
	psyco.profile()
except:
	pass

import array,cgi,fcntl,glob,mimetypes,optparse,os,pty,random,re,signal,select,sys,threading,time,termios,struct,pwd,Cookie
from datetime import datetime, timedelta

try:
	from hashlib import sha1
except ImportError:
	import sha
	sha1 = sha.new

os.chdir(os.path.normpath(os.path.dirname(__file__)))
# Optional: Add QWeb in sys path
sys.path[0:0]=glob.glob('../../python')

import qweb, codecs
utf8decoder = codecs.getincrementaldecoder('utf8')()

from socket import gethostname


def debug(str):
	now = datetime.datetime.now()
	print "%s - %s" % (now.isoformat(), str)

class Terminal:
	def __init__(self,width=80,height=24):
		self.width=width
		self.height=height
		self.init()
		self.reset()
	def init(self):
		self.esc_seq={
			"\x00": None,
			"\x05": self.esc_da,
			"\x07": None,
			"\x08": self.esc_0x08,
			"\x09": self.esc_0x09,
			"\x0a": self.esc_0x0a,
			"\x0b": self.esc_0x0a,
			"\x0c": self.esc_0x0a,
			"\x0d": self.esc_0x0d,
			"\x0e": None,
			"\x0f": None,
			"\x1b#8": None,
			"\x1b=": None,
			"\x1b>": None,
			"\x1b(0": None,
			"\x1b(A": None,
			"\x1b(B": None,
			"\x1b[c": self.esc_da,
			"\x1b[0c": self.esc_da,
			"\x1b]R": None,
			"\x1b7": self.esc_save,
			"\x1b8": self.esc_restore,
			"\x1bD": None,
			"\x1bE": None,
			"\x1bH": None,
			"\x1bM": self.esc_ri,
			"\x1bN": None,
			"\x1bO": None,
			"\x1bZ": self.esc_da,
			"\x1ba": None,
			"\x1bc": self.reset,
			"\x1bn": None,
			"\x1bo": None,
		}
		for k,v in self.esc_seq.items():
			if v==None:
				self.esc_seq[k]=self.esc_ignore
		# regex
		d={
			r'\[\??([0-9;]*)([@ABCDEFGHJKLMPXacdefghlmnqrstu`])' : self.csi_dispatch,
			r'\]([^\x07]+)\x07' : self.esc_ignore,
		}
		self.esc_re=[]
		for k,v in d.items():
			self.esc_re.append((re.compile('\x1b'+k),v))
		# define csi sequences
		self.csi_seq={
			'@': (self.csi_at,[1]),
			'`': (self.csi_G,[1]),
			'J': (self.csi_J,[0]),
			'K': (self.csi_K,[0]),
		}
		for i in [i[4] for i in dir(self) if i.startswith('csi_') and len(i)==5]:
			if not self.csi_seq.has_key(i):
				self.csi_seq[i]=(getattr(self,'csi_'+i),[1])
	def reset(self,s=u""):
		self.scr=array.array('l',[0x07000000]*(self.width*self.height))
		self.st=0
		self.sb=self.height-1
		self.cx_bak=self.cx=0
		self.cy_bak=self.cy=0
		self.cl=0
		self.sgr=0x07000000
		self.buf=""
		self.outbuf=""
		self.last_html=""
	def peek(self,y1,x1,y2,x2):
		return self.scr[self.width*y1+x1:self.width*y2+x2]
	def poke(self,y,x,s):
		pos=self.width*y+x
		self.scr[pos:pos+len(s)]=s
	def zero(self,y1,x1,y2,x2):
		w=self.width*(y2-y1)+x2-x1+1
		z=array.array('l',[0x07000000]*w)
		self.scr[self.width*y1+x1:self.width*y2+x2+1]=z
	def scroll_up(self,y1,y2):
		self.poke(y1,0,self.peek(y1+1,0,y2,self.width))
		self.zero(y2,0,y2,self.width-1)
	def scroll_down(self,y1,y2):
		self.poke(y1+1,0,self.peek(y1,0,y2-1,self.width))
		self.zero(y1,0,y1,self.width-1)
	def scroll_right(self,y,x):
		self.poke(y,x+1,self.peek(y,x,y,self.width))
		self.zero(y,x,y,x)
	def cursor_down(self):
		if self.cy>=self.st and self.cy<=self.sb:
			self.cl=0
			q,r=divmod(self.cy+1,self.sb+1)
			if q:
				self.scroll_up(self.st,self.sb)
				self.cy=self.sb
			else:
				self.cy=r
	def cursor_right(self):
		q,r=divmod(self.cx+1,self.width)
		if q:
			self.cl=1
		else:
			self.cx=r
	def echo(self,c):
		if self.cl:
			self.cursor_down()
			self.cx=0
		self.scr[(self.cy*self.width)+self.cx]=self.sgr|ord(c)
		self.cursor_right()
	def esc_0x08(self,s):
		self.cx=max(0,self.cx-1)
	def esc_0x09(self,s):
		x=self.cx+8
		q,r=divmod(x,8)
		self.cx=(q*8)%self.width
	def esc_0x0a(self,s):
		self.cursor_down()
	def esc_0x0d(self,s):
		self.cl=0
		self.cx=0
	def esc_save(self,s):
		self.cx_bak=self.cx
		self.cy_bak=self.cy
	def esc_restore(self,s):
		self.cx=self.cx_bak
		self.cy=self.cy_bak
		self.cl=0
	def esc_da(self,s):
		self.outbuf="\x1b[?6c"
	def esc_ri(self,s):
		self.cy=max(self.st,self.cy-1)
		if self.cy==self.st:
			self.scroll_down(self.st,self.sb)
	def esc_ignore(self,*s):
		pass
#		print "term:ignore: %s"%repr(s)
	def csi_dispatch(self,seq,mo):
	# CSI sequences
		s=mo.group(1)
		c=mo.group(2)
		f=self.csi_seq.get(c,None)
		if f:
			try:
				l=[min(int(i),1024) for i in s.split(';') if len(i)<4]
			except ValueError:
				l=[]
			if len(l)==0:
				l=f[1]
			f[0](l)
#		else:
#			print 'csi ignore',c,l
	def csi_at(self,l):
		for i in range(l[0]):
			self.scroll_right(self.cy,self.cx)
	def csi_A(self,l):
		self.cy=max(self.st,self.cy-l[0])
	def csi_B(self,l):
		self.cy=min(self.sb,self.cy+l[0])
	def csi_C(self,l):
		self.cx=min(self.width-1,self.cx+l[0])
		self.cl=0
	def csi_D(self,l):
		self.cx=max(0,self.cx-l[0])
		self.cl=0
	def csi_E(self,l):
		self.csi_B(l)
		self.cx=0
		self.cl=0
	def csi_F(self,l):
		self.csi_A(l)
		self.cx=0
		self.cl=0
	def csi_G(self,l):
		self.cx=min(self.width,l[0])-1
	def csi_H(self,l):
		if len(l)<2: l=[1,1]
		self.cx=min(self.width,l[1])-1
		self.cy=min(self.height,l[0])-1
		self.cl=0
	def csi_J(self,l):
		if l[0]==0:
			self.zero(self.cy,self.cx,self.height-1,self.width-1)
		elif l[0]==1:
			self.zero(0,0,self.cy,self.cx)
		elif l[0]==2:
			self.zero(0,0,self.height-1,self.width-1)
	def csi_K(self,l):
		if l[0]==0:
			self.zero(self.cy,self.cx,self.cy,self.width-1)
		elif l[0]==1:
			self.zero(self.cy,0,self.cy,self.cx)
		elif l[0]==2:
			self.zero(self.cy,0,self.cy,self.width-1)
	def csi_L(self,l):
		for i in range(l[0]):
			if self.cy<self.sb:
				self.scroll_down(self.cy,self.sb)
	def csi_M(self,l):
		if self.cy>=self.st and self.cy<=self.sb:
			for i in range(l[0]):
				self.scroll_up(self.cy,self.sb)
	def csi_P(self,l):
		w,cx,cy=self.width,self.cx,self.cy
		end=self.peek(cy,cx,cy,w)
		self.csi_K([0])
		self.poke(cy,cx,end[l[0]:])
	def csi_X(self,l):
		self.zero(self.cy,self.cx,self.cy,self.cx+l[0])
	def csi_a(self,l):
		self.csi_C(l)
	def csi_c(self,l):
		#'\x1b[?0c' 0-8 cursor size
		pass
	def csi_d(self,l):
		self.cy=min(self.height,l[0])-1
	def csi_e(self,l):
		self.csi_B(l)
	def csi_f(self,l):
		self.csi_H(l)
	def csi_h(self,l):
		if l[0]==4:
			pass
#			print "insert on"
	def csi_l(self,l):
		if l[0]==4:
			pass
#			print "insert off"
	def csi_m(self,l):
		for i in l:
			if i==0 or i==39 or i==49 or i==27:
				self.sgr=0x07000000
			elif i==1:
				self.sgr=(self.sgr|0x08000000)
			elif i==7:
				self.sgr=0x70000000
			elif i>=30 and i<=37:
				c=i-30
				self.sgr=(self.sgr&0xf8ffffff)|(c<<24)
			elif i>=40 and i<=47:
				c=i-40
				self.sgr=(self.sgr&0x0fffffff)|(c<<28)
#			else:
#				print "CSI sgr ignore",l,i
#		print 'sgr: %r %x'%(l,self.sgr)
	def csi_r(self,l):
		if len(l)<2: l=[0,self.height]
		self.st=min(self.height-1,l[0]-1)
		self.sb=min(self.height-1,l[1]-1)
		self.sb=max(self.st,self.sb)
	def csi_s(self,l):
		self.esc_save(0)
	def csi_u(self,l):
		self.esc_restore(0)
	def escape(self):
		e=self.buf
		if len(e)>32:
#			print "error %r"%e
			self.buf=""
		elif e in self.esc_seq:
			self.esc_seq[e](e)
			self.buf=""
		else:
			for r,f in self.esc_re:
				mo=r.match(e)
				if mo:
					f(e,mo)
					self.buf=""
					break
#		if self.buf=='': print "ESC %r\n"%e
	def write(self,s):
		for i in utf8decoder.decode(s):
			if len(self.buf) or (i in self.esc_seq):
				self.buf+=chr(ord(i)&255)
				self.escape()
			elif i == '\x1b':
				self.buf+=chr(ord(i)&255)
			else:
				self.echo(i)
	def read(self):
		b=self.outbuf
		self.outbuf=""
		return b
	def dump(self):
		r=u''
		for i in self.scr:
			r+=unichr(i&0xFFFFFF)
		return r
	def dumputf8(self):
		return self.dump().encode('utf8')
	def dumphtml(self,color=1):
		h=self.height
		w=self.width
		r=""
		span=u""
		span_bg,span_fg=-1,-1
		for i in range(h*w):
			q,c=divmod(self.scr[i],256*256*256)
			if color:
				bg,fg=divmod(q,16)
				bg &= 0x7
			else:
				bg,fg=0,7
			if i==self.cy*w+self.cx:
				bg,fg=1,7
			if (bg!=span_bg or fg!=span_fg or i==h*w-1):
				if len(span):
					r+='<span class="f%d b%d">%s</span>'%(span_fg,span_bg,cgi.escape(span.replace(u' ',u'\xa0').encode('utf8')))
				span=u""
				span_bg,span_fg=bg,fg
			if c == 0:
				span+=u' '
			elif c > 0x10000:
				span+=u'?'
			else:
				span+=unichr(c&0xFFFF)
			if i%w==w-1:
				span+=u'\n'
		r='<?xml version="1.0" encoding="utf8"?><pre class="term">%s</pre>'%r
		if self.last_html==r:
			return '<?xml version="1.0"?><idem></idem>'
		else:
			self.last_html=r
#			print self
			return r
	def __repr__(self):
		d=self.dumputf8()
		r=u""
		for i in range(self.height):
			r+="|%s|\n"%d[self.width*i:self.width*(i+1)]
		return r

class SynchronizedMethod:
	def __init__(self,lock,orig):
		self.lock=lock
		self.orig=orig
	def __call__(self,*l):
		self.lock.acquire()
		r=self.orig(*l)
		self.lock.release()
		return r

class Reaper:
	WAKEUP_FREQUENCY=5

	def __init__(self,multi):
		self.multi = multi
		self.thread = threading.Thread(target = self.reaper_thread)
		self.thread.setDaemon(True)
		self.thread.start()

	def reaper_thread(self):
		while True:
			time.sleep(Reaper.WAKEUP_FREQUENCY)
			self.multi.proc_kill_inactive()

class Multiplex:
	INACTIVE_PROCESS_TIMEOUT=120    # I guess this is the IP max packet lifetime

	def __init__(self,cmd=None,serverport=None):
		signal.signal(signal.SIGCHLD, signal.SIG_IGN)
		self.cmd=cmd
		self.serverport=serverport
		self.proc={}
		self.lock=threading.RLock()
		self.thread=threading.Thread(target=self.loop)
		self.alive=1
		# synchronize methods
		for name in ['create','fds','proc_read','proc_write','dump','die','run']:
			orig=getattr(self,name)
			setattr(self,name,SynchronizedMethod(self.lock,orig))
		self.thread.start()
	def create(self,w=80,h=25):
		pid,fd=pty.fork()
		if pid==0:
			try:
				fdl=[int(i) for i in os.listdir('/proc/self/fd')]
			except OSError:
				fdl=range(256)
			for i in [i for i in fdl if i>2]:
				try:
					os.close(i)
				except OSError:
					pass
			if self.cmd:
				cmd=['/bin/sh','-c',self.cmd]
			elif os.getuid()==0:
				cmd=['/bin/login']
			else:
				sys.stdout.write(gethostname() + u" login: ")
				login=sys.stdin.readline().strip()
				if re.match('^[0-9A-Za-z-_. ]+$',login):
					cmd=['ssh']
					cmd+=['-oPreferredAuthentications=keyboard-interactive,password']
					cmd+=['-oNoHostAuthenticationForLocalhost=yes']
					cmd+=['-oLogLevel=FATAL']
					cmd+=['-F/dev/null']
					if self.serverport:
						cmd+=['-p %s'%self.serverport]
					cmd+=['-l', login, 'localhost']
				else:
					os._exit(0)
			env={}
			env["COLUMNS"]=str(w)
			env["LINES"]=str(h)
			env["TERM"]="linux"
			env["PATH"]=os.environ['PATH']
			os.execvpe(cmd[0],cmd,env)
		else:
			fcntl.fcntl(fd, fcntl.F_SETFL, os.O_NONBLOCK)
			# python bug http://python.org/sf/1112949 on amd64
			fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack("HHHH",h,w,0,0))
			self.proc[fd]={'pid':pid,'term':Terminal(w,h),'buf':'','time':time.time()}
			return fd
	def die(self):
		self.alive=0
	def run(self):
		return self.alive
	def fds(self):
		return self.proc.keys()
	def proc_kill(self, fd):
		try:
			os.close(fd)
			os.kill(self.proc[fd]['pid'],signal.SIGHUP)
		except (IOError,OSError):
			pass
		del self.proc[fd]
	def proc_kill_inactive(self):
		t=time.time()
		for i in self.proc.keys():
			t0=self.proc[i]['time']
			if (t-t0)>Multiplex.INACTIVE_PROCESS_TIMEOUT:
				self.proc_kill(i)
	def proc_read(self,fd):
		try:
			t=self.proc[fd]['term']
			t.write(os.read(fd,65536))
			reply=t.read()
			if reply:
				os.write(fd,reply)
			self.proc[fd]['time']=time.time()
		except (KeyError,IOError,OSError):
			self.proc_kill(fd)
	def proc_write(self,fd,s):
		try:
			os.write(fd,s)
		except (IOError,OSError):
			self.proc_kill(fd)
	def dump(self,fd,color=1):
		try:
			return self.proc[fd]['term'].dumphtml(color)
		except KeyError:
			return False
	def loop(self):
		while self.run():
			fds=self.fds()
			i,o,e=select.select(fds, [], [], 1.0)
			for fd in i:
				self.proc_read(fd)
			if len(i):
				time.sleep(0.002)
		for i in self.proc.keys():
			try:
				os.close(i)
				os.kill(self.proc[i]['pid'],signal.SIGTERM)
			except (IOError,OSError):
				pass

class AjaxTerm:
	def __init__(self,cmd=None,index_file='ajaxterm.html',serverport=None,token=None):
		self.files={}
		self.token=token
		for i in ['css','html','js']:
			for j in glob.glob('*.%s'%i):
				self.files[j]=file(j).read()
		self.files['index']=file(index_file).read()
		self.mime = mimetypes.types_map.copy()
		self.mime['.html']= 'text/html; charset=UTF-8'
		self.multi = Multiplex(cmd,serverport)
		self.reaper = Reaper(self.multi)
		self.session = {}
		self.session_ip = {}
		self.sessions_limit = 20
		self.sessions_user_limit = 4
		m = sha1()
		m.update(os.urandom(128))
		self.cookie_name = m.hexdigest()
	def __call__(self, environ, start_response):
		req = qweb.QWebRequest(environ, start_response,session=None)
		if req.PATH_INFO.endswith('/u'):
			req.response_headers['Content-Type']='text/xml'
			uid=""
			if self.cookie_name not in req.request_cookies:
				req.write('<?xml version="1.0"?><idem></idem>')
				return req
			uid = req.request_cookies[self.cookie_name].value
			s=req.REQUEST["s"]
			k=req.REQUEST["k"]
			c=req.REQUEST["c"]
			w=req.REQUEST.int("w")
			h=req.REQUEST.int("h")
			ip="unknown"
			if environ.has_key("REMOTE_ADDR"):
				ip=environ['REMOTE_ADDR']
				if ip == "127.0.0.1" and environ.has_key("HTTP_X_FORWARDED_FOR"):
				    ip=environ["HTTP_X_FORWARDED_FOR"]

			if (uid+s) in self.session:
				term=self.session[uid+s]
				req.response_cookies.load(req.request_cookies[self.cookie_name].OutputString())
				req.response_cookies[self.cookie_name]['expires'] = datetime.utcnow()+timedelta(seconds=60)
			else:
				if not (w>2 and w<256 and h>2 and h<100):
					w,h=80,25
				# check if there aren't too many open sessions
				if len(self.session) < self.sessions_limit:
					count=0
					for i in self.session_ip.keys():
						if self.session_ip[i] == ip:
							count+=1
					if count <= self.sessions_user_limit:
						term=self.session[uid+s]=self.multi.create(w,h)
						self.session_ip[uid+s]=ip
					else:
						req.write('<?xml version="1.0"?><idem></idem>')
						return req
			if k:
				self.multi.proc_write(term,k)
			time.sleep(0.002)
			dump=self.multi.dump(term,c)
			if isinstance(dump,str):
				req.write(dump)
				req.response_gzencode=1
			else:
				del self.session[uid+s]
				del self.session_ip[uid+s]
				req.write('<?xml version="1.0"?><idem></idem>')
#			print "sessions %r"%self.session
		else:
			n=os.path.basename(req.PATH_INFO)
			if n in self.files:
				req.response_headers['Content-Type'] = self.mime.get(os.path.splitext(n)[1].lower(), 'application/octet-stream')
				req.write(self.files[n])
			elif (not self.token) or (req.REQUEST['token'] == self.token):
				if self.cookie_name not in req.request_cookies:
				    self.genSidCookie(req)
				req.response_headers['Content-Type'] = 'text/html; charset=UTF-8'
				req.write(self.files['index'])
			else:
				raise Exception('Not Authorized')
		return req
	def genSidCookie(self, req):
		m = sha1()
		m.update(os.urandom(160))
		req.response_cookies[self.cookie_name] = m.hexdigest()
		# try to set httponly if supported (added in 2.6)
		try:
		    req.response_cookies[self.cookie_name]['httponly'] = 1
		except (Cookie.CookieError):
		    pass
		req.response_cookies[self.cookie_name]['path'] = req.PATH_INFO
		req.response_cookies[self.cookie_name]['expires'] = datetime.utcnow()+timedelta(seconds=60)
		return req

def main():
	parser = optparse.OptionParser()
	parser.add_option("-p", "--port", dest="port", default="8022", help="Set the TCP port (default: 8022)")
	parser.add_option("-c", "--command", dest="cmd", default=None,help="set the command (default: /bin/login or ssh localhost)")
	parser.add_option("-l", "--log", action="store_true", dest="log",default=0,help="log requests to stderr (default: quiet mode)")
	parser.add_option("-d", "--daemon", action="store_true", dest="daemon", default=0, help="run as daemon in the background")
	parser.add_option("-P", "--pidfile",dest="pidfile",default="/var/run/ajaxterm.pid",help="set the pidfile (default: /var/run/ajaxterm.pid)")
	parser.add_option("-i", "--index", dest="index_file", default="ajaxterm.html",help="default index file (default: ajaxterm.html)")
	parser.add_option("-u", "--uid", dest="uid", help="Set the daemon's user id")
	parser.add_option("-s", "--serverport", dest="serverport", help="Use a different port than 22 to connect to the ssh server")
	parser.add_option("-t", "--token", dest="token", help="Set authorization token")
	(o, a) = parser.parse_args()
	if o.daemon:
		pid=os.fork()
		if pid == 0:
			#os.setsid() ?
			os.setpgrp()
			nullin = file('/dev/null', 'r')
			nullout = file('/dev/null', 'w')
			os.dup2(nullin.fileno(), sys.stdin.fileno())
			os.dup2(nullout.fileno(), sys.stdout.fileno())
			os.dup2(nullout.fileno(), sys.stderr.fileno())
			if os.getuid()==0 and o.uid:
				try:
					os.setuid(int(o.uid))
				except:
					os.setuid(pwd.getpwnam(o.uid).pw_uid)
		else:
			try:
				file(o.pidfile,'w+').write(str(pid)+'\n')
			except:
				pass
			print 'AjaxTerm at http://localhost:%s/ pid: %d' % (o.port,pid)
			sys.exit(0)
	else:
		print 'AjaxTerm at http://localhost:%s/' % o.port
	at=AjaxTerm(o.cmd,o.index_file,o.serverport,o.token)
#	f=lambda:os.system('firefox http://localhost:%s/&'%o.port)
#	qweb.qweb_wsgi_autorun(at,ip='localhost',port=int(o.port),threaded=0,log=o.log,callback_ready=None)
	try:
		qweb.QWebWSGIServer(at,ip='localhost',port=int(o.port),threaded=1,log=o.log).serve_forever()
	except KeyboardInterrupt,e:
		sys.excepthook(*sys.exc_info())
	at.multi.die()

if __name__ == '__main__':
	main()

