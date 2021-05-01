# IVS calculator, but in C

## Motivation
We made a calculator in python with PyQt5 and Pyinstaller, but the result was a bit over 160MB.
I wanted to make something more lightweight.

## Disclaimer
I will add better UI eventually. Meanwhile enjoy this console version, and this bad UI :)
Oh, and it's not tested at all. I'd gladly appreciate you telling me if you find an error.

## Compilation
```sh
make
```

## Usage
```sh
./eval '2 + 3'
```

## Features
* binary addition (`+`), subtraction (`-`), multiplication (`*`) and division (`/`)
* unary negation (`-`) and no op (`+`) for some reason
* power (`^`), roots (either (`√` or `#` because inner handling))
* unary factorial (`!`) (actually a gamma function, because of floats)
* nesting parentheses `{[()]}`
* decimal numbers (`1.2` or even `.4`)
* complex numbers (`1 - 2i`)
* symbols (`pi`, `e`, `i` (imaginary number))
* functions, although only `sqrt` and `abs` have been implemented so far

## Prerequisities
* gtk3 (only for the UI program)
