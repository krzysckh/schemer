(use "core")

(set-window-option "nowindow")

(print "no window was spawned. now i can play with this cursed edition of scheme")
(print "command line arguments are:")

(for-each (lambda (s) (print (string-append "  - " s))) argv)
