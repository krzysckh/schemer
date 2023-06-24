(use "core")
(use "game2d")
(use "colors")
(use "shapes")

(define-background-color black)
(set-window-option '(noresizable))

(define window-width 800)
(define window-height 600)

(define block-w 60)
(define block-h 20)

(define get-every-pos
  (lambda (xf xt yf yt sx sy)
    (apply append (map
                    (lambda (x y)
                      (map (lambda (yy) (list x yy)) (range-stepped yf yt sy)))
                    (range-stepped xf xt sx)
                    (range-stepped xf xt sy)))))

(define blocks-pos (get-every-pos 10 (- window-width 10) 10
                                  (- window-height 200) (+ block-w 5)
                                  (+ block-h 5)))

(print blocks-pos)

(define blocks
  (map (lambda (x)
         (sprite (lambda (self)
                   (if (spr-v self 'exists)
                     (draw-square yellow (spr-x self) (spr-y self)
                                  (spr-v self 'w) (spr-v self 'h))))
                 (lambda (self) #t)))
       blocks-pos))

(define lose
  (lambda ()
    (close-window)
    (print "you lose!")))

(define paddle
  (sprite (lambda (self)
            (draw-square white (spr-x self) (spr-y self) (spr-v self 'w)
                         (spr-v self 'h)))
          (lambda (self)
            (if (is-key-pressed "h")
              (sspr-x self (- (spr-x self) (spr-v self 's))))
            (if (is-key-pressed "l")
              (sspr-x self (+ (spr-x self) (spr-v self 's))))

            (if (< (spr-x self) 0)
              (sspr-x self 0))
            (if (> (spr-x self) (- window-width (spr-v self 'w)))
              (sspr-x self (- window-width (spr-v self 'w)))))))

(define ball
  (sprite (lambda (self)
            (draw-square white (spr-x self) (spr-y self)
                         (spr-v self 'w) (spr-v self 'h)))
          (lambda (self)
            (define state (spr-v self 'state))

            (if (eq? state 'on-paddle)
              (begin
                (if (is-key-pressed " ") (sspr-v self 'state 'in-air))
                (sspr-x self (- (+ (spr-x paddle) (/ (spr-v paddle 'w) 2))
                                (/ (spr-v self 'w) 2)))
                (sspr-y self (- (spr-y paddle) (spr-v self 'h))))
              (begin
                (sspr-x self (floor (+ (spr-x self) (spr-v self 'vx))))
                (sspr-y self (floor (+ (spr-y self) (spr-v self 'vy))))

                ; check if hit window border
                (if (< 0 (spr-x self))
                  (sspr-v self 'vx (- 0 (spr-v self 'vx))))
                (if (> window-width (+ (spr-x self) (spr-v self 'w)))
                  (sspr-v self 'vx (- 0 (spr-v self 'vx))))
                (if (< (spr-y self) 0)
                  (sspr-v self 'vy (- 0 (spr-v self 'vy))))

                ; check if hit paddle, if yes, also fix the position so
                ; it won'i wobble and bug
                (if (intersect? (spr2rect paddle) (spr2rect ball))
                  (begin
                    (sspr-v self 'vy (- 0 (spr-v self 'vy)))
                    (sspr-y self (- (spr-y paddle) (spr-v self 'h)))))

                ; collisions with blocks
                (for-each (lambda (block)
                            (if (and (intersect? (spr2rect block)
                                                 (spr2rect self))
                                     (spr-v block 'exists))
                              (begin
                                (sspr-v block 'exists #f)
                                (if (> (spr-x self) (spr-x block))
                                  (sspr-v self 'vy (- 0 (spr-v self 'vy))))
                                (if (< (spr-y self) (spr-y block))
                                  (sspr-v self 'vx (- 0 (spr-v self 'vx)))))))
                          blocks)

                (if (> (+ (spr-y self) (spr-v self 'h)) window-height) ; lose
                  (lose)))))))



(define on-load
  (lambda ()
    (set-window-size window-width window-height)
    (sspr-v paddle 'w (/ window-width 10))
    (sspr-v paddle 'h 20)
    (sspr-v paddle 's 15)
    (sspr-v paddle 'padding (spr-v paddle 'h))
    (sspr-y paddle (- window-height 20 20))

    (sspr-v ball 'vx 10)
    (sspr-v ball 'vy -10)
    (sspr-v ball 'w 10)
    (sspr-v ball 'h 10)
    (sspr-x ball #f)
    (sspr-y ball #f)
    (sspr-v ball 'state 'on-paddle)

    (map (lambda (block pos)
           (define x (list-ref pos 0))
           (define y (list-ref pos 1))
           (sspr-x block x)
           (sspr-y block y)
           (sspr-v block 'exists #t)
           (sspr-v block 'w block-w)
           (sspr-v block 'h block-h))
         blocks blocks-pos)))

(define update-screen
  (lambda ()
    (game2d)))
