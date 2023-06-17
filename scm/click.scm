(use "core")

; this can (for now) only register click objects that are rectangles.

; list of '(rect f drawf)
; so: '((rect1 f1 drawf1) (rect2 f2 drawf2))
; so: '((((0 0) (1 1)) a-function draw-function) ...)
(define click-objs '())

; rect <- '((x1 y1) (x2 y2))
(define on-click
  (lambda (rect f drawf)
    (set! click-objs (append click-objs (list `(,rect ,f ,drawf))))))

(define in-rect
  (lambda (rect point)
    (define x1 (list-ref (list-ref rect 0) 0))
    (define y1 (list-ref (list-ref rect 0) 1))
    (define x2 (list-ref (list-ref rect 1) 0))
    (define y2 (list-ref (list-ref rect 1) 1))
    (define px (list-ref point 0))
    (define py (list-ref point 1))
    (and (<= x1 px) (>= x2 px) (<= y1 py) (>= y2 py))))

; execute that in (update-screen)
(define handle-click
  (lambda ()
    (for-each
      (lambda (v)
        ((list-ref v 2)))
      click-objs) ; draw objects

    (if (is-mouse-pressed 0)
      (for-each
        (lambda (v)
          (if (in-rect (list-ref v 0) (get-mouse-pos))
            ((list-ref v 1)))) ; execute the f
        click-objs)
      #f)))
