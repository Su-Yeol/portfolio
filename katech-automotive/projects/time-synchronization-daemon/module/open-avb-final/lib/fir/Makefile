OBJS=fir
INCL=fir.h
AVBLIB=libfir.a
#CFLAGS=-ggdb
CFLAGS=-fPIC

CC?=gcc
RANLIB?=ranlib

all: $(AVBLIB)

$(AVBLIB): $(addsuffix .o,$(OBJS))
	$(RM) $@
	$(AR) rcs $@ $^
	$(RANLIB) $@

fir.o: fir.c $(INCL)
	$(CC) -c $(INCFLAGS) $(CFLAGS) fir.c

clean:
	$(RM) `find . -name "*~" -o -name "*.[oa]" -o -name "\#*\#" -o -name TAGS -o -name core -o -name "*.orig"`
