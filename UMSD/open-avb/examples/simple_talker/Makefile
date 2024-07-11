CC?=gcc
OPT=-O2 -g
CFLAGS=$(OPT) -Wall -Wextra -Wno-parentheses
#############################################################################
# jay.choi.no.lib.igb
# 1. Intel Ethernet driver, igb lib is excluded 
# INCFLAGS=-I../../lib/igb -I../../daemons/mrpd -I../common -I../../daemons/common
# LDLIBS=-ligb -lpci -lrt -lm -pthread
# LDFLAGS=-L../../lib/igb
#############################################################################
INCFLAGS=-I../../daemons/mrpd -I../common -I../../daemons/common
LDLIBS=-lpci -lrt -lm -pthread

all: simple_talker

simple_talker: simple_talker.o ../common/talker_mrp_client.o

simple_talker.o: simple_talker.c
	$(CC) $(CFLAGS) $(INCFLAGS) -c simple_talker.c

../common/talker_mrp_client.o: ../common/talker_mrp_client.c ../common/talker_mrp_client.h
	make -C ../common/ talker_mrp_client.o

%: %.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) simple_talker
	$(RM) `find . -name "*~" -o -name "*.[oa]" -o -name "\#*\#" -o -name TAGS -o -name core -o -name "*.orig"`
