(use "__CORE__")
; chibi-scheme init file -> scm2bin.scm -> chibi-scheme init file (as object)
; -> __CORE__ defined in scm.c as include string for the file
; -> schemer executable (linked)
;
; idk man

(define stdout (open-output-file "/dev/stdout"))
(define stderr (open-output-file "/dev/stderr"))

(define print
  (lambda (s)
    (display s stdout)
    (display "\n" stdout)))

(define get-window-width
  (lambda ()
    (list-ref (get-window-size) 0)))

(define get-window-height
  (lambda ()
    (list-ref (get-window-size) 1)))

(define range-stepped
  (lambda (from to step)
    (if (>= from to)
      '()
      (cons from (range-stepped (+ from step) to step)))))

(define range
  (lambda (from to)
    (range-stepped from to 1)))

(define sum
  (lambda (l)
    (apply + l)))

(define avg
  (lambda (l)
    (/ (sum l) (length l))))

(define last
  (lambda (l)
    (cond ((null? (cdr l)) (car l))
          (else (last (cdr l))))))

(define flatten
  (lambda (l)
    (cond ((null? l) '())
          ((pair? (car l))
           (append (flatten (car l))
                   (flatten (cdr l))))
          (else (cons (car l) (flatten (cdr l)))))))
