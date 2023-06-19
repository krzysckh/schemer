; on-click example

(use "core")
(use "colors")
(use "click")
(use "shapes")

(define btn1
  (on-click
    '((50 50) (100 100))
    (lambda () (begin
                 (print "clicked btn1")
                 (draw-square red 50 50 50 50)))
    (lambda () (begin
                 (rect black 50 50 100 100)
                 (text "btn1" 60 60)))))

(define btn2
  (on-click
    '((200 200) (300 300))
    (lambda () (begin
                 (print "clicked btn2")
                 (draw-square blue 200 200 100 100)))
    (lambda () (begin
                 (rect black 200 200 300 300)
                 (text "btn2" 250 250)))))

(define on-load
  (lambda () #t))

(define update-screen
  (lambda ()
    (handle-click)))
