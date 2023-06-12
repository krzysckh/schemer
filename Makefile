CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic \
       -I./chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
       -g

LDFLAGS=-L./chibi-scheme -L/usr/local/lib -lchibi-scheme -lraylib -lm
TARGET=schemer
OFILES=schemer.o scm.o gui.o

SCHEME=./chibi-scheme/chibi-scheme
SCMFLAGS=-A ./chibi-scheme/lib/
#CHIBI-FFI=$(SCHEME) $(SCMFLAGS) ./chibi-scheme/tools/chibi-ffi

all: chibi $(OFILES) startup-image
	$(CC) $(LDFLAGS) -o $(TARGET) $(OFILES)
startup-image:
	$(SCHEME) $(SCMFLAGS) -d startup-image
.c.o:
	$(CC) $(CFLAGS) -c $<
chibi:
	$(MAKE) -C chibi-scheme
clean:
	rm -rf $(TARGET) *.o startup-image *.core
full-clean: clean
	$(MAKE) -C chibi-scheme clean
