/* vim: set sw=8 ts=8 si : */
/*
 * Serial line daemon program for shutdown of your server.
 * A blinking LED indicates the server is running.
 * The program uses the serial interface lines
 * DTR (output: data terminal ready), RTS (output:request to send)  
 * and CD (input: carrier detect)
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * See http://www.gnu.org/copyleft/ for details.
 *
 * Written by Guido Socher <guido@linuxfocus.org> 
 *
 * Some ideas were taken from an earlier progam called 
 * cpanel written by Mirko Dölle <dg2fer@qsl.net>
 */
#define VERINFO "version 0.1"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


static int fd = 0;
enum out {txd, dtr, rts, all};
enum in  {cd, rxd, dsr, cts, ri};
enum state {on, off, toggle};

/* Signal handler: all led off then exit */
void offandexit(int code)
{
	int state = 0;
	ioctl(fd, TIOCMSET, &state);
	exit(0);
}

/* Switch the LEDs on or off */
int setled( enum out whichled, int fd, enum state onoff, int *ledstate){
	int bitpat;
	/* the blinking LED first */
	if       (whichled == rts){ bitpat=TIOCM_RTS;
	}else if (whichled == dtr){ bitpat=TIOCM_DTR;
	}else if (whichled == all){ 
		bitpat=(TIOCM_RTS | TIOCM_DTR);
	}
	printf("RTS, DTR, TXD : %i, %i\n",
	       (bitpat & TIOCM_RTS), 
	       (bitpat & TIOCM_DTR));
	if (onoff == on){
		/* on */
		*ledstate |= bitpat;
	}else if(onoff == off ) {
		*ledstate &= ~bitpat;
	}else{
		*ledstate ^= bitpat;
	}
	ioctl(fd, TIOCMSET, ledstate);
	return *ledstate;
}

/* get the current state of the push button 
 * and return it. */
int getpushbutton(int fd)
{
	int state;
	int button=0;
	ioctl(fd, TIOCMGET, &state);	/* read interface */
	if (state & TIOCM_CTS) { button += 1; }
	if (state & TIOCM_DSR) { button += 2; }
	if (state & TIOCM_RNG) { button += 4; }
	if (state & TIOCM_CAR) { button += 8; }
        if (button > 0) {
	  printf("%i %i %i %i ",
	         (state & TIOCM_CTS)>0, 
	         (state & TIOCM_DSR)>0,
	         (state & TIOCM_RNG)>0,
	         (state & TIOCM_CAR)>0);
	}
	usleep(500000);
	setled(all, fd, toggle, &state);
	return (button);
}

void help()
{
	printf("Help not implemented yet!  ");
#ifdef VERINFO
	puts(VERINFO);
#endif	 
exit(0); 
}


int main(int argc, char **argv)
{
	int lstate = 0;
	int i = 0;
	int but;
	pid_t pid;
	fd_set readfs;
	int state;

	/* exactly one argument must be given */
	if (argc != 2)
		help();
	/* open device */
	fd = open(argv[1], O_RDWR | O_NDELAY);
	if (fd < 0) {
		fprintf(stderr, "ERROR: can not open \"%s\"\n", argv[1]);
		exit(2);
	}

	/* catch signals INT and TERM and switch of LEDs before 
	 * terminating */
	signal(SIGINT, offandexit);
	signal(SIGTERM, offandexit);

	/* first set a defined state, all off */
	ioctl(fd, TIOCMSET, &lstate);
	/* set DTR to high */
        setled( dtr, fd, 1, &lstate);
        setled( rts, fd, 1, &lstate);

	while(1){
		FD_SET(fd, &readfs);
		select(fd, &readfs, NULL, NULL, NULL);
		if (FD_ISSET(fd, &readfs)) {
			but = 0;
			if (state & TIOCM_CTS) { but += 1; }
			if (state & TIOCM_DSR) { but += 2; }
			if (state & TIOCM_RNG) { but += 4; }
			if (state & TIOCM_CAR) { but += 8; }
			if (but > 0) {
				printf("button: %i\n",but);
			}
		}	
	}
	/* we never get here */
	return (2);
}
