(use "colors")
(use "shapes")
(use "core")

(define plot-padding 20) ; in pixels

(define default-plot-options
  `((n-axis 2) ; 2 | 3
    (axis-names ("x axis" "y axis" "z axis"))
    (axis-colors (,red ,blue ,green))
    (box-color ,black)
    (x-values ())
    (y-values ())
    (z-values ()))) ; may be ommitted if n-axis = 2


(define _get-opt
  (lambda (l o)
    (list-ref (assq o l) 1))) ; am i supposed to do that??

; TODO: you can do better
; TODO: take font size into account
(define draw-2d-scale
  (lambda (xb yb)
    (text (number->string yb) (/ plot-padding 4) (/ plot-padding 4))
    (text (number->string xb) (- (get-window-width) plot-padding)
          (- (get-window-height) plot-padding))))

(define create-xy-list 
  (lambda (xv yv)
    (map (lambda (x y) `(,x ,y)) xv yv)))

; TODO: don't assume xv is sorted
(define plot2d
  (lambda (opt)
    (let ((names       (_get-opt opt 'axis-names))
          (axis-names  (_get-opt opt 'axis-names))
          (axis-colors (_get-opt opt 'axis-colors))
          (box-color   (_get-opt opt 'box-color))
          (xv          (_get-opt opt 'x-values))
          (yv          (_get-opt opt 'y-values)))
      (rect
        box-color
        plot-padding
        plot-padding
        (- (get-window-width) plot-padding)
        (- (get-window-height) plot-padding))

      (if (not (eq? (length xv) (length yv)))
        (error "invalid data"))

      ; cuh??
      (define xs (apply max xv))
      (define ys (apply max yv))

      (map (lambda (x y)
             ; TODO: przyszly ja prosze zrob to w jakis fajny sposob bo ja
             ; nie mam dzisiaj juz sily serio
             (draw-square orange x y 5 5))
           xv yv)
      (draw-2d-scale xs ys))))

(define plot
  (lambda (opt)
    (cond
      ((= (_get-opt opt 'n-axis) 2) (plot2d opt))
      ((> (_get-opt opt 'n-axis) 2) (error "not implemented"))
      (error "wrong n-axis"))))

