; draw a rectange (without filling)
(define rect
  (lambda (c x1 y1 x2 y2) ; IT TAKES x1 y1 x2 y2 - NOT x y w h !!
    (draw-line c x1 y1 x2 y1)
    (draw-line c x1 y1 x1 y2)
    (draw-line c x1 y2 x2 y2)
    (draw-line c x2 y1 x2 y2)
    (list (list x1 y1) (list x2 y2))))

(define intersect?
  (lambda (rect1 rect2)
    (define l1 (list-ref rect1 0))
    (define r1 (list-ref rect1 1))
    (define l2 (list-ref rect2 0))
    (define r2 (list-ref rect2 1))

    (define l1x (list-ref l1 0))
    (define l1y (list-ref l1 1))
    (define r1x (list-ref r1 0))
    (define r1y (list-ref r1 1))

    (define l2x (list-ref l2 0))
    (define l2y (list-ref l2 1))
    (define r2x (list-ref r2 0))
    (define r2y (list-ref r2 1))

    (if (or (eq? l1x r1x)
                   (eq? l1y r1y)
                   (eq? r2x l2x)
                   (eq? l2y r2y)
                   (> l1x r2x)
                   (> l2x r1x)
                   (> l1y r2y)
                   (> l2y r1y)) #f #t)))
