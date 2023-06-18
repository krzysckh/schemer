; vim: filetype=scheme:
; usage:
;  $ chibi-scheme scm2bin.scm file.scm      # -> file.c
;  $ chibi-scheme scm2bin.scm -lib=file.o file.scm # -> file.o
;                (exports void include_<filename>(ctx))

(import (chibi))
(import (chibi process))
(import (chibi regexp))

(define t "c") ; c | lib
(define fname #f)
(define libname #f)

(define file->clist
  (lambda (path)
    (call-with-input-file
      path
      (lambda (input-port)
        (let loop ((x (read-char input-port)))
          (cond
            ((eof-object? x) '())
            (#t (begin
                  (cons x (loop (read-char input-port)))))))))))

(define string-replace-char
  (lambda (s c1 c2)
    (define normalize
      (lambda (c)
        (cond
          ((char? c) (char->integer c))
          ((integer? c) c)
          ((string? c) (char->integer (car (string->list c)))))))

    (define n-c1 (normalize c1))
    (define n-c2 (normalize c2))

    (apply string
           (map integer->char
                (map (lambda (x) (if (eq? x n-c1) n-c2 x))
                     (map char->integer (string->list s))))))) ; wow

(define escape-name-for-cfun
  (lambda (s)
    (string-replace-char
      (string-replace-char
        (string-replace-char s "-" "_") "/" "_") "." "_"))) ; yeah i know it
                                                            ; wont work with
                                                            ; stupid file names

(for-each
  (lambda (x)
    (cond
      ((regexp-search "-lib" x) (begin
                                  (set! t "lib")
                                  (set! libname (list-ref
                                                  (regexp-split "=" x) 1))))
      ((not (eq? (string->list x) '())) (set! fname x))))
  (cdr (command-line)))
; when calling as "chibi-scheme scm2bin.scm", skip "scm2bin.scm"

(if (eq? fname #f)
  (error "no filename given"))

(define fv
  (map char->integer (file->clist fname)))

(define of (open-output-file (string-append fname ".c")))
(define safe-fname (escape-name-for-cfun fname))

(display "static char v[] = {\n" of)
(for-each
  (lambda (x) (display (string-append "  " (number->string x) ",\n") of))
  (append fv '(0)))
(display "};\n" of)

(display (string-append "
/* TODO: czy _na serio_ muszę wrzucać całe to łajno do pliku tylko po to
 * żeby potem wywołać na nim sexp_load()???
 * serio nie ma jakiegoś hmmm... pomyślmy.... idkkk serio 
 * sexp_load_string() ?!?!?!?!?!?!!?
 *
 * spoiler: nie ma
 */

static long int v_l = " (number->string (length fv)) ";

#include <chibi/eval.h>
#include <stdio.h>
#include \"schemer.h\"

int include_" safe-fname "(sexp ctx) {
  sexp e, obj;
  char *fname = tmpnam(NULL);
  FILE *f = fopen(fname, \"w\");

  fwrite(v, 1, v_l, f);
  fclose(f);

  obj = sexp_c_string(ctx, fname, -1);
  e = sexp_load(ctx, obj, NULL);

  return print_if_exception(e);
}

") of)

(close-output-port of)

(display (string-append "-> " fname ".c"))
(newline)

(if (string=? t "lib")
  (begin
    (system `(cc -I. -o ,libname -c ,(string-append fname ".c")))
    (display (string-append "-> " libname))
    (newline)))
