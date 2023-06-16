chibi scheme is used as the language for schemer.
it is extended with various functions. they are defined in scm.c

FUNCTIONS
---------

`(text string x y)` draws *string* at *(x y)*

`(define-background-color c)` defines background color

`(define-font-color c)` defines font color

`(draw-square c x y w h)` draws a filled square, colored *c*, at *(x y)* with size
*w* *h*

`(draw-line c x1 y1 x2 y2)` draws a line from *(x1 y1)* to *(x2 y2)* with color *c*

`(get-window-size)` returns *(window-width window-height)*

`(use string)` adds a library or a file. if string is one of:
- core
- plot
- colors
- shapes
- ...TODO
this function will add (for now) scm/..., and if not, it will include file
*string*

MAIN LOOP
---------

every frame, the program will call *(update-screen)*, and it **hopes** it is
defined.

if any draw function is called not from update-screen (before the program
starts loading raylib) it will crash (or i'll fix it - who knows).


TUTORIAL
--------

todo. (lmao)

well, you probably want to
```scheme
(use "core")
```
because it includes some *nice* functions, and init-7.scm from chibi-scheme
that defines important stuff.

then you need to define
```scheme
(define update-screen
  (lambda ()
    (text "hello" 50 50)))
```
and you're good to go
