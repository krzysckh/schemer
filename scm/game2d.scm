(use "core")
(use "colors")

(define g2d-sprites '())

(define sprite
  (lambda (draw move)
    (define id (length g2d-sprites))
    (set! g2d-sprites
      (append g2d-sprites
              (list `((id ,id) (draw ,draw) (move ,move)
                                                 (x 0) (y 0) (v ())))))
    id))

(define set-sprite-value
  (lambda (s n v)
    (if (eq? n 'id)
      (error "game2d: n cannot be 'id"))
    (set! g2d-sprites
      (set-nth g2d-sprites
               s
               (aput (list-ref g2d-sprites s) n v)))))

(define get-sprite-value
  (lambda (s v)
    (list-ref (assq v (list-ref g2d-sprites s)) 1)))

(define spr-x
  (lambda (s)
    (get-sprite-value s 'x)))

(define spr-y
  (lambda (s)
    (get-sprite-value s 'y)))

(define sspr-x
  (lambda (s x)
    (set-sprite-value s 'x x)))

(define sspr-y
  (lambda (s y)
    (set-sprite-value s 'y y)))

(define spr-vr
  (lambda (s)
    (get-sprite-value s 'v)))

(define sspr-vr
  (lambda (s x)
    (set-sprite-value s 'v x)))

(define spr-v
  (lambda (s k)
    (list-ref (assq k (spr-vr s)) 1)))

(define sspr-v
  (lambda (s k v)
    (sspr-vr s (aput (spr-vr s) k v))))

(define spr2rect ; assumes '(v (w <width>) (h <height))
  (lambda (s)
    (define x1 (spr-x s))
    (define y1 (spr-y s))
    (define x2 (+ x1 (spr-v s 'w)))
    (define y2 (+ y1 (spr-v s 'h)))
    `((,x1 ,y1) (,x2 ,y2))))

(define game2d
  (lambda ()
    (for-each (lambda (l)
                ((list-ref (assq 'move l) 1) (list-ref (assq 'id l) 1))
                ((list-ref (assq 'draw l) 1) (list-ref (assq 'id l) 1))) g2d-sprites)))
