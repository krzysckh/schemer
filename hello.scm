; (define-width 640)
; (define-height 480)

(use "colors")
(use "plot")
(use "core")

;(define-background-color '(255 255 255))
(define-font-color black)

(define y-fn
  (lambda (x)
    (if (= x 0)
      0
      (/ 1 x))))

(define opt (list-copy default-plot-options))
(define draw-min -10)
(define draw-max 10)

(plot-set-xy opt (list
		   (range-stepped draw-min draw-max 1/30)
		   (map y-fn (range-stepped draw-min draw-max 1/30))))

(define update-screen
  (lambda ()
    (plot opt)
    (define-font-color red)
    (define-font-color black)))

