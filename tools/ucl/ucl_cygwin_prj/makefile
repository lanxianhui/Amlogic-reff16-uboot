uclobjs = $(obj)ucl_linux

UTILOBJS = $(patsubst %.c,$(uclobjs)/%.o,$(wildcard src/*.c))

CC=gcc
AR=ar
LD=gcc
RM=rm -rf 
MKDIR=@mkdir

CFLAGS = -g -O2 -Wall -Iinclude -I.
LDFLAGS = -Iinclude -I.
SYSLIB = -lwinmm 

all: $(uclobjs)/uclpack

$(uclobjs)/uclpack:   $(uclobjs)/uclpack.o $(uclobjs)/libucl_linux.a
	$(LD) $(LDFLAGS) -o $@ $^

$(uclobjs)/libucl_linux.a: $(UTILOBJS)
	$(AR) r $@ $^


$(uclobjs)/%.o : %.cpp
	$(MKDIR) -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@
	
$(uclobjs)/%.o : %.c
	$(MKDIR) -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(uclobjs)
	rm *.exe




