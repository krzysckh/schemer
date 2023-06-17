(use "core")
(use "shapes")
(use "colors")

(define update-screen
  (lambda ()
    (rect black 100 100 300 300)
    (rect black 250 250 400 400)
    (text (bool->string
            (intersect?
              '((100 100) (300 300))
              '((250 250) (400 400)))) 100 100)))
