(use "core")

; this can (for now) only register click objects that are rectangles.

; list of '(rect f drawf)
; so: '((rect1 f1 drawf1) (rect2 f2 drawf2))
; so: '((((0 0) (1 1)) a-function draw-function) ...)
(define click-objs '())
(define mouse-holding #f)

; rect <- '((x1 y1) (x2 y2))
(define on-click
  (lambda (rect f drawf)
    (set! click-objs (append click-objs (list `(,rect ,f ,drawf))))))

; execute that in (update-screen)
(define handle-click
  (lambda ()
    (for-each
      (lambda (v)
        ((list-ref v 2)))
      click-objs) ; draw objects

    (if (and (is-mouse-pressed 0) (eq? mouse-holding #f))
      (begin
        (set! mouse-holding #t)
        (for-each
          (lambda (v)
            (if (point-in-rect? (get-mouse-pos) (list-ref v 0))
              ((list-ref v 1)))) ; execute the f
          click-objs))
      #t)
    (if (not (is-mouse-pressed 0))
      (set! mouse-holding #f))))
