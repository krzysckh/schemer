; TODO: don't be retarded

(define get-window-width
  (lambda ()
    (list-ref (get-window-size) 0)))

(define get-window-height
  (lambda ()
    (list-ref (get-window-size) 1)))

(define range
  (lambda (from to)
    (if (>= from to)
      '()
      (cons from (range (+ from 1) to)))))

(define sum
  (lambda (l)
    (apply + l)))

(define avg
  (lambda (l)
    (/ (sum l) (length l))))
