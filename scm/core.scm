(use "__CORE__")
; chibi-scheme init file -> scm2bin.scm -> chibi-scheme init file (as object)
; -> __CORE__ defined in scm.c as include string for the file
; -> schemer executable (linked)
;
; idk man

(define print
  (lambda (s)
    (display s)
    (display "\n")))

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

(define bool->string
  (lambda (v)
    (if v "#t" "#f")))

(define ->string
  (lambda (x)
    (cond
      ((list? x) (apply string-append
                        (map (lambda (x) (string-append (->string x) " ")) x)))
      ((number? x) (number->string x))
      ((symbol? x) (symbol->string x))
      ((boolean? x) (bool->string x))
      ((string? x) x)
      (else (error "unexpected type")))))

(define sys
  (lambda (l)
    (system (->string l))))

(define set-window-option
  (lambda (s)
    (cond
      ((list? s) (for-each set-window-option (map ->string s)))
      ((string? s) (cond
                     ((string=? s "nowindow") (dont-init-graphics))
                     ((string=? s "noresizable") (set-window-resizable #f))
                     ((string=? s "resizable") (set-window-resizable #t))
                     (else (error (string-append "unknown option: " s)))))
      (else (error "unexpected type")))))
