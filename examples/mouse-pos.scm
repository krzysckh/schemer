(use "core")
(use "colors")

(define (on-load)
  #t)

(define (update-screen)
  (define x (car (get-mouse-pos)))
  (define y (cadr (get-mouse-pos)))

  (draw-square black x y 10 10))
