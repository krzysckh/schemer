; plot of y = 1/x

(use "colors")
(use "plot")
(use "core")

(define-background-color '(34 34 34))
(define-font-color '(222 222 222))

(define y-fn
  (lambda (x)
    (if (= x 0)
      0
      (/ 1 x))))

(define opt (list-copy default-plot-options))
(set-cdr! (assq 'line-color opt) (list '(222 222 222)))
(set-cdr! (assq 'box-color opt) (list '(34 34 34)))

(define draw-min -10)
(define draw-max 10)

(plot-set-xy opt (list
		   (range-stepped draw-min draw-max 1/30)
		   (map y-fn (range-stepped draw-min draw-max 1/30))))

(define update-screen
  (lambda ()
    (plot opt)))

