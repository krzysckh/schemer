CC=clang
CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic \
       -I. -I./chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
	   -Wno-unused-command-line-argument \
	   -Wno-implicit-function-declaration \
       -D_POSIX_C_SOURCE=2 \
       -g

LDFLAGS=-L./chibi-scheme -L/usr/local/lib -lraylib -lm -lutil
TARGET=schemer
OFILES=unifont.o schemer.o scm.o gui.o \
	   scm/colors.o scm/plot.o scm/core.o scm/shapes.o \
	   chibi-scheme/lib/init-7.o
       # what the hell lmaoo

SCHEME=./chibi-scheme/chibi-scheme
SCMFLAGS=-A ./chibi-scheme/lib/ -q
#CHIBI-FFI=$(SCHEME) $(SCMFLAGS) ./chibi-scheme/tools/chibi-ffi

.PHONY: chibi
.SUFFIXES: .scm .o

all: chibi $(OFILES) startup-image.img
	$(CC) $(LDFLAGS) -o $(TARGET) $(OFILES) \
		./chibi-scheme/libchibi-scheme.a
startup-image.img:
	$(SCHEME) $(SCMFLAGS) -d startup-image.img
unifont.c:
	xxd -include ./third-party/unifont-15.0.06.ttf unifont.c
.c.o:
	$(CC) $(CFLAGS) -c $<
.scm.o:
	$(SCHEME) $(SCMFLAGS) ./bin/scm2bin.scm -lib=$@ $<
	$(CC) $(CFLAGS) -c -o $@ $<
chibi:
	$(MAKE) -C ./chibi-scheme clibs.c
	$(MAKE) -C ./chibi-scheme -B libchibi-scheme.a chibi-scheme-static \
		SEXP_USE_DL=0 \
			   CPPFLAGS="-DSEXP_USE_STATIC_LIBS -DSEXP_USE_STATIC_LIBS_NO_INCLUDE=0"
clean:
	rm -rf $(TARGET) *.o *.img *.core unifont.c scm/*.scm.c scm/*.o
full-clean: clean
	$(MAKE) -C chibi-scheme clean
cloc:
	cloc `ls | grep -v chibi-scheme | grep -v third-party`
