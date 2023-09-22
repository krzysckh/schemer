#!/usr/local/bin/schemer

(use "core")

(set-window-option 'nowindow)

(define pvals (list-ref (assq 'optarg (get-args)) 1))
(define flags (list-ref (assq 'arg (get-args)) 1))

(define nflag #f)

(for-each (lambda (x) (if (string=? (car x) "-n") (set! nflag #t))) flags)
(map (lambda (s) (display (string-append s " "))) pvals)

(if (not nflag)
  (newline))
