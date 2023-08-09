(use "core")
(use "ui")
(use "colors")

(define win-w 600)
(define win-h 600)
(set-window-option 'noresizable)

(define color-ctr -1)

;(define (next-color)
  ;(set! color-ctr (modulo (+ 1 color-ctr) (length named-colors-list)))
  ;(list-ref named-colors-list color-ctr))

;(define tbl-rects (ui-table 0 0 win-w win-h (map (lambda (x) (range 0 x))
                                                 ;(range 1 20))))
;(define tbl-colors (map (lambda (_) (next-color))
                        ;(range 0 (/ (length (flatten tbl-rects)) 4))))


(define (on-load)
  (set-window-size win-w win-h)
  (ui-init)
  (print "WILL ADD TO ROOT")
  (ui-add-child ui-root
                (ui-table (list
                            (list (ui-block dummy-f dummy-f dummy-f)
                                  (ui-block dummy-f dummy-f dummy-f))
                            (list (ui-block dummy-f dummy-f dummy-f)
                                  (ui-block dummy-f dummy-f dummy-f)))))

  (print ui-root))


;(define (update-screen)
  ;(define i 0)
  ;(for-each (lambda (row)
              ;(for-each (lambda (rect)
                          ;(draw-square
                            ;(list-ref tbl-colors i)
                            ;(list-ref rect 0)
                            ;(list-ref rect 1)
                            ;(- (list-ref rect 2) (list-ref rect 0))
                            ;(- (list-ref rect 3) (list-ref rect 1)))
                          ;(set! i (+ i 1)))
                        ;row)) tbl-rects))

(define (update-screen)
  (ui-draw))
