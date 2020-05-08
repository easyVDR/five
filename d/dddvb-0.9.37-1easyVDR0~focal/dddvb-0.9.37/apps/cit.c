#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <getopt.h>

uint32_t adapter = 0, device = 0, snum = 256, rnum = 256;

uint8_t fill[188]={0x47, 0x1f, 0xff, 0x10,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };

#if 0
uint8_t ts[188]={0x47, 0x0a, 0xaa, 0x00,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		   0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
#else
uint8_t ts[188]={0x47, 0x0a, 0xaa, 0x00,
   0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
   0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
   0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
   0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
   0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
   0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
   0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
   0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
   0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
   0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
   0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
		   0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa };
#endif

void proc_buf(uint8_t *buf, uint32_t *d)
{
	uint32_t c;
	static uint32_t tc=0;
	

	if (buf[1]==0x1f && buf[2]==0xff) {
		//printf("fill\n");
		return;
	}
	if (buf[1]==0x9f && buf[2]==0xff) {
		//printf("fill\n");
		return;
	}
	if (buf[1]==0x1a && buf[2]==0xbb) {
		tc++;
		if (!(tc&0xfff))
			printf("T %d\n", tc);
		return;
	}
	//printf("%02x %02x %02x %02x\n", buf[0], buf[1], buf [2], buf[3]);
	if (buf[1]!=0x0a || buf[2]!=0xaa)
		return;
	c=(buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|buf[7];
	if (c!=*d) {
		printf("CONT ERROR: got %08x  expected %08x\n", c, *d);
		*d=c;
	} else {
		if (memcmp(ts+8, buf+8, 180))
			printf("error\n");
		if (!(c&0xffff)) {
			printf("R %08x\r", c);
			fflush(0);
		}
	}
	(*d)++;
}

void *get_ts(void *a)
{
	uint8_t *buf;
	int len, off;
	int fdi;
	char fname[80];
	uint32_t d=0;

	buf = malloc(188*rnum);
	if (!buf)
		return NULL;
	sprintf(fname, "/dev/dvb/adapter%u/ci%u", adapter, device);
	printf("using %s for reading\n", fname);
	fdi = open(fname, O_RDONLY);
	if (fdi == -1) {
		printf("Failed to open %s for read: %m\n", fname);
		return NULL;
	}
	 
	while (1) {
		memset(buf, 0, 188*rnum);
		len=read(fdi, buf, 188*rnum);
		if (len<0)
			continue;
		//printf("read %u\n", len);
		if (buf[0]!=0x47) {
			read(fdi, buf, 1);
			continue;
		}
		for (off=0; off<len; off+=188) {
			proc_buf(buf+off, &d);
		}
	}	
}


int send(void)
{
	uint8_t *buf, *cts;
	int i;
	uint32_t c=0;
	int fdo;
	char fname[80];
	
	buf = malloc(188*snum);
	if (!buf)
		return -1;
	sprintf(fname, "/dev/dvb/adapter%u/ci%u", adapter, device);
	printf("using %s for writing\n", fname);
	fdo=open(fname, O_WRONLY);
	if (fdo == -1) {
		printf("Failed to open %s to write: %m\n", fname);
		exit(2);
	}

	while (1) {
		for (i=0; i<snum; i++) {
			cts=buf+i*188;
			memcpy(cts, ts, 188);
			cts[4]=(c>>24);
			cts[5]=(c>>16);
			cts[6]=(c>>8);
			cts[7]=c;
			//write(fdo, fill, 188);
			//printf("S %d\n", c); 
			c++;
			//usleep(100000+0xffff&rand());
			//usleep(1000);
		}
		write(fdo, buf, 188*snum);
	}
}


int main(int argc, char **argv)
{
	pthread_t th;

	while (1) {
                int option_index = 0;
		int c;
                static struct option long_options[] = {
			{"adapter", required_argument, 0, 'a'},
			{"device", required_argument, 0, 'd'},
			{"snum", required_argument, 0, 's'},
			{"rnum", required_argument, 0, 'r'},
			{"help", no_argument , 0, 'h'},
			{0, 0, 0, 0}
		};
                c = getopt_long(argc, argv, 
				"a:d:h",
				long_options, &option_index);
		if (c==-1)
 			break;
		
		switch (c) {
		case 'd':
			device = strtoul(optarg, NULL, 10);
			break;
		case 'a':
			adapter = strtoul(optarg, NULL, 10);
			break;
		case 's':
			snum = strtoul(optarg, NULL, 10);
			break;
		case 'r':
			rnum = strtoul(optarg, NULL, 10);
			break;
		case 'h':
			printf("cit -a<adapter> -d<device>\n");
			exit(-1);
		default:
			break;
			
		}
	}
	if (optind < argc) {
		printf("Warning: unused arguments\n");
	}
	printf("adapter %d, device: %d\n", adapter, device);
	memset(ts+8, 180, 0x5a);
	pthread_create(&th, NULL, get_ts, NULL);
	usleep(10000);
	send();
}