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
      (else (error "->string: unexpected type")))))

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
      (else (error "set-window-option: unexpected type")))))

(define set-nth ; returns a new list
  (lambda (l n v)
    (if (null? l) l (if (= n 0)
                      (cons v (cdr l))
                      (cons (car l) (set-nth (cdr l) (- n 1) v))))))

(define file->char-list
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

