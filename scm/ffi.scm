(use "core")

(error "not implemented")

; (ffi "/lib/libc.so"
;     '((void putchar (int) putchar)))
;        type name   args   name for scheme

; from libffi ffi.h
(define void   0)
(define uint8  1)
(define sint8  2)
(define uint16 3)
(define sint16 4)
(define uint32 5)
(define sint32 6)
(define uint64 7)
(define sint64 8)
(define float  9)
(define double 10)
(define ptr    11)
(define int sint32)

(define ffi
  (lambda (lib data)
    (define lib-loaded (ffi-load lib))
    (for-each
      (lambda (l)
        (define type (eval (list-ref l 0)))
        (define name (->string (list-ref l 1)))
        (define args (map eval (list-ref l 2)))
        (define new-name (->string (list-ref l 3)))

        (ffi-define lib-loaded type name args new-name))
      data)))
