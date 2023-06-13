; (define-width 640)
; (define-height 480)

(use "colors")
(use "plot")
(use "core")

;(define-background-color '(255 255 255))
(define-font-color black)

(define opt (list-copy default-plot-options))
(set-cdr! (assq 'x-values opt) (list (range 0 10)))
(set-cdr! (assq 'y-values opt) (list (range 0 10)))

(define update-screen
  (lambda ()
    (plot opt)
    (define-font-color red)
    (text "obiecuje, kiedys zadziala" 300 300)
    (define-font-color black)))

