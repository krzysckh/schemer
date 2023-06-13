CC=clang
CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic \
       -I./chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
       -g

LDFLAGS=-L./chibi-scheme -L/usr/local/lib -lchibi-scheme -lraylib -lm
TARGET=schemer
OFILES=unifont.o schemer.o scm.o gui.o

SCHEME=./chibi-scheme/chibi-scheme
SCMFLAGS=-A ./chibi-scheme/lib/
#CHIBI-FFI=$(SCHEME) $(SCMFLAGS) ./chibi-scheme/tools/chibi-ffi

all: chibi $(OFILES) startup-image.img
	$(CC) $(LDFLAGS) -o $(TARGET) $(OFILES)
startup-image.img:
	$(SCHEME) $(SCMFLAGS) -d startup-image.img
unifont.c:
	xxd -include ./third-party/unifont-15.0.06.ttf unifont.c
.c.o:
	$(CC) $(CFLAGS) -c $<
chibi:
	$(MAKE) -C chibi-scheme
clean:
	rm -rf $(TARGET) *.o *.img *.core unifont.c
full-clean: clean
	$(MAKE) -C chibi-scheme clean
