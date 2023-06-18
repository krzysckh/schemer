(use "core")

(set-window-option "nowindow")
(define make-resources '())

(define define-resource
  (lambda (path)
    (set! make-resources (append make-resources (list path)))))

(define make
  (lambda ()
    (sys '(echo "this will build the project in the future"))))
