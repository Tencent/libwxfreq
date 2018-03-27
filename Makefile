NAME = libwxfreq 
SERVERNAME = freq 
LIBNAME = libwxfreq.a
OUTPUTDIR = ./output
INCLUDE = $(OUTPUTDIR)/include/$(NAME)
LIB64 = $(OUTPUTDIR)/lib64
LIBINSTALLDIR = /usr/local/lib64/
HEADERINSTALLDIR = /usr/local/include/

CC = g++
LDFLAGS = -lwxfreq -pthread -L$(LIB64) -I $(dir $(INCLUDE))
CPPFLAGS = $(CFLAGS)
CPPFLAGS += -g -O0 -Wall -std=c++11
INC = -I ./util -I ./freq -I ./net --std=c++11

CPPFILE += $(wildcard freq/*.cpp)
CPPFILE += $(wildcard util/*.cpp)
OBJ = $(patsubst %.cpp,  %.o, $(CPPFILE))
DFILE = $(patsubst %.o,  %.d, $(OBJ))

NETCPPFILE += main.cpp
NETCPPFILE += $(wildcard net/*.cpp)
NETOBJ = $(patsubst %.cpp,  %.o, $(NETCPPFILE))
NETDFILE = $(patsubst %.o,  %.d, $(NETOBJ))


.PHONY : all clean test

$(SERVERNAME) : $(OBJ) $(NETOBJ) main.o
	@rm -rf $(DFILE)
	@rm -rf $(NETDFILE)
	$(CC) $(CPPFLAGS)  $^ -o $@.out -pthread -levent -lrt -static -L /data1/mm64/arthurzou/archive

$(LIBNAME) : $(OBJ)
	@rm -rf $(OUTPUTDIR)
	@rm -rf $(DFILE)
	@mkdir -p $(LIB64)
	@mkdir -p $(INCLUDE)
	ar scr $@ $(OBJ)
	mv $@  $(LIB64)
	cp freq/libwxfreq.h $(INCLUDE)
	@$(CC) -MM freq/libwxfreq.h $(INC) | \
		tr " " "\n" | grep ".h" | xargs -I{} cp {} $(INCLUDE)
	@find $(INCLUDE) -name "*.h" | xargs sed -i 's;#include ";#include "$(NAME)/;'

%: test/%.cpp $(LIBNAME)
	$(CC) $(CPPFLAGS)  $< -o $@.out $(LDFLAGS)


-include $(DFILE)
-include $(NETDFILE)

%.d: %.cpp
	@$(CC) $(INC) -MT $*.o -MM $< > $@ && \
		echo -e '\t$$(CC) $$(CPPFLAGS) $(INC) -c $$< -o $$@' >> $@

clean:
	@echo -n "clean workspace ..."
	@rm  -rf $(OUTPUTDIR) $(OBJ) $(NETOBJ) $(NETDFILE) $(DFILE) *.out
	@echo -e "\t\t\033[31m[done]\033[0m"

install: $(LIBNAME)
	cp -r $(INCLUDE)  $(HEADERINSTALLDIR)
	cp -r $(LIB64)/$(LIBNAME) $(LIBINSTALLDIR)
