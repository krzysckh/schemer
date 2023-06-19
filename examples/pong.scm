(use "core")
(use "colors")
(use "shapes")

; it is kinda buggy, because of use of intersect? to detect colisions
; but it "works"

(set-window-option "noresizable")

(define paddle-height (/ (get-window-height) 4))
(define paddle-width (/ (get-window-width) 20))

(define p1-y (- (/ (get-window-width) 2) paddle-height))
(define p2-y (- (/ (get-window-width) 2) paddle-height))

(define p1-x 0)
(define p2-x (- (get-window-width) paddle-width))

(define paddle-speed 15)

(define ball-sz 20)
(define ball-x (- (/ (get-window-width) 2) (/ ball-sz 2)))
(define ball-y (- (/ (get-window-height) 2) (/ ball-sz 2)))

(define ball-dx 1)
(define ball-dy -1/2)
(define ball-speed 6)

(define p1-pts 0)
(define p2-pts 0)

(define-background-color black)
(define-font-color white)

(define handle-input
  (lambda ()
    (if (is-key-pressed "w") (set! p1-y (- p1-y 10)))
    (if (is-key-pressed "s") (set! p1-y (+ p1-y 10)))
    (if (is-key-pressed "k") (set! p2-y (- p2-y 10)))
    (if (is-key-pressed "j") (set! p2-y (+ p2-y 10)))))

(define reset-ball
  (lambda ()
    (set! ball-speed 6)
    (set! ball-x (- (/ (get-window-width) 2) (/ ball-sz 2)))
    (set! ball-y (- (/ (get-window-height) 2) (/ ball-sz 2)))))

(define handle-paddle-colision
  (lambda ()
    (if (> (+ p1-y paddle-height) (get-window-height))
      (set! p1-y (- (get-window-height) paddle-height)))
    (if (< p1-y 0)
      (set! p1-y 0))

    (if (> (+ p2-y paddle-height) (get-window-height))
      (set! p2-y (- (get-window-height) paddle-height)))
    (if (< p2-y 0)
      (set! p2-y 0))))

(define update-ball
  (lambda ()
    (set! ball-x (floor (+ ball-x (* ball-speed ball-dx))))
    (set! ball-y (floor (+ ball-y (* ball-speed ball-dy))))

    (cond
      ((or (intersect?
             (list (list ball-x ball-y)
                   (list (+ ball-x ball-sz) (+ ball-y ball-sz)))
             (list (list p1-x p1-y)
                   (list (+ p1-x paddle-width) (+ p1-y paddle-height))))
           (intersect?
             (list (list ball-x ball-y)
                   (list (+ ball-x ball-sz) (+ ball-y ball-sz)))
             (list (list p2-x p2-y)
                   (list (+ p2-x paddle-width) (+ p2-y paddle-height)))))
       (begin
         (set! ball-speed (+ ball-speed 1))
         (set! ball-dx (- ball-dx)))) ; hit paddle (and make ball faster)
      ((< ball-x 0) (begin
                      (set! p2-pts (+ p2-pts 1))
                      (reset-ball))) ; point for p2
      ((> ball-x (- (get-window-width) ball-sz)) (begin
                      (set! p1-pts (+ p1-pts 1))
                      (reset-ball))) ; point for p1
      ((or (< ball-y 0)
           (> (+ ball-y ball-sz) (get-window-height)))
       (set! ball-dy (- ball-dy)))))) ; bounce ball


(define update-screen
  (lambda ()
    (handle-input)
    (handle-paddle-colision)
    (update-ball)

    (text (string-append "ball speed: "
                         (number->string ball-speed))
          (floor (/ (get-window-width) 3)) 0)
    (text (number->string p1-pts) 0 0)
    (text (number->string p2-pts) (- (get-window-width) 60) 0)

    (draw-square white p1-x p1-y paddle-width paddle-height)
    (draw-square white p2-x p2-y paddle-width paddle-height)
    (draw-square alice-blue ball-x ball-y ball-sz ball-sz)))

