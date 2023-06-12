; (define-width 640)
; (define-height 480)

;(prefer-graphics-type "plot")

(define c-red '(255 0 0 255))

;(define-background-color '(255 255 255))
(define-font-color c-red)

(define update-screen
  (lambda ()
    (text "halo" 50 50)
    (draw-line c-red 0 0 640 480)
    (draw-line c-red 0 480 640 0)))
