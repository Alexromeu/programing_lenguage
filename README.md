# Building a Programming Language

A from-scratch toy programming language written in C++, built to learn how
languages work from the inside — lexer → parser → AST → (eventually) interpreter
and a C transpiler.

> **Note on the folder name:** this repo lives in a directory called
> `OpenGL_practice` for historical reasons (it started life as OpenGL
> experiments — see [test.cpp](test.cpp)). The actual project is the language
> implementation below.

## Goal

Learn the internals of programming languages by building one. The plan is to
build a tree-walking **interpreter** first (fastest path to a running language),
then reuse the entire front end (lexer + parser + AST) to bolt on a second back
end that **transpiles to C**.

The full design conversation and reasoning lives in
[building_a_language_notes.md](building_a_language_notes.md) — start there for
the "why" behind every decision.

## The pipeline

```
text -> Lexer -> tokens -> Parser -> AST -> Type-checker -> C codegen -> C -> gcc -> exe
```

The front end (lexer + parser + AST) is identical whether the back end
interprets or transpiles.

## Current status

- ✅ **Scanner** — splits raw source text into whitespace-separated chunks,
  tracking newlines.
- ✅ **Lexer** — turns chunks into `Token`s, looking each word up in a
  keyword/operator table (`stringToTypeMap`); unknown words become
  `UNIDENTIFIED`.
- 🚧 **AST type structures** — node structs (`ASTNode`, `ExpressionBinary`,
  `ExpressionVarDecl`, `Block`, `Function`, …) are being defined.
- ⬜ **Parser** — not yet implemented (recursive descent, operator precedence).
- ⬜ **Interpreter / transpiler** back end.

## Files

| File | What it is |
|------|------------|
| [structs.cpp](structs.cpp) | `TokenType` enum, keyword/operator map, `Token` struct, and the AST node structs. |
| [parcer.cpp](parcer.cpp) | `Scanner`, `Lexer`, (stub) `Parser`/`AST` classes, and `main()`. Includes `structs.cpp`. |
| [building_a_language_notes.md](building_a_language_notes.md) | Design notes: interpreter vs transpiler, the lexer, token design, next steps. |
| [test.cpp](test.cpp) | Leftover OpenGL/GLFW window demo (the original use of this folder). |

## Build & run

The language front end has no external dependencies — just a C++ compiler.
`parcer.cpp` `#include`s `structs.cpp`, so compile only `parcer.cpp`:

```bash
g++ parcer.cpp -o out
./out
```

Sample input (hard-coded in `main`): `int a = 10 int b = 2 print a + b`

Expected output (token kind id -> value):

```
0 -> int
10 -> a
7 -> =
10 -> 10
0 -> int
10 -> b
7 -> =
10 -> 2
9 -> print
10 -> a
4 -> +
10 -> b
```

### Building the OpenGL demo (optional)

[test.cpp](test.cpp) needs GLEW and GLFW:

```bash
g++ test.cpp -o test -lGLEW -lglfw -lGL
```

## Next steps

- Lock in the `Token` shape (add `line`/`column` for error reporting).
- Implement the **Parser** (recursive descent — the one genuinely clever stage,
  where operator precedence gets baked into the tree shape).
- Walk the AST with an `eval(node)` interpreter.

## Reference

*Crafting Interpreters* by Bob Nystrom (free online) — builds an interpreter
first, then a bytecode compiler. Matches this project's plan.
```