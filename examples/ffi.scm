(use "core")
(use "ffi")

(set-window-option 'nowindow)

(sys '(cc -shared -o examples/add.so examples/add.c))
(ffi "./examples/add.so" '((int add3 (int int int) add3)))
(ffi "/lib/libc.so.6" '((void putchar (int) c-putchar)
                        (int strlen (ptr) c-strlen)))

(print (add3 '(2 1 3)))
(c-putchar '(97))
(c-putchar '(10))
(print (c-strlen '("abcdefg")))
