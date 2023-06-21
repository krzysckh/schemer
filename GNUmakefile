CC=clang
TARGET=schemer
PREFIX=/usr/local
CFLAGS=-Wall -Wextra -std=c99 -pedantic \
       -I. -I./third-party/chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
       -Wno-unused-command-line-argument \
       -Wno-implicit-function-declaration \
       -g

LDFLAGS=-L./third-party/chibi-scheme -L/usr/local/lib -lraylib -lm -lutil

OFILES=scm.o unifont.o gui.o compiler.o \
	   scm/colors.o scm/plot.o scm/core.o scm/shapes.o scm/click.o scm/game.o \
	   scm/make.o \
	   third-party/chibi-scheme/lib/init-7.o
# what the hell lmaoo

SCHEME=LD_LIBRARY_PATH="./third-party/chibi-scheme/:" \
	DYLD_LIBRARY_PATH="./third-party/chibi-scheme/:" \
	./third-party/chibi-scheme/chibi-scheme-static
SCMFLAGS=-A ./third-party/chibi-scheme/lib/ -q
#CHIBI-FFI=$(SCHEME) $(SCMFLAGS) ./chibi-scheme/tools/chibi-ffi

.PHONY: chibi
.SUFFIXES: .scm .o

all: any2c chibi $(OFILES) schemer.o res-handler.o
	rm -f libschemer.a
	$(CC) $(LDFLAGS) -o $(TARGET) $(OFILES) schemer.o res-handler.o \
		./third-party/chibi-scheme/libchibi-scheme.a
	$(MAKE) libschemer.a
libschemer.a:
	mkdir -p /tmp/schemer-tmp/ || false
	rm -f /tmp/schemer-tmp/*
	ar --output /tmp/schemer-tmp/ x ./third-party/chibi-scheme/libchibi-scheme.a
	ar -r libschemer.a /tmp/schemer-tmp/*.o $(OFILES)
	rm -rf /tmp/schemer-tmp
any2c: any2c.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DANY2C compiler.c any2c.c -o any2c
doc:
	$(MAKE) -C doc all
unifont.c:
	xxd -include ./third-party/unifont-15.0.06.ttf unifont.c
res-handler.c:
	echo "#include <chibi/eval.h>" > res-handler.c
	echo "int is_compiled_in(char *path) { return 0; }" >> res-handler.c
	echo "void compiled_include(sexp ctx, char *s) { return; }" >> res-handler.c
	echo "char *get_contents_of(char *path) { return NULL; }" >> res-handler.c
	echo "int get_length_of(char *path) { return -1; }  " >> res-handler.c
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
	rm -rf $(TARGET) *.o *.core unifont.c scm/*.scm.c scm/*.o any2c res-handler.c libschemer.a
	rm -rf /tmp/schemer-tmp
full-clean: clean
	rm -f .chibi-compiled
	$(MAKE) -C ./third-party/chibi-scheme clean
cloc:
	cloc `ls | grep -v third-party`
install: all libschemer.a
	mkdir -p $(PREFIX)/bin
	cp $(TARGET) $(PREFIX)/bin/
	cp libschemer.a $(PREFIX)/lib/
uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
