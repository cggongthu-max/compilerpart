# Design Notes: Why There Is No Semantic Analysis Module

> **This is not an omission. It is a deliberate architectural choice.**

If you are reading this codebase and looking for a traditional pipeline—
`Lexer → Parser → AST → Semantic Analyzer → Code Generator`—you will not find it.
That pipeline has been collapsed into something far simpler:

```
Lexer → Token Buffer + Match Table → (Parse + Generate)
```

There is no `ASTNode`. There is no `ast_new()`. There is no `ast_free()`.
The call stack of the recursive-descent parser **is** the tree.

---

## 1. The Problem with Explicit ASTs

In a conventional compiler, the parser allocates heap nodes for every syntactic construct:

```c
ASTNode *n = ast_new(ND_WHILE);
n->cond = parse_expression();
n->body = parse_statement();
```

Then the code generator walks these nodes:

```c
switch (node->type) {
    case ND_WHILE:  gen_while(node);  break;
    case ND_ASSIGN: gen_assign(node); break;
    // ... dozens more cases
}
```

This approach is correct, but it is **heavy**:

- **Memory churn**: Every expression, statement, and block becomes a heap allocation.
- **Boilerplate explosion**: You need node types, constructors, destructors, child arrays, and visitor dispatch for every syntactic category.
- **Indirection tax**: The generator chases pointers (`node->left`, `node->right`, `node->children[i]`) instead of acting on the source directly.

For a small, single-pass language, much of this machinery is dead weight.

---

## 2. The Insight: The Call Stack Is the Tree

Consider how `fib(n)` computes without ever building a call-graph data structure:

```c
int fib(int i) {
    if (i <= 1) return i;
    return fib(i - 1) + fib(i - 2);
}
```

The call graph of `fib(10)` is a binary tree, yet no `TreeNode` is ever allocated.
The function *knows* the structure of the tree because the structure is encoded in the **control flow**.

We apply the same idea to compilation.

When the parser sees:

```c
while (i <= 100) { s = s + i; i = i + 1; }
```

It does not construct a `ND_WHILE` node with `cond` and `body` pointers.
Instead, it recursively descends into the condition, emits comparison code,
then recursively descends into the body, emits assignment code,
and finally emits the backward jump.

The stack frames of `gen_statement()` → `gen_expr()` → `gen_additive()` → ...
*are* the tree path. The tree exists in time, not in space.

---

## 3. Two-Pass Design with a Sparse Tree

If we eliminate the AST, how do we know where a `while` block ends,
or which `)` matches a given `(`?

We use a **two-pass strategy** over the flat token stream:

### Pass 1: Lex + Structural Metadata

All tokens are read into a flat array. Simultaneously, we build a **match table**—a parallel integer array where:

- `match[i]` = the index of the matching `)` or `}` for a `(` or `{` at position `i`.
- `match[i]` = the index of the matching `(` or `{` for a `)` or `}` at position `i`.

This is a **sparse tree representation**. It tells us the *span* of every parenthesized expression and every braced block without allocating a single tree node.

### Pass 2: Recursive Descent + Direct Emission

The generator walks the token stream with a cursor (`pos`).
Because the grammar is LL(1), the parser always knows what comes next:

- A `TK_WHILE` means: consume it, expect `(`, parse expression, expect `)`, parse statement, emit loop instructions.
- A `TK_LBRACE` means: consume it, parse statements until `TK_RBRACE`, consume it.

No node allocation. No pointer chasing. Just **parse, then emit**.

---

## 4. What Happened to "Semantic Analysis"?

In traditional compilers, the semantic analyzer walks the AST to:

1. **Build a symbol table** — We do this in Pass 1 by scanning the token stream for `int ident ;` sequences. No AST required.
2. **Type checking** — Our language is monomorphic (all `int`). Type checking collapses to token validation.
3. **Scope resolution** — Our language has a single flat scope. Variable lookup is a linear scan of the symbol table.

Because the language is small and the grammar is simple, all semantic concerns are folded into the parsing/generation pass. There is no separate "semantic analysis module" because there is no data structure for it to analyze.

---

## 5. Trade-offs and Limitations

This design is not a universal replacement for AST-based compilers.
It is a **domain-specific optimization** for languages that are:

- **Small**: A handful of statement types and one data type.
- **Single-pass**: No need for inter-procedural analysis or complex optimizations.
- **Source-to-source or simple target**: The backend emits instructions directly during parse.

### When you *should* use an AST

- You need an **interpreter** that evaluates the same tree multiple times.
- You need **multi-pass optimizations** (constant folding, dead code elimination, inlining).
- You need **source-to-source transformation** (pretty-printing, refactoring).
- Your language has **complex scoping** or **type inference** that requires non-local analysis.

### When this design shines

- Educational compilers where clarity matters more than industrial robustness.
- Prototype languages where syntax is still in flux.
- Target architectures (like MMIX) where the goal is straight-line code emission with minimal overhead.

---

## 6. Summary

| Concern | Traditional (AST) | This Compiler (Implicit Tree) |
|---------|-------------------|-------------------------------|
| Heap allocations | One per syntactic node | Zero (only token array) |
| Memory management | Recursive `ast_free()` | Single `free()` on token buffer |
| Node dispatch | `switch(node->type)` | `switch(token->type)` at parse time |
| Expression handling | Build tree, then walk | Emit instructions as you parse |
| Semantic analysis | Separate AST pass | Folded into token scan + parse |

The absence of a semantic analysis module is not a bug.
It is the consequence of asking:

> *"Do we need to build the tree in memory, or do we only need to traverse it?"*

If traversal is sufficient, the call stack is the most elegant tree you will ever use.
