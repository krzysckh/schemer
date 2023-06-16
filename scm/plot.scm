(use "colors")
(use "shapes")
(use "core")

(define plot-padding 20) ; in pixels

(define default-plot-options
  `((n-axis 2) ; 2 | 3
    (axis-names ("x axis" "y axis" "z axis"))
    (axis-colors (,red ,blue ,green))
    (line-color ,red)
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

; TODO: don't assume xv is sorted
(define plot2d
  (lambda (opt)
    (let ((names       (_get-opt opt 'axis-names))
          (axis-names  (_get-opt opt 'axis-names))
          (axis-colors (_get-opt opt 'axis-colors))
          (box-color   (_get-opt opt 'box-color))
          (line-color  (_get-opt opt 'line-color))
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
      (define plot-width (- (get-window-width) (* plot-padding 2)))
      (define plot-height (- (get-window-height) (* plot-padding 2)))

      (define x-min (apply min xv))
      (define y-min (apply min yv))

      (define points
        (map (lambda (x y)
               ; this is very cringe
               ; update: i already forgot why and how is it working
               (define sx (+ plot-padding
                             (floor (* (+ (abs x-min) x)
                                       (/ plot-width (+ (abs x-min) xs))))))

               (define sy (- (get-window-height)
                             (+ plot-padding
                                (floor (* (+ (abs y-min) y)
                                          (/ plot-height (+ (abs y-min) ys)))))))
               (list sx sy))
             xv yv))

      (for-each (lambda (i)
                  ;(print (list
                           ;(list-ref points i)
                           ;(list-ref points (+ i 1))))
                  (draw-line
                    line-color
                    (list-ref (list-ref points i) 0)
                    (list-ref (list-ref points i) 1)
                    (list-ref (list-ref points (+ i 1)) 0)
                    (list-ref (list-ref points (+ i 1)) 1)))
                (range 0 (- (length points) 1))) ; LMAOOO

      (draw-2d-scale xs ys))))

(define plot-set-xy
  (lambda (opt v)
    (define vx (list-ref v 0))
    (define vy (list-ref v 1))
    (set-cdr! (assq 'x-values opt ) `(,vx))
    (set-cdr! (assq 'y-values opt ) `(,vy))))

(define plot
  (lambda (opt)
    (cond
      ((= (_get-opt opt 'n-axis) 2) (plot2d opt))
      ((> (_get-opt opt 'n-axis) 2) (error "not implemented"))
      (error "wrong n-axis"))))

