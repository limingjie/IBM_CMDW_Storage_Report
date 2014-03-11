#Compiler
CC        = gcc
CXX       = g++

#Extension of executable
EXEEXT    = .exe
#Extension of resource object
RCOBJEXT  = _rc.o

#Top path
DEVHOME   = /c/Develop
MINGWHOME = $(DEVHOME)/MinGW

#SQLite threadsafe option
#SQLITEDEF = -DSQLITE_THREADSAFE=1
SQLITEDEF =

#Flags and includes for compiler
#CFLAGS    = -mthreads -Wall -Wundef -O2
CFLAGS    = -Wall -Wundef -O2
#CXXFLAGS  = -mthreads -Wall -Wundef -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing
CXXFLAGS  = -Wall -Wundef -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing
INCLS     = -I. -I$(MINGWHOME)/include

#Flags and library paths for linker
#LDFLAGS   = -mthreads -mwindows
#LDFLAGS   = -mwindows
LIBS      = -lrpcrt4 -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 \
            -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32

#Source codes
SQLITE    = sqlite3.c sqlite3.h genhtml.h
DATABASE  = SQLite3Database.cpp SQLite3Database.h

#Object files
OBJ       = SQLite3Database.o import.o genhtml.o report.o
SQLITEOBJ = sqlite3.o
OBJS      = $(SQLITEOBJ) $(OBJ)

#Targets
all: report$(EXEEXT)

report$(EXEEXT): $(OBJS)
	@echo "==>Linking project..."
	$(CXX) -o $@  $(OBJS)  $(LDFLAGS)  -L$(MINGWHOME)/lib $(LIBS)

report.o: report.cpp report.h import.h SQLite3Database.h
	@echo "==>Compiling report.cpp..."
	$(CXX) -c -o $@  $(INCLS)  $(CXXFLAGS) $<
	@echo

genhtml.o: genhtml.cpp genhtml.h SQLite3Database.h
	@echo "==>Compiling genhtml.cpp..."
	$(CXX) -c -o $@  $(INCLS)  $(CXXFLAGS) $<
	@echo

import.o: import.cpp import.h SQLite3Database.h
	@echo "==>Compiling import.cpp..."
	$(CXX) -c -o $@  $(INCLS)  $(CXXFLAGS) $<
	@echo

SQLite3Database.o: SQLite3Database.cpp SQLite3Database.h sqlite3.h
	@echo "==>Compiling SQLite3Database.cpp..."
	$(CXX) -c -o $@  $(INCLS)  $(CXXFLAGS)  $<
	@echo
	
sqlite3.o: sqlite3.c sqlite3.h
	@echo "==>Compiling sqlite3.c..."
	$(CC) -c -o $@ -O2  $(SQLITEDEF)  $<
	@echo

clean: 
	rm -f $(OBJ)
	rm -f report$(EXEEXT)
