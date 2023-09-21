#!/bin/sh

[ "$1" = "i am running from a makefile" ] || exit 1

set -e

LDFLAGS="-lm -lopengl32 -lgdi32 -lwinmm -lmingw32 -lssp -fstack-protector"
CFLAGS="-g -w -I./third-party/chibi-scheme/include/"
CFILES=`ls ./*.c ./scm/*.c ./third-party/chibi-scheme/lib/init-7.scm.c | grep -v any2c`
LIBS='./libchibi-scheme.a ./libraylib.a'
MINGW="x86_64-w64-mingw32"
WINE="/usr/bin/wine"

download() {
  name=$1
  path=$2

  [ -f "$name" ] && return

  echo "-----------------------------------------------------------"
  echo "i will now get pre-compiled $name from $path"
  echo "if you're not ok with that, interrupt this build script"
  echo "-----------------------------------------------------------"

  wget "$path" -O "$name"
}

#build_win_chibi() {
  #wd=`pwd`
  #mkdir -p ./third-party/chibi-scheme/build
  #cd ./third-party/chibi-scheme/build
  #$MINGW-cmake -DBUILD_SHARED_LIBS=YES -DCMAKE_CROSSCOMPILING_EMULATOR=$WINE ..
  # fix ....math.c
  #$MINGW-make
  #cd "$wd"
#}

get_raylib() {
  download libraylib.a "https://pub.krzysckh.org/libraylib.a"
}

get_chibi() {
  download libchibi-scheme.a "https://pub.krzysckh.org/libchibi-scheme.a"
}

fix_files() {
  sed -i.bak 's/^#include.*windows\.h.*$//' ./third-party/chibi-scheme/include/chibi/sexp.h
}

build_libschemer() {
  rm -f ./libschemer-w64.a

  for f in $CFILES; do
    $MINGW-gcc $CFLAGS -c $f -o $f.obj
  done
  wd=`pwd`

  rm -fr /tmp/schemer-tmp/
  mkdir -p /tmp/schemer-tmp/
  cp ./libchibi-scheme.a /tmp/schemer-tmp/
  cd /tmp/schemer-tmp/ || exit 1
  $MINGW-ar x "$wd/libchibi-scheme.a"
  cd "$wd"
	$MINGW-ar rsc libschemer-w64.a /tmp/schemer-tmp/*.obj \
    `echo $CFILES \
      | sed 's/\.\/schemer.c//' \
      | sed 's/\.\/res-handler.c//' \
      | sed 's/\.c/.c.obj/g'`
}

build_schemer() {
  $MINGW-gcc $CFLAGS $CFILES -static $LIBS $LDFLAGS -o schemer.exe
}

get_raylib
get_chibi
fix_files
build_schemer
build_libschemer
