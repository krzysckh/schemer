CC=clang
CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic \
       -I. -I./third-party/chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
       -Wno-unused-command-line-argument \
       -Wno-implicit-function-declaration \
       -g

LDFLAGS=-L./third-party/chibi-scheme -L/usr/local/lib -lraylib -lm -lutil
TARGET=schemer
OFILES=unifont.o schemer.o scm.o gui.o compiler.o \
	   scm/colors.o scm/plot.o scm/core.o scm/shapes.o scm/click.o scm/game.o \
	   scm/make.o \
	   third-party/chibi-scheme/lib/init-7.o \
	   chibi-scheme.o
# what the hell lmaoo

SCHEME=LD_LIBRARY_PATH="./third-party/chibi-scheme/:" \
	DYLD_LIBRARY_PATH="./third-party/chibi-scheme/:" \
	./third-party/chibi-scheme/chibi-scheme-static
SCMFLAGS=-A ./third-party/chibi-scheme/lib/ -q
#CHIBI-FFI=$(SCHEME) $(SCMFLAGS) ./chibi-scheme/tools/chibi-ffi

.PHONY: chibi
.SUFFIXES: .scm .o

all: any2c chibi $(OFILES)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OFILES) \
		./third-party/chibi-scheme/libchibi-scheme.a
any2c: any2c.c compiler.o
	$(CC) $(CFLAGS) $(LDFLAGS) compiler.o any2c.c -o any2c
doc:
	$(MAKE) -C doc all
unifont.c:
	xxd -include ./third-party/unifont-15.0.06.ttf unifont.c
chibi-scheme.c: chibi
	xxd -include ./third-party/chibi-scheme/libchibi-scheme.a chibi-scheme.c
.c.o:
	$(CC) $(CFLAGS) -c $<
.scm.o:
	./any2c $<
	$(CC) $(CFLAGS) -c $<.c -o $@
chibi:
	[ -e .chibi-compiled ] || $(MAKE) -C ./third-party/chibi-scheme clibs.c lib/chibi/ast.so
	[ -e .chibi-compiled ] || ($(MAKE) -C ./third-party/chibi-scheme -B libchibi-scheme.a chibi-scheme-static \
		SEXP_USE_DL=0 \
		CPPFLAGS="-DSEXP_USE_STATIC_LIBS -DSEXP_USE_STATIC_LIBS_NO_INCLUDE=0" && touch .chibi-compiled)
clean:
	$(MAKE) -C doc clean
	rm -rf $(TARGET) *.o *.core unifont.c scm/*.scm.c scm/*.o chibi-scheme.c any2c
full-clean: clean
	rm -f .chibi-compiled
	$(MAKE) -C ./third-party/chibi-scheme clean
cloc:
	cloc `ls | grep -v third-party`
install: all
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/
uninstall:
	rm -f /usr/local/bin/$(TARGET)
