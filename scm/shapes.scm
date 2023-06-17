; draw a rectange (without filling)
(define rect
  (lambda (c x1 y1 x2 y2) ; IT TAKES x1 y1 x2 y2 - NOT x y w h !!
    (draw-line c x1 y1 x2 y1)
    (draw-line c x1 y1 x1 y2)
    (draw-line c x1 y2 x2 y2)
    (draw-line c x2 y1 x2 y2)
    `((,x1 ,y1) (,x2 ,y2))))

