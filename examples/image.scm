; images :)

(use "core")

(define last-window-w 0)
(define last-window-h 0)
(define img #f)

(define on-load
  (lambda ()
    (set! img (load-image "/tmp/wall.png"))))

(define update-screen
  (lambda ()
    (if (or (not (= (get-window-width) last-window-w))
            (not (= (get-window-height) last-window-h)))
      (begin
        (resize-image img (get-window-width) (get-window-height))
        (set! last-window-w (get-window-width))
        (set! last-window-h (get-window-height))))
    ; resize-image should not be used like that, because in this example
    ; image quality will worsen dramatically when resized down, and then up

    (show-image img 0 0)))


