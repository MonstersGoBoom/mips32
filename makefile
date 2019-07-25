M32_SRCS = m32_disasm.c
M32_LINK = -s 
M32_OBJS=$(subst .c,.o,$(M32_SRCS))
CFLAGS= -Os 
LDFLAGS= -s 
CC=gcc

#.c.o: 
#	$(CC) $(CFLAGS) -c -o $@ $<

all: m32_disasm.exe

m32_disasm.exe: $(M32_OBJS)
	$(CC) $(CFLAGS) -s -o $@ $(M32_OBJS)

clean:
	rm -v -f $(M32_OBJS) *~
	
