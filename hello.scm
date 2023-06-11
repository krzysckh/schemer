; (define-width 640)
; (define-height 480)

(define update-screen
  (lambda ()
    (draw-square '(255 0 255 255) 10 10 100 100)))

(for-each (lambda (x) (text "balls" 10 10)) (list 1 2 3))
