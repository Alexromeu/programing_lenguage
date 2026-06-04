# Building My Own Programming Language — Notes

A running record of the design conversation. Goal: learn the internals of
programming languages by building one in C++.

---

## The big picture / goals

- **Why:** learn how languages work from the inside; build a compiler, libraries,
  and eventually bootstrap. Long project, that's fine.
- **Implementation language:** C++ (classes map naturally onto AST nodes).
- **DECIDED: build an interpreter.** Walk the AST and execute directly (`eval(node)`).
  Fastest path to a working language; surfaces design problems early. A C-transpiler
  back end is a *maybe later* — the front end (lexer/parser/AST) is reused 100% if so.
- **First feature set (MVP):** variables, basic operations, functions, loops,
  conditionals. Standard library comes much later.

---

## Assumptions that got corrected

1. **"A language translates to a lower-level language."**
   Mostly true, but there's a fork:
   - **Compiler** — translate ahead of time (to C, asm, bytecode).
   - **Interpreter** — walk the program's structure and execute directly.
   - **Transpiler** = compiler whose output is another high-level language (our case).

2. **"Targeting C++ is hard, so target C."** Correct instinct. C is the better
   target: smaller, simpler, fewer ways for generated code to break.

3. **"Type-free (dynamic typing) is easier."** WRONG — it's *harder*, especially
   when transpiling to C. Dynamic typing forces every value into a tagged box
   (type tag + data), every operation into a runtime type check, plus you'll want
   garbage collection. That's a whole runtime system.
   **Decision: start statically typed** with a tiny type set (`int`, `bool`, maybe
   `float`, `string`). Static types map ~1:1 onto C. Add dynamic typing later if
   still wanted.

4. **"An interpreter is two programs talking over sockets."** WRONG. An interpreter
   is ONE program. It builds the AST, then a function `eval(node)` walks the tree
   and executes each node immediately, in memory. No sockets, no second process.
   (The socket idea probably came from REPLs/debuggers — tooling *around* an
   interpreter, not the interpreter itself.)

---

## The pipeline

```
text -> Lexer -> tokens -> Parser -> AST -> Type-checker -> C codegen -> C -> gcc -> exe
```

- **Lexer (tokenizer/scanner):** character stream -> flat list of tokens. Easy stage.
- **Parser:** flat token list -> AST (tree). The recursive, clever stage; bakes
  operator precedence into the tree shape.
- Front end (lexer + parser + AST) is **identical** whether we end up interpreting
  or transpiling.

### AST shape example — `x = 2 + 3 * 4`

```
        Assignment
        /        \
     "x"       BinaryOp(+)
                /        \
           Number(2)   BinaryOp(*)
                        /       \
                  Number(3)   Number(4)
```

