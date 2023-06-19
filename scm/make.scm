(use "core")

(set-window-option "nowindow")
(define make-resources '())

; name is the name by which the loaded resource will be known as
; in the rest of the source
(define define-resource
  (lambda (path name)
    (set! make-resources (append make-resources (list `(,path ,name))))))

(define write-schemer-build-files
  (lambda ()
    (with-output-to-file "build/schemer-build"
                         (lambda ()
                           (for-each (lambda (s)
                                       (print (string-append (car s)
                                                             "\t"
                                                             (car (cdr s)))))
                                     make-resources)))))

(define make
  (lambda ()
    (write-schemer-build-files)
    (sys '(echo "this will build the project in the future"))))
