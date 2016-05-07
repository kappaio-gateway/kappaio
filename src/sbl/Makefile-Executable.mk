#====================================================================================
#     The MIT License (MIT)
#
#     Copyright (c) 2011 Kapparock LLC
#
#     Permission is hereby granted, free of charge, to any person obtaining a copy
#     of this software and associated documentation files (the "Software"), to deal
#     in the Software without restriction, including without limitation the rights
#     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#     copies of the Software, and to permit persons to whom the Software is
#     furnished to do so, subject to the following conditions:
#
#     The above copyright notice and this permission notice shall be included in
#     all copies or substantial portions of the Software.
#
#     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#     THE SOFTWARE.
#====================================================================================
# build rsserial executable when user executes "make" 

# mipsel => __LITTLE_ENDIAN__
# armhf => __LITTLE_ENDIAN__
# mips => __BIG_ENDIAN__

#ifdef ARCH
#  ifeq ($(ARCH),mipsel) 
#    $(info ARCH is $(ARCH), use little endian)
#    ENDIAN=__LITTLE_ENDIAN__
#  else ifeq ($(ARCH),mips)
#    $(info ARCH is $(ARCH), use big endian)
#    ENDIAN=__BIG_ENDIAN__
#  else 
#    $(error unknown architecture $(ARCH))
#  endif
#else
#  $(error Architecture "ARCH" is missing)
#endif

SRC:=${CURDIR}/
#CFLAGS+= -x c++
LIBNAME:=libsbl
SONAME:=$(LIBNAME).so.0
EXEC:=$(LIBNAME).so.0.0.0
LINKERNAME:=$(LIBNAME).so
OBJ= serial_bootload.o
	 	
#CPPFLAGS+=$(INCLUDE_DIR) -std=c++0x -D_TIMT_ -D $(processor_family) -D$(ENDIAN) -g -Wall -fPIC -pedantic #-fpermissive 
CPPFLAGS:=-fPIC -pedantic 
LDFLAGS+= -shared 
unexport LDFLAGS

VPATH=$(SRC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -Wl,-soname,$(SONAME) -o $(EXEC)
	ln -s $(EXEC) $(LINKERNAME)
	ln -s $(EXEC) $(SONAME)
	cp -d *.so* ${LIBDIR}
	cp -d *.h* ${LIBINCLUDE}
	touch ./.built
	
${CURDIR}/%.o: %.c

${CURDIR}/%.o: %.cpp
.PHONY : clean
clean:
	rm *.o *.so*
