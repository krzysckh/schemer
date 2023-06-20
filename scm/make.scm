(use "core")

(define make-resources '())

; name is the name by which the loaded resource will be known as
; in the rest of the source
(define define-resource
  (lambda (path name)
    (compiler-add-resource path)
    (set! make-resources (append make-resources (list `(,path ,name))))))

(define write-schemer-build-files
  (lambda ()
    (with-output-to-file "build/schemer-build"
                         (lambda ()
                           (for-each (lambda (s)
                                       (print (string-append
                                                (car s)
                                                "\t"
                                                (->string (list-ref s 1)))))
                                     make-resources)))))

(define ->symbol
  (lambda (x)
    (cond
      ((string? x) (string->symbol x))
      ((symbol? x) x)
      (else (error "unexpected type")))))

(define load-resources
  (lambda ()
    (for-each (lambda (x)
                (let ((s (->symbol (list-ref x 1))))
                  (eval `(define ,s (load-image ,(car x))))))
              make-resources)))

(define make
  (lambda ()
    (write-schemer-build-files)
    (sys '(echo "this will build the project in the future"))))
