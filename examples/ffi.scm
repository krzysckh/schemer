(use "core")
(use "ffi")

(set-window-option 'nowindow)

(ffi "/lib/libm.so.6" '((float sqrt (float) sqrt)))
(print (ffi-call "sqrt" '(4)))

