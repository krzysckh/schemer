CC=clang
TARGET=schemer
PREFIX=/usr/local
CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic \
       -I. -I./third-party/chibi-scheme/include -I/usr/local/include \
       -Wno-unused-parameter \
       -Wno-unused-command-line-argument \
       -Wno-implicit-function-declaration \
       -g

LDFLAGS=-L./third-party/chibi-scheme -L/usr/local/lib -lraylib -lm -lutil

OFILES=scm.o canada1500.o gui.o compiler.o \
	   scm/colors.o scm/plot.o scm/core.o scm/shapes.o scm/click.o scm/game.o \
	   scm/make.o scm/ui.o \
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
libschemer.a: create-chibi-ofiles
	mkdir -p /tmp/schemer-tmp/ || false
	rm -f /tmp/schemer-tmp/*
	sh ./create-chibi-ofiles
	ar r libschemer.a /tmp/schemer-tmp/*.o $(OFILES)
	rm -rf /tmp/schemer-tmp
create-chibi-ofiles:
	echo "set -xe ; SCHEMER_BUILD_DIR=`pwd`" > create-chibi-ofiles
	echo "cd /tmp/schemer-tmp/" >> create-chibi-ofiles
	echo 'ar x "$$SCHEMER_BUILD_DIR/third-party/chibi-scheme/libchibi-scheme.a"' >> create-chibi-ofiles
any2c: any2c.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DANY2C compiler.c any2c.c -o any2c
doc:
	$(MAKE) -C doc all
canada1500.c:
	xxd -include ./third-party/canada1500.ttf canada1500.c
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
	rm -rf $(TARGET) *.o *.core canada1500.c scm/*.scm.c scm/*.o any2c res-handler.c libschemer.a create-chibi-ofiles
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
