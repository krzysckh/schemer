<div id="main">

USAGE
-----

Initialize a project:
```sh
$ schemer init project-name
```

Run a project (in the projects' directory):
```sh
$ shemer run [--] arguments for program
```

Run a standalone file
```sh
$ shemer file-name.scm [--] arguments for program
```

FUNCTIONS
---------

Chibi scheme is used as the language for schemer,
it is extended with various functions.

`(text string x y)` draws *string* at *(x y)*

`(define-background-color c)` defines background color

`(define-font-color c)` defines font color

`(draw-square c x y w h)` draws a filled square, colored *c*, at *(x y)* with size
*w* *h*

`(draw-line c x1 y1 x2 y2)` draws a line from *(x1 y1)* to *(x2 y2)* with color *c*

`(get-window-size)` returns *(window-width window-height)*

`(is-mouse-pressed)` returns *#t* or *#f*

`(get-mouse-pos)` returns *(mouse-x mouse-y)*

`(get-key-pressed)` returns currently pressed key

`(is-key-pressed x)` checks if *x* is pressed

`(system s)` calls system command *s* (see EXTENSIONS -> core -> *sys*)

`(dont-init-graphics)` - do not init raylib (see EXTENSIONS -> core -> 
*set-window-option*)

`(set-window-size w h)` - set window size to *w* by *h*

`(set-window-resizable b)` - set window to resizable or not (see EXTENSIONS ->
core -> *set-window-option*)

`(use string)` adds a library or a file. if string is one of:

MAIN LOOP
---------

When graphics are loaded, schemer executes *(on-load)*, which should be defined
in your program.

Every frame, schemer will call *(update-screen)*, and it **hopes** it is
defined.


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


EXTENSIONS
----------

Extensions are loaded using the `(use)` method.
The builtin ones are defined in scm/* and compiled into schemer.
Loading builtin extensions is as simple as `(use "ext-name")`, where *ext-name*
is the file name without the extension (so for *scm/colors.scm* - the colors
extension - you would write `(use "colors")`).

List of extensions and their description:
- *colors*, defines colors using kebab-case (look in *scm/colors.scm* for a list)
- *click*, defines functions that can easily register clicks for ui elements.
  It defines:
  - `(on-click rect f drawf)` - where *rect* is a list *'((x1 y1) (x2 y1))*
    containing the rectangle that is the click area, *f* is a function
    that needs to be called when the *rect* is clicked, and *drawf* is a function
    that is called every time a frame is drawn, to draw the click obj.
  - `(handle-click)` - needs to be called every frame (in *(update-screen)*).
- *core*, imports the standard scheme functions, and defines:
  - `(print s)` - where s is the printed string
  - `(get-window-width)` - returns window width
  - `(get-window-height)` - returns window height
  - `(range-stepped from to step)` - returns a list of range from *from* to *to*
    stepped by *step*
  - `(range from to)` - returns a list of range from *from* to *to* stepped by 1
  - `(sum l)` - sums list l by applying *+*
  - `(avg l)` - return average of numbers in *l*
  - `(last l)` - returns last value of list *l*
  - `(flatten l)` - returns flattened *l*
  - `(bool->string b)` - returns boolean *b* as string
  - `(->string x)` - returns stringified x
  - `(sys x)` - executes *(system)* with *x*, where *x* stringified with
                *->string*, so can be a list of arguments
  - `(set-window-option opt)` - sets window option `opt`, where opt is one of:
    - *"nowindow"* - don't initialize graphics, or close graphics
    - *"noresizable"* - disallow resizing the window
    - *"resizable"* - allow resizing the window
    - list of opts
- *plot*, defines functions for drawing plots, and defines:
  - `default-plot-options` - default list of options passed to *(plot)*
  - `(plot-set-xy opt v)` - sets x and y axis for opt *opt*, where v =
     *'((x-values) (y-values))*, e.g. *'((0 1 2 3 4 5) (0 1 2 3 4 5)))* (y = x)
  - `(plot opt)` - plots *opt*
- *shapes*, defines functions for drawing shapes. All of them return values that
  can be then applied as a *rect* for *(on-click)*. Defines:
  - `(rect c x1 y1 x2 y2)` - draws a not-filled rectangle of color c from
    *'(x1 y1)* to *'(x2 y2)*.
  - `(intersect? rect1 rect2)` - checks if rect1 intersects rect2


EXAMPLES
--------

See *examples/*.

</div>
