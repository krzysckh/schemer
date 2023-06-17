; animated plot o_O

(use "core")
(use "colors")
(use "plot")

(define-background-color '(34 34 34))
(define-font-color '(222 222 222))

(define y-fn
  (lambda (x)
    (expt x 3)))

(define opt (list-copy default-plot-options))

(set-cdr! (assq 'line-color opt) (list '(222 222 222)))
(set-cdr! (assq 'box-color opt) (list '(34 34 34)))

(define draw-min -1)
(define draw-max 1)

(define update-screen
  (lambda ()
    (plot-set-xy
      opt
      (list
        (flatten (range-stepped draw-min draw-max 1/20))
        (flatten (map y-fn (range-stepped draw-min draw-max 1/20)))))

    (set! draw-min (- draw-min 1/20))
    (set! draw-max (+ draw-max 1/20))

    (plot opt)))

