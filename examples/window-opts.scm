(use "core")
(use "colors")

(set-window-option "noresizable")

(define w 100)
(define h 100)

(define-background-color black)

(define on-exit
  (lambda ()
    (print "window closed, yet i'm still here. called just once, \
because update-screen is not called anymore, because there's no frames \
to draw")))


(define update-screen
  (lambda ()
    (set! w (+ w 10))
    (set! h (+ h 10))

    (set-window-size w h)

    (if (= w 300)
      (begin
        (set-window-option "nowindow")
        (on-exit)))))
