(use "core")
(use "shapes")
(use "colors")

(begin
  (set-window-option 'nowindow)
  (error "ui not implemented")
  (exit 1))

(define layout-tree '())
(define ui-positions-set #f)
(define ui-root #f)
(define id-ctr 0)

; types of objects:
; 'block (anything, drawable)
; 'none (hidden)
; 'table
; 'table-row
; 'table-column

(define default-ui-object `((_x1 0)
                            (_y1 0)
                            (_x2 0)
                            (_y2 0)
                            (_id #f)
                            (_name #f)
                            (type 'block)
                            (parent #f)
                            (children '())
                            (on-click ,(lambda (self) #t))
                            (on-hover ,(lambda (self) #t))
                            (draw ,(lambda (self) #t))))

(define dummy-f
  (lambda (_) #f))

(define ui-children-of
  (lambda (el)
    (car (cdr (assq 'children el)))))

; popierdoli mnie
(define ui-add-child
  (lambda (parent child)
    (set-cdr! parent (achange parent 'children (append (ui-children-of parent)
                                                   (list child))))))

(define ui-obj
  (lambda (type on-click on-hover draw)
    (set! ui-positions-set #f)
    (achange (achange (achange (achange default-ui-object 'on-click on-click)
                               'on-hover on-hover)
                      'draw draw)
             'type type)))


(define ui-block
  (lambda (on-click on-hover draw)
    (ui-obj 'block on-click on-hover draw)))

; example:
; (ui-add-child ui-root
;               (ui-table (list
;                           (list (ui-block ...) (ui-block ...))
;                           (list (ui-block ...) (ui-block ...)))))
; creates a 2x2 table

(define ui-table
  (lambda (layout)
    (define tbl (ui-obj 'table dummy-f dummy-f dummy-f))
    (for-each (lambda (row-elements)
                (define row (ui-obj 'table-row dummy-f dummy-f dummy-f))

                (for-each (lambda (el) (ui-add-child row el)) row-elements)
                (ui-add-child tbl row))
              layout)
    tbl))

(define ui-init
  (lambda ()
    (define root (achange (achange default-ui-object '_name "root") '_id -1))
    (set! layout-tree (list root))
    (set! ui-root (car layout-tree))))

(define ui-draw-obj
  (lambda (obj)
    (for-each ui-draw-obj (cdr (assq 'children obj)))
    ; drawable types
    (if (eq? 'block (car (cdr (assq 'type obj))))
      ((list-ref (assq 'draw obj) 1) (list-ref (assq 'draw obj) 1)))))

(define ui-set-opt
  (lambda (el opt v)
    (set! el (achange el opt v))))

(define ui-update-table-row-positions
  (lambda (x1 y1 x2 y2 row)
    (define block-w (ceiling (/ (- x2 x1) (length row))))
    (for-each (lambda (i)
                (ui-set-opt (list-ref row i) '_x1 (+ x1 (* i block-w)))
                (ui-set-opt (list-ref row i) '_y1 y1)
                (ui-set-opt (list-ref row i) '_x2 (+ block-w x1 (* i block-w)))
                (ui-set-opt (list-ref row i) '_y2 y2))
           (range 0 (length row)))))

(define ui-update-table-positions
  (lambda (tbl)
    (define tbl-h (- (list-ref (assq '_y2) 1) (list-ref (assq '_y1) 1)))
    (define row-h (ceiling (/ tbl-h (length (ui-children-of el)))))
    ; TODO: maybe floor, not ceiling?
    (for-each (lambda (i)
                (ui-update-table-row-positions
                  (list-ref (assq '_x1) 1)
                  (+ (list-ref (assq '_y1) 1) (* i row-h))
                  (list-ref (assq '_x2) 1)
                  (+ row-h (list-ref (assq '_y2) 1) (* i row-h))))
              (range 0 (length tbl)))))

;;
;(define ui-table-add-row
  ;(lambda (x1 y1 x2 y2 row)
    ;(define block-w (ceiling (/ (- x2 x1) (length row))))
    ;(map (lambda (i) (list (+ x1 (* i block-w)) y1
                           ;(+ block-w x1 (* i block-w)) y2))
           ;(range 0 (length row)))))
;;;
;(define ui-table
  ;(lambda (layout)
    ;(define block-h (ceiling (/ (- y2 y1) (length layout))))
    ;(map (lambda (i)
                ;(ui-table-add-row x1 (+ y1 (* i block-h))
                                  ;x2 (+ block-h y1 (* i block-h))
                                  ;(list-ref layout i)))
         ;(range 0 (length layout)))))


(define ui-update-all-positions
  (lambda (x1 y1 x2 y2 start)
    (define rc (ui-children-of start))
    ;(define rc (filter (lambda (x) (not (eq? (car (cdr (assq 'type x))) 'none)))
                       ;(ui-children-of start)))
    ;; all but children with t 'none
    (define row-h (/ (- y2 y1) (length rc)))

    (for-each print rc)
    (for-each (lambda (i)
                (ui-set-opt (list-ref rc i) _y1 (+ y1 (* row-h i)))
                (ui-set-opt (list-ref rc i) _y2 (+ y2 row-h (* row-h i)))
                (ui-set-opt (list-ref rc i) _x1 x1)
                (ui-set-opt (list-ref rc i) _x2 x2)
                (define t (list-ref (assq 'type (list-ref rc i)) 1))
                (if (eq? t 'table)
                  (ui-update-table-positions (list-ref rc i))))
              (range 0 (length (ui-children-of start))))))

(define ui-draw
  (lambda ()
    (if (eq? ui-positions-set #f)
      (ui-update-all-positions 0 0 (get-window-width) (get-window-height)
                               ui-root))
    (ui-draw-obj ui-root)))
