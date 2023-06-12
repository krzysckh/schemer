; (define-width 640)
; (define-height 480)

;(prefer-graphics-type "plot")

(define update-screen
  (lambda ()
    (text "balls" 0 0)
    (draw-line '(255 0 0 255) 0 0 640 480)
    (draw-line '(255 0 0 255) 0 480 640 0)))
