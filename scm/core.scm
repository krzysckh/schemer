(use "__CORE__")
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
    (cond
      ((null? l) #f)
      (else (car (reverse l))))))

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
      ((char? x) (string x))
      ((string? x) x)
      (else (error "->string: unexpected type")))))

(define sys
  (lambda (l)
    (system (->string l))))

(define set-window-option
  (lambda (s)
    (cond
      ((list? s) (for-each set-window-option (map ->string s)))
      (else (begin
              (let ((ss (->string s)))
                (cond
                  ((string=? ss "nowindow") (dont-init-graphics))
                  ((string=? ss "noresizable") (set-window-resizable #f))
                  ((string=? ss "resizable") (set-window-resizable #t))
                  (else (error (string-append "unknown option: " ss))))))))))

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

(define close-window
  (lambda ()
    (set-window-option 'nowindow)))

(define achange
  (lambda (asc ok ov)
    (map (lambda (x)
           (define k (list-ref x 0))
           (define v (list-ref x 1))
           (if (eq? ok k)
             (list k ov)
             (list k v))) asc)))

(define aput
  (lambda (asc k v)
    (if (assq k asc)
      (achange asc k v)
      (append asc (list (list k v))))))

(define keys
  (lambda (asc)
    (map car asc)))

(define filter
  (lambda (f l)
    (cond
      ((null? l) '())
      ((f (car l)) (cons (car l) (filter f (cdr l))))
      (else (filter f (cdr l))))))

(define has
  (lambda (l x)
    (if (null? l)
      #f
      (if (eq? (car l) x)
        #t
        (has (cdr l) x)))))

(define ->char
  (lambda (x)
    (cond
      ((number? x) (->char (number->string x)))
      ((string? x) (car (string->list x)))
      ((char? x) x)
      (else
        (error "->char: unexpected type")))))

; faster than my implementation
(define string-split
  (lambda (str c)
    (let ((end (string-length str)) (ch (->char c)))
      (let lp ((from 0) (to 0) (res '()))
        (cond
         ((>= to end)
          (reverse (if (> to from) (cons (substring str from to) res) res)))
         ((eqv? ch (string-ref str to))
          (lp (+ to 1) (+ to 1) (cons (substring str from to) res)))
         (else
          (lp from (+ to 1) res)))))))

(define split-string string-split)

(define basename
  (lambda (s)
    (last (split-string s "/"))))

(define starts-with
  (lambda (haystack needle)
    (and (> (string-length haystack) (string-length needle))
         (string=?
           (substring haystack 0 (string-length needle))
           needle))))

(define all-but-last
  (lambda (l)
    (reverse (cdr (reverse l)))))

; TODO: figure out a better way || fail if not on unix
(define directory-files
  (lambda (dir)
    (split-string (sys `(ls -1 ,dir)) "\n")))
    ;(all-but-last (split-string (sys `(ls -1 ,dir)) "\n"))))

(define file-exists?
  (lambda (f)
    (if (string=? "0" (sys `(ls ,f ">/dev/null" "2>/dev/null" ";" echo -n $?)))
      #t #f)))

(define get-args
  (lambda ()
    (define all-args
      (map (lambda (x)
             (if (eq? (car (string->list x)) #\-)
               (if (has (string->list x) #\=)
                 (split-string x "=")
                 (list x #t))
               (list 'optarg x)))
           (cdr argv)))
    (list (list 'optarg
                (flatten
                  (map cdr
                       (filter (lambda (x) (eq? (car x) 'optarg)) all-args))))
          (list 'arg
                (filter (lambda (x) (not (eq? (car x) 'optarg))) all-args)))))

(define true?
  (lambda (x) x))

(define false? true?)
