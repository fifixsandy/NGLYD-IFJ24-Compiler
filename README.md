# NGLYD IFJ24 Compiler
## Description
In this repository you can find implementation of a compiler for a subset of Zig programming language - IFJ24.
It has been developed by team NGLYD as a semestral project for [IFJ (Formal languages and compilers)](https://www.fit.vut.cz/study/course/280931/.cs) course on Brno University of Technology, Faculty of information technologies.
---
Compiler consists of following parts:

- lexical analyser based on *finite automaton*
- *LL1 recursive* top-down syntactic analyser
- syntactic analyser of expressions based on *precedence table*
- generator of *abstract syntactic tree*
- symbol table implemented as *AVS-tree*
- generator of intermediate code "IFJcode24"

Lexical, syntactic and semantic checks are perfomed during tokenizing and parsing.
For more detailed description, refer to documentation.pdf in /doc.

## Environment
Linux

## Build
| Target | Description                                                  | Command Example |
|--------|--------------------------------------------------------------|-----------------|
| `dev`  | Compiles source files into objects and links them.            | `make dev`       |
| `test` | Runs the test script using the compiled binary and interpreter.| `make test`      |
| `pack` | Creates a zip package of source code and documentation.        | `make pack`      |
| `doc`  | Compiles the LaTeX documentation into a PDF.                   | `make doc`       |
| `run`  | Runs the compiled executable with input/output redirection.    | `make run`       |

## Usage
Make sure you have downloaded an interpreter for *IFJcode24* from [this link](https://www.fit.vut.cz/study/course/IFJ/private/projekt/ifj24/ic24int_linux_2024-11-21.zip) and have it in root directory.
Use `make run` with input file *in.ifj* or execute the compiler with source file in IFJ24 with redirection to standard input.
Compiler prints *IFJcode24* to standard output. Redirect it to the interpreter or another file.

## Test outputs and percentages
| Module                                      | Score  | Passed/Total | Incorrect Return Codes | Incorrect Outputs |
|---------------------------------------------|--------|-------------|------------------------|-------------------|
| Lexical Analysis (error detection)          | 91 %   | 192/210      | 8 %                     | -                 |
| Syntax Analysis (error detection)           | 97 %   | 261/267      | 2 %                     | -                 |
| Semantic Analysis (error detection)         | 98 %   | 394/402      | 1 %                     | -                 |
| Code Interpretation (basic)                 | 96 %   | 346/360      | 2 %                     | 1 %               |
| Code Interpretation (expressions, built-ins)| 100 %  | 170/170      | 0 %                     | 0 %               |
| Code Interpretation (complex)               | 86 %   | 363/421      | 13 %                    | 0 %               |
| Function Expressions (FUNEXP)               | 100 %  | 150/150      | 0 %                     | 0 %               |
| **Total (excluding extensions)**            | **94 %**| **1726/1830**| **-**                    | **-**              |

## Authors

NGLYD

- [xfignam00 Matúš Fignár](https://github.com/dotZupan)
- [xmalegt00 Tibor Malega](https://github.com/tajborr)
- [xnovakf00 Filip Novák](https://github.com/fifixsandy)
- [xskovaj00 Ján Skovajsa](https://github.com/jansko03)

## License
This program is licensed under the GNU General Public License v3.0