TOPDIR=`pwd`
CROSS = 

CPP = $(CROSS)g++
CC  = $(CROSS)gcc
LD  = $(CROSS)ld
AR  = $(CROSS)ar
STRIP = $(CROSS)strip

CFLAGS += -Wall -O2 -Os
CFLAGS += -D_REENTRANT -msoft-float
CFLAGS += -g
LDFLAGS += -lpthread -lrt -lssl -lcrypto
AFLAGS += -r

CFLAGS+=-I $(TOPDIR)/Depends/include/libevent/linux/
##LDFLAGS+=-L ./Depends/lib/libevent/ -levent
LDFLAGS+=-L ./Depends/lib/libevent/linux -levent_core
LDFLAGS+=-L ./Depends/lib/libevent/linux -levent
##LDFLAGS+=-L ./Depends/lib/libcurl -lcurl
LDFLAGS+=-lcurl

SRC_PATH := src src/utils
#src/utils
SRC_FILE_CPP := $(foreach dir, ${SRC_PATH}, $(wildcard $(dir)/*.cpp))
LIB_CPP_OBJS = $(patsubst %.cpp, %.o,${SRC_FILE_CPP})

LIB_TARGET=proxy
ALL : $(LIB_TARGET)

$(LIB_TARGET) : $(LIB_CPP_OBJS)
	$(CPP) $(LIB_CPP_OBJS) -o $(LIB_TARGET) $(LDFLAGS) 
%.o: %.cpp
	$(CPP) -c $(CFLAGS) $(CFLAGS_NDBG) $< -o $@
clean:
	rm src/*.o -f
	rm $(LIB_TARGET)
