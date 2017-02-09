ifeq ($(SCIDB),)
  X := $(shell which scidb 2>/dev/null)
  ifneq ($(X),)
    X := $(shell dirname ${X})
    SCIDB := $(shell dirname ${X})
  endif
  $(info SciDB installed at $(SCIDB))
endif

# A development environment will have SCIDB_VER defined, and SCIDB
# will not be in the same place... but the 3rd party directory *will*
# be, so build it using SCIDB_VER .
ifeq ($(SCIDB_VER),)
  SCIDB_3RDPARTY = $(SCIDB)/3rdparty
else
  SCIDB_3RDPARTY = /opt/scidb/$(SCIDB_VER)/3rdparty
endif

INSTALL_DIR = $(SCIDB)/lib/scidb/plugins

# Include the OPTIMIZED flags for non-debug use
OPTIMIZED=-O2 -DNDEBUG
DEBUG=-g -ggdb3
CFLAGS = -pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing \
         -Wno-long-long -Wno-unused-parameter -fPIC -D_STDC_FORMAT_MACROS \
         -Wno-system-headers -isystem  $(OPTIMIZED) -D_STDC_LIMIT_MACROS -std=c99
CCFLAGS = -pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing \
         -Wno-long-long -Wno-unused-parameter -fPIC -D_STDC_FORMAT_MACROS \
         -Wno-system-headers -isystem  $(OPTIMIZED) -D_STDC_LIMIT_MACROS \
         -std=c++11 -DCPP11
INC = -I. -DPROJECT_ROOT="\"$(SCIDB)\"" -I"$(SCIDB_3RDPARTY)/boost/include/" \
      -I"$(SCIDB)/include" -I./extern

LIBS = -shared -Wl,-soname,libknn.so -ldl -L. \
       -L"$(SCIDB)/3rdparty/boost/lib" -L"$(SCIDB)/lib" \
       -Wl,-rpath,$(SCIDB)/lib:$(RPATH)

SRCS = Logicalknn.cpp \
       Physicalknn.cpp

CC := "/usr/bin/gcc-4.9"
CXX := "/usr/bin/g++-4.9"

all: libknn.so

clean:
	rm -rf *.so *.o

libknn.so: $(SRCS)
	@if test ! -d "$(SCIDB)"; then echo  "Error. Try:\n\nmake SCIDB=<PATH TO SCIDB INSTALL PATH>"; exit 1; fi
	$(CC) $(CFLAGS) $(INC) -c -o rowsort.o rowsort.c
	$(CC) $(CFLAGS) $(INC) -c -o merge.o merge.c
	$(CXX) $(CCFLAGS) $(INC) -o Logicalknn.o -c Logicalknn.cpp
	$(CXX) $(CCFLAGS) $(INC) -o Physicalknn.o -c Physicalknn.cpp
	$(CXX) $(CCFLAGS) $(INC) -o libknn.so plugin.cpp merge.o rowsort.o Logicalknn.o Physicalknn.o $(LIBS)
	@echo "Now copy libknn.so to $(INSTALL_DIR) on all your SciDB nodes, and restart SciDB."

test:
	echo write me XXX
