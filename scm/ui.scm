(use "core")
(use "shapes")
(use "colors")

(define layout-tree '())

(define ui-add-child
  (lambda (parent child)
    (set-cdr! parent (append (cdr parent) (list child)))))

(define ui-table-add-row
  (lambda (x1 y1 x2 y2 row)
    (define block-w (floor (/ (- x2 x1) (length row))))
    (map (lambda (i) (list (+ x1 (* i block-w)) y1
                           (+ block-w x1 (* i block-w)) y2))
           (range 0 (length row)))))

(define ui-table
  (lambda (x1 y1 x2 y2 layout)
    (define block-h (floor (/ (- y2 y1) (length layout))))
    (map (lambda (i)
                (ui-table-add-row x1 (+ y1 (* i block-h))
                                  x2 (+ block-h y1 (* i block-h))
                                  (list-ref layout i)))
         (range 0 (length layout)))))

