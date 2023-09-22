(use "core")
(use "shapes")
(use "game2d")
(use "colors")

(set-window-option 'noresizable)

(define mouse-holding #f)
(define mouse-dx #f)
(define mouse-dy #f)

(define block
  (sprite
    (lambda (self)
      (draw-square red (spr-x self) (spr-y self) (spr-v self 'w)
                   (spr-v self 'h)))
    (lambda (self) #t)))

(define movable
  (sprite
    (lambda (self) (rect black (spr-x self) (spr-y self)
                         (+ (spr-v self 'w) (spr-x self))
                         (+ (spr-v self 'h) (spr-y self))))
    (lambda (self)
      (cond
        ((and (not mouse-holding)
              (is-mouse-pressed 0)
              (point-in-rect? (get-mouse-pos) (spr2rect self)))
         (let ((mx (car (get-mouse-pos)))
               (my (cadr (get-mouse-pos))))
           (set! mouse-holding self)
           (set! mouse-dx (- (spr-x self) mx))
           (set! mouse-dy (- (spr-y self) my))))
        ((and mouse-holding
              (not (is-mouse-pressed 0)))
         (set! mouse-holding #f))
        ((eq? mouse-holding self)
         (let ((mx (car (get-mouse-pos)))
               (my (cadr (get-mouse-pos))))
           (sspr-x self (+ mouse-dx mx))
           (sspr-y self (+ mouse-dy my))))))))

(define (on-load)
  (sspr-x movable 10)
  (sspr-y movable 10)
  (sspr-v movable 'w 100)
  (sspr-v movable 'h 100)

  (sspr-x block 200)
  (sspr-y block 200)
  (sspr-v block 'w 100)
  (sspr-v block 'h 200))

(define (update-screen)
  (text (if (intersect? (spr2rect block) (spr2rect movable)) "yes" "no") 400 10)
  (game2d))
