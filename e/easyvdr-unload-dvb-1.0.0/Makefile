#--------- dont touch here. -------------------------------------------------------------
OBJS = $(shell ls -x *.c | sed -e 's/\.c/\.o/g')
#--------- you may change opts here at own risk. ----------------------------------------
CC = $(shell which g++)
CFLAGS = -D_GNU_SOURCE -Wall -Werror=overloaded-virtual -Wformat=2 -Wswitch-default
#--------- adopt paths to your needs. ---------------------------------------------------
INCLUDES=-I .

all: unload_dvb

unload_dvb: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $<

clean:
	@-rm -f $(OBJS)
	@-rm -f unload_dvb
