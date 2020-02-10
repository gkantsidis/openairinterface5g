CC	= gcc
DEPS	= OpenAirCommon.h ldpc.h
OBJ	= ldpc.o

ROOT 	= ../../../
OPENAIR1 = $(ROOT)/openair1
CODING	= $(ROOT)/openair1/PHY/CODING
BINARIES = $(ROOT)/cmake_targets/phy_simulators/build/
LIBPHY	= libPHY_NR.a

CFLAGS	= -I. -I$(CODING) -I$(OPENAIR1) -fpic -DOPENAIRDLL_EXPORTS -D__UNIX__ -Wall -O3

PYTHONDIR = ../../OpenAirPython/OpenAirLDPC/.libs/

%.o:	%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libLDPC.so: $(OBJ) $(BINARIES)/$(LIBPHY)
	$(CC) -shared -o libLDPC.so $(OBJ) $(BINARIES)/$(LIBPHY)
	cp libLDPC.so ../../OpenAirPython/OpenAirLDPC/.libs/libLDPC-x64.so

ALL: libLDPC.so

.PHONY: clean

clean:
	rm -f *.o *~ core

