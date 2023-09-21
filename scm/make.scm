(use "core")

(define make-resources '())
(define executable-name '((local "a.out")))
(define targets '(local))

(define define-resource
  (lambda (path)
    (compiler-add-resource path)
    (set! make-resources (append make-resources (list path)))))

(define define-source define-resource)

(define set-executable-name
  (lambda (target s)
    (set! executable-name (aput executable-name target s))))

(define set-target
  (lambda (l)
    (if (list? l)
      (set! targets l)
      (set! targets (list l)))))

(define ->symbol
  (lambda (x)
    (cond
      ((string? x) (string->symbol x))
      ((symbol? x) x)
      (else (error "->symbol: unexpected type")))))

(define filename-to-cfun
  (lambda (s)
    (string-replace-char
      (string-replace-char
        (string-replace-char s "-" "_") "/" "_") "." "_"))) ; yeah i know it
                                                            ; wont work with
                                                            ; stupid file names

(define write-resource-header
  (lambda ()
    (with-output-to-file
      "./build/resources/resources.h"
      (lambda ()
        (for-each
          (lambda (s)
            (print (string-append "char *get_contents_of_"
                                  (filename-to-cfun s)
                                  "();")))
          make-resources)))))

(define write-resource-handler ; this is a mess :^)
  (lambda ()
    (with-output-to-file
      "./build/res-handler.c"
      (lambda ()
        (display
"#include <string.h>
#include <chibi/eval.h>
#include \"resources/resources.h\"
int is_compiled_in(char *path) {\n")
        (for-each
          (lambda (s)
            (display
              (string-append
                "if (strcmp(path, \"" s "\") == 0) return 1;\n")))
          make-resources)
        (if (> (length make-resources) 0) (display "else "))
        (display "return 0; }\n")
        (display "char *get_contents_of(char *path) {\n")
        (for-each
          (lambda (s)
            (display
              (string-append
                "if (strcmp(path, \"" s "\") == 0) { return get_contents_of_"
                (filename-to-cfun s) "();}\n")))
          make-resources)
        (if (> (length make-resources) 0) (display "else "))
        (display "return NULL; }\n")
        (display "int get_length_of(char *path) {\n")
        (for-each
          (lambda (s)
            (display
              (string-append
                "if (strcmp(path, \"" s "\") == 0) return get_length_of_"
                (filename-to-cfun s) "();\n")))
          make-resources)
        (if (> (length make-resources) 0) (display "else "))
        (display "return -1; }\n")
        (display "void compiled_include(sexp ctx, char *path) {\n")
        (for-each
          (lambda (s)
            (display
              (string-append
                "if (strcmp(path, \"" s "\") == 0) include_"
                (filename-to-cfun s) "(ctx);\n")))
          make-resources)
        (display "}\n")))))

(define write-main
  (lambda ()
    (with-output-to-file
      "./build/main.c"
      (lambda ()
        (display
"
/* a more harsh version of schemer.c */
#include <stdio.h>
#include <assert.h>
#include <raylib.h>

int is_compiled = 1;
char *argv0;

static char *args_shift(int *argc, char ***argv) {
    char *r;

    assert(**argv);

    r = **argv;
    (*argc) -= 1;
    (*argv) += 1;

    return r;
}

int main (int argc, char **argv) {
  char *path = NULL,
       *arg;
  extern char *argv0;
  int skip = 0;

  argv0 = args_shift(&argc, &argv);

  while (*argv) {
    arg = args_shift(&argc, &argv);
    add_scheme_arg(arg);
  }

  init_scheme(\"scm/init.scm\");
  init_gui();
  run_gui();
  end_gui();
  end_scheme();
}

")))))

; TODO: more universal approach
; also, downloading pre-compiled stuff from my website is kinda cringe,
; but i do that so i can save time myself
(define make-download
  (lambda (url path)
    (print (string-append "... " path))
    (sys `(wget -q ,url -O ,path))
    (print (string-append "OK  " path))))

(define __make-continue__
  (lambda ()
    (write-resource-header)
    (write-main)
    (for-each (lambda (target)
      (define CC "cc")
      (define CFLAGS '(-g -I/usr/local/include -L/usr/local/lib -I./build
        -Wno-unused-parameter -Wno-int-conversion
        -Wno-implicit-function-declaration))
      (define LDFLAGS '(-lraylib -lm -lutil -lffi))
      (define LIBSCHEMER "/usr/local/lib/libschemer.a")
      (define LIBS '())
      ;(define SCMOFILES "./build/lscm/*.o")
      (define SCMOFILES "")

      (cond
        ((eq? target 'local) #t)
        ((eq? target 'win64)
         (begin
           (set! SCMOFILES "")
           (set! LIBSCHEMER "build/libschemer.a")
           (set! CC "x86_64-w64-mingw32-gcc")
           (set! LIBS '("-static" "build/libraylib.a"))
           (set! LDFLAGS '(-lm -lopengl32 -lgdi32 -lwinmm -lmingw32 -lssp
                               -fstack-protector))
           (set! CFLAGS '(-g -w -I./build/inc/))
           (sys '(mkdir -p ./build/inc/))
           (make-download
             "https://pub.krzysckh.org/libraylib.a"
             "./build/libraylib.a")
           (make-download
             "https://pub.krzysckh.org/chibi-include.tgz"
             "./build/inc/chibi-include.tgz")
           (make-download
             "https://pub.krzysckh.org/libschemer-w64.a"
             "./build/libschemer.a")
           (sys '(cd ./build/inc && tar xzf chibi-include.tgz))))
        (else (error (string-append "unknown target: " target))))

      (sys `(,CC -o ,(list-ref (assq target executable-name) 1)
             ,(->string CFLAGS)
             ,SCMOFILES
             ,(if (> (length make-resources) 0) "./build/resources/*.c" "")
             ./build/res-handler.c
             ./build/main.c
             ,LIBSCHEMER
             ,(->string LIBS)
             ,(->string LDFLAGS))))
      targets)))

(define make
  (lambda ()
    (write-resource-handler))) ; wait for compiler.c, then __make-continue__
