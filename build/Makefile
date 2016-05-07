# build rsserial executable when user executes "make" 

SRC=../src/
INCLUDE_DIR=-I/usr/include -I$(SRC) 
#CFLAGS= $(INCLUDE_DIR) -c -pthread -g -std=c99 `xml2-config --cflags` #-Wall
CFLAGS= -x c++
CXXFLAGS = -std=c++11
CPPFLAGS= $(INCLUDE_DIR) -D_TIMT_ -DPROCESSOR_FAMILY=$(processor_family) -fpermissive -c  -pthread -g #`xml2-config --cflags` -Wall  
# the -lstdc++ is NOT for openwrt build, this is only for Itanium ABI
LDFLAGS= -o rsserial -lstdc++ -fno-common -pthread -lrt -ljansson #`xml2-config --libs` -Wall
OBJ= \
	rsserial.o \
	mtintf.o \
	mtproc.o \
	mtdebug.o \
	rssys.o \
	mtsys.o \
	mtzdo.o \
	mtaf.o \
	localintf.o \
	apsdb.o \
	systask.o \
	zapp.o \
	zbha.o \
	zbprofiles.o \
	zclgeneral.o \
	mtutil.o \
	zclclusters.o \
	webintf.o \
	serial_bootload.o \
	zdo.o \
	zha.o \
	hal.o \
	rsjson.o \
	mt_helper.o \
	afmessage_buffer.o
VPATH=$(SRC)

rsserial: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS)
${CURDIR}/%.o: %.c
#	$(CC) -c -o $@ $< 
${CURDIR}/%.o: %.cpp
#	$(CPP) -c -o $@ $< 
# remove object files and executable when user executes "make clean"
.PHONY : clean
clean:
	rm *.o rsserial 