The tree *encodes precedence* (`*` deeper than `+` => happens first). In C++ the
AST is a class hierarchy: base `Node`/`Expr`/`Stmt`, subclasses `NumberLiteral`,
`BinaryExpr`, `VariableExpr`, `Assignment`, `IfStmt`, `WhileStmt`, `FunctionDecl`.
(Templates: NOT needed early. Don't reach for them yet.)

---

## Interpreter vs Transpiler — difficulty

Both share the entire front end. They differ only in the back end:

| | Interpreter | Transpiler |
|---|---|---|
| Back end | `eval(node)` — "do the thing in C++" | `emit(node)` — produce correct C text |
| Time to first running program | minutes after parser works | longer (codegen + toolchain) |
| Debugging | step through your C++ | debug generated C (miserable) |
| Type system | can cheat, check at runtime | really want it nailed down first |
| Speed of user's program | slower (re-walks tree) | fast (compiled C) |

**The hard/educational parts (grammar, precedence, scopes, functions, control flow)
are identical in both.** Transpiling just adds a translation layer + toolchain.

**Plan:** build the **interpreter first** (proves the language works fast, surfaces
design problems early). Then KEEP lexer/parser/AST and bolt on a second back end
that emits C — compare `eval(node)` vs `emit(node)` over the same tree.
(This is the order *Crafting Interpreters* — free online — uses.)

---

## The Lexer

- Simplest stage — comparable to / easier than an HTML parser. No recursion, no tree.
- Job: char stream -> flat list of tokens. Flat in, flat out.
- Built around a **cursor** (index into source) that only moves forward.

### Core loop
> While chars remain: look at current char, decide what token *starts* here,
> consume all chars belonging to it, emit one token, repeat.

Dispatch on current char:
1. **Whitespace** -> consume & discard (but track newlines for line numbers).
2. **Digit** -> consume run of digits -> NUMBER.
3. **Letter/underscore** -> consume run of letters/digits -> a "word" -> check
   keyword table (`if`/`while` -> keyword token) else IDENT.
   (Trick: don't lex keywords separately; lex identifiers then look them up.)
4. **Operator/punctuation** -> emit matching token (see maximal munch).
5. **End of input** -> emit END_OF_FILE, stop.

### Maximal munch
When a char could start a longer token (`=` vs `==`, `<` vs `<=`, `/` vs `//`),
**grab the longest match.** Requires the ability to **peek** ahead without
consuming. (We'll need comparison operators `==`, `<=`, etc. for if/while conditions.)

### Lexer/parser boundary
The lexer does NOT care if the program makes sense. `+ ) while = = (` tokenizes
fine. It only fails on non-tokens (stray `@`, unterminated string). Catching
"you can't put `)` there" is the **parser's** job.

### Lexer class will need
- **State:** source string, cursor position, line (+ column).
- **Helpers:** `peek()` (look, don't consume), `advance()` (consume one),
  `match(expected)` (consume only if next char matches — for maximal munch).
- **Output:** run the loop, return `vector<Token>` (or hand out one at a time).

---

## The Token struct — decisions

### `kind`: use the ENUM, not `std::string`
Reasons: typos in strings compile fine (enum typos are compile errors); enum
compares are one int op vs walking chars; enum is 4 bytes vs ~32+heap for string.
For debug printing, write one helper `tokenTypeName(TokenType)` that switches and
returns a string — the only place strings are used.

### `value`: what type?
- `auto` can't be a struct member type (no initializer to infer from). Out.
- This is the static/dynamic typing problem in miniature.
- **Option A (recommended now):** don't store a value at all — keep `lexeme`
  (`"12"`), recover `12` with `std::stoi` when needed. Simplest.
- **Option B:** store one numeric type (`long` or `double`); only NUMBER fills it.
- **Option C (later):** `std::variant<long, double, std::string, bool>` once there
  are 2+ literal types. The type-safe tagged box. Not yet.

### `line` / position: just numbers
`int line; int column;` — both 1-based. Not a shape, just counters the lexer
maintains (bump line + reset column on `\n`; bump column otherwise). Stamp the
token with the position where it *started*. Build this in NOW — every later stage
reports errors by token position; painful to retrofit.

### Good starting Token
```cpp
struct Token {
    TokenType   kind;    // the enum, not a string
    std::string lexeme;  // raw source text
    int         line;
    int         column;
};
```
No `value` yet; grow it later.

### Other code fix
- **`EOF` clashes** with the `<cstdio>` macro (`-1`). Rename the enum member
  (`END_OF_FILE` / `TOK_EOF`). Also name the enum itself (`enum TokenType`).

---

## Open / next steps

- Lock in the Token shape (kind=enum, lexeme, line, column).
- Design the Lexer internals: cursor + `peek`/`advance`/`match` + main loop.
- After lexer: the parser and how it handles precedence (recursive descent) —
  the one genuinely clever algorithm in the front end.
- Decide a sample program / syntax we want the language to look like (a compass).

### Recommended resource
*Crafting Interpreters* by Bob Nystrom — free online. Builds an interpreter first,
then a bytecode compiler. Matches our plan.
