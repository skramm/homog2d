# Dev information

This page will gather misc. information useful for anyone wanting to contribute to code.

## Introduction

`master` branch is supposed to stay stable, with all tests passing.
`devX` branches are where things happen.

Building tests, demos, showcases, ...: everything is handled through the makefile, using the "out-of-source" principle:
everything that gets build ends up in the `BUILD` folder, and `make clean` cleans it up.

Figures that appear in the manual are of two types
 - LaTeX Tikz based, located in `docs` fodler, see the `.tex` files.
 They are generated with<br>
 `$ make doc_fig_tex`

 - Opencv-based: the programs that builds them are located in `docs/figures_src`.
 They are generated with<br>
 `$ make doc_fig`


The showcase gif images are generated with<br>
 `$ make showcase`<br>
 and require Opencv installed on the system.

## Coding style

- TABS, not spaces (1 byte per level)
- types have first character upper case, variables lowercas
- `camelCase` for identifiers
- spaces after braces (`if( someBool )`)


