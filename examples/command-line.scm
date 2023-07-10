(use "core")

(set-window-option "nowindow")

(print "no window was spawned. now i can play with this cursed edition of scheme")
(print "command line arguments are:")

; if the program is run as:
; schemer command-line.scm args ...
; argv will be set to '(schemer args ...), so it will ommit the program name, but keep argv0
; that's why i skip the first arg here
(for-each (lambda (s) (print (string-append "  - " s))) (cdr argv))
