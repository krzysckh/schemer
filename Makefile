CC=clang
CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic \
       -I./chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
       -D_POSIX_C_SOURCE=2 \
       -g

LDFLAGS=-L./chibi-scheme -L/usr/local/lib -lraylib -lm -lutil
TARGET=schemer
OFILES=unifont.o schemer.o scm.o gui.o

SCHEME=./chibi-scheme/chibi-scheme-static
SCMFLAGS=-A ./chibi-scheme/lib/ -q
#CHIBI-FFI=$(SCHEME) $(SCMFLAGS) ./chibi-scheme/tools/chibi-ffi

.PHONY: chibi

all: chibi $(OFILES) startup-image.img
	$(CC) $(LDFLAGS) -o $(TARGET) $(OFILES) \
		./chibi-scheme/libchibi-scheme.a
startup-image.img:
	$(SCHEME) $(SCMFLAGS) -d startup-image.img
unifont.c:
	xxd -include ./third-party/unifont-15.0.06.ttf unifont.c
.c.o:
	$(CC) $(CFLAGS) -c $<
chibi:
	$(MAKE) -C ./chibi-scheme clibs.c
	$(MAKE) -C ./chibi-scheme -B libchibi-scheme.a chibi-scheme-static \
		SEXP_USE_DL=0 \
	       	CPPFLAGS="-DSEXP_USE_STATIC_LIBS -DSEXP_USE_STATIC_LIBS_NO_INCLUDE=0"
clean:
	rm -rf $(TARGET) *.o *.img *.core unifont.c
full-clean: clean
	$(MAKE) -C chibi-scheme clean
cloc:
	cloc `ls | grep -v chibi-scheme | grep -v third-party`
