(use "core")
(use "shapes")
(use "colors")

(define layout-tree '())
(define ui-root #f)
(define default-ui-object `((_x1 0)
                            (_y1 0)
                            (_x2 0)
                            (_y2 0)
                            (_id #f)
                            (_name #f)
                            (parent #f)
                            (children '())
                            (on-click ,(lambda (self) #t))
                            (on-hover ,(lambda (self) #t))
                            (draw ,(lambda (self) #t))))

(define ui-add-child
  (lambda (parent child)
    (set-cdr! parent (append (cdr parent) (list child)))))

(define ui-table-add-row
  (lambda (x1 y1 x2 y2 row)
    (define block-w (ceiling (/ (- x2 x1) (length row))))
    (map (lambda (i) (list (+ x1 (* i block-w)) y1
                           (+ block-w x1 (* i block-w)) y2))
           (range 0 (length row)))))

(define ui-table
  (lambda (x1 y1 x2 y2 layout)
    (define block-h (ceiling (/ (- y2 y1) (length layout))))
    (map (lambda (i)
                (ui-table-add-row x1 (+ y1 (* i block-h))
                                  x2 (+ block-h y1 (* i block-h))
                                  (list-ref layout i)))
         (range 0 (length layout)))))

(define ui-init
  (lambda ()
    (define root (achange default-ui-object '_name "root"))
    (set! layout-tree (list root))
    (set! ui-root (car layout-tree))))

(define ui-draw-obj
  (lambda (obj)
    (for-each ui-draw-obj (cdr (assq 'children obj)))
    ((list-ref (assq 'draw obj) 1))))

(define ui-draw
  (lambda ()
    (ui-draw-obj ui-root)))
